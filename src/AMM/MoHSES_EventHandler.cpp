#include "MoHSES_EventHandler.h"


/*MoHSES_EventHandler::MoHSES_EventHandler(biogears::Logger *pLogger) {
    patientEventStates.resize(static_cast<size_t>(biogears::SEPatientEventType::_end));
}*/

void MoHSES_EventHandler::setEventState(biogears::SEPatientEventType event, bool state) {
    patientEventStates[static_cast<size_t>(event)].state = state;
}

void MoHSES_EventHandler::setMessageSent(biogears::SEPatientEventType event, bool sent) {
    patientEventStates[static_cast<size_t>(event)].messageSent = sent;
}

void MoHSES_EventHandler::HandlePatientEvent(biogears::SEPatientEventType type, bool active,
                                           const biogears::SEScalarTime *time) {
  std::lock_guard<std::mutex> lg(meh_mutex);
    bool dontLog = false;

    LOG_DEBUG << "Event received";

    setEventState(type, active);

    if (active) {
        switch (type) {
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
            default:
                break;
        }
        //if (!dontLog) {
            LOG_INFO << " Patient has entered state: " << type;
        //}
    } else {
        switch (type) {
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
            default:
                break;
        }
        //if (!dontLog) {
            LOG_INFO << " Patient has exited state: " << type;
        //}
    }

}

void MoHSES_EventHandler::HandleAnesthesiaMachineEvent(biogears::SEAnesthesiaMachineEvent type, bool active,
                                                     const biogears::SEScalarTime *time) {

}


