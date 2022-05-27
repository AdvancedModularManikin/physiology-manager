#include "PhysiologyEngineManager.h"

using namespace std;
using namespace std::chrono;
using namespace tinyxml2;

std::string get_filename_date(void) {
    time_t now;
    char the_date[18];

    the_date[0] = '\0';

    now = time(NULL);

    if (now != -1) {
        strftime(the_date, 18, "%Y%m%d_%H%M%S", gmtime(&now));
    }

    return std::string(the_date);
}

std::map <std::string, std::string> config;

namespace AMM {
    PhysiologyEngineManager::PhysiologyEngineManager() {
        static plog::ColorConsoleAppender <plog::TxtFormatter> consoleAppender;


        stateFile = "./states/StandardMale@0s.xml";
        patientFile = "./patients/StandardMale.xml";

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

        m_uuid.id(m_mgr->GenerateUuidString());

        InitializeBiogears();
    }


    void PhysiologyEngineManager::PublishOperationalDescription() {
        AMM::OperationalDescription od;
        od.name(moduleName);
        od.model("Physiology Engine Manager");
        od.manufacturer("Vcom3D");
        od.serial_number("1.0.0");
        od.module_id(m_uuid);
        od.module_version("1.0.0");
        const std::string capabilities = Utility::read_file_to_string("config/pe_manager_capabilities.xml");
        od.capabilities_schema(capabilities);
        od.description();
        m_mgr->WriteOperationalDescription(od);
    }

    void PhysiologyEngineManager::PublishConfiguration() {
        AMM::ModuleConfiguration mc;
        uint64_t ms = static_cast<uint64_t>(duration_cast<milliseconds>(
                system_clock::now().time_since_epoch()).count());
        mc.timestamp(ms);
        mc.module_id(m_uuid);
        mc.name(moduleName);
        const std::string configuration = Utility::read_file_to_string("config/pe_manager_configuration.xml");
        mc.capabilities_configuration(configuration);
        m_mgr->WriteModuleConfiguration(mc);
    }

    PhysiologyEngineManager::~PhysiologyEngineManager() {
        if (m_pe != nullptr) {
            m_mutex.lock();
            m_pe->Shutdown();
            m_pe = nullptr;
            m_mutex.unlock();
        }
        m_mgr->Shutdown();
    }

    bool PhysiologyEngineManager::isRunning() { return running; }

    void PhysiologyEngineManager::SendShutdown() {

    }

    void PhysiologyEngineManager::PrintAvailableNodePaths() {
        nodePathMap = m_pe->GetNodePathTable();
        auto it = nodePathMap->begin();
        while (it != nodePathMap->end()) {
            std::string word = it->first;
            std::cout << word << std::endl;
            ++it;
        }
    }

    void PhysiologyEngineManager::PrintAllCurrentData() {
        nodePathMap = m_pe->GetNodePathTable();
        auto it = nodePathMap->begin();
        while (it != nodePathMap->end()) {
            std::string node = it->first;
            double dbl = m_pe->GetNodePath(node);
            std::cout << node << "\t\t\t" << dbl << std::endl;
            ++it;
        }
    }

    int PhysiologyEngineManager::GetNodePathCount() {
        return static_cast<int>(nodePathMap->size());
    }

    void PhysiologyEngineManager::WriteNodeData(std::string node) {
        AMM::PhysiologyValue dataInstance;
        try {
            dataInstance.name(node);
            dataInstance.value(m_pe->GetNodePath(node));
            m_mgr->WritePhysiologyValue(dataInstance);
        } catch (std::exception &e) {
            // LOG_ERROR << "Unable to write node data  " << node << ": " << e.what();
        }
    }

    void PhysiologyEngineManager::WriteHighFrequencyNodeData(std::string node) {
        AMM::PhysiologyWaveform dataInstance;
        try {
            dataInstance.name(node);
            dataInstance.value(m_pe->GetNodePath(node));
            m_mgr->WritePhysiologyWaveform(dataInstance);
        } catch (std::exception &e) {
            // LOG_ERROR << "Unable to write high frequency node data  " << node << ": " << e.what();
        }
    }

