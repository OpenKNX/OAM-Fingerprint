#include <FingerprintModule.h>

const std::string FingerprintModule::name()
{
    return "Fingerprint";
}

const std::string FingerprintModule::version()
{
    return MAIN_Version;
}

void FingerprintModule::setup()
{
    finger = Fingerprint(FINGERPRINT_PASSWORD);

    pinMode(LED_GREEN_PIN, OUTPUT);
    pinMode(LED_RED_PIN, OUTPUT);

    digitalWrite(LED_RED_PIN, HIGH);

    pinMode(UNLOCK_PIN, INPUT);
    pinMode(LOCK_PIN, INPUT);

    pinMode(TOUCH_PIN, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(TOUCH_PIN), FingerprintModule::interruptTouched, FALLING);

    attachInterrupt(digitalPinToInterrupt(UNLOCK_PIN), FingerprintModule::interruptUnlock, RISING);
    attachInterrupt(digitalPinToInterrupt(LOCK_PIN), FingerprintModule::interruptLock, RISING);

    pinMode(PWR_PIN, OUTPUT_4MA);
    setFingerprintPower(true);

    digitalWrite(LED_RED_PIN, LOW);
    digitalWrite(LED_GREEN_PIN, HIGH);
    finger.setLed(Fingerprint::State::Success);

    resetLedsTimer = millis();
    logInfoP("Fingerprint module ready.");
}

void FingerprintModule::interruptTouched()
{
    touched = true;
}

void FingerprintModule::interruptUnlock()
{
    unlockTouched = true;
}

void FingerprintModule::interruptLock()
{
    lockTouched = true;
}

void FingerprintModule::setFingerprintPower(bool on)
{
    if (scanerHasPower == on)
    {
        return;
    }

    digitalWrite(PWR_PIN, on ? HIGH : LOW);
    logInfoP("Fingerprint power: %s", (on ? "ON" : "OFF"));

    if (on)
    {
        finger.init();
    }

    scanerHasPower = on;
}

