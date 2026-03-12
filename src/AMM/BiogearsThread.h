#pragma once

#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <memory>

#include <ctime>
#include <fstream>
#include <iostream>

#include <sstream>
#include <stdexcept>
#include <thread>
#include <unordered_set>

// #include "amm_std.h"

#include "amm/BaseLogger.h"

// Boost dependencies
#include <boost/algorithm/string.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/exception/all.hpp>
#include <boost/filesystem.hpp>

// BioGears core
#include <biogears/cdm/CommonDataModel.h>
#include <biogears/cdm/scenario/SEActionManager.h>
#include <biogears/cdm/scenario/SEPatientActionCollection.h>
#include <biogears/engine/BioGearsPhysiologyEngine.h>
#include <biogears/engine/Controller/BioGears.h>
#include <biogears/engine/Controller/BioGearsEngine.h>

#include <biogears/cdm/compartment/SECompartmentManager.h>
#include <biogears/cdm/compartment/fluid/SEGasCompartment.h>
#include <biogears/cdm/compartment/fluid/SELiquidCompartment.h>
#include <biogears/cdm/patient/SEPatient.h>
#include <biogears/cdm/system/physiology/SEBloodChemistrySystem.h>
#include <biogears/cdm/system/physiology/SECardiovascularSystem.h>
#include <biogears/cdm/system/physiology/SEEnergySystem.h>
#include <biogears/cdm/system/physiology/SERespiratorySystem.h>

#include <biogears/cdm/system/physiology/SERenalSystem.h>

#include <biogears/cdm/Serializer.h>
#include <biogears/cdm/engine/PhysiologyEngineTrack.h>
#include <biogears/cdm/properties/SEScalarTime.h>
#include <biogears/cdm/scenario/SEAdvanceTime.h>
#include <biogears/cdm/scenario/SEScenario.h>
#include <biogears/cdm/scenario/SEScenarioInitialParameters.h>

#include <biogears/cdm/compartment/SECompartmentManager.h>
#include <biogears/cdm/engine/PhysiologyEngineTrack.h>
#include <biogears/cdm/patient/actions/SEAirwayObstruction.h>
#include <biogears/cdm/patient/actions/SEAsthmaAttack.h>
#include <biogears/cdm/patient/actions/SEBrainInjury.h>
#include <biogears/cdm/patient/actions/SEHemorrhage.h>
#include <biogears/cdm/patient/actions/SENasalCannula.h>
#include <biogears/cdm/patient/actions/SEPainStimulus.h>
#include <biogears/cdm/patient/actions/SEPupillaryResponse.h>
#include <biogears/cdm/patient/actions/SESubstanceBolus.h>

#include <biogears/cdm/substance/SESubstanceManager.h>
#include <biogears/cdm/system/physiology/SEBloodChemistrySystem.h>
#include <biogears/cdm/system/physiology/SECardiovascularSystem.h>
#include <biogears/cdm/system/physiology/SEDrugSystem.h>
#include <biogears/cdm/system/physiology/SENervousSystem.h>

#include <biogears/cdm/substance/SESubstance.h>
#include <biogears/cdm/substance/SESubstanceManager.h>

#include <biogears/cdm/engine/PhysiologyEngineTrack.h>
#include <biogears/cdm/scenario/SEScenarioExec.h>
#include <biogears/cdm/utils/SEEventHandler.h>

#include <biogears/cdm/properties/SEFunctionVolumeVsTime.h>
#include <biogears/cdm/properties/SEScalarAmountPerVolume.h>
#include <biogears/cdm/properties/SEScalarFlowResistance.h>
#include <biogears/cdm/properties/SEScalarFraction.h>
#include <biogears/cdm/properties/SEScalarFrequency.h>
#include <biogears/cdm/properties/SEScalarLength.h>
#include <biogears/cdm/properties/SEScalarMass.h>
#include <biogears/cdm/properties/SEScalarMassPerVolume.h>
#include <biogears/cdm/properties/SEScalarOsmolality.h>
#include <biogears/cdm/properties/SEScalarOsmolarity.h>
#include <biogears/cdm/properties/SEScalarPressure.h>
#include <biogears/cdm/properties/SEScalarTemperature.h>
#include <biogears/cdm/properties/SEScalarTime.h>
#include <biogears/cdm/properties/SEScalarTypes.h>
#include <biogears/cdm/properties/SEScalarVolume.h>
#include <biogears/cdm/properties/SEScalarVolumePerTime.h>