    void PhysiologyEngineManager::PublishData(bool force = false) {
        if (m_pe == nullptr || !running) {
            LOG_WARNING << "Physiology engine not running, cannot publish data.";
            return;
        }
        auto it = nodePathMap->begin();
        while (it != nodePathMap->end()) {
            if ((lastFrame % 10) == 0 || force) {
                WriteNodeData(it->first);
            }
            if ((std::find(m_pe->highFrequencyNodes.begin(), m_pe->highFrequencyNodes.end(), it->first) !=
                 m_pe->highFrequencyNodes.end())) {
                WriteHighFrequencyNodeData(it->first);
            }
            ++it;
        }
    }

    void PhysiologyEngineManager::
    ExecutePhysiologyModification(std::string pm) {
        if (m_pe == nullptr) {
            LOG_WARNING << "Physiology engine not running, cannot execute physiology modification.";
            return;
        }

        tinyxml2::XMLDocument doc;
        doc.Parse(pm.c_str());

        if (doc.ErrorID() == 0) {
            tinyxml2::XMLElement *pRoot;

            pRoot = doc.FirstChildElement("PhysiologyModification");

            while (pRoot) {
                std::string pmType = pRoot->ToElement()->Attribute("type");
                boost::algorithm::to_lower(pmType);
                LOG_INFO << "Physmod type " << pmType;

                if (pmType == "acutestress") {

                } else if (pmType == "airwayobstruction") {
                    double pSev = stod(pRoot->FirstChildElement("Severity")->ToElement()->GetText());
                    m_pe->SetAirwayObstruction(pSev);
                    return;
                } else if (pmType == "apnea") {
                } else if (pmType == "asthmaattack") {
                    double pSev = stod(pRoot->FirstChildElement("Severity")->ToElement()->GetText());
                    m_pe->SetAsthmaAttack(pSev);
                    return;
                } else if (pmType == "braininjury") {
                    double pSev = stod(pRoot->FirstChildElement("Severity")->ToElement()->GetText());
                    std::string pType = pRoot->FirstChildElement("Type")->ToElement()->GetText();
                    m_pe->SetBrainInjury(pSev, pType);
                    return;
                } else if (pmType == "bronchoconstriction") {
                } else if (pmType == "burn") {
                } else if (pmType == "bardiacarrest") {
                } else if (pmType == "chestcompression") {
                } else if (pmType == "consciousrespiration") {
                } else if (pmType == "consumenutrients") {
                } else if (pmType == "exercise") {
                } else if (pmType == "hemorrhage") {
                    std::string pLoc = pRoot->FirstChildElement("Location")->ToElement()->GetText();
                    tinyxml2::XMLElement *pFlow = pRoot->FirstChildElement("Flow")->ToElement();
                    double flow = stod(pFlow->GetText());
                    std::string flowUnit = pFlow->Attribute("unit");
                    m_pe->SetHemorrhage(pLoc, flow);
                    return;
                } else if (pmType == "infection") {
                } else if (pmType == "intubation") {
                } else if (pmType == "mechanicalventilation") {
                } else if (pmType == "needledecompression") {
                    std::string pLoc = pRoot->FirstChildElement("Location")->ToElement()->GetText();
                    m_pe->SetNeedleDecompression(pLoc);
                    return;
                } else if (pmType == "occlusivedressing") {
                } else if (pmType == "painstimulus") {
                    double pSev = stod(pRoot->FirstChildElement("Severity")->ToElement()->GetText());
                    std::string pLoc = pRoot->FirstChildElement("Location")->ToElement()->GetText();
                    m_pe->SetPain(pLoc, pSev);
                    return;
                } else if (pmType == "pericardialeffusion") {
                } else if (pmType == "sepsis") {
                    double pSev = stod(pRoot->FirstChildElement("Severity")->ToElement()->GetText());
                    std::string pLoc = pRoot->FirstChildElement("Location")->ToElement()->GetText();
                    m_pe->SetSepsis(pLoc, pSev);
                    return;
                } else if (pmType == "substancebolus") {
                    std::string pSub = pRoot->FirstChildElement("Substance")->ToElement()->GetText();

                    tinyxml2::XMLElement *pConc = pRoot->FirstChildElement("Concentration")->ToElement();
                    double concentration;
                    if (pConc->GetText() == NULL) {
                        concentration = stod(pConc->Attribute("value"));
                    } else {
                        concentration = stod(pConc->GetText());
                    }

                    std::string cUnit = pConc->Attribute("unit");

                    tinyxml2::XMLElement *pDose = pRoot->FirstChildElement("Dose")->ToElement();
                    double dose;
                    if (pDose->Attribute("value") != NULL) {
                        dose = stod(pDose->Attribute("value"));
                    } else {
                        dose = stod(pDose->GetText());
                    }
                    std::string dUnit = pDose->Attribute("unit");

                    tinyxml2::XMLElement *pAR = pRoot->FirstChildElement("AdminRoute")->ToElement();
                    std::string adminRoute = pAR->GetText();

                    m_pe->SetSubstanceBolus(pSub, concentration, cUnit, dose, dUnit, adminRoute);

                    return;
                } else if (pmType == "substancecompoundinfusion") {
                    std::string pSub = pRoot->FirstChildElement("SubstanceCompound")->ToElement()->GetText();

                    tinyxml2::XMLElement *pVol = pRoot->FirstChildElement("BagVolume")->ToElement();
                    double bagVolume;
                    if (pVol->Attribute("value") != NULL) {
                        bagVolume = stod(pVol->Attribute("value"));
                    } else {
                        bagVolume = stod(pVol->GetText());
                    }
                    std::string bvUnit = pVol->Attribute("unit");

                    tinyxml2::XMLElement *pRate = pRoot->FirstChildElement("Rate")->ToElement();
                    double rate;
                    if (pRate->Attribute("value") != NULL) {
                        rate = stod(pRate->Attribute("value"));
                    } else {
                        rate = stod(pRate->GetText());
                    }
                    std::string rUnit = pRate->Attribute("unit");

                    m_pe->SetSubstanceCompoundInfusion(pSub, bagVolume, bvUnit, rate, rUnit);
                    return;
                } else if (pmType == "substanceinfusion") {
                    std::string pSub = pRoot->FirstChildElement("Substance")->ToElement()->GetText();

                    tinyxml2::XMLElement *pConc = pRoot->FirstChildElement("Concentration")->ToElement();
                    double concentration;
                    if (pConc->Attribute("value") != NULL) {
                        concentration = stod(pConc->Attribute("value"));
                    } else {
                        concentration = stod(pConc->GetText());
                    }
                    std::string cUnit = pConc->Attribute("unit");

                    tinyxml2::XMLElement *pRate = pRoot->FirstChildElement("Rate")->ToElement();
                    double rate;
                    if (pRate->Attribute("value") != NULL) {
                        rate = stod(pRate->Attribute("value"));
                    } else {
                        rate = stod(pRate->GetText());
                    }
                    std::string rUnit = pRate->Attribute("unit");
                    m_pe->SetSubstanceInfusion(pSub, concentration, cUnit, rate, rUnit);
                    return;
                } else if (pmType == "substancenasaldose") {
                    std::string pSub = pRoot->FirstChildElement("Substance")->ToElement()->GetText();

                    tinyxml2::XMLElement *pDose = pRoot->FirstChildElement("Dose")->ToElement();
                    double dose;
                    if (pDose->Attribute("value") != NULL) {
                        dose = stod(pDose->Attribute("value"));
                    } else {
                        dose = stod(pDose->GetText());
                    }
                    std::string dUnit = pDose->Attribute("unit");

                    m_pe->SetSubstanceNasalDose(pSub, dose, dUnit);
                    return;
                } else if (pmType == "tensionpneumothorax") {
                } else if (pmType == "urinate") {
                } else {
                    LOG_INFO << "Unknown phys mod type: " << pmType;
                    return;
                }
            }
        } else {
            LOG_ERROR << "Document parsing error, ID: " << doc.ErrorID();
            doc.PrintError();
        }
    }

