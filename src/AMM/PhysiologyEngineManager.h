#pragma once

#include <chrono>
#include <ctime>

#include <mutex>
#include <thread>

#include "amm_std.h"

#include "tinyxml2.h"

#include "BiogearsThread.h"

using namespace tinyxml2;

namespace AMM {
    class BiogearsThread;

    class PhysiologyEngineManager {
    public:
        PhysiologyEngineManager();

        virtual ~PhysiologyEngineManager();

        BiogearsThread *m_pe;
        std::string stateFile;
        std::string patientFile;
        std::string scenarioFile;
        bool authoringMode = false;

        void PublishOperationalDescription();
        void PublishConfiguration();

        void SetLogging(bool logging_enabled);

        void StartSimulation();

        void StopSimulation();

        void Shutdown();

        void StartTickSimulation();

        void StopTickSimulation();

        void ExecutePhysiologyModification(std::string pm);

        void PublishData(bool force);

        void PrintAvailableNodePaths();

        void PrintAllCurrentData();

        void Status();

        int GetNodePathCount();

        int GetTickCount();

        bool isRunning();

        void SendShutdown();

        void WriteNodeData(std::string node);

        void WriteHighFrequencyNodeData(std::string node);

        void AdvanceTimeTick();

        void InitializeBiogears();

        void SendPatientStateRendMod(std::string rendModType);

        void ProcessStates();

        bool paused = false;
        bool running = false;
        int lastFrame = 0;
        bool logging_enabled = false;
        bool moduleEnabled = true;

        void OnNewModuleConfiguration(AMM::ModuleConfiguration &mc, SampleInfo_t *info);

        void ParseXML(std::string &xmlConfig);

        void ReadConfig(XMLElement *_root);

        void ReadCapabilities(XMLElement *_root);

        void OnNewTick(AMM::Tick &ti, SampleInfo_t *info);

        void OnNewCommand(Command &cm, SampleInfo_t *info);

        void OnNewSimulationControl(SimulationControl &simControl, SampleInfo_t *info);

        void OnNewInstrumentData(InstrumentData &i, SampleInfo_t *info);

        void OnNewPhysiologyModification(AMM::PhysiologyModification &physMod, SampleInfo_t *info);

        std::map<std::string, double (BiogearsThread::*)()> *nodePathMap;

        std::string sysPrefix = "[SYS]";
        std::string loadPrefix = "LOAD_STATE:";
        std::string loadPatient = "LOAD_PATIENT:";
        std::string loadScenarioFile = "LOAD_SCENARIOFILE:";
        std::string stateFilePrefix = "xml";
        std::string patientFilePrefix = "xml";

    protected:
        AMM::UUID m_uuid;
        std::string moduleName = "AMM_PhysiologyEngine";
        std::string configFile = "config/pe_manager_amm.xml";
        AMM::DDSManager<AMM::PhysiologyEngineManager> *m_mgr = new DDSManager<AMM::PhysiologyEngineManager>(configFile);

        std::mutex m_mutex;

    };
}
