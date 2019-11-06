#include "PhysiologyEngineManager.h"

using namespace std;
using namespace std::chrono;

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

namespace AMM {
    PhysiologyEngineManager::PhysiologyEngineManager() {

       static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
       //static plog::DDS_Log_Appender<plog::TxtFormatter> DDSAppender(mgr);
       //plog::init(plog::verbose, &consoleAppender).addAppender(&DDSAppender);

       if (bg == nullptr) {
          LOG_WARNING << "BioGears thread not initialized.";
       }

       stateFile = "./states/StandardMale@0s.xml";

       m_mgr->InitializeTick();
       m_mgr->InitializeCommand();
       m_mgr->InitializeInstrumentData();
       m_mgr->InitializeSimulationControl();
       m_mgr->InitializePhysiologyModification();

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

       m_mgr->CreateTickSubscriber(this, &AMM::PhysiologyEngineManager::OnNewTick);
       m_mgr->CreateSimulationControlSubscriber(this, &AMM::PhysiologyEngineManager::OnNewSimulationControl);
       m_mgr->CreateCommandSubscriber(this, &AMM::PhysiologyEngineManager::OnNewCommand);
       m_mgr->CreatePhysiologyModificationSubscriber(this, &AMM::PhysiologyEngineManager::OnNewPhysiologyModification);
       m_mgr->CreateInstrumentDataSubscriber(this, &AMM::PhysiologyEngineManager::OnNewInstrumentData);

       m_uuid.id(m_mgr->GenerateUuidString());
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
       auto ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
       mc.timestamp(ms);
       mc.module_id(m_uuid);
       mc.name(moduleName);
       const std::string configuration = Utility::read_file_to_string("config/pe_manager_configuration.xml");
       mc.capabilities_configuration(configuration);
       m_mgr->WriteModuleConfiguration(mc);
    }

    PhysiologyEngineManager::~PhysiologyEngineManager() {
       if (bg != nullptr) {
          bg->Shutdown();
       }
       m_mgr->Shutdown();
    }

    bool PhysiologyEngineManager::isRunning() { return running; }

    void PhysiologyEngineManager::TickLoop() {
       while (running) {
          std::this_thread::sleep_for(std::chrono::milliseconds(200));
       }
    }

    void PhysiologyEngineManager::SendShutdown() {

    }

    void PhysiologyEngineManager::PrintAvailableNodePaths() {
       nodePathMap = bg->GetNodePathTable();
       auto it = nodePathMap->begin();
       while (it != nodePathMap->end()) {
          std::string word = it->first;
          std::cout << word << std::endl;
          ++it;
       }
    }

    void PhysiologyEngineManager::PrintAllCurrentData() {
       nodePathMap = bg->GetNodePathTable();
       auto it = nodePathMap->begin();
       while (it != nodePathMap->end()) {
          std::string node = it->first;
          double dbl = bg->GetNodePath(node);
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
          dataInstance.value(bg->GetNodePath(node));
          m_mgr->WritePhysiologyValue(dataInstance);
       } catch (std::exception &e) {
          // LOG_ERROR << "Unable to write node data  " << node << ": " << e.what();
       }
    }

    void PhysiologyEngineManager::WriteHighFrequencyNodeData(std::string node) {
       AMM::PhysiologyWaveform dataInstance;
       try {
          dataInstance.name(node);
          dataInstance.value(bg->GetNodePath(node));
          m_mgr->WritePhysiologyWaveform(dataInstance);
       } catch (std::exception &e) {
          // LOG_ERROR << "Unable to write high frequency node data  " << node << ": " << e.what();
       }
    }

    void PhysiologyEngineManager::PublishData(bool force = false) {
       auto it = nodePathMap->begin();
       while (it != nodePathMap->end()) {
          if ((lastFrame % 10) == 0 || force) {
             WriteNodeData(it->first);
          }
          if ((std::find(bg->highFrequencyNodes.begin(), bg->highFrequencyNodes.end(), it->first) !=
               bg->highFrequencyNodes.end())) {
             WriteHighFrequencyNodeData(it->first);
          }
          ++it;
       }
    }

    void PhysiologyEngineManager::StartTickSimulation() {
       std::size_t pos = stateFile.find("@");
       std::string state2 = stateFile.substr(pos);
       std::size_t pos2 = state2.find("s");
       std::string state3 = state2.substr(1, pos2 - 1);
       double startPosition = atof(state3.c_str());

       if (!running) {
          LOG_INFO << "Initializing Biogears thread";
          this->SetLogging(logging_enabled);
          bg = new BiogearsThread("logs/biogears.log");
          LOG_INFO << "Loading " << stateFile << " at " << startPosition;
          if (bg->LoadState(stateFile.c_str(), startPosition)) {
             running = true;
          }

          nodePathMap = bg->GetNodePathTable();
       }

       paused = false;
    }