#include <biogears/cdm/patient/actions/SEPainStimulus.h>
#include <biogears/cdm/patient/actions/SESubstanceBolus.h>
#include <biogears/cdm/patient/actions/SESubstanceCompoundInfusion.h>
#include <biogears/cdm/patient/actions/SESubstanceInfusion.h>
#include <biogears/cdm/patient/actions/SESubstanceNasalDose.h>
#include <biogears/cdm/patient/assessments/SEArterialBloodGasAnalysis.h>
#include <biogears/cdm/patient/assessments/SECompleteBloodCount.h>
#include <biogears/cdm/patient/assessments/SEComprehensiveMetabolicPanel.h>
#include <biogears/cdm/patient/assessments/SEPulmonaryFunctionTest.h>
#include <biogears/cdm/patient/assessments/SEUrinalysis.h>
#include <biogears/cdm/substance/SESubstanceCompound.h>
#include <biogears/cdm/system/physiology/SEDrugSystem.h>
#include <biogears/cdm/system/physiology/SEEnergySystem.h>

#include <biogears/cdm/system/equipment/ElectroCardioGram/SEElectroCardioGram.h>

#include <biogears/cdm/system/equipment/Anesthesia/SEAnesthesiaMachine.h>
#include <biogears/cdm/system/equipment/Anesthesia/SEAnesthesiaMachineOxygenBottle.h>
#include <biogears/cdm/system/equipment/Anesthesia/actions/SEAnesthesiaMachineConfiguration.h>
#include <biogears/cdm/system/equipment/Anesthesia/actions/SEMaskLeak.h>
#include <biogears/cdm/system/equipment/Anesthesia/actions/SEOxygenWallPortPressureLoss.h>

#include <biogears/cdm/scenario/SEAdvanceTime.h>
#include <biogears/cdm/scenario/SEScenario.h>
#include <biogears/cdm/scenario/SEScenarioExec.h>

#include "amm/Utility.h"

#include "MoHSES_EventHandler.h"

