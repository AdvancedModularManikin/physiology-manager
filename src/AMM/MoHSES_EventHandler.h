#pragma once
#include "amm_std.h"
#include <biogears/cdm/utils/SEEventHandler.h>
#include <bitset>
#include <mutex>

class MoHSES_EventHandler : public biogears::SEEventHandler {
public:

	bool paralyzed = false;
	bool paralyzedSent = false;
	bool irreversible = false;
	bool irreversibleSent = false;
	bool startOfExhale = false;
	bool startOfInhale = false;
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

	MoHSES_EventHandler() : biogears::SEEventHandler()
	{
		patientEventStates.resize(static_cast<size_t>(biogears::SEPatientEventType::_end));
	}

	void HandlePatientEvent(biogears::SEPatientEventType type, bool active,
	                        const biogears::SEScalarTime *time) override;

	void HandleAnesthesiaMachineEvent(biogears::SEAnesthesiaMachineEvent type, bool active,
	                                  const biogears::SEScalarTime *time) override;

	void setEventState(biogears::SEPatientEventType event, bool state);

	void setMessageSent(biogears::SEPatientEventType event, bool sent);

	std::vector<AMM::EventStatus> patientEventStates;
};
