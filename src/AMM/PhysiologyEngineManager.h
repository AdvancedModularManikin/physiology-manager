#pragma once

#include <chrono>
#include <time.h>

#include <mutex>
#include <thread>

#include "amm_std.h"

#include "BiogearsThread.h"

namespace AMM {
    class BiogearsThread;

    class PhysiologyEngineManager {
    public:
        PhysiologyEngineManager();

        virtual ~PhysiologyEngineManager();

        BiogearsThread *bg;
        std::string stateFile;

        void PublishOperationalDescription();
        void PublishConfiguration();

        void SetLogging(bool logging_enabled);

        void StartSimulation();

        void StopSimulation();

        void Shutdown();

        void StartTickSimulation();

        void StopTickSimulation();

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

        void TickLoop();

        void AdvanceTimeTick();

        bool closed = false;
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
        std::string actPrefix = "[ACT]";
        std::string loadPrefix = "LOAD_STATE:";
        std::string loadScenarioPrefix = "LOAD_SCENARIO:";

    protected:
        AMM::UUID m_uuid;
        std::string moduleName = "AMM_PhysiologyEngine";
        std::string configFile = "config/pe_manager_amm.xml";
        AMM::DDSManager<AMM::PhysiologyEngineManager> *m_mgr = new DDSManager<AMM::PhysiologyEngineManager>(configFile);

        std::mutex m_mutex;

    };
}