    void PhysiologyEngineManager::InitializeBiogears() {

        if (!running) {
            LOG_INFO << "Initializing Biogears thread";
            m_mutex.lock();
            m_pe = new BiogearsThread("logs/biogears.log");
            m_mutex.unlock();

            if (m_pe == nullptr) {
                LOG_WARNING << "Physiology engine not running, unable to start tick simulation.";
                return;
            }

            this->SetLogging(logging_enabled);

            if (authoringMode) {
                m_mutex.lock();
                if (m_pe->LoadPatient(patientFile.c_str())) {
                    LOG_INFO << "Patient loaded";
                }
                m_mutex.unlock();
            } else {
                std::size_t pos = stateFile.find("@");
                double startPosition;
                if (pos != std::string::npos) {
                    std::string state2 = stateFile.substr(pos);
                    std::size_t pos2 = state2.find("s");
                    std::string state3 = state2.substr(1, pos2 - 1);
                    startPosition = atof(state3.c_str());
                } else {
                    startPosition = 0;
                }

                m_mutex.lock();
                LOG_INFO << "Loading " << stateFile << " at " << startPosition;
                if (m_pe->LoadState(stateFile.c_str(), startPosition)) {
                    LOG_INFO << "State loaded.";
                }
                m_mutex.unlock();
            }
            nodePathMap = m_pe->GetNodePathTable();
        } else {
            LOG_ERROR << "Initialization failed because the sim is already running";
        }
        return;
    }

