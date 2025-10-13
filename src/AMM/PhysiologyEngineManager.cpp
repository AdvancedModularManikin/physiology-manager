#include "PhysiologyEngineManager.h"

using namespace std;
using namespace std::chrono;
using namespace tinyxml2;

std::string get_filename_date() {
	time_t now;
	char the_date[18];

	the_date[0] = '\0';

	now = time(NULL);

	if (now != -1) {
		strftime(the_date, 18, "%Y%m%d_%H%M%S", gmtime(&now));
	}

	return the_date;
}

namespace AMM {
	std::map<std::string, std::string> config;

	PhysiologyEngineManager::PhysiologyEngineManager() {
		static plog::ColorConsoleAppender <plog::TxtFormatter> consoleAppender;

		stateFile = "./states/StandardMale@0s.xml";
		patientFile = "./patients/StandardMale.xml";

		m_mgr = std::make_unique < DDSManager < AMM::PhysiologyEngineManager >> (configFile);

		m_mgr->InitializeTick();
		m_mgr->InitializeCommand();
		m_mgr->InitializeInstrumentData();
		m_mgr->InitializeSimulationControl();
		m_mgr->InitializePhysiologyModification();

		m_mgr->InitializeEventRecord();
		m_mgr->InitializeRenderModification();

		m_mgr->InitializeOperationalDescription();
		m_mgr->InitializeModuleConfiguration();
		m_mgr->InitializeStatus();

		m_mgr->InitializePhysiologyValue();
		m_mgr->InitializePhysiologyWaveform();

		m_mgr->CreateOperationalDescriptionPublisher();
		m_mgr->CreateModuleConfigurationPublisher();
		m_mgr->CreateStatusPublisher();

		m_mgr->CreatePhysiologyValuePublisher();
		m_mgr->CreatePhysiologyWaveformPublisher();

		m_mgr->CreateEventRecordPublisher();
		m_mgr->CreateRenderModificationPublisher();

		m_mgr->CreateTickSubscriber(this, &AMM::PhysiologyEngineManager::OnNewTick);
		m_mgr->CreateSimulationControlSubscriber(this, &AMM::PhysiologyEngineManager::OnNewSimulationControl);
		m_mgr->CreateCommandSubscriber(this, &AMM::PhysiologyEngineManager::OnNewCommand);
		m_mgr->CreatePhysiologyModificationSubscriber(this, &AMM::PhysiologyEngineManager::OnNewPhysiologyModification);
		m_mgr->CreateInstrumentDataSubscriber(this, &AMM::PhysiologyEngineManager::OnNewInstrumentData);
		m_mgr->CreateModuleConfigurationSubscriber(this, &AMM::PhysiologyEngineManager::OnNewModuleConfiguration);

		m_uuid.id(AMM::DDSManager<AMM::PhysiologyEngineManager>::GenerateUuidString());

		InitializeBiogears();
	}

	void PhysiologyEngineManager::PublishOperationalDescription() {
		std::lock_guard<std::mutex> lg(mgr_mutex);
		AMM::OperationalDescription od;
		od.name(moduleName);
		od.model("Physiology Engine Manager");
		od.manufacturer("Vcom3D");
		od.serial_number("1.0.0");
		od.module_id(m_uuid);
		od.module_version("1.0.0");
		const std::string capabilities = Utility::read_file_to_string(CAPABILITIES_FILE);
		od.capabilities_schema(capabilities);
		od.description();
		m_mgr->WriteOperationalDescription(od);
	}

	void PhysiologyEngineManager::PublishConfiguration() {
		std::lock_guard<std::mutex> lg(mgr_mutex);
		AMM::ModuleConfiguration mc;
		uint64_t ms = static_cast<uint64_t>(duration_cast<milliseconds>(
				system_clock::now().time_since_epoch())
				.count());
		mc.timestamp(ms);
		mc.module_id(m_uuid);
		mc.name(moduleName);
		const std::string configuration = Utility::read_file_to_string(CONFIG_FILE);
		mc.capabilities_configuration(configuration);
		m_mgr->WriteModuleConfiguration(mc);
	}

	PhysiologyEngineManager::~PhysiologyEngineManager() {
		m_pe.reset();
		m_mgr->Shutdown();
	}

	bool PhysiologyEngineManager::isRunning() const { return running; }

	void PhysiologyEngineManager::SendShutdown() {
	}

	void PhysiologyEngineManager::PrintAvailableNodePaths() const {
		auto it = nodePathMap->begin();
		while (it != nodePathMap->end()) {
			std::string word = it->first;
			std::cout << word << std::endl;
			++it;
		}
	}

	void PhysiologyEngineManager::PrintAllCurrentData() const {
		auto it = nodePathMap->begin();
		while (it != nodePathMap->end()) {
			std::string node = it->first;
			double dbl = m_pe->GetNodePath(node);
			std::cout << node << "\t\t\t" << dbl << std::endl;
			++it;
		}
	}

