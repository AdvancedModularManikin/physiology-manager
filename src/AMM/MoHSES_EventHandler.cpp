#include "MoHSES_EventHandler.h"


AMM::EventHandler::EventHandler(biogears::Logger *pLogger) {

}

void AMM::EventHandler::HandlePatientEvent(biogears::SEPatientEventType type, bool active,
                                           const biogears::SEScalarTime *time) {
    if (active) {
        switch (type) {
            case biogears::SEPatientEventType::IrreversibleState:
                LOG_INFO


                            << " Patient has entered irreversible state";
                irreversible = true;
                break;
            case biogears::SEPatientEventType::StartOfCardiacCycle:
                break;
            case biogears::SEPatientEventType::StartOfExhale:
                startOfExhale = true;
                startOfInhale = false;
                break;
            case biogears::SEPatientEventType::StartOfInhale:
                startOfInhale = true;
                startOfExhale = false;
                break;
            case biogears::SEPatientEventType::MildHypothermia:
                mildHypothermia = true;
                break;
            case biogears::SEPatientEventType::ModerateHypothermia:
                moderateHypothermia = true;
                break;
            case biogears::SEPatientEventType::SevereHypothermia:
                severeHypothermia = true;
                break;
            case biogears::SEPatientEventType::Shivering:
                shivering = true;
                break;
            default:
                LOG_INFO << " Patient has entered state : " << type;
                break;


        }
    } else {
        switch (type) {
            case biogears::SEPatientEventType::StartOfCardiacCycle:
                break;
            case biogears::SEPatientEventType::StartOfExhale:
                startOfExhale = false;
                break;
            case biogears::SEPatientEventType::StartOfInhale:
                startOfInhale = false;
                break;
            case biogears::SEPatientEventType::MildHypothermia:
                mildHypothermia = false;
                break;
            case biogears::SEPatientEventType::ModerateHypothermia:
                moderateHypothermia = false;
                break;
            case biogears::SEPatientEventType::SevereHypothermia:
                severeHypothermia = false;
                break;
            case biogears::SEPatientEventType::Shivering:
                shivering = false;
                break;
            default:
                LOG_INFO << " Patient has exited state : " << type;
                break;
        }
    }
}

void AMM::EventHandler::HandleAnesthesiaMachineEvent(biogears::SEAnesthesiaMachineEvent type, bool active,
                                                     const biogears::SEScalarTime *time) {

}


