#pragma once

//#define CALC_TEMPLATE_CHECKUM
#define TEMPLATE_SIZE 1536

#include "Adafruit_Fingerprint.h"
#include "OpenKNX.h"
#ifdef CALC_TEMPLATE_CHECKUM
    #include "crc16.h"
#endif
#include <string>
#include <functional>

#if defined ESP32_WESP32 || defined ARDUINO_ARCH_RP2040
    #define mySerial Serial2
#else
    #define mySerial Serial1
#endif

class Fingerprint
{
  using fingerprint_delay_fptr_t = void (*)(uint32_t);

  public:
    enum State
    {
        None,
        ScanFinger,
        ScanMatch,
        ScanNoMatch,
        EnrollCreateModel,
        WaitForFinger,
        RemoveFinger,
        Success,
        Failed,
        Busy
    };

    struct FindFingerResult
    {
        uint8_t found;
        uint16_t location;
        uint16_t checksum;
    };

    bool scannerReady;

    Fingerprint(uint32_t overridePassword = 0);
    Fingerprint(fingerprint_delay_fptr_t delayCallback, uint32_t overridePassword = 0);

    bool init();
    std::string logPrefix();
    bool setLed(State state);
    bool hasFinger();

    uint16_t getTemplateCount();
    uint16_t getNextFreeLocation();
    FindFingerResult findFingerprint();
    bool createTemplate();
    bool retrieveTemplate(uint8_t *templateData);
    bool sendTemplate(uint8_t *templateData);
    bool writeCrc(uint16_t location, uint8_t *templateData, uint32_t secret);
    bool storeTemplate(uint16_t location);
    bool deleteTemplate(uint16_t location);
    bool setPassword(uint32_t newPasswort);
    bool emptyDatabase(void);

  private:
    struct GetNotepadPageIndexResult
    {
        uint8_t page;
        uint8_t index;
    };

    Adafruit_Fingerprint _finger;
    fingerprint_delay_fptr_t _delayMs;

    bool _listTemplates();
    GetNotepadPageIndexResult _getNotepadPageIndex(u_int16_t templateLocation);
    static void _delayCallbackDefault(uint32_t period);
};