#pragma once

#include <chrono>
#include <ctime>

#include <mutex>
#include <thread>

#include "amm_std.h"

#include "tinyxml2.h"

#include "BiogearsThread.h"

namespace AMM {
    class BiogearsThread;

    class PhysiologyEngineManager {
    public:
        PhysiologyEngineManager();

        virtual ~PhysiologyEngineManager();

        BiogearsThread *m_pe;
        std::string stateFile;

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

        bool paused = false;
        bool running = false;
        int lastFrame = 0;
        bool logging_enabled = false;

        void OnNewTick(AMM::Tick &ti, SampleInfo_t *info);

        void OnNewCommand(Command &cm, SampleInfo_t *info);

        void OnNewSimulationControl(SimulationControl &simControl, SampleInfo_t *info);

        void OnNewInstrumentData(InstrumentData &i, SampleInfo_t *info);

        void OnNewPhysiologyModification(AMM::PhysiologyModification &physMod, SampleInfo_t *info);

        std::map<std::string, double (BiogearsThread::*)()> *nodePathMap;

        std::string sysPrefix = "[SYS]";
        std::string loadPrefix = "LOAD_STATE:";
        std::string stateFilePrefix = "xml";

    protected:
        AMM::UUID m_uuid;
        std::string moduleName = "AMM_PhysiologyEngine";
        std::string configFile = "config/pe_manager_amm.xml";
        AMM::DDSManager<AMM::PhysiologyEngineManager> *m_mgr = new DDSManager<AMM::PhysiologyEngineManager>(configFile);

        std::mutex m_mutex;

    };
}