class CustomEventHandler : public biogears::SEEventHandler {
private:
	biogears::Logger* m_Logger;

public:
	CustomEventHandler()
			: SEEventHandler()
	{
		std::cout << "Constructed Event Handler" << std::endl;
	}
	virtual void HandlePatientEvent(biogears::SEPatientEventType type, bool active, const biogears::SEScalarTime* time = nullptr)
	{

		static int event_count = 0;
		//if ( event_count++ % 31 != 0){
		//	return ;
		//}

		std::cout << "\n";
		std::cout << "  OO O o o o...      _______________________________________ \n";
		std::cout << "  O     ____          |                                      |\n";

		std::string event;
		std::string marquee_1 = "                                      ";
		std::string marquee_2 = "                                      ";
		std::string prefix = " ][_n_i_| (   ooo___  |";
		std::string end = "|\n";

		switch (type) {
			case biogears::SEPatientEventType::AcuteLungInjury:
				event = "AcuteLungInjury";
				break;
			case biogears::SEPatientEventType::AcuteRespiratoryDistress:
				event = "AcuteRespiratoryDistress";
				break;
			case biogears::SEPatientEventType::Antidiuresis:
				event = "Antidiuresis";
				break;
			case biogears::SEPatientEventType::Asystole:
				event = "Asystoley";
				break;
			case biogears::SEPatientEventType::Bradycardia:
				event = "Bradycardia";
				break;
			case biogears::SEPatientEventType::Bradypnea:
				event = "Bradypnea";
				break;
			case biogears::SEPatientEventType::BrainOxygenDeficit:
				event = "BrainOxygenDeficit";
				break;
			case biogears::SEPatientEventType::CardiacArrest:
				event = "CardiacArrest";
				break;
			case biogears::SEPatientEventType::CardiogenicShock:
				event = "CardiogenicShock";
				break;
			case biogears::SEPatientEventType::CriticalBrainOxygenDeficit:
				event = "CriticalBrainOxygenDeficit";
				break;
			case biogears::SEPatientEventType::Dehydration:
				event = "Dehydration";
				break;
			case biogears::SEPatientEventType::Diuresis:
				event = "AcuteLungInjury";
				break;
			case biogears::SEPatientEventType::Fasciculation:
				event = "Fasciculation";
				break;
			case biogears::SEPatientEventType::Fatigue:
				event = "Fatigue";
				break;
			case biogears::SEPatientEventType::FunctionalIncontinence:
				event = "FunctionalIncontinence";
				break;
			case biogears::SEPatientEventType::HemolyticTransfusionReaction:
				event = "HemolyticTransfusionReaction";
				break;
			case biogears::SEPatientEventType::Hypercapnia:
				event = "Hypercapnia";
				break;
			case biogears::SEPatientEventType::Hyperglycemia:
				event = "Hyperglycemia";
				break;
			case biogears::SEPatientEventType::MildHyperkalemia:
				event = "MildHyperkalemia";
				break;
			case biogears::SEPatientEventType::SevereHyperkalemia:
				event = "SevereHyperkalemia";
				break;
			case biogears::SEPatientEventType::MildHypernatremia:
				event = "MildHypernatremia";
				break;
			case biogears::SEPatientEventType::SevereHypernatremia:
				event = "SevereHypernatremia";
				break;
			case biogears::SEPatientEventType::Hyperthermia:
				event = "Hyperthermia";
				break;
			case biogears::SEPatientEventType::Hypoglycemia:
				event = "Hypoglycemia";
				break;
			case biogears::SEPatientEventType::HypoglycemicShock:
				event = "HypoglycemicShock";
				break;
			case biogears::SEPatientEventType::HypoglycemicComa:
				event = "HypoglycemicComa";
				break;
			case biogears::SEPatientEventType::MildHypothermia:
				event = "MildHypothermia";
				break;
			case biogears::SEPatientEventType::MildHypokalemia:
				event = "MildHypokalemia";
				break;
			case biogears::SEPatientEventType::SevereHypokalemia:
				event = "SevereHypokalemia";
				break;
			case biogears::SEPatientEventType::MildHyponatremia:
				event = "MildHyponatremia";
				break;
			case biogears::SEPatientEventType::SevereHyponatremia:
				event = "SevereHyponatremia";
				break;
			case biogears::SEPatientEventType::Hypoxia:
				event = "Hypoxia";
				break;
			case biogears::SEPatientEventType::HypovolemicShock:
				event = "HypovolemicShock";
				break;
			case biogears::SEPatientEventType::IntracranialHypertension:
				event = "IntracranialHypertension";
				break;
			case biogears::SEPatientEventType::IntracranialHypotension:
				event = "IntracranialHypotension";
				break;
			case biogears::SEPatientEventType::IrreversibleState:
				event = "IrreversibleState";
				break;
			case biogears::SEPatientEventType::Ketoacidosis:
				event = "Ketoacidosis";
				break;
			case biogears::SEPatientEventType::LacticAcidosis:
				event = "LacticAcidosis";
				break;
			case biogears::SEPatientEventType::LiverGlycogenDepleted:
				event = "LiverGlycogenDepleted";
				break;
			case biogears::SEPatientEventType::MaximumPulmonaryVentilationRate:
				event = "MaximumPulmonaryVentilationRate";
				break;
			case biogears::SEPatientEventType::MetabolicAcidosis:
				event = "MetabolicAcidosis";
				break;
			case biogears::SEPatientEventType::MetabolicAlkalosis:
				event = "MetabolicAlkalosis";
				break;
			case biogears::SEPatientEventType::MuscleCatabolism:
				event = "MuscleCatabolism";
				break;
			case biogears::SEPatientEventType::MuscleGlycogenDepleted:
				event = "MuscleGlycogenDepleted";
				break;
			case biogears::SEPatientEventType::MyocardiumOxygenDeficit:
				event = "MyocardiumOxygenDeficit";
				break;
			case biogears::SEPatientEventType::Natriuresis:
				event = "Natriuresis";
				break;
			case biogears::SEPatientEventType::NutritionDepleted:
				event = "NutritionDepleted";
				break;
			case biogears::SEPatientEventType::PulselessRhythm:
				event = "PulselessRhythm";
				break;
			case biogears::SEPatientEventType::RenalHypoperfusion:
				event = "RenalHypoperfusion";
				break;
			case biogears::SEPatientEventType::RespiratoryAcidosis:
				event = "RespiratoryAcidosis";
				break;
			case biogears::SEPatientEventType::RespiratoryAlkalosis:
				event = "RespiratoryAlkalosis";
				break;
			case biogears::SEPatientEventType::SevereAcuteRespiratoryDistress:
				event = "SevereAcuteRespiratoryDistress";
				break;
			case biogears::SEPatientEventType::StartOfCardiacCycle:
				event = "AcuteLungInjury";
				break;
			case biogears::SEPatientEventType::StartOfExhale:
				event = "StartOfExhale";
				break;
			case biogears::SEPatientEventType::StartOfInhale:
				event = "StartOfInhale";
				break;
			case biogears::SEPatientEventType::SevereSepsis:
				event = "SevereSepsis";
				break;
			case biogears::SEPatientEventType::Tachycardia:
				event = "Tachycardia";
				break;
			case biogears::SEPatientEventType::Tachypnea:
				event = "Tachypnea";
				break;
			case biogears::SEPatientEventType::_TotalPatientEvents:
				event = "TotalPatientEvents";
				break;
			default:
				event = "Unknown";
				break;
		}
		std::string state = (active) ? "(On)" : "(Off)";
		if (event.size() < marquee_1.size()) {
			marquee_1.replace(marquee_1.begin() + (marquee_1.size() / 2) - (event.size() / 2),
			                  marquee_1.begin() + (marquee_1.size() / 2) + (event.size() / 2) + (state.size() % 2),
			                  event.begin(), event.end());
		} else {
			marquee_1 = event;
		}
		if (state.size() < marquee_2.size()) {
			marquee_2.replace(marquee_2.begin() + (marquee_2.size() / 2) - (state.size() / 2),
			                  marquee_2.begin() + (marquee_2.size() / 2) + (state.size() / 2) + (state.size() % 2),
			                  state.begin(), state.end());
		} else {
			marquee_1 = state;
		}
		std::cout << prefix << marquee_1 << end;
		std::cout << prefix << marquee_2 << end;
		std::cout << "(__________|_[______]_|______________________________________|\n";
		std::cout << "  0--0--0      0  0      0       0     0        0        0    \n";
		std::cout << std::endl;
	}
	virtual void HandleAnesthesiaMachineEvent(biogears::SEAnesthesiaMachineEvent type, bool active, const biogears::SEScalarTime* time = nullptr)
	{
	}
};

