#include "OpenKNX.h"
#include "hardware.h"
#include "Fingerprint.h"
#include "ActionChannel.h"
#include "secrets.h"

#define DISPLAY_PWR_PIN 1
#define DISPLAY_TOUCH_PIN 2

#define TOUCH_LEFT_PIN 26
#define TOUCH_RIGHT_PIN 27
#define LED_GREEN_PIN 24
#define LED_RED_PIN 25

#define EXT0 14
#define EXT1 15
#define EXT2 28 // ADC
#define EXT3 18
#define EXT4 29 // ADC
#define EXT5 19

#define LED_RESET_TIMEOUT 1000
#define ENROLL_REQUEST_DELAY 100
#define CAPTURE_RETRIES_TOUCH_TIMEOUT 500
#define CAPTURE_RETRIES_LOCK_TIMEOUT 3000

#define MAX_FINGERS 1500

#define FIN_CaclStorageOffset(fingerId) fingerId * 29 + 4096 + 1 // first byte free for finger info storage format version

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
    static void interruptDisplayTouched();
    static void interruptTouchLeft();
    static void interruptTouchRight();
    void processScanSuccess(uint16_t location, bool external = false);
    bool enrollFinger(uint16_t location);
    bool deleteFinger(uint16_t location);
    void setFingerprintPower(bool on);
    void handleFunctionPropertyEnrollFinger(uint8_t *data, uint8_t *resultData, uint8_t &resultLength);
    void handleFunctionPropertyDeleteFinger(uint8_t *data, uint8_t *resultData, uint8_t &resultLength);
    void handleFunctionPropertyResetScanner(uint8_t *data, uint8_t *resultData, uint8_t &resultLength);
    void handleFunctionPropertySearchPersonByFingerId(uint8_t *data, uint8_t *resultData, uint8_t &resultLength);
    void handleFunctionPropertySearchFingerIdByPerson(uint8_t *data, uint8_t *resultData, uint8_t &resultLength);
    static void delayCallback(uint32_t period);

    OpenKNX::Flash::Driver _fingerprintStorage;
    ActionChannel *_channels[FIN_ChannelCount];

    Fingerprint finger;
    bool scanerHasPower = false;
    uint32_t resetLedsTimer = 0;
    uint32_t enrollRequested = 0;
    uint16_t enrollRequestedLocation = 0;
    inline static bool delayCallbackActive = false;

    inline volatile static bool touched = false;
};

extern FingerprintModule openknxFingerprintModule;