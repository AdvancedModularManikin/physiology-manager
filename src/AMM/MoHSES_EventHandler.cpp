#include "MoHSES_EventHandler.h"

void MoHSES_EventHandler::setEventState(biogears::SEPatientEventType event, bool state) {
	patientEventStates[static_cast<size_t>(event)].state = state;
}

void MoHSES_EventHandler::setMessageSent(biogears::SEPatientEventType event, bool sent) {
	patientEventStates[static_cast<size_t>(event)].messageSent = sent;
}

void MoHSES_EventHandler::HandlePatientEvent(biogears::SEPatientEventType type, bool active,
                                             const biogears::SEScalarTime *time) {

	bool dontLog = false;

	setEventState(type, active);

	if (active) {
		switch (type) {
			case biogears::SEPatientEventType::IrreversibleState:
				irreversible = true;
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
				// LOG_INFO << " Patient has entered state : " << type;
				break;
		}

		if (!dontLog) {
			std::cout << " Patient has entered state: " << type << std::endl;
		}
	} else {
		switch (type) {
			case biogears::SEPatientEventType::StartOfCardiacCycle:
				dontLog = true;
				break;
			case biogears::SEPatientEventType::StartOfExhale:
				dontLog = true;
				startOfExhale = false;
				break;
			case biogears::SEPatientEventType::StartOfInhale:
				dontLog = true;
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
				break;
		}
		if (!dontLog) {
			std::cout << " Patient has exited state: " << type << std::endl;
		}
	}

}

void MoHSES_EventHandler::HandleAnesthesiaMachineEvent(biogears::SEAnesthesiaMachineEvent type, bool active,
                                                       const biogears::SEScalarTime *time) {

}