// Forward declare what we will use in our thread
namespace AMM {

	class BiogearsThread {

	public:
		explicit BiogearsThread(const std::string &stateFile);

		~BiogearsThread();

		bool PostLoad();

		bool LoadScenarioFile(const std::string &scenarioFile);

		bool LoadPatient(const std::string &patientFile);

		bool LoadState(const std::string &stateFile, double sec);

		bool SaveState(const std::string &stateFile);

		std::string wrapActionsTag(const std::string &xmlInput);

		bool ExecuteXMLCommand(const std::string &cmd);

		bool ExecuteCommand(const std::string &cmd);

		bool InitializeBioGearsSubstances();

		bool InitializeBioGearsLogging();

		bool InitializePatientActions();

		bool AttachEventHandler();

		void Shutdown();

		void StartSimulation();

		void StopSimulation();

		void AdvanceTimeTick();

		double GetSimulationTime();

		double GetPatientTime();

		std::map<std::string, double (BiogearsThread::*)()> *GetNodePathTable();

		double GetNodePath(const std::string &nodePath);

		void SetVentilator(const std::string &ventilatorSettings);

		void SetBVMMask(const std::string &ventilatorSettings);

		void SetIVPump(const std::string &pumpSettings);

		// AMM Standard patient actions
		void SetAcuteStress(const std::string &actionSettings);