	int PhysiologyEngineManager::GetNodePathCount() const {
		return static_cast<int>(nodePathMap->size());
	}

	void PhysiologyEngineManager::WriteNodeData(const std::string &node) {
		AMM::PhysiologyValue dataInstance;
		std::lock_guard<std::mutex> lg(mgr_mutex);
		try {
			dataInstance.name(node);
			double data = m_pe->GetNodePath(node);
			dataInstance.value(data);
			m_mgr->WritePhysiologyValue(dataInstance);
		} catch (std::exception &e) {
			LOG_ERROR << "Unable to write node data  " << node << ": " << e.what();
		}
	}

/**
 * @brief write physiology data from "high frequency" nodes through the manager to DDS
 *
 * @param node a string that is used to specify the specific data in a map
 */
	void PhysiologyEngineManager::WriteHighFrequencyNodeData(const std::string &node) {
		AMM::PhysiologyWaveform dataInstance;
		std::lock_guard<std::mutex> lg(mgr_mutex);
		try {
			dataInstance.name(node);
			dataInstance.value(m_pe->GetNodePath(node));
			m_mgr->WritePhysiologyWaveform(dataInstance);
		} catch (std::exception &e) {
			LOG_ERROR << "Unable to write high frequency node data  " << node << ": " << e.what();
		}
	}

	void PhysiologyEngineManager::PublishData(bool force = false) {
		for (const auto& [node, accessor] : *nodePathMap) {
			if (localHighFrequencyNodes.contains(node)) {
				WriteHighFrequencyNodeData(node);
			}

			if ((lastFrame % LOW_FREQUENCY_INTERVAL) == 0 || force) {
				WriteNodeData(node);
			}
		}
	}

// Retrieves text from a specified child element or an attribute if the element is null
	std::string
	PhysiologyEngineManager::getElementText(tinyxml2::XMLElement *parent, const char *elementName, const char *attr) {
		if (!parent)
			return ""; // If parent is null, return empty string

		tinyxml2::XMLElement *elem = parent->FirstChildElement(elementName);
		if (elem) { // If element exists, return its text content or attribute
			const char *textValue = elem->GetText();
			return textValue ? textValue : "";
		} else if (attr) { // If element is null, fallback to parent attribute if provided
			const char *attrValue = parent->Attribute(attr);
			return attrValue ? attrValue : "";
		}
		return ""; // If both element and attribute are missing, return empty string
	}

// Retrieves a double value from a specified child element or an attribute if the element is null
	double
	PhysiologyEngineManager::getElementDouble(tinyxml2::XMLElement *parent, const char *elementName, const char *attr) {
		std::string text = getElementText(parent, elementName, attr);
		return text.empty() ? 0.0 : std::stod(text); // Convert text to double if non-empty, else return 0.0
	}

/**
 * @brief this function executes a modification by sting comparison
 *
 * @param pm a string that is used to match to an existing physioligy modification, sting should be a formated supported xml type
 */
	void PhysiologyEngineManager::ExecutePhysiologyModification(const std::string &pm) {
		if (m_pe == nullptr) {
			LOG_WARNING << "Physiology engine not running, cannot execute physiology modification.";
			return;
		}

		tinyxml2::XMLDocument doc;
		doc.Parse(pm.c_str());
		if (doc.ErrorID() != 0) {
			LOG_ERROR << "Document parsing error, ID: " << doc.ErrorID();
			doc.PrintError();
			return;
		}

		tinyxml2::XMLElement *pRoot = doc.FirstChildElement("PhysiologyModification");
		if (!pRoot) {
			LOG_ERROR << "Missing PhysiologyModification root element.";
			return;
		}



		while (pRoot) {
		  // Get type attribute directly from the root element
		  const char* typeAttr = pRoot->Attribute("type");
		  std::string pmType = typeAttr ? typeAttr : "";
		  
		  if (pmType.empty()) {
		    LOG_ERROR << "Missing or empty PhysiologyModification type.";
		    return;
		  }
		  boost::algorithm::to_lower(pmType);
		  LOG_INFO << "Physiology Modification: " << pmType;
		  
		  std::string pState = getElementText(pRoot, "State", "state");
		  std::string pSide = getElementText(pRoot, "Side", "side");
		  std::string pType = getElementText(pRoot, "Type", "type");
		  std::string pLoc = getElementText(pRoot, "Location", "location");
		  double pSev = getElementDouble(pRoot, "Severity", "severity");
		  double pFlow = getElementDouble(pRoot, "Flow", "flow");
		  

			/**
			   LOG_DEBUG << "\tState:\t" << pState;
			   LOG_DEBUG << "\tSide:\t" << pSide;
			   LOG_DEBUG << "\tType:\t" << pType;
			   LOG_DEBUG << "\tLocation:\t" << pLoc;
			   LOG_DEBUG << "\tSeverity:\t" << pSev;
			   LOG_DEBUG << "\tFlow:\t" << pFlow;
			**/
			
			// Process based on pmType
			if (pmType == "airwayobstruction") {
			  m_pe->SetAirwayObstruction(pSev);
			} else if (pmType == "asthmaattack") {
			  m_pe->SetAsthmaAttack(pSev);
			} else if (pmType == "braininjury") {
			  m_pe->SetBrainInjury(pSev, pType);
			} else if (pmType == "hemorrhage") {
			  m_pe->SetHemorrhage(pLoc, pFlow);
			} else if (pmType == "tourniquet") {
			  m_pe->SetTourniquet(pLoc, pState);
			} else if (pmType == "nasalcannula") {
			  handleNasalCannula(pRoot);
			} else if (pmType == "chesttube") {
			  m_pe->SetChestTube(pState, pSide);
			} else if (pmType == "needledecompression") {
			  m_pe->SetNeedleDecompression(pState, pSide);
			} else if (pmType == "occlusivedressing") {
			  m_pe->SetChestOcclusiveDressing(pState, pSide);
			} else if (pmType == "chestseal") {
			  m_pe->SetChestOcclusiveDressing(pState, pSide);
			} else if (pmType == "painstimulus") {
			  m_pe->SetPain(pLoc, pSev);
			} else if (pmType == "sepsis") {
			  m_pe->SetSepsis(pLoc, pSev);
			} else if (pmType == "substancebolus") {
			  handleSubstanceBolus(pRoot);
			} else if (pmType == "substancecompoundinfusion") {
			  handleSubstanceCompoundInfusion(pRoot);
			} else if (pmType == "substanceinfusion") {
			  handleSubstanceInfusion(pRoot);
			} else if (pmType == "substancenasaldose") {
			  handleSubstanceNasalDose(pRoot);
			} else if (pmType == "tensionpneumothorax") {
			  m_pe->SetTensionPneumothorax(pType, pSide, pSev);
			} else {
			  LOG_WARNING << "Unknown physiology modification type: " << pmType;
			}

			pRoot = pRoot->NextSiblingElement("PhysiologyModification");
		}
	}

