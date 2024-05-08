#include "ActionChannel.h"

ActionChannel::ActionChannel(uint8_t index)
{
    _channelIndex = index;
}

const std::string ActionChannel::name()
{
    return "Action";
}

void ActionChannel::processInputKo(GroupObject &ko)
{
    switch (FIN_KoCalcIndex(ko.asap()))
    {
        case FIN_KoActionCall:
            // start timer
            // reset KoFIN_ActionCall after timer
            break;
    }


}

void ActionChannel::processScan(uint16_t location)
{
    if (!ParamFIN_ActionAuthenticate || KoFIN_ActionCall.value(DPT_Switch))
    {

    }
}