		void SetAirwayObstruction(double severity);

		void SetApnea(const std::string &actionSettings);

		void SetAsthmaAttack(double severity);

		void SetBrainInjury(double severity, const std::string &type);

		void SetBronchoconstriction(const std::string &actionSettings);

		void SetBurnWound(const std::string &actionSettings);

		void SetCardiacArrest(const std::string &actionSettings);

		void SetChestCompression(const std::string &actionSettings);

		void SetChestOcclusiveDressing(const std::string &state, const std::string &side);

		void SetConsciousRespiration(const std::string &actionSettings);

		void SetBreathHold(const std::string &actionSettings);

		void SetForcedExhale(const std::string &actionSettings);

		void SetForcedInhale(const std::string &actionSettings);

		void SetConsciousRespirationCommand(const std::string &actionSettings);

		void SetConsumeNutrients(const std::string &actionSettings);

		void SetExercise(const std::string &actionSettings);

		void SetHemorrhage(const std::string &location, double flow);

		void SetTourniquet(const std::string &location, const std::string &state);

		void SetInfection(const std::string &actionSettings);

		void SetIntubation(const std::string &actionSettings);

		void SetMechanicalVentilation(const std::string &actionSettings);

		void SetNasalCannula(double flowRate, const std::string &unit);

		void SetNeedleDecompression(const std::string &state, const std::string &side);

		void SetChestTube(const std::string &state, const std::string &side);

		void SetPain(const std::string &location, double severity);

		void SetPatientAction(const std::string &actionSettings);

		void SetPatientAssessmentRequest(const std::string &actionSettings);

		void SetPericardialEffusion(const std::string &actionSettings);

		void SetPupillaryResponse(const std::string &actionSettings);

		void SetSepsis(const std::string &location, double severity);

		void SetSubstanceAdministration(const std::string &actionSettings);

		void
		SetSubstanceBolus(const std::string &substance, double concentration, const std::string &concUnit, double dose,
		                  const std::string &doseUnit, const std::string &adminRoute);

		void SetSubstanceCompoundInfusion(const std::string &substance, double bagVolume, const std::string &bvUnit,
		                                  double rate,
		                                  const std::string &rUnit);

		void
		SetSubstanceInfusion(const std::string &substance, double bagVolume, const std::string &bvUnit, double rate,
		                     const std::string &rUnit);

		void SetSubstanceNasalDose(const std::string &substance, double dose, const std::string &doseUnit);

		void SetSubstanceOralDose(const std::string &actionSettings);

		void SetTensionPneumothorax(const std::string &type, const std::string &side, double severity);

		void SetUrinate(const std::string &actionSettings);

		void SetUseInhaler(const std::string &actionSettings);

		void SetLogging(bool log);

		void SetLastFrame(int lastFrame);

		void Status();

		bool fixed_timestep = true;

		bool scenarioLoading = false;

		bool running = false;

		std::map<std::string, double (BiogearsThread::*)()> nodePathTable;
		std::unordered_set<std::string> highFrequencyNodes;

		static constexpr int DEFAULT_LOGGING_FREQUENCY = 50;
		static constexpr char DEFAULT_LOG_PATH[] = "./logs/AMM_Output_";

		static constexpr size_t numEvents = static_cast<size_t>(biogears::SEPatientEventType::_end);
		std::vector<EventStatus> patientEventStates;

		bool paralyzed = false;
		bool paralyzedSent = false;
		bool irreversible = false;
		bool irreversibleSent = false;