	void PhysiologyEngineManager::handleNasalCannula(tinyxml2::XMLElement *pRoot) {
		double rate = getElementDouble(pRoot, "Rate", "value");
		std::string pUnit = getElementText(pRoot, "Rate", "unit");
		m_pe->SetNasalCannula(rate, pUnit);
	}

	void PhysiologyEngineManager::handleSubstanceBolus(tinyxml2::XMLElement *pRoot) {
		std::string pSub = getElementText(pRoot, "Substance");
		double concentration = getElementDouble(pRoot, "Concentration", "value");
		std::string cUnit = getElementText(pRoot, "Concentration", "unit");
		double dose = getElementDouble(pRoot, "Dose", "value");
		std::string dUnit = getElementText(pRoot, "Dose", "unit");
		std::string adminRoute = getElementText(pRoot, "AdminRoute");
		m_pe->SetSubstanceBolus(pSub, concentration, cUnit, dose, dUnit, adminRoute);
	}

	void PhysiologyEngineManager::handleSubstanceCompoundInfusion(tinyxml2::XMLElement *pRoot) {
		std::string pSub = getElementText(pRoot, "SubstanceCompound");
		double bagVolume = getElementDouble(pRoot, "BagVolume", "value");
		std::string bvUnit = getElementText(pRoot, "BagVolume", "unit");
		double rate = getElementDouble(pRoot, "Rate", "value");
		std::string rUnit = getElementText(pRoot, "Rate", "unit");
		m_pe->SetSubstanceCompoundInfusion(pSub, bagVolume, bvUnit, rate, rUnit);
	}

	void PhysiologyEngineManager::handleSubstanceInfusion(tinyxml2::XMLElement *pRoot) {
		std::string pSub = getElementText(pRoot, "Substance");
		double concentration = getElementDouble(pRoot, "Concentration", "value");
		std::string cUnit = getElementText(pRoot, "Concentration", "unit");
		double rate = getElementDouble(pRoot, "Rate", "value");
		std::string rUnit = getElementText(pRoot, "Rate", "unit");
		m_pe->SetSubstanceInfusion(pSub, concentration, cUnit, rate, rUnit);
	}

