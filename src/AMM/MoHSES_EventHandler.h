#pragma once

#include "amm/BaseLogger.h"
#include <biogears/cdm/utils/SEEventHandler.h>

namespace AMM {
    class EventHandler : public biogears::SEEventHandler {
    public:
        //EventHandler(biogears::Logger *logger) : SEEventHandler() {};

        EventHandler(biogears::Logger *pLogger);

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
        bool tachypnea = false;
        bool tachypneaSent = false;
        bool tachycardia = false;
        bool tachycardiaSent = false;

        void HandlePatientEvent(biogears::SEPatientEventType type, bool active,
                                const biogears::SEScalarTime *time = nullptr) override;

        void HandleAnesthesiaMachineEvent(biogears::SEAnesthesiaMachineEvent type, bool active,
                                          const biogears::SEScalarTime *time = nullptr) override;

    };
}