		bool startOfExhale = false;
		bool startOfExhaleSent = false;

		bool startOfInhale = false;
		bool startOfInhaleSent = false;

		bool pneumothoraxLClosed = false;
		bool pneumothoraxLClosedSent = false;
		bool pneumothoraxRClosed = false;
		bool pneumothoraxRClosedSent = false;
		bool pneumothoraxLOpen = false;
		bool pneumothoraxLOpenSent = false;
		bool pneumothoraxROpen = false;
		bool pneumothoraxROpenSent = false;
		bool hemorrhage = false;
		bool hemorrhageSent = false;
		bool acuteStress = false;
		bool acuteStressSent = false;
		bool asthmaAttack = false;
		bool asthmaAttackSent = false;
		bool brainInjury = false;
		bool brainInjurySent = false;
		bool mildHypothermia = false;
		bool mildHypothermiaSent = false;
		bool moderateHypothermia = false;
		bool moderateHypothermiaSent = false;
		bool severeHypothermia = false;
		bool severeHypothermiaSent = false;
		bool shivering = false;
		bool shiveringSent = false;
		bool tachypnea = false;
		bool tachypneaSent = false;
		bool tachycardia = false;
		bool tachycardiaSent = false;

	private:
		void PopulateNodePathTable();

		double GetLoggingStatus();

		double GetShutdownMessage();

		double GetPatientAge();

		double GetPatientWeight();

		double GetPatientGender();

		double GetPatientHeight();

		double GetPatient_BodyFatFraction();

		double GetGCSValue();

		double GetCerebralPerfusionPressure();

		double GetCerebralBloodFlow();

		double GetBloodLoss();

		double GetBloodLossPercentage();

		double GetHeartRate();

		double GetBloodVolume();

		double GetArterialSystolicPressure();

		double GetArterialDiastolicPressure();

		double GetMeanArterialPressure();

		double GetArterialPressure();

		double GetMeanCentralVenousPressure();

		double GetInspiratoryFlow();

		double GetRespiratoryTotalPressure();

		double GetEndTidalCarbonDioxideFraction();

		double GetEndTidalCarbonDioxidePressure();

		double GetOxygenSaturation();

		double GetCarbonMonoxideSaturation();

		double GetRawRespirationRate();

		double GetRespirationRate();

		double GetPulmonaryResistance();

		double GetCoreTemperature();

		double GetECGWaveform();

		double GetSodiumConcentration();

		double GetSodium();

		double GetGlucoseConcentration();

		double GetBUN();

		double GetIntracranialPressure();

		double GetCreatinineConcentration();

		double GetWhiteBloodCellCount();

		double GetRedBloodCellCount();

		double GetHemoglobinConcentration();

		double GetOxyhemoglobinConcentration();

		double GetCarbaminohemoglobinConcentration();

		double GetOxyCarbaminohemoglobinConcentration();

		double GetCarboxyhemoglobinConcentration();

		double GetIonizedCalcium();

		double GetAnionGap();

		double GetHematocrit();

		double GetRawBloodPH();

		double GetModBloodPH();

		double GetBloodPH();

		double GetArterialCarbonDioxidePressure();

		double GetArterialOxygenPressure();

		double GetVenousOxygenPressure();

		double GetVenousCarbonDioxidePressure();

		double GetBicarbonateConcentration();

		double GetBicarbonate();

		double GetBicarbonateRaw();

		double GetBaseExcess();

		double GetBaseExcessRaw();

		double GetCO2();

		double GetPotassium();

		double GetChloride();

		double GetPlateletCount();

		double GetExhaledCO2();

		double GetExhaledO2();

		double GetTidalVolume();

		double GetTotalLungVolume();

		double GetLeftLungVolume();

		double GetRightLungVolume();

		double GetLeftLungTidalVolume();

		double GetRightLungTidalVolume();

		double GetLeftPleuralCavityVolume();

		double GetRightPleuralCavityVolume();

		double GetLeftAlveoliBaselineCompliance();

