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

/*
    for (size_t i = 0; i < ParamFINACT_FingerActionCount; i++)
    {
        knx.paramWord(FINACT_faFingerId + FINACT_ParamBlockOffset + i * FINACT_ParamBlockSize);

        knx.paramWord(FINACT_faActionId + FINACT_ParamBlockOffset + i * FINACT_ParamBlockSize);
    }
*/
}

void ActionChannel::processScan(uint16_t location)
{
    //if (FIN_ChannelFingerId != location)
    //    return;
    
    if (!ParamFIN_ActionAuthenticate || KoFIN_ActionCall.value(DPT_Switch))
    {

    }
}