	void PhysiologyEngineManager::handleSubstanceNasalDose(tinyxml2::XMLElement *pRoot) {
		std::string pSub = getElementText(pRoot, "Substance");
		double dose = getElementDouble(pRoot, "Dose", "value");
		std::string dUnit = getElementText(pRoot, "Dose", "unit");
		m_pe->SetSubstanceNasalDose(pSub, dose, dUnit);
	}

/**
 * @brief creates the biogears thread and inializes engine
 *
 */
	void PhysiologyEngineManager::InitializeBiogears() {
		if (running) {
			LOG_ERROR << "Initialization failed because the sim is already running";
			return;
		}

		LOG_INFO << "Initializing Biogears thread";
		m_pe = std::make_unique<BiogearsThread>("logs/biogears.log");

		if (m_pe == nullptr) {
			LOG_WARNING << "Physiology engine not running, unable to start tick simulation.";
			return;
		}

		this->SetLogging(logging_enabled);

		bool loadSuccess = false;
		if (authoringMode) {
			LOG_INFO << "Authoring mode is enabled. Loading patient: " << patientFile;
			loadSuccess = m_pe->LoadPatient(patientFile);
			if (loadSuccess) {
				LOG_INFO << "Patient loaded successfully";
			} else {
				LOG_ERROR << "Failed to load patient";
				return;
			}
		} else {
			LOG_INFO << "Standard mode. Loading state file: " << stateFile;
			double startPosition = 0;

			std::size_t pos = stateFile.find('@');
			if (pos != std::string::npos) {
				std::string state2 = stateFile.substr(pos);
				std::size_t pos2 = state2.find('s');
				if (pos2 != std::string::npos) {
					std::string state3 = state2.substr(1, pos2 - 1);
					try {
						startPosition = std::stod(state3);
					} catch (const std::exception &e) {
						LOG_ERROR << "Failed to parse start position from state file: " << e.what();
						return;
					}
				} else {
					LOG_WARNING << "Failed to find 's' in state file. Defaulting start position to 0.";
				}
			}

			loadSuccess = m_pe->LoadState(stateFile, startPosition);
			if (loadSuccess) {
				LOG_INFO << "State loaded successfully";
			} else {
				LOG_ERROR << "Failed to load state";
				return;
			}
		}

		// Safely call PostLoad
		if (!m_pe->PostLoad()) {
			LOG_ERROR << "PostLoad failed for BiogearsThread.";
			return;
		}

		// Safely get the NodePathTable
		auto nodePathTablePtr = m_pe->GetNodePathTable();
		if (nodePathTablePtr == nullptr) {
			LOG_ERROR << "Failed to get NodePathTable from BiogearsThread.";
			return;
		}
		nodePathMap = std::make_unique<std::map<std::string, double (BiogearsThread::*)()>>(*nodePathTablePtr);

		localHighFrequencyNodes = m_pe->highFrequencyNodes;

		LOG_INFO << "Biogears initialization complete.";
	}

/**
 * @brief manages enums that define if the engine is running
 *
 */
	void PhysiologyEngineManager::StartTickSimulation() {
		LOG_INFO << "Starting tick simulation";
		running = true;
		m_pe->running = true;
		paused = false;
	}

/**
 * @brief deletes the physiology engine and sets enums
 *
 */
	void PhysiologyEngineManager::StopTickSimulation() {
		std::lock_guard <std::mutex> lg(m_mutex);
		paused = true;
		running = false;

		if (m_pe == nullptr) {
			LOG_WARNING << "Physiology engine not running, all other settings reset.";
			return;
		}

		LOG_INFO << "Deleting Physiology Engine thread";
		m_pe.reset();
		LOG_INFO << "Simulation stopped and reset.";
	}

	void PhysiologyEngineManager::StartSimulation() { m_pe->StartSimulation(); }

	void PhysiologyEngineManager::StopSimulation() { m_pe->StopSimulation(); }


