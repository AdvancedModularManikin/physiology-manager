#pragma once

#include "amm/BaseLogger.h"
#include "amm/Utility.h"
#include <biogears/cdm/utils/SEEventHandler.h>
#include <bitset>

namespace AMM {
    class EventHandler : public biogears::SEEventHandler {
    public:
        bool startOfInhale;
        bool startOfExhale;

        EventHandler(biogears::Logger *pLogger);

        void HandlePatientEvent(biogears::SEPatientEventType type, bool active,
                                const biogears::SEScalarTime *time) override;

        void HandleAnesthesiaMachineEvent(biogears::SEAnesthesiaMachineEvent type, bool active,
                                          const biogears::SEScalarTime *time) override;

        void setEventState(biogears::SEPatientEventType event, bool state);
        void setMessageSent(biogears::SEPatientEventType event, bool sent);

        std::vector<EventStatus> patientEventStates;


    };
}