		double GetRightAlveoliBaselineCompliance();

		double GetCardiacOutput();

		double GetCalciumConcentration();

		double GetAlbuminConcentration();

		double GetLactateConcentration();

		double GetLactateConcentrationMMOL();

		double GetTotalBilirubin();

		double GetTotalProtein();

		double GetPainVisualAnalogueScale();

		double GetUrineSpecificGravity();

		double GetUrineProductionRate();

		double GetUrineOsmolality();

		double GetUrineOsmolarity();

		double GetBladderGlucose();

		double GetShuntFraction();

		int GlasgowEstimator(double cbf);

		biogears::SESubstance *sodium = nullptr;
		biogears::SESubstance *glucose = nullptr;
		biogears::SESubstance *creatinine = nullptr;
		biogears::SESubstance *calcium = nullptr;
		biogears::SESubstance *albumin = nullptr;
		biogears::SESubstance *hemoglobin = nullptr;
		biogears::SESubstance *bicarbonate = nullptr;
		biogears::SESubstance *CO2 = nullptr;
		biogears::SESubstance *N2 = nullptr;
		biogears::SESubstance *O2 = nullptr;
		biogears::SESubstance *CO = nullptr;
		biogears::SESubstance *Hb = nullptr;
		biogears::SESubstance *HbO2 = nullptr;
		biogears::SESubstance *HbCO2 = nullptr;
		biogears::SESubstance *HbCO = nullptr;
		biogears::SESubstance *HbO2CO2 = nullptr;
		biogears::SESubstance *potassium = nullptr;
		biogears::SESubstance *chloride = nullptr;
		biogears::SESubstance *lactate = nullptr;

		const biogears::SEGasCompartment *carina = nullptr;
		const biogears::SEGasCompartment *leftLung = nullptr;
		const biogears::SEGasCompartment *rightLung = nullptr;
		const biogears::SELiquidCompartment *bladder = nullptr;

	protected:
		std::mutex m_mutex;
		mutable std::shared_mutex m_nodePathMutex;

		std::unique_ptr<biogears::BioGearsEngine> m_pe;
		// biogears::BioGears *bg;

		// AMM
		// AMM::UUID m_uuid;
		// std::string moduleName = "MoHSES_BioGearsThread";
		// std::string configFile = "config/pe_manager_amm.xml";

		// AMM::DDSManager<BiogearsThread> *m_mgr = new DDSManager<BiogearsThread>(configFile);
		// std::unique_ptr<AMM::DDSManager<BiogearsThread>> m_mgr;

		double thresh = 1.0;

		bool falling_L = false;
		double lung_vol_L = 0.0, new_min_L = 1500.0, new_max_L = 0.0, min_lung_vol_L = 0.0, max_lung_vol_L = 0.0;
		double chestrise_pct_L = 0.0;
		double leftLungTidalVol = 0.0;

		bool falling_R = false;
		double lung_vol_R = 0.0, new_min_R = 1500.0, new_max_R = 0.0, min_lung_vol_R = 0.0, max_lung_vol_R = 0.0;
		double chestrise_pct_R = 0.0;
		double rightLungTidalVol = 0.0;

		bool eventHandlerAttached = false;

		double bloodPH = 0.0;
		double rawBloodPH = 0.0;
		double lactateConcentration = 0.0;
		double lactateMMOL = 0.0;
		double startingBloodVolume = 5423.53;
		double currentBloodVolume = 0.0;
		double rawRespirationRate = 0.0;

		int lastFrame = 0;

		bool logging_enabled = false;

		// Helper function to check if the engine is initialized
		bool IsEngineInitialized() const;

		template <typename Func>
		auto WithEngineLock(Func &&func) -> decltype(func()) {
			std::lock_guard<std::mutex> lock(m_mutex); // Lock the mutex to ensure thread safety
			return func(); // Execute the lambda function and return its result
		}

		MoHSES_EventHandler myEventHandler;

		// Initialization and cleanup helpers
		void Cleanup();
	};
}
