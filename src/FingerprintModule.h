#include "OpenKNX.h"
#include "hardware.h"
#include <Fingerprint.h>
#include <secrets.h>

#define PWR_PIN 1
#define TOUCH_PIN 2

#define LOCK_PIN 27
#define UNLOCK_PIN 26
#define LED_GREEN_PIN 28
#define LED_RED_PIN 29

#define CAPTURE_RETRIES_TOUCH_TIMEOUT 500
#define CAPTURE_RETRIES_LOCK_TIMEOUT 3000

class FingerprintModule : public OpenKNX::Module
{
  public:
    void loop() override;
    void setup() override;
    void processAfterStartupDelay() override;
    void processInputKo(GroupObject &ko) override;

    const std::string name() override;
    const std::string version() override;
    // void writeFlash() override;
    // void readFlash(const uint8_t* data, const uint16_t size) override;
    // uint16_t flashSize() override;

  private:
    static void interruptTouched();
    static void interruptUnlock();
    static void interruptLock();
    void setFingerprintPower(bool on);
    void updateLockLeds(bool showGreenWhenUnlock = true);

    Fingerprint finger;
    bool scanerHasPower = false;
    bool lockRequested = false;
    bool isLocked = false;
    unsigned long resetLedsTimer = 0;

    inline volatile static bool touched = false;
    inline volatile static bool unlockTouched = false;
    inline volatile static bool lockTouched = false;
};

extern FingerprintModule openknxFingerprintModule;