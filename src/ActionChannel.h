#include "OpenKNX.h"

#define ACTION_CALL_TIMEOUT 3000

class ActionChannel : public OpenKNX::Channel
{
    private:
        uint32_t _actionCallResetTime = 0;
        uint32_t _stairLightTime = 0;

    public:
        ActionChannel(uint8_t index);
        const std::string name() override;

        void loop();
        void processInputKo(GroupObject &ko) override;
        void processScan(uint16_t location);
};