    void PhysiologyEngineManager::StartTickSimulation() {
        LOG_INFO << "Starting tick simulation";
        running = true;
        m_pe->running = true;
        paused = false;
    }

    void PhysiologyEngineManager::StopTickSimulation() {
        m_mutex.lock();
        paused = true;
        running = false;

        if (m_pe == nullptr) {
            LOG_WARNING << "Physiology engine not running, all other settings reset.";
            return;
        }

        LOG_INFO << "Deleting Physiology Engine thread";
        m_pe = nullptr;
        m_mutex.unlock();
        LOG_INFO << "Simulation stopped and reset.";
    }

    void PhysiologyEngineManager::StartSimulation() { m_pe->StartSimulation(); }

    void PhysiologyEngineManager::StopSimulation() { m_pe->StopSimulation(); }

    void PhysiologyEngineManager::ProcessStates() {
        if (m_pe->startOfInhale) {
            // LOG_DEBUG << "Start of inhale, sending render mod";
            AMM::RenderModification renderMod;
            renderMod.data("<RenderModification type='START_OF_INHALE'/>");
            m_mgr->WriteRenderModification(renderMod);
            m_pe->startOfInhale = false;
        } else if (m_pe->startOfExhale) {
            // LOG_DEBUG << "Start of exhale, sending render mod";
            AMM::RenderModification renderMod;
            renderMod.data("<RenderModification type='START_OF_EXHALE'/>");
            m_mgr->WriteRenderModification(renderMod);
            m_pe->startOfExhale = false;
        }

        if (m_pe->irreversible && !m_pe->irreversibleSent) {
            LOG_DEBUG << "Patient has entered an irreversible state, sending render mod.";

            AMM::UUID erID;
            erID.id(m_mgr->GenerateUuidString());

            FMA_Location fma;
            AMM::UUID agentID;

            AMM::EventRecord er;
            er.id(erID);
            er.location(fma);
            er.agent_id(agentID);
            er.type("PATIENT_STATE_IRREVERSIBLE");
            m_mgr->WriteEventRecord(er);

            AMM::RenderModification renderMod;
            renderMod.event_id(erID);
            renderMod.data("<RenderModification type='PATIENT_STATE_IRREVERSIBLE'/>");
            m_mgr->WriteRenderModification(renderMod);
            m_pe->irreversibleSent = true;
        }

        if (m_pe->paralyzed && !m_pe->paralyzedSent) {
            LOG_DEBUG << "Patient is paralyzed but we haven't sent the render mod.";
            AMM::UUID erID;
            erID.id(m_mgr->GenerateUuidString());

            FMA_Location fma;
            AMM::UUID agentID;

            AMM::EventRecord er;
            er.id(erID);
            er.location(fma);
            er.agent_id(agentID);
            er.type("PATIENT_STATE_PARALYZED");
            m_mgr->WriteEventRecord(er);

            AMM::RenderModification renderMod;
            renderMod.event_id(erID);
            renderMod.data("<RenderModification type='PATIENT_STATE_PARALYZED'/>");
            m_mgr->WriteRenderModification(renderMod);
            m_pe->paralyzedSent = true;
        }
    }

    void PhysiologyEngineManager::AdvanceTimeTick() {
        if (m_pe == nullptr || !running) {
            LOG_WARNING << "Physiology engine not running, cannot advance time.";
            return;
        }

        ProcessStates();

        m_mutex.lock();
        m_pe->AdvanceTimeTick();
        m_mutex.unlock();
    }