    void PhysiologyEngineManager::StopTickSimulation() {
       if (running) {
          m_mutex.lock();
          running = false;
          paused = false;
          std::this_thread::sleep_for(std::chrono::milliseconds(200));
          bg->Shutdown();
          m_mutex.unlock();

          delete bg;
       }
    }

    void PhysiologyEngineManager::StartSimulation() { bg->StartSimulation(); }

    void PhysiologyEngineManager::StopSimulation() { bg->StopSimulation(); }

    void PhysiologyEngineManager::AdvanceTimeTick() { bg->AdvanceTimeTick(); }

    void PhysiologyEngineManager::SetLogging(bool log) {
#ifdef _WIN32
       return;
#endif

       logging_enabled = log;
       if (bg != nullptr) {
          m_mutex.lock();
          bg->SetLogging(logging_enabled);
          m_mutex.unlock();
       }
    }

    int PhysiologyEngineManager::GetTickCount() { return lastFrame; }

    void PhysiologyEngineManager::Status() { bg->Status(); }

    void PhysiologyEngineManager::Shutdown() {
       SendShutdown();

       LOG_DEBUG << "[PhysiologyManager][BG] Shutting down BioGears.";
       bg->Shutdown();
    }

// Listener events

    void PhysiologyEngineManager::OnNewPhysiologyModification(AMM::PhysiologyModification &pm, SampleInfo_t *info) {
       // If the payload is empty, use the type to execute an XML file.
       // Otherwise, the payload is considered to be XML to execute.
       if (pm.type() == "pain") {
          LOG_INFO << "Pain payload received: " << pm.data();
          m_mutex.lock();
          bg->SetPain(pm.data());
          m_mutex.unlock();
       } else if (pm.type() == "hemorrhage") {
          LOG_INFO << "Hemorrhage payload received: " << pm.data();
          m_mutex.lock();
          bg->SetHemorrhage("", pm.data());
          m_mutex.unlock();
       } else {
          LOG_INFO << "Physiology modification received (type " << pm.type() << "): " << pm.data();
          bg->ExecuteXMLCommand(pm.data());
       }
    }

    void PhysiologyEngineManager::OnNewSimulationControl(AMM::SimulationControl &simControl, SampleInfo_t *info) {
       LOG_INFO << "Received simulation control message";
       switch (simControl.type()) {
          case AMM::ControlType::RUN: {
             LOG_DEBUG << "Message recieved; Run sim.";
             StartTickSimulation();
             break;
          }

          case AMM::ControlType::HALT: {
             LOG_DEBUG << "Message recieved; Halt sim";
             paused = true;
             break;
          }


          case AMM::ControlType::RESET: {
             LOG_DEBUG << "Reset simulation, clearing engine data and preparing for next run.";
             StopTickSimulation();
             running = false;
             paused = false;
             break;
          }

          case AMM::ControlType::SAVE: {
             LOG_INFO << "Message recieved; Save sim";
             std::ostringstream ss;
             double simTime = bg->GetSimulationTime();
             std::string filenamedate = get_filename_date();
             ss << "./states/SavedState_" << filenamedate << "@" << (int) std::round(simTime) << "s.xml";
             LOG_INFO << "Saved state to " << ss.str();
             bg->SaveState(ss.str());
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
             StopTickSimulation();
             stateFile = "./states/" + value.substr(loadPrefix.size()) + ".xml";
          } else {
             LOG_DEBUG << "Unknown system command received: " << cm.message();
          }
       } else {
          LOG_DEBUG << "Unknown command received: " << cm.message();
       }
    }

    void PhysiologyEngineManager::OnNewTick(AMM::Tick &ti, SampleInfo_t *info) {
       if (running) {
          if (ti.frame() > 0 || !paused) {
             lastFrame = static_cast<int>(ti.frame());
             bg->SetLastFrame(lastFrame);
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
       std::string instrument(i.instrument());
       m_mutex.lock();
       if (instrument == "ventilator") {
          bg->SetVentilator(i.payload());
       } else if (instrument == "bvm_mask") {
          bg->SetBVMMask(i.payload());
       } else if (instrument == "ivpump") {
          bg->SetIVPump(i.payload());
       }
       m_mutex.unlock();
    }
}


