#include "MoHSES_EventHandler.h"


AMM::EventHandler::EventHandler(biogears::Logger *pLogger) {

}

    void AMM::EventHandler::HandlePatientEvent(biogears::SEPatientEventType type, bool active,
                                           const biogears::SEScalarTime *time) {
    bool dontLog = false;
    if (active) { // entering state
        switch (type) {
            case biogears::SEPatientEventType::AcuteLungInjury:
                break;
            case biogears::SEPatientEventType::AcuteRespiratoryDistress:
                break;
            case biogears::SEPatientEventType::Antidiuresis:
                break;
            case biogears::SEPatientEventType::Asystole:
                break;
            case biogears::SEPatientEventType::Bradycardia:
                break;
            case biogears::SEPatientEventType::Bradypnea:
                break;
            case biogears::SEPatientEventType::BrainOxygenDeficit:
                break;
            case biogears::SEPatientEventType::CardiacArrest:
                break;
            case biogears::SEPatientEventType::CardiogenicShock:
                break;
            case biogears::SEPatientEventType::CriticalBrainOxygenDeficit:
                break;
            case biogears::SEPatientEventType::Dehydration:
                break;
            case biogears::SEPatientEventType::Diuresis:
                break;
            case biogears::SEPatientEventType::Fasciculation:
                break;
            case biogears::SEPatientEventType::Fatigue:
                break;
            case biogears::SEPatientEventType::FunctionalIncontinence:
                break;
            case biogears::SEPatientEventType::HemolyticTransfusionReaction:
                break;
            case biogears::SEPatientEventType::Hypercapnia:
                break;
            case biogears::SEPatientEventType::Hyperglycemia:
                break;
            case biogears::SEPatientEventType::Hyperthermia:
                break;
            case biogears::SEPatientEventType::Hypoglycemia:
                break;
            case biogears::SEPatientEventType::HypoglycemicComa:
                break;
            case biogears::SEPatientEventType::HypoglycemicShock:
                break;
            case biogears::SEPatientEventType::HypovolemicShock:
                break;
            case biogears::SEPatientEventType::Hypoxia:
                break;
            case biogears::SEPatientEventType::IntracranialHypertension:
                break;
            case biogears::SEPatientEventType::IntracranialHypotension:
                break;
            case biogears::SEPatientEventType::IrreversibleState:
                irreversible = true;
                break;
            case biogears::SEPatientEventType::Ketoacidosis:
                break;
            case biogears::SEPatientEventType::LacticAcidosis:
                break;
            case biogears::SEPatientEventType::LiverGlycogenDepleted:
                break;
            case biogears::SEPatientEventType::MaximumPulmonaryVentilationRate:
                break;
            case biogears::SEPatientEventType::MetabolicAcidosis:
                break;
            case biogears::SEPatientEventType::MetabolicAlkalosis:
                break;
            case biogears::SEPatientEventType::MildHyperkalemia:
                break;
            case biogears::SEPatientEventType::MildHypernatremia:
                break;
            case biogears::SEPatientEventType::MildHypokalemia:
                break;
            case biogears::SEPatientEventType::MildHyponatremia:
                break;
            case biogears::SEPatientEventType::MildHypothermia:
                break;
            case biogears::SEPatientEventType::MuscleCatabolism:
                break;
            case biogears::SEPatientEventType::MuscleGlycogenDepleted:
                break;
            case biogears::SEPatientEventType::MyocardiumOxygenDeficit:
                break;
            case biogears::SEPatientEventType::Natriuresis:
                break;
            case biogears::SEPatientEventType::NutritionDepleted:
                break;
            case biogears::SEPatientEventType::PulselessRhythm:
                break;
            case biogears::SEPatientEventType::RenalHypoperfusion:
                break;
            case biogears::SEPatientEventType::RespiratoryAcidosis:
                break;
            case biogears::SEPatientEventType::RespiratoryAlkalosis:
                break;
            case biogears::SEPatientEventType::SevereAcuteRespiratoryDistress:
                break;
            case biogears::SEPatientEventType::SevereHyperkalemia:
                break;
            case biogears::SEPatientEventType::SevereHypernatremia:
                break;
            case biogears::SEPatientEventType::SevereHypokalemia:
                break;
            case biogears::SEPatientEventType::SevereHyponatremia:
                break;
            case biogears::SEPatientEventType::SevereHypothermia:
                break;
            case biogears::SEPatientEventType::SevereSepsis:
                break;
            case biogears::SEPatientEventType::Shivering:
                shivering = true;
                break;
            case biogears::SEPatientEventType::StartOfCardiacCycle:
                dontLog = true;
                break;
            case biogears::SEPatientEventType::StartOfExhale:
                dontLog = true;
                startOfExhale = true;
                startOfInhale = false;
                break;
            case biogears::SEPatientEventType::StartOfInhale:
                dontLog = true;
                startOfInhale = true;
                startOfExhale = false;
                break;
            case biogears::SEPatientEventType::Tachycardia:
                tachycardia = true;
                break;
            case biogears::SEPatientEventType::Tachypnea:
                tachypnea = true;
                break;
            default:
                if (!dontLog) {
                    LOG_INFO << " Patient has entered state : " << type;
                }
                break;


        }
    } else { // exit state
        switch (type) {
            case biogears::SEPatientEventType::AcuteLungInjury:
                break;
            case biogears::SEPatientEventType::AcuteRespiratoryDistress:
                break;
            case biogears::SEPatientEventType::Antidiuresis:
                break;
            case biogears::SEPatientEventType::Asystole:
                break;
            case biogears::SEPatientEventType::Bradycardia:
                break;
            case biogears::SEPatientEventType::Bradypnea:
                break;
            case biogears::SEPatientEventType::BrainOxygenDeficit:
                break;
            case biogears::SEPatientEventType::CardiacArrest:
                break;
            case biogears::SEPatientEventType::CardiogenicShock:
                break;
            case biogears::SEPatientEventType::CriticalBrainOxygenDeficit:
                break;
            case biogears::SEPatientEventType::Dehydration:
                break;
            case biogears::SEPatientEventType::Diuresis:
                break;
            case biogears::SEPatientEventType::Fasciculation:
                break;
            case biogears::SEPatientEventType::Fatigue:
                break;
            case biogears::SEPatientEventType::FunctionalIncontinence:
                break;
            case biogears::SEPatientEventType::HemolyticTransfusionReaction:
                break;
            case biogears::SEPatientEventType::Hypercapnia:
                break;
            case biogears::SEPatientEventType::Hyperglycemia:
                break;
            case biogears::SEPatientEventType::Hyperthermia:
                break;
            case biogears::SEPatientEventType::Hypoglycemia:
                break;
            case biogears::SEPatientEventType::HypoglycemicComa:
                break;
            case biogears::SEPatientEventType::HypoglycemicShock:
                break;
            case biogears::SEPatientEventType::HypovolemicShock:
                break;
            case biogears::SEPatientEventType::Hypoxia:
                break;
            case biogears::SEPatientEventType::IntracranialHypertension:
                break;
            case biogears::SEPatientEventType::IntracranialHypotension:
                break;
            case biogears::SEPatientEventType::IrreversibleState:
                irreversible = false;
                break;
            case biogears::SEPatientEventType::Ketoacidosis:
                break;
            case biogears::SEPatientEventType::LacticAcidosis:
                break;
            case biogears::SEPatientEventType::LiverGlycogenDepleted:
                break;
            case biogears::SEPatientEventType::MaximumPulmonaryVentilationRate:
                break;
            case biogears::SEPatientEventType::MetabolicAcidosis:
                break;
            case biogears::SEPatientEventType::MetabolicAlkalosis:
                break;
            case biogears::SEPatientEventType::MildHyperkalemia:
                break;
            case biogears::SEPatientEventType::MildHypernatremia:
                break;
            case biogears::SEPatientEventType::MildHypokalemia:
                break;
            case biogears::SEPatientEventType::MildHyponatremia:
                break;
            case biogears::SEPatientEventType::MildHypothermia:
                mildHypothermia = false;
                break;
            case biogears::SEPatientEventType::ModerateHypothermia:
                moderateHypothermia = false;
                break;
            case biogears::SEPatientEventType::MuscleCatabolism:
                break;
            case biogears::SEPatientEventType::MuscleGlycogenDepleted:
                break;
            case biogears::SEPatientEventType::MyocardiumOxygenDeficit:
                break;
            case biogears::SEPatientEventType::Natriuresis:
                break;
            case biogears::SEPatientEventType::NutritionDepleted:
                break;
            case biogears::SEPatientEventType::PulselessRhythm:
                break;
            case biogears::SEPatientEventType::RenalHypoperfusion:
                break;
            case biogears::SEPatientEventType::RespiratoryAcidosis:
                break;
            case biogears::SEPatientEventType::RespiratoryAlkalosis:
                break;
            case biogears::SEPatientEventType::SevereAcuteRespiratoryDistress:
                break;
            case biogears::SEPatientEventType::SevereHyperkalemia:
                break;
            case biogears::SEPatientEventType::SevereHypernatremia:
                break;
            case biogears::SEPatientEventType::SevereHypokalemia:
                break;
            case biogears::SEPatientEventType::SevereHyponatremia:
                break;
            case biogears::SEPatientEventType::SevereHypothermia:
                severeHypothermia = false;
                break;
            case biogears::SEPatientEventType::SevereSepsis:
                break;
            case biogears::SEPatientEventType::Shivering:
                shivering = true;
                break;
            case biogears::SEPatientEventType::StartOfCardiacCycle:
                break;
            case biogears::SEPatientEventType::StartOfExhale:
                dontLog = true;
                startOfExhale = true;
                startOfInhale = false;
                break;
            case biogears::SEPatientEventType::StartOfInhale:
                dontLog = true;
                startOfInhale = true;
                startOfExhale = false;
                break;
            case biogears::SEPatientEventType::Tachycardia:
                tachycardia = false;
                break;
            case biogears::SEPatientEventType::Tachypnea:
                tachypnea = false;
                break;
            default:
                if (!dontLog) {
                    LOG_INFO << " Patient has exited state : " << type;
                }
                break;
        }
    }
}

void AMM::EventHandler::HandleAnesthesiaMachineEvent(biogears::SEAnesthesiaMachineEvent type, bool active,
                                                     const biogears::SEScalarTime *time) {

}