    void PhysiologyEngineManager::SetLogging(bool log) {
#ifdef _WIN32
        LOG_WARNING << "Unable to set logging on Windows systems.";
        return;
#endif

        logging_enabled = log;
        if (m_pe != nullptr) {
            m_mutex.lock();
            m_pe->SetLogging(logging_enabled);
            m_mutex.unlock();
        }
    }

    int PhysiologyEngineManager::GetTickCount() { return lastFrame; }

    void PhysiologyEngineManager::Status() {
        if (m_pe != nullptr) {
            return m_pe->Status();
        }
    }

    void PhysiologyEngineManager::Shutdown() {
        SendShutdown();

        LOG_DEBUG << "[PhysiologyManager] Shutting down physiology engine.";
        m_pe->Shutdown();
    }

// Listener events

    void PhysiologyEngineManager::OnNewPhysiologyModification(AMM::PhysiologyModification &pm, SampleInfo_t *info) {
        LOG_INFO << "Physiology modification received (type " << pm.type() << "): " << pm.data();
        if (m_pe == nullptr || !running) {
            LOG_WARNING << "Physiology engine not running, cannot execute physiology modification.";
            return;
        }

        // If the payload is empty, use the type to execute an XML file.
        // Otherwise, the payload is considered to be XML to execute.
        if (pm.data().empty()) {
            LOG_INFO << "Executing scenario file: " << pm.type();
            m_mutex.lock();
            m_pe->ExecuteCommand(pm.type());
            m_mutex.unlock();
            return;
        } else {
            if (pm.type().empty() || pm.type() == "biogears") {
                LOG_INFO << "Executing Biogears PhysMod XML patient action";
                m_mutex.lock();
                m_pe->ExecuteXMLCommand(pm.data());
                m_mutex.unlock();
                return;
            }
            LOG_INFO << "Executing AMM PhysMod XML patient action, type " << pm.type();
            try {
                ExecutePhysiologyModification(pm.data());
            }  catch (std::exception &e) {
                LOG_ERROR << "Unable to apply physiology modification: " << e.what();
            }
        }
    }

    void PhysiologyEngineManager::OnNewSimulationControl(AMM::SimulationControl &simControl, SampleInfo_t *info) {
        switch (simControl.type()) {
            case AMM::ControlType::RUN: {
                LOG_DEBUG << "SimControl recieved: Run sim.";
                if (!running) {
                    LOG_INFO << "Not running, calling starttick.";
                    StartTickSimulation();
                }
                break;
            }

            case AMM::ControlType::HALT: {
                LOG_DEBUG << "SimControl recieved: Halt sim";
                if (running) {
                    paused = true;
                }
                break;
            }


            case AMM::ControlType::RESET: {
                LOG_DEBUG << "SimControl recieved: Reset simulation, clearing engine data and preparing for next run.";
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
                LOG_DEBUG << "SimControl recieved: Save sim";
                if (m_pe != nullptr) {
                    std::ostringstream ss;
                    double simTime = m_pe->GetSimulationTime();
                    std::string filenamedate = get_filename_date();
                    ss << "SavedState_" << filenamedate << "@" << (int) std::round(simTime) << "s."
                       << stateFilePrefix;
                    LOG_INFO << "Saved state to " << ss.str();
                    m_mutex.lock();
                    m_pe->SaveState(ss.str());
                    m_mutex.unlock();
                } else {
                    LOG_ERROR << "Simulation has not been run, no state to save.";
                }

                break;
            }
        }
    }