void FingerprintModule::loop()
{
    if (touched)
    {
        touched = false;
        logInfoP("Touched");
        setFingerprintPower(true);

        KoFIN_Touched.value(true, DPT_Switch);

        unsigned long captureStart = millis();
        while (millis() - captureStart < CAPTURE_RETRIES_TOUCH_TIMEOUT)
        {
            if (finger.hasFinger())
            {
                Fingerprint::FindFingerResult findFingerResult = finger.findFingerprint();

                if (findFingerResult.found)
                {
                    logInfoP("Finger found in location %d", findFingerResult.location);
                    KoFIN_ScanSuccess.value(true, DPT_Switch);

                    KoFIN_ScanSuccessData.valueNoSend(findFingerResult.location, Dpt(15, 0, 0)); // access identification code
                    KoFIN_ScanSuccessData.valueNoSend(false, Dpt(15, 0, 1));                     // detection error
                    KoFIN_ScanSuccessData.valueNoSend(true, Dpt(15, 0, 2));                      // permission accepted
                    KoFIN_ScanSuccessData.valueNoSend(false, Dpt(15, 0, 3));                     // read direction (not used)
                    KoFIN_ScanSuccessData.valueNoSend(false, Dpt(15, 0, 4));                     // encryption (not used for now)
                    KoFIN_ScanSuccessData.value((uint8_t)0, Dpt(15, 0, 5));                      // index of access identification code (not used)
                }
                else
                {
                    logInfoP("Finger not found");
                    KoFIN_ScanFailed.value(true, DPT_Switch);

                    KoFIN_ScanFailedData.valueNoSend((uint32_t)0, Dpt(15, 0, 0)); // access identification code (unknown)
                    KoFIN_ScanFailedData.valueNoSend(true, Dpt(15, 0, 1));        // detection error
                    KoFIN_ScanFailedData.valueNoSend(false, Dpt(15, 0, 2));       // permission accepted
                    KoFIN_ScanFailedData.valueNoSend(false, Dpt(15, 0, 3));       // read direction (not used)
                    KoFIN_ScanFailedData.valueNoSend(false, Dpt(15, 0, 4));       // encryption (not used for now)
                    KoFIN_ScanFailedData.value((uint8_t)0, Dpt(15, 0, 5));        // index of access identification code (not used)
                }

                resetLedsTimer = millis();
                break;
            }
        }
    }

    if (unlockTouched)
    {
        unlockTouched = false;

        if (isLocked)
        {
            logInfoP("Unlock button touched, unlock requested.");
            lockRequested = false;
        }
        else
        {
            logInfoP("Unlock button touched, but already unlocked.");
            updateLockLeds(true);
        }
    }
    else if (lockTouched)
    {
        lockTouched = false;

        if (!isLocked)
        {
            logInfoP("Lock button touched, lock requested.");
            lockRequested = true;
        }
        else
        {
            logInfoP("Lock button touched, but already locked.");
        }
    }

    if (isLocked != lockRequested)
    {
        if (!isLocked && lockRequested)
        {
            logInfoP("Locking, waiting for finger...");

            unsigned long captureStart = millis();
            finger.setLed(Fingerprint::State::WaitForFinger);
            while (millis() - captureStart < CAPTURE_RETRIES_LOCK_TIMEOUT)
            {
                if (finger.hasFinger())
                {
                    Fingerprint::FindFingerResult findFingerResult = finger.findFingerprint();

                    if (findFingerResult.found)
                    {
                        logInfoP("Finger identified and locked.");
                        isLocked = true;
                    }
                    else
                    {
                        logInfoP("Finger could not be identified.");
                        resetLedsTimer = millis();
                    }

                    break;
                }
            }
        }
        else if (isLocked && !lockRequested)
        {
            logInfoP("Unlocking, waiting for finger...");

            unsigned long captureStart = millis();
            finger.setLed(Fingerprint::State::WaitForFinger);
            while (millis() - captureStart < CAPTURE_RETRIES_LOCK_TIMEOUT)
            {
                if (finger.hasFinger())
                {
                    Fingerprint::FindFingerResult findFingerResult = finger.findFingerprint();

                    if (findFingerResult.found)
                    {
                        logInfoP("Finger identified and unlocked.");
                        isLocked = false;
                    }
                    else
                    {
                        logInfoP("Finger could not be identified.");
                        resetLedsTimer = millis();
                    }

                    break;
                }
            }
        }

        if (isLocked == lockRequested)
        {
            KoFIN_LockStatus.value(isLocked, DPT_Switch);
            updateLockLeds();
        }
        else
        {
            lockRequested = isLocked;
            logInfoP("Not identified and lock status reset.");
            updateLockLeds(false);
        }

        if (resetLedsTimer == 0)
        {
            finger.setLed(Fingerprint::State::None);
        }

        touched = false;
    }

    if (resetLedsTimer > 0 && millis() > resetLedsTimer + 1000)
    {
        finger.setLed(Fingerprint::State::None);
        digitalWrite(LED_GREEN_PIN, LOW);
        resetLedsTimer = 0;
    }
}

void FingerprintModule::updateLockLeds(bool showGreenWhenUnlock)
{
    if (isLocked)
    {
        digitalWrite(LED_GREEN_PIN, LOW);
        digitalWrite(LED_RED_PIN, HIGH);
    }
    else
    {
        if (showGreenWhenUnlock)
        {
            digitalWrite(LED_GREEN_PIN, HIGH);
            resetLedsTimer = millis();
        }
        else
        {
            digitalWrite(LED_GREEN_PIN, LOW);
        }

        digitalWrite(LED_RED_PIN, LOW);
    }
}

