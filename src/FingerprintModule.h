#include "OpenKNX.h"
#include "hardware.h"
#include "Fingerprint.h"
#include "ActionChannel.h"
#include "secrets.h"

#define PWR_PIN 1
#define TOUCH_PIN 2

#define LOCK_PIN 26
#define UNLOCK_PIN 27
#define LED_GREEN_PIN 24
#define LED_RED_PIN 25

#define EXT0 14
#define EXT1 15
#define EXT2 28 // ADC
#define EXT3 18
#define EXT4 29 // ADC
#define EXT5 19

#define LED_RESET_TIMEOUT 1000
#define CAPTURE_RETRIES_TOUCH_TIMEOUT 500
#define CAPTURE_RETRIES_LOCK_TIMEOUT 3000

#define MAX_FINGERS 1500

#define FIN_CaclStorageOffset(fingerId) (fingerId - 1) * 29 + 1 // first byte free for storage format version

class FingerprintModule : public OpenKNX::Module
{
  public:
    void loop() override;
    void setup() override;
    void processAfterStartupDelay() override;
    void processInputKo(GroupObject &ko) override;
		bool processFunctionProperty(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength) override;
		// bool processFunctionPropertyState(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength) override;

    const std::string name() override;
    const std::string version() override;
    // void writeFlash() override;
    // void readFlash(const uint8_t* data, const uint16_t size) override;
    // uint16_t flashSize() override;

  private:
    static void interruptTouched();
    static void interruptUnlock();
    static void interruptLock();
    void processScanSuccess(uint16_t location, bool external = false);
    bool enrollFinger(uint16_t location);
    bool deleteFinger(uint16_t location);
    void setFingerprintPower(bool on);
    void updateLockLeds(bool showGreenWhenUnlock = true);
    void handleFunctionPropertyEnrollFinger(uint8_t *data, uint8_t *resultData, uint8_t &resultLength);
    void handleFunctionPropertyDeleteFinger(uint8_t *data, uint8_t *resultData, uint8_t &resultLength);
    void handleFunctionPropertySearchPersonByFingerId(uint8_t *data, uint8_t *resultData, uint8_t &resultLength);
    void handleFunctionPropertySearchFingerIdByPerson(uint8_t *data, uint8_t *resultData, uint8_t &resultLength);
    static void delayCallback(uint32_t period);

    OpenKNX::Flash::Driver _fingerprintStorage;
    ActionChannel *_channels[FIN_ChannelCount];

    Fingerprint finger;
    bool scanerHasPower = false;
    bool lockRequested = false;
    bool isLocked = false;
    unsigned long resetLedsTimer = 0;
    inline static bool delayCallbackActive = false;

    inline volatile static bool touched = false;
    inline volatile static bool unlockTouched = false;
    inline volatile static bool lockTouched = false;
};

extern FingerprintModule openknxFingerprintModule;