    void PhysiologyEngineManager::OnNewCommand(Command &cm, SampleInfo_t *info) {
        if (!cm.message().compare(0, sysPrefix.size(), sysPrefix)) {
            std::string value = cm.message().substr(sysPrefix.size());
            if (value.compare("ENABLE_LOGGING") == 0) {
                LOG_DEBUG << "Enabling logging";
                this->SetLogging(true);
            } else if (value.compare("DISABLE_LOGGING") == 0) {
                LOG_DEBUG << "Disabling logging";
                this->SetLogging(false);
            } else if (!value.compare(0, loadPrefix.size(), loadPrefix)) {
                if (running || m_pe != nullptr) {
                    LOG_INFO << "Loading state, but shutting down existing sim and physiology engine thread first.";
                    StopTickSimulation();
                }

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
                InitializeBiogears();
            } else if (!value.compare(0, loadPatient.size(), loadPatient)) {
                if (running || m_pe != nullptr) {
                    LOG_INFO << "Loading patient, but shutting down existing sim and physiology engine thread first.";
                    StopTickSimulation();
                }

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
                InitializeBiogears();
            } else if (!value.compare(0, loadScenarioFile.size(), loadScenarioFile)) {
                if (running || m_pe != nullptr) {
                    LOG_INFO << "Loading state, but shutting down existing sim and physiology engine thread first.";
                    StopTickSimulation();
                }

                authoringMode = false;
                LOG_INFO << "Loading scenario.  Setting scenario file to " << value.substr(loadScenarioFile.size());
                scenarioFile = "./Scenarios/" + value.substr(loadScenarioFile.size()) + ".xml";
                LOG_INFO << "Scenario file is " << scenarioFile;
                std::ifstream infile(scenarioFile);
                if (!infile.good()) {
                    LOG_ERROR << "Scenario file does not exist: " << scenarioFile;
                }
                infile.close();

                LOG_INFO << "Initializing Biogears thread to call LoadScenarioFile";
                m_mutex.lock();
                m_pe = new BiogearsThread("logs/biogears.log");
                m_mutex.unlock();

                if (m_pe == nullptr) {
                    LOG_WARNING << "Physiology engine not running, unable to start tick simulation.";
                    return;
                }

                this->SetLogging(logging_enabled);

                m_pe->scenarioLoading = true;
                m_pe->LoadScenarioFile(scenarioFile);
                m_pe->scenarioLoading = false;

                nodePathMap = m_pe->GetNodePathTable();

                //		running = true;
                //		m_pe->running = true;
                paused = true;

            } else {
                LOG_DEBUG << "Unknown system command received: " << cm.message();
            }
        } else {
            LOG_DEBUG << "Unknown command received: " << cm.message();
        }
    }

    void PhysiologyEngineManager::OnNewModuleConfiguration(AMM::ModuleConfiguration &mc, SampleInfo_t *info) {
        if (mc.name() == "physiology_engine") {
            LOG_DEBUG << "Entering ModuleConfiguration for physiology engine.";
            ParseXML(mc.capabilities_configuration());
            auto it = config.find("state_file");
            if (it != config.end()) {
                LOG_INFO << "(find) state_file is " << it->second;
                StopTickSimulation();
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
            else LOG_WARNING << "No configuration_data found";
        }
    }

    void PhysiologyEngineManager::ReadConfig(XMLElement *_root) {
        // Grab the first data element
        XMLElement *node = _root->FirstChildElement("data");

        string tempName = "";
        string tempValue = "";

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
            // const char *capabilityType = ele->Attribute("type");
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
            else LOG_WARNING << "No configuration_data found";
        } else {
            LOG_WARNING << "No capabilities found.";
        }
    }

    void PhysiologyEngineManager::OnNewTick(AMM::Tick &ti, SampleInfo_t *info) {
        if (running) {
            if (ti.frame() > 0 || !paused) {
                m_pe->running = true;
                lastFrame = static_cast<int>(ti.frame());
                m_pe->SetLastFrame(lastFrame);
                // Per-frame stuff happens here
                try {
                    AdvanceTimeTick();
                    PublishData(false);
                } catch (std::exception &e) {
                    LOG_ERROR << "Unable to advance time: " << e.what();
                }
            } else {
                std::cout.flush();
            }
        }
    }

    void PhysiologyEngineManager::OnNewInstrumentData(AMM::InstrumentData &i, SampleInfo_t *info) {
        LOG_DEBUG << "Instrument data for " << i.instrument() << " received with payload: " << i.payload();
        if (m_pe == nullptr || !running) {
            LOG_WARNING << "Physiology engine not running, cannot execute instrument data.";
            return;
        }
        std::string instrument(i.instrument());
        m_mutex.lock();
        if (instrument == "ventilator" || instrument == "erventilator") {
            m_pe->SetVentilator(i.payload());
        } else if (instrument == "bvm_mask") {
            m_pe->SetBVMMask(i.payload());
        } else if (instrument == "ivpump") {
            m_pe->SetIVPump(i.payload());
        }
        m_mutex.unlock();
    }
}