  void PhysiologyEngineManager::SendPatientStateRendMod(std::string rendModType, std::string location, std::string state) {
    AMM::UUID erID;
    erID.id(AMM::DDSManager<AMM::PhysiologyEngineManager>::GenerateUuidString());
    FMA_Location fma;
    AMM::UUID agentID;
    
    AMM::EventRecord er;
    er.id(erID);
    er.location(fma);
    er.agent_id(agentID);
    er.type(rendModType);
    m_mgr->WriteEventRecord(er);
    
    AMM::RenderModification renderMod;
    renderMod.event_id(erID);
    renderMod.type(rendModType);
    renderMod.data("<RenderModification type='" + rendModType + "' location='" + location + "' state='" + state + "'/>");
    m_mgr->WriteRenderModification(renderMod);
  }
  
/**
 * @brief send patient states rendermod
 *
 */
	void PhysiologyEngineManager::SendPatientStateRendMod(std::string rendModType) {
		AMM::UUID erID;
		erID.id(AMM::DDSManager<AMM::PhysiologyEngineManager>::GenerateUuidString());
		FMA_Location fma;
		AMM::UUID agentID;

		AMM::EventRecord er;
		er.id(erID);
		er.location(fma);
		er.agent_id(agentID);
		er.type(rendModType);
		m_mgr->WriteEventRecord(er);

		AMM::RenderModification renderMod;
		renderMod.event_id(erID);
		renderMod.type(rendModType);
		renderMod.data("<RenderModification type='" + rendModType + "'/>");
		m_mgr->WriteRenderModification(renderMod);
	}

/**
 * @brief processes patient states in biogears (defined by the biogears physiology)
 *
 */
	void PhysiologyEngineManager::ProcessStates() {
		std::lock_guard<std::mutex> lg(mgr_mutex);
		if (m_pe->startOfInhale && !m_pe->startOfInhaleSent) {
			// LOG_TRACE << "Start of inhale, sending render mod";
			AMM::RenderModification renderMod;
			renderMod.type("START_OF_INHALE");
			renderMod.data("<RenderModification type='START_OF_INHALE'/>");
			m_mgr->WriteRenderModification(renderMod);
			m_pe->startOfInhale = false;
		} else if (m_pe->startOfExhale && !m_pe->startOfExhaleSent) {
			// LOG_TRACE << "Start of exhale, sending render mod";
			AMM::RenderModification renderMod;
			renderMod.type("START_OF_EXHALE");
			renderMod.data("<RenderModification type='START_OF_EXHALE'/>");
			m_mgr->WriteRenderModification(renderMod);
			m_pe->startOfExhale = false;
		}

		if (m_pe->irreversible && !m_pe->irreversibleSent) {
			LOG_DEBUG << "Patient has entered an irreversible state, sending render mod.";
			SendPatientStateRendMod("PATIENT_STATE_IRREVERSIBLE");
			m_pe->irreversibleSent = true;
		}

		if (m_pe->tachypnea && !m_pe->tachypneaSent) {
			LOG_DEBUG << "Patient has entered state Tachypnea, sending render mod.";
			SendPatientStateRendMod("PATIENT_STATE_TACHYPNEA");
			m_pe->tachypneaSent = true;
		}

		if (m_pe->tachycardia && !m_pe->tachycardiaSent) {
			LOG_DEBUG << "Patient has entered state Tachycardia, sending render mod.";
			SendPatientStateRendMod("PATIENT_STATE_TACHYCARDIA");
			m_pe->tachycardiaSent = true;
		}

		if (m_pe->paralyzed && !m_pe->paralyzedSent) {
			LOG_DEBUG << "Patient is paralyzed, sending render mod.";
			SendPatientStateRendMod("PATIENT_STATE_PARALYZED");
			m_pe->paralyzedSent = true;
		}

		if (m_pe->mildHypothermia && !m_pe->mildHypothermiaSent) {
			LOG_DEBUG << "Patient has mild hypothermia, sending render mod.";
			SendPatientStateRendMod("MILD_HYPOTHERMIA");
			m_pe->mildHypothermiaSent = true;
		}

		if (m_pe->moderateHypothermia && !m_pe->moderateHypothermiaSent) {
			LOG_DEBUG << "Patient has moderate hypothermia, sending render mod.";
			SendPatientStateRendMod("MODERATE_HYPOTHERMIA");
			m_pe->moderateHypothermiaSent = true;
		}

		if (m_pe->severeHypothermia && !m_pe->severeHypothermiaSent) {
			LOG_DEBUG << "Patient has severe hypothermia, sending render mod.";
			SendPatientStateRendMod("SEVERE_HYPOTHERMIA");
			m_pe->severeHypothermiaSent = true;
		}

		if (autosend_enabled) {
			if (m_pe->pneumothoraxLClosed && !m_pe->pneumothoraxLClosedSent) {
				LOG_DEBUG << "Patient has left closed pneumothorax, sending render mod.";
				SendPatientStateRendMod("PNEUMOTHORAX_CLOSED_L_SEVERE", "LeftChest", "On");
				m_pe->pneumothoraxLClosedSent = true;
			}

			if (m_pe->pneumothoraxLOpen && !m_pe->pneumothoraxLOpenSent) {
				LOG_DEBUG << "Patient has left open pneumothorax, sending render mod.";
				SendPatientStateRendMod("PNEUMOTHORAX_OPEN_L_SEVERE", "LeftChest", "On");
				m_pe->pneumothoraxLOpenSent = true;
			}

			if (m_pe->pneumothoraxRClosed && !m_pe->pneumothoraxRClosedSent) {
				LOG_DEBUG << "Patient has right closed pneumothorax, sending render mod.";
				SendPatientStateRendMod("PNEUMOTHORAX_CLOSED_R_SEVERE", "RightChest", "On");
				m_pe->pneumothoraxRClosedSent = true;
			}

			if (m_pe->pneumothoraxROpen && !m_pe->pneumothoraxROpenSent) {
				LOG_DEBUG << "Patient has right open pneumothorax, sending render mod.";
				SendPatientStateRendMod("PNEUMOTHORAX_OPEN_R_SEVERE", "RightChest", "On");
				m_pe->pneumothoraxROpenSent = true;
			}

			if (m_pe->hemorrhage && !m_pe->hemorrhageSent) {
				// @TODO: Get hemorrhage details, apply to proper location and with proper flow rate
				LOG_DEBUG << "Patient has a hemorrhage, sending render mod.";
				SendPatientStateRendMod("HEMORRHAGE", "LeftLeg", "on");
				m_pe->hemorrhageSent = true;
			}
		}

		if (m_pe->acuteStress && m_pe->acuteStressSent) {
			LOG_DEBUG << "Patient has acute stress, sending render mod.";
			SendPatientStateRendMod("ACUTE_STRESS");
			m_pe->acuteStressSent = true;
		}

		if (m_pe->asthmaAttack && !m_pe->asthmaAttackSent) {
			LOG_DEBUG << "Patient has an asthma attack, sending render mod.";
			SendPatientStateRendMod("ASTHMA_ATTACK");
			m_pe->asthmaAttackSent = true;
		}

		if (m_pe->brainInjury && !m_pe->brainInjurySent) {
			LOG_DEBUG << "Patient has a brain injury, sending render mod.";
			SendPatientStateRendMod("BRAIN_INJURY");
			m_pe->brainInjurySent = true;
		}
	}

/**
 * @brief manage and advance a single time step, includes checking for physiology patient events
 *
 */
	void PhysiologyEngineManager::AdvanceTimeTick() {
		m_pe->AdvanceTimeTick();
	}

/**
 * @brief checks if logging is configured and sets up logging if it is
 *
 * @param log bool that determines if logging is enabled
 */
	void PhysiologyEngineManager::SetLogging(bool log) {
		logging_enabled = log;
		if (m_pe != nullptr) {
			m_pe->SetLogging(logging_enabled);
		}
	}

