#include "ActionChannel.h"

ActionChannel::ActionChannel(uint8_t index)
{
    _channelIndex = index;
}

const std::string ActionChannel::name()
{
    return "Action";
}

void ActionChannel::loop()
{
    if (_actionCallResetTime > 0 && delayCheck(_actionCallResetTime, ACTION_CALL_TIMEOUT))
    {
        KoFIN_ActionCall.value(false, DPT_Switch);
        _actionCallResetTime = 0;
    }

    if (_stairLightTime > 0 && delayCheck(_stairLightTime, ParamFIN_ActionDelayTimeMS))
    {
        KoFIN_ActionSwitch.value(false, DPT_Switch);
        _stairLightTime = 0;
    }
}

void ActionChannel::processInputKo(GroupObject &ko)
{
    switch (FIN_KoCalcIndex(ko.asap()))
    {
        case FIN_KoActionCall:
            KoFIN_ActionCall.value(true, DPT_Switch);
            _actionCallResetTime = millis();
            break;
    }
}

void ActionChannel::processScan(uint16_t location)
{
    if (!ParamFIN_ActionAuthenticate || KoFIN_ActionCall.value(DPT_Switch))
    {
        switch (ParamFIN_ActionActionType)
        {
            case 0: // action deactivated
                break;
            case 1: // switch
                KoFIN_ActionSwitch.value(ParamFIN_ActionOnOff, DPT_Switch);
                break;
            case 2: // toggle
                KoFIN_ActionSwitch.value(!KoFIN_ActionState.value(DPT_Switch), DPT_Switch);
                KoFIN_ActionState.value(KoFIN_ActionSwitch.value(DPT_Switch), DPT_Switch);
                break;
            case 3: // stair light
                KoFIN_ActionSwitch.value(true, DPT_Switch);
                _stairLightTime = millis();
                break;
        }

        if (KoFIN_ActionCall.value(DPT_Switch))
            KoFIN_ActionCall.value(false, DPT_Switch);
    }
}