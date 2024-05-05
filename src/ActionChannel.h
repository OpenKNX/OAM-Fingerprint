#include "OpenKNX.h"

class ActionChannel : public OpenKNX::Channel
{
  private:

  public:
    ActionChannel(uint8_t index);
    const std::string name() override;

    void processInputKo(GroupObject &ko) override;

    void processScan(uint16_t location);
};