void FingerprintModule::processInputKo(GroupObject& iKo)
{
    bool success;
    uint16_t location;

    uint16_t lAsap = iKo.asap();
    switch (lAsap)
    {
        case FIN_KoEnrollReqest:
        case FIN_KoEnrollReqestSlot:
            logInfoP("Enroll request:");
            logIndentUp();

            success = finger.createTemplate();
            if (success)
            {
                if (lAsap == FIN_KoEnrollReqest)
                {
                    location = finger.getNextFreeLocation();
                    logInfoP("Next availabe location: %d", location);
                }
                else
                {
                    location = iKo.value(Dpt(15, 0, 0));
                    logInfoP("Location provided: %d", location);
                }

                success = finger.storeTemplate(location);
                if (!success)
                {
                    logInfoP("Storing template failed.");
                }
            }
            else
            {
                logInfoP("Creating template failed.");
            }

            if (success)
            {
                logInfoP("Enrolled to location %d.", location);

                KoFIN_EnrollSuccess.valueNoSend(location, Dpt(15, 0, 0)); // access identification code
                KoFIN_EnrollSuccess.valueNoSend(false, Dpt(15, 0, 1));    // detection error
                KoFIN_EnrollSuccess.valueNoSend(true, Dpt(15, 0, 2));     // permission accepted
                KoFIN_EnrollSuccess.valueNoSend(false, Dpt(15, 0, 3));    // read direction (not used)
                KoFIN_EnrollSuccess.valueNoSend(false, Dpt(15, 0, 4));    // encryption (not used for now)
                KoFIN_EnrollSuccess.value((uint8_t)0, Dpt(15, 0, 5));     // index of access identification code (not used)
            }
            else
            {
                logInfoP("Enrolling template failed.");

                KoFIN_EnrollFailed.valueNoSend(location, Dpt(15, 0, 0)); // access identification code
                KoFIN_EnrollFailed.valueNoSend(true, Dpt(15, 0, 1));     // detection error
                KoFIN_EnrollFailed.valueNoSend(false, Dpt(15, 0, 2));    // permission accepted
                KoFIN_EnrollFailed.valueNoSend(false, Dpt(15, 0, 3));    // read direction (not used)
                KoFIN_EnrollFailed.valueNoSend(false, Dpt(15, 0, 4));    // encryption (not used for now)
                KoFIN_EnrollFailed.value((uint8_t)0, Dpt(15, 0, 5));     // index of access identification code (not used)
            }

            logIndentDown();
            resetLedsTimer = millis();
            break;
        case FIN_KoDeleteReqestSlot:
            logInfoP("Delete request:");
            logIndentUp();

            location = iKo.value(Dpt(15, 0, 0));
            logInfoP("Location provided: %d", location);

            success = finger.deleteTemplate(location);
            if (success)
            {
                logInfoP("Template deleted from location %d.", location);

                KoFIN_DeleteSuccess.valueNoSend(location, Dpt(15, 0, 0)); // access identification code
                KoFIN_DeleteSuccess.valueNoSend(false, Dpt(15, 0, 1));    // detection error
                KoFIN_DeleteSuccess.valueNoSend(true, Dpt(15, 0, 2));     // permission accepted
                KoFIN_DeleteSuccess.valueNoSend(false, Dpt(15, 0, 3));    // read direction (not used)
                KoFIN_DeleteSuccess.valueNoSend(false, Dpt(15, 0, 4));    // encryption (not used for now)
                KoFIN_DeleteSuccess.value((uint8_t)0, Dpt(15, 0, 5));     // index of access identification code (not used)
            }
            else
            {
                logInfoP("Deleting template failed.");

                KoFIN_DeleteFailed.valueNoSend(location, Dpt(15, 0, 0)); // access identification code
                KoFIN_DeleteFailed.valueNoSend(true, Dpt(15, 0, 1));     // detection error
                KoFIN_DeleteFailed.valueNoSend(false, Dpt(15, 0, 2));    // permission accepted
                KoFIN_DeleteFailed.valueNoSend(false, Dpt(15, 0, 3));    // read direction (not used)
                KoFIN_DeleteFailed.valueNoSend(false, Dpt(15, 0, 4));    // encryption (not used for now)
                KoFIN_DeleteFailed.value((uint8_t)0, Dpt(15, 0, 5));     // index of access identification code (not used)
            }

            logIndentDown();
            resetLedsTimer = millis();
            break;
        case FIN_KoLock:
            lockRequested = KoFIN_Lock.value(DPT_Switch);
            logInfoP("Lock requested: %d", lockRequested);
            break;
        case FIN_KoLockStatus:
            isLocked = KoFIN_LockStatus.value(DPT_Switch);
            logInfoP("Lock status set: %d", isLocked);

            lockRequested = isLocked;
            updateLockLeds();
            break;
    }
}

void FingerprintModule::processAfterStartupDelay()
{
}

FingerprintModule openknxFingerprintModule;

// void FingerprintModule::writeFlash()
// {
//     for (size_t i = 0; i < flashSize(); i++)
//     {
//         //openknx.flash.writeByte(0xd0 + i);
//     }
// }

// void FingerprintModule::readFlash(const uint8_t* data, const uint16_t size)
// {
//     // printHEX("RESTORE:", data,  len);
// }

// uint16_t FingerprintModule::flashSize()
// {
//     return 10;
// }