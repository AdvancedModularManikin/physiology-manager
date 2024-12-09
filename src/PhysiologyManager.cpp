#include "amm/BaseLogger.h"
#include "AMM/PhysiologyEngineManager.h"



using namespace AMM;

bool closed = false;
int autostart = 0;
bool autosend = false;
bool logging = false;


static void show_usage(const std::string &name) {
   std::cerr << "Usage: " << name << " <option(s)>"
             << "\nOptions:\n"
             << "\t-a\t\tAuto-start based on ticks\n"
	     << "\t-s\t\tSend render mods on startup\n"
             << "\t-l\t\tEnable physiology CSV logging\n"
             << "\t-h,--help\t\tShow this help message\n"
             << std::endl;
}

void show_menu(AMM::PhysiologyEngineManager *pe) {
   std::string action;

   std::cout << std::endl;
   std::cout << " [1]Status" << std::endl;
   std::cout << " [2]Advance Time Tick" << std::endl;
   std::cout
      << " [3]Start\t\t\tRun physiology engine with simulation-manager ticks"
      << std::endl;
   std::cout << " [4]Stop\t\t\tStop running based on simulation-manager ticks"
             << std::endl;
   std::cout
      << " [5]Publish data\t\tPublish all data, right now (running or not)"
      << std::endl;
   std::cout << " [6]Toggle logging  (currently ";
   if (logging) {
      std::cout << "enabled";
   } else {
      std::cout << "disabled";
   }
   std::cout << ")" << std::endl;
   std::cout << " [7]Quit" << std::endl;
   std::cout << " [8]Test physmod XML" << std::endl;
   std::cout << " [9]Save state" << std::endl;
   std::cout << " >> ";
   getline(std::cin, action);
   transform(action.begin(), action.end(), action.begin(), ::toupper);

   if (action == "1") {
      pe->Status();
      if (pe->isRunning()) {
         std::cout << " == Running!  At tick count: ";
      } else {
         std::cout << " == Not currently running, paused at tick count: ";
      }
      std::cout << pe->GetTickCount() << std::endl;
   } else if (action == "2") {
      if (!pe->isRunning()) {
         std::cout << " == Simulation not running" << std::endl;
         return;
      }
      std::cout << " == Advancing time one tick" << std::endl;
      pe->AdvanceTimeTick();
   } else if (action == "3") {
      if (!pe->isRunning()) {
         std::cout << " == Starting simulation based on ticks..." << std::endl;
         pe->StartTickSimulation();
      } else {
         std::cout << " == Already running" << std::endl;
      }
   } else if (action == "4") {
      if (pe->isRunning()) {
         std::cout << " == Stopping simulation based on ticks..." << std::endl;
         pe->StopTickSimulation();
      } else {
         std::cout << " == Not running" << std::endl;
      }
   } else if (action == "5") {
      if (pe->isRunning()) {
         std::cout << " == Publishing all data" << std::endl;
         pe->PublishData(true);
         std::cout << " == Done publishing " << pe->GetNodePathCount() << " items." << std::endl;
      }
   } else if (action == "6") {
      logging = !logging;
      pe->SetLogging(logging);
   } else if (action == "7") {
      if (!pe->isRunning()) {
         std::cout << " == Simulation not running, but shutting down anyway" << std::endl;
      }
      pe->StopSimulation();
      pe->Shutdown();
      closed = true;
   } else if (action == "8") {
      if (!pe->isRunning()) {
         std::cout << " == Simulation not running" << std::endl;
         return;
      }
      std::string XML = // "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                        "<PhysiologyModification type=\"SubstanceBolus\">\n"
                        "  <Substance>Succinylcholine</Substance>\n"
                        "  <Concentration value=\"20\" unit=\"mg/mL\"/>\n"
                        "  <Dose value=\"5\" unit=\"mL\"/>\n"
                        "  <AdminRoute>Intravenous</AdminRoute>\n"
                        "</PhysiologyModification>";
      pe->ExecutePhysiologyModification(XML);
   } else if (action == "Z") {
       if (!pe->isRunning()) {
           std::cout << " == Simulation not running" << std::endl;
           return;
       }
       /** std::string XML = // "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
               "<PhysiologyModification type=\"SubstanceNasalDose\">\n"
               "  <Substance>Naloxone</Substance>\n"
               "  <Dose unit=\"mg\">1.6</Dose>\n"
               "</PhysiologyModification>"; */
	   std::string XML = "<PhysiologyModification type=\"NeedleDecompression\" State=\"On\" Side=\"Right\"/>";
       pe->ExecutePhysiologyModification(XML);
   } else if (action == "9") {
      pe->m_pe->SaveState("test.xml");
      return;
   } else if (action == "0") {
      pe->StopSimulation();
   } else if (action == "LIST") {
      pe->PrintAvailableNodePaths();
   } else if (action == "PRINT") {
      pe->PrintAllCurrentData();
   }
}

int main(int argc, char *argv[]) {
   static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
   plog::init(plog::verbose, &consoleAppender);

   for (int i = 1; i < argc; ++i) {
      std::string arg = argv[i];
      if ((arg == "-h") || (arg == "--help")) {
         show_usage(argv[0]);
         return 0;
      }

      if (arg == "-l") {
         logging = true;
      }

      if (arg == "-a") {
         autostart = 1;
      }

      if (arg == "-s") {
	autosend = true;
      }
   }

   auto *pe = new AMM::PhysiologyEngineManager();
   pe->SetLogging(logging);
   pe->SetAutosend(autosend);
   std::this_thread::sleep_for(std::chrono::milliseconds(250));

   pe->PublishOperationalDescription();
   pe->PublishConfiguration();

   if (autostart == 1) {
      LOG_INFO << "Physiology engine wrapper started.";
   }

   while (!closed) {
      if (autostart != 1) {
         show_menu(pe);
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      std::cout.flush();
   }

   pe->Shutdown();

   LOG_INFO << "Exiting.";

   return 0;
}