	void PhysiologyEngineManager::SetAutosend(bool autosend) {
		autosend_enabled = autosend;
	}

	int PhysiologyEngineManager::GetTickCount() { return lastFrame; }

	void PhysiologyEngineManager::Status() {
		std::lock_guard <std::mutex> lg(m_mutex);
		if (m_pe != nullptr) {
			return m_pe->Status();
		}
	}

	void PhysiologyEngineManager::Shutdown() {
		SendShutdown();

		LOG_DEBUG << "[PhysiologyManager] Shutting down physiology engine.";
		m_pe->Shutdown();
	}

/**
 * @brief Listens for new physiology modifications and execute them if they are a patient action
 *
 * @param pm  a physiology modification (can be a few different types)
 * @param info rtps data
 */
	void PhysiologyEngineManager::OnNewPhysiologyModification(AMM::PhysiologyModification &pm, SampleInfo_t *info) {
		LOG_INFO << "Physiology modification received (type " << pm.type() << "): " << pm.data();

		if (m_pe == nullptr || !running) {
			LOG_WARNING << "Physiology engine not running, cannot execute physiology modification.";
			return;
		}

		// If the payload is empty, use the type to execute an XML file.
		// Otherwise, the payload is considered to be XML to execute.
		std::string pmData = pm.data().to_string();

		std::lock_guard <std::mutex> lg(m_mutex);
		if (pmData.empty()) {
			LOG_INFO << "Scenario file execution NOT SUPPORTED anymore: " << pm.type();
			try {
				//		    m_pe->ExecuteCommand(pm.type());
			} catch (std::exception &e) {
				LOG_ERROR << "Unable to execute scenario physiology modification: " << e.what();
			}
			return;
		} else {
			if (pm.type().empty() || pm.type() == "biogears") {
				LOG_INFO << "Executing Biogears PhysMod XML patient action";
				try {
					m_pe->ExecuteXMLCommand(pmData);
				} catch (std::exception &e) {
					LOG_ERROR << "Unable to apply XML physiology modification: " << e.what();
				}
				return;
			}
			LOG_INFO << "Executing AMM PhysMod XML patient action, type " << pm.type();
			try {
				ExecutePhysiologyModification(pmData);
			} catch (std::exception &e) {
				LOG_ERROR << "Unable to apply physiology modification: " << e.what();
			}
		}
	}

/**
 * @brief checks for changes in the simulation controller and adjusts simulation
 *
 * @param simControl
 * @param info
 */
	void PhysiologyEngineManager::OnNewSimulationControl(AMM::SimulationControl &simControl, SampleInfo_t *info) {
		switch (simControl.type()) {
			case AMM::ControlType::RUN: {
				LOG_DEBUG << "SimControl received: Run sim.";
				if (!running) {
					LOG_INFO << "Not running, calling starttick.";
					StartTickSimulation();
				}
				break;
			}

			case AMM::ControlType::HALT: {
				LOG_DEBUG << "SimControl received: Halt sim";
				if (running) {
					paused = true;
				}
				break;
			}

			case AMM::ControlType::RESET: {
				LOG_DEBUG << "SimControl received: Reset simulation, clearing engine data and preparing for next run.";
				if (running) {
					paused = true;
				}
				authoringMode = false;
				StopTickSimulation();
				std::this_thread::sleep_for(std::chrono::milliseconds(150));
				InitializeBiogears();
				break;
			}

			case AMM::ControlType::SAVE: {
				LOG_DEBUG << "SimControl received: Save sim";
				if (m_pe != nullptr) {
					std::ostringstream ss;
					double simTime = m_pe->GetSimulationTime();
					std::string filenamedate = get_filename_date();
					ss << "SavedState_" << filenamedate << "@" << (int) std::round(simTime) << "s."
					   << stateFilePrefix;
					LOG_INFO << "Saved state to " << ss.str();
					m_pe->SaveState(ss.str());
				} else {
					LOG_ERROR << "Simulation has not been run, no state to save.";
				}

				break;
			}
		}
	}

/**
 * @brief listens for configuration changes to the physiology manager
 *
 * @param cm command as defined in the std idl
 * @param info
 */
	void PhysiologyEngineManager::OnNewCommand(Command &cm, SampleInfo_t *info) {
		if (!cm.message().compare(0, sysPrefix.size(), sysPrefix)) {
			std::string value = cm.message().substr(sysPrefix.size());
			if (value == "ENABLE_LOGGING") {
				LOG_DEBUG << "Enabling logging";
				this->SetLogging(true);
			} else if (value == "DISABLE_LOGGING") {
				LOG_DEBUG << "Disabling logging";
				this->SetLogging(false);
			} else if (!value.compare(0, loadPrefix.size(), loadPrefix)) {
				authoringMode = false;
				LOG_INFO << "Loading state.  Setting state file to " << value.substr(loadPrefix.size());
				std::string holdStateFile = stateFile;
				stateFile = "./states/" + value.substr(loadPrefix.size()) + "." + stateFilePrefix;
				std::ifstream infile(stateFile);
				if (!infile.good()) {
					LOG_ERROR << "State file does not exist: " << stateFile;
					stateFile = holdStateFile;
					LOG_ERROR << "Returning to last good state: " << stateFile;
				}
				infile.close();
				if (running || m_pe != nullptr) {
				  LOG_INFO << "Loading state, but shutting down existing sim and physiology engine thread first.";
				  StopTickSimulation();
				}
				InitializeBiogears();
			} else if (!value.compare(0, loadPatient.size(), loadPatient)) {
				authoringMode = true;
				LOG_INFO << "Loading patient.  Setting patient file to " << value.substr(loadPatient.size());
				std::string holdPatientFile = patientFile;
				patientFile = "./patients/" + value.substr(loadPatient.size()) + "." + patientFilePrefix;
				std::ifstream infile(patientFile);
				if (!infile.good()) {
					LOG_ERROR << "Patient file does not exist: " << patientFile;
					patientFile = holdPatientFile;
					LOG_ERROR << "Returning to last good patient: " << patientFile;
				}
				infile.close();
				if (running || m_pe != nullptr) {
				  LOG_INFO << "Loading patient, but shutting down existing sim and physiology engine thread first.";
				  StopTickSimulation();
				}
				InitializeBiogears();
			} else if (!value.compare(0, loadScenarioFile.size(), loadScenarioFile)) {


				authoringMode = false;
				LOG_INFO << "Loading scenario.  Setting scenario file to " << value.substr(loadScenarioFile.size());
				scenarioFile = "./Scenarios/" + value.substr(loadScenarioFile.size()) + ".xml";
				LOG_INFO << "Scenario file is " << scenarioFile;
				std::ifstream infile(scenarioFile);
				if (!infile.good()) {
					LOG_ERROR << "Scenario file does not exist: " << scenarioFile;
				}
				infile.close();

				if (running || m_pe != nullptr) {
				  LOG_INFO << "Loading state, but shutting down existing sim and physiology engine thread first.";
				  StopTickSimulation();
				}

				std::lock_guard <std::mutex> lg(m_mutex);

				LOG_INFO << "Initializing Biogears thread to call LoadScenarioFile";
				m_pe = std::make_unique<BiogearsThread>("logs/biogears.log");

				if (m_pe == nullptr) {
					LOG_WARNING << "Physiology engine not running, unable to start tick simulation.";
					return;
				}

				this->SetLogging(logging_enabled);

				m_pe->scenarioLoading = true;
				m_pe->LoadScenarioFile(scenarioFile);
				m_pe->scenarioLoading = false;

				nodePathMap = std::make_unique<std::map<std::string, double (BiogearsThread::*)()>>(*m_pe->GetNodePathTable());

				paused = true;

			} else {
				LOG_DEBUG << "Unknown system command received: " << cm.message();
			}
		} else {
			LOG_DEBUG << "Unknown command received: " << cm.message();
		}
	}

/**
 * @brief resets state file for the physiology engine and parses the capabilities xml
 *
 * @param mc module configuration defined by idl file
 * @param info
 */
	void PhysiologyEngineManager::OnNewModuleConfiguration(AMM::ModuleConfiguration &mc, SampleInfo_t *info) {
		if (mc.name() == "physiology_engine") {
			std::lock_guard<std::mutex> lg(mgr_mutex);
			LOG_DEBUG << "Entering ModuleConfiguration for physiology engine.";
			std::string capabilities = mc.capabilities_configuration().to_string();
			ParseXML(capabilities);
			auto it = config.find("state_file");
			if (it != config.end()) {
				LOG_INFO << "(find) state_file is " << it->second;
				StopTickSimulation();

				AMM::SimulationControl simControl;
				auto ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
				simControl.timestamp(ms);
				simControl.type(AMM::ControlType::RESET);
				m_mgr->WriteSimulationControl(simControl);

				authoringMode = false;
				LOG_INFO << "Loading state.  Setting state file to " << it->second;
				std::string holdStateFile = stateFile;
				stateFile = "./states/" + it->second;
				std::ifstream infile(stateFile);
				if (!infile.good()) {
					LOG_ERROR << "State file does not exist: " << stateFile;
					stateFile = holdStateFile;
					LOG_ERROR << "Returning to last good state: " << stateFile;
				}
				infile.close();
				
				InitializeBiogears();
			}
		}
	}

/**
 * @brief parse the xml configuration file
 *
 * @param xmlConfig strin config file to be read by tinyxml
 */
	void PhysiologyEngineManager::ParseXML(std::string &xmlConfig) {
		LOG_INFO << "Loading XML config from string...";
		XMLDocument doc;
		doc.Parse(xmlConfig.c_str());
		XMLElement *root = doc.RootElement();

		if (strcasecmp(root->Value(), "AMMModuleConfiguration") == 0) {
			ReadCapabilities(root);
		} else {
			XMLElement *config = root->FirstChildElement("configuration_data");
			if (config != nullptr)
				ReadConfig(config);
			else
				LOG_WARNING << "No configuration_data found";
		}
	}

