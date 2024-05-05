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
        case FIN_KoChannelActionStatus:
            break;
    }
}

void ActionChannel::processScan(uint16_t location)
{
    //if (FIN_ChannelFingerId != location)
    //    return;
    

}