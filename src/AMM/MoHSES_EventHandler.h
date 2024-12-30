#pragma once

#include "amm/BaseLogger.h"
#include "amm/Utility.h"
#include <biogears/cdm/utils/SEEventHandler.h>
#include <bitset>
#include <mutex>

class MoHSES_EventHandler : public biogears::SEEventHandler {
public:
	bool startOfInhale{};
	bool startOfExhale{};
	std::mutex meh_mutex;

	explicit MoHSES_EventHandler(biogears::Logger *pLogger)
			: SEEventHandler() {
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