	void PhysiologyEngineManager::ReadConfig(XMLElement *_root) {
		// Grab the first data element
		XMLElement *node = _root->FirstChildElement("data");

		string tempName;
		string tempValue;

		while (node) {
			tempName = "";
			tempValue = "";

			if (node->Attribute("name"))
				tempName = node->Attribute("name");

			if (node->Attribute("value"))
				tempValue = node->Attribute("value");

			config[tempName] = tempValue;

			// move to the next node
			node = node->NextSiblingElement();
		}
	}

	void PhysiologyEngineManager::ReadCapabilities(XMLElement *_root) {
		XMLElement *ele = _root->FirstChildElement("capabilities")->FirstChildElement("capability");

		if (ele != nullptr) {
			const char *capEnabled = ele->Attribute("enabled");
			if (capEnabled != nullptr) {
				if (strcasecmp(capEnabled, "false") == 0) {
					LOG_INFO << "Module is explicitly disabled";
					moduleEnabled = false;
				} else {
					LOG_INFO << "Module is enabled";
					moduleEnabled = true;
				}
			}

			XMLElement *config = ele->FirstChildElement("configuration_data");
			if (config != nullptr)
				ReadConfig(config);
			else
				LOG_WARNING << "No configuration_data found";
		} else {
			LOG_WARNING << "No capabilities found.";
		}
	}

/**
 * @brief controls adancing time with the physiology engine
 *
 * @param ti idk tick object
 * @param info
 */
	void PhysiologyEngineManager::OnNewTick(AMM::Tick &ti, SampleInfo_t *info) {
		if (running) {
			if (ti.frame() > 0 || !paused) {
				m_pe->running = true;
				lastFrame = static_cast<int>(ti.frame());
				m_pe->SetLastFrame(lastFrame);
				// Per-frame stuff happens here
				try {
					AdvanceTimeTick();
					ProcessStates();
					PublishData(false);
				} catch (std::exception &e) {
					LOG_ERROR << "Unable to advance time: " << e.what();
				}
			} else {
				std::cout.flush();
			}
		}
	}

/**
 * @brief manages the instrument data with the physiology engine
 *
 * @param i instrument data as defined by the idl (ie ventilator..)
 * @param info
 */
	void PhysiologyEngineManager::OnNewInstrumentData(AMM::InstrumentData &i, SampleInfo_t *info) {
		LOG_DEBUG << "Instrument data for " << i.instrument() << " received with payload: " << i.payload();
		if (m_pe == nullptr || !running) {
			LOG_WARNING << "Physiology engine not running, cannot execute instrument data.";
			return;
		}
		std::string instrument(i.instrument());
		std::string payload = i.payload().to_string();
		if (instrument == "ventilator" || instrument == "erventilator") {
			m_pe->SetVentilator(payload);
		} else if (instrument == "bvm_mask") {
			m_pe->SetBVMMask(payload);
		} else if (instrument == "ivpump") {
			m_pe->SetIVPump(payload);
		}
	}
}
