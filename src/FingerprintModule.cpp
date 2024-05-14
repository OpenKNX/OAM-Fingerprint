#include "FingerprintModule.h"

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
    _fingerprintStorage.init("fingerprint", FINGERPRINT_FLASH_OFFSET, FINGERPRINT_FLASH_SIZE);

    finger = Fingerprint(delayCallback, FINGERPRINT_PASSWORD);

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

    for (uint16_t i = 0; i < ParamFIN_VisibleActions; i++)
    {
        _channels[i] = new ActionChannel(i, finger);
        _channels[i]->setup();
    }

    resetLedsTimer = delayTimerInit();
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
    if (delayCallbackActive)
        return;

    if (touched)
    {
        touched = false;
        logInfoP("Touched");
        setFingerprintPower(true);

        KoFIN_Touched.value(true, DPT_Switch);

        unsigned long captureStart = delayTimerInit();
        while (!delayCheck(captureStart, CAPTURE_RETRIES_TOUCH_TIMEOUT))
        {
            if (finger.hasFinger())
            {
                Fingerprint::FindFingerResult findFingerResult = finger.findFingerprint();

                if (findFingerResult.found)
                {
                    logInfoP("Finger found in location %d", findFingerResult.location);
                    processScanSuccess(findFingerResult.location);
                }
                else
                {
                    finger.setLed(Fingerprint::ScanNoMatch);

                    logInfoP("Finger not found");
                    KoFIN_ScanFailed.value(true, DPT_Switch);

                    KoFIN_ScanFailedData.valueNoSend((uint32_t)0, Dpt(15, 1, 0)); // access identification code (unknown)
                    KoFIN_ScanFailedData.valueNoSend(true, Dpt(15, 1, 1));        // detection error
                    KoFIN_ScanFailedData.valueNoSend(false, Dpt(15, 1, 2));       // permission accepted
                    KoFIN_ScanFailedData.valueNoSend(false, Dpt(15, 1, 3));       // read direction (not used)
                    KoFIN_ScanFailedData.valueNoSend(false, Dpt(15, 1, 4));       // encryption (not used for now)
                    KoFIN_ScanFailedData.value((uint8_t)0, Dpt(15, 1, 5));        // index of access identification code (not used)
                }

                resetLedsTimer = delayTimerInit();
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

            unsigned long captureStart = delayTimerInit();
            finger.setLed(Fingerprint::State::WaitForFinger);
            while (!delayCheck(captureStart, CAPTURE_RETRIES_LOCK_TIMEOUT))
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
                        resetLedsTimer = delayTimerInit();
                    }

                    break;
                }
            }
        }
        else if (isLocked && !lockRequested)
        {
            logInfoP("Unlocking, waiting for finger...");

            unsigned long captureStart = delayTimerInit();
            finger.setLed(Fingerprint::State::WaitForFinger);
            while (!delayCheck(captureStart, CAPTURE_RETRIES_LOCK_TIMEOUT))
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
                        resetLedsTimer = delayTimerInit();
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

    if (resetLedsTimer > 0 && delayCheck(resetLedsTimer, LED_RESET_TIMEOUT))
    {
        finger.setLed(Fingerprint::State::None);
        digitalWrite(LED_GREEN_PIN, LOW);
        resetLedsTimer = 0;
    }

    for (uint16_t i = 0; i < ParamFIN_VisibleActions; i++)
        _channels[i]->loop();
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
            resetLedsTimer = delayTimerInit();
        }
        else
        {
            digitalWrite(LED_GREEN_PIN, LOW);
        }

        digitalWrite(LED_RED_PIN, LOW);
    }
}

void FingerprintModule::processScanSuccess(uint16_t location, bool external)
{
    KoFIN_ScanSuccess.value(true, DPT_Switch);
    KoFIN_ScanSuccessId.value(location, Dpt(7, 1));

    KoFIN_ScanSuccessData.valueNoSend(location, Dpt(15, 1, 0)); // access identification code
    KoFIN_ScanSuccessData.valueNoSend(false, Dpt(15, 1, 1));    // detection error
    KoFIN_ScanSuccessData.valueNoSend(true, Dpt(15, 1, 2));     // permission accepted
    KoFIN_ScanSuccessData.valueNoSend(false, Dpt(15, 1, 3));    // read direction (not used)
    KoFIN_ScanSuccessData.valueNoSend(false, Dpt(15, 1, 4));    // encryption (not used for now)
    KoFIN_ScanSuccessData.value((uint8_t)0, Dpt(15, 1, 5));     // index of access identification code (not used)

    bool actionFound = false;
    for (size_t i = 0; i < ParamFINACT_FingerActionCount; i++)
    {
        uint16_t fingerId = knx.paramWord(FINACT_faFingerId + FINACT_ParamBlockOffset + i * FINACT_ParamBlockSize);
        if (fingerId == location)
        {
            uint16_t actionId = knx.paramWord(FINACT_faActionId + FINACT_ParamBlockOffset + i * FINACT_ParamBlockSize) - 1;
            if (actionId < FIN_VisibleActions)
            {
                _channels[actionId]->processScan(location);
                actionFound = true;
            }
            else
                logInfoP("Invalid ActionId: %d", actionId);
        }
    }

    if (actionFound)
    {
        if (!external)
            finger.setLed(Fingerprint::ScanMatch);
    }
    else
    {
        if (external)
            finger.setLed(Fingerprint::ScanMatchNoAction);
        
        KoFIN_TouchedNoAction.value(true, DPT_Switch);
    }
}

bool FingerprintModule::enrollFinger(uint16_t location)
{
    logInfoP("Enroll request:");
    logIndentUp();

    bool success = finger.createTemplate();
    if (success)
    {
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
        KoFIN_EnrollSuccess.value(true, DPT_Switch);
        KoFIN_EnrollSuccessId.value(location, Dpt(7, 1));

        KoFIN_EnrollSuccess.valueNoSend(location, Dpt(15, 1, 0)); // access identification code
        KoFIN_EnrollSuccess.valueNoSend(false, Dpt(15, 1, 1));    // detection error
        KoFIN_EnrollSuccess.valueNoSend(true, Dpt(15, 1, 2));     // permission accepted
        KoFIN_EnrollSuccess.valueNoSend(false, Dpt(15, 1, 3));    // read direction (not used)
        KoFIN_EnrollSuccess.valueNoSend(false, Dpt(15, 1, 4));    // encryption (not used for now)
        KoFIN_EnrollSuccess.value((uint8_t)0, Dpt(15, 1, 5));     // index of access identification code (not used)
    }
    else
    {
        logInfoP("Enrolling template failed.");
        KoFIN_EnrollFailed.value(true, DPT_Switch);
        KoFIN_EnrollFailedId.value(location, Dpt(7, 1));

        KoFIN_EnrollFailed.valueNoSend(location, Dpt(15, 1, 0)); // access identification code
        KoFIN_EnrollFailed.valueNoSend(true, Dpt(15, 1, 1));     // detection error
        KoFIN_EnrollFailed.valueNoSend(false, Dpt(15, 1, 2));    // permission accepted
        KoFIN_EnrollFailed.valueNoSend(false, Dpt(15, 1, 3));    // read direction (not used)
        KoFIN_EnrollFailed.valueNoSend(false, Dpt(15, 1, 4));    // encryption (not used for now)
        KoFIN_EnrollFailed.value((uint8_t)0, Dpt(15, 1, 5));     // index of access identification code (not used)
    }

    logIndentDown();
    resetLedsTimer = delayTimerInit();

    return success;
}

bool FingerprintModule::deleteFinger(uint16_t location)
{
    logInfoP("Delete request:");
    logIndentUp();

    bool success = finger.deleteTemplate(location);
    if (success)
    {
        logInfoP("Template deleted from location %d.", location);
        KoFIN_DeleteSuccess.value(true, DPT_Switch);
        KoFIN_DeleteSuccessId.value(location, Dpt(7, 1));

        KoFIN_DeleteSuccess.valueNoSend(location, Dpt(15, 1, 0)); // access identification code
        KoFIN_DeleteSuccess.valueNoSend(false, Dpt(15, 1, 1));    // detection error
        KoFIN_DeleteSuccess.valueNoSend(true, Dpt(15, 1, 2));     // permission accepted
        KoFIN_DeleteSuccess.valueNoSend(false, Dpt(15, 1, 3));    // read direction (not used)
        KoFIN_DeleteSuccess.valueNoSend(false, Dpt(15, 1, 4));    // encryption (not used for now)
        KoFIN_DeleteSuccess.value((uint8_t)0, Dpt(15, 1, 5));     // index of access identification code (not used)
    }
    else
    {
        logInfoP("Deleting template failed.");
        KoFIN_DeleteFailed.value(true, DPT_Switch);
        KoFIN_DeleteFailedId.value(location, Dpt(7, 1));

        KoFIN_DeleteFailed.valueNoSend(location, Dpt(15, 1, 0)); // access identification code
        KoFIN_DeleteFailed.valueNoSend(true, Dpt(15, 1, 1));     // detection error
        KoFIN_DeleteFailed.valueNoSend(false, Dpt(15, 1, 2));    // permission accepted
        KoFIN_DeleteFailed.valueNoSend(false, Dpt(15, 1, 3));    // read direction (not used)
        KoFIN_DeleteFailed.valueNoSend(false, Dpt(15, 1, 4));    // encryption (not used for now)
        KoFIN_DeleteFailed.value((uint8_t)0, Dpt(15, 1, 5));     // index of access identification code (not used)
    }

    logIndentDown();
    resetLedsTimer = delayTimerInit();

    return success;
}

void FingerprintModule::processInputKo(GroupObject& iKo)
{
    uint16_t location;

    uint16_t lAsap = iKo.asap();
    switch (lAsap)
    {
        case FIN_KoEnrollNext:
        case FIN_KoEnrollId:
        case FIN_KoEnrollData:
            if (lAsap == FIN_KoEnrollNext)
            {
                location = finger.getNextFreeLocation();
                logInfoP("Next availabe location: %d", location);
            }
            else if (lAsap == FIN_KoEnrollId)
            {
                location = iKo.value(Dpt(7, 1));
                logInfoP("Location provided: %d", location);
            }
            else
            {
                location = iKo.value(Dpt(15, 1, 0));
                logInfoP("Location provided: %d", location);
            }

            enrollFinger(location);
            break;
        case FIN_KoDeleteId:
        case FIN_KoDeleteData:
            if (lAsap == FIN_KoDeleteId)
            {
                location = iKo.value(Dpt(7, 1));
                logInfoP("Location provided: %d", location);
            }
            else
            {
                location = iKo.value(Dpt(15, 1, 0));
                logInfoP("Location provided: %d", location);
            }

            deleteFinger(location);
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
        case FIN_KoExternFingerId:
            location = iKo.value(Dpt(7, 1));
            logInfoP("FingerID received: %d", location);

            processScanSuccess(location, true);
            break;
        default:
        {
            for (uint16_t i = 0; i < ParamFIN_VisibleActions; i++)
                _channels[i]->processInputKo(iKo);
        }
    }
}

bool FingerprintModule::processFunctionProperty(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    if (!knx.configured() || objectIndex != 160 || propertyId != 3)
        return false;

    switch(data[0])
    {
        case 1:
            handleFunctionPropertyEnrollFinger(data, resultData, resultLength);
            return true;
        case 2:
            handleFunctionPropertyDeleteFinger(data, resultData, resultLength);
            return true;
        case 11:
            handleFunctionPropertySearchPersonByFingerId(data, resultData, resultLength);
            return true;
        case 12:
            handleFunctionPropertySearchFingerIdByPerson(data, resultData, resultLength);
            return true;
    }

    return false;
}

void FingerprintModule::handleFunctionPropertyEnrollFinger(uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    logInfoP("Function property: Enroll request");

    uint16_t fingerId = (data[1] << 8) | data[2];
    logDebugP("fingerId: %d", fingerId);

    uint8_t personFinger = data[3];
    logDebugP("personFinger: %d", personFinger);

    char personName[28] = {};
    for (size_t i = 0; i < 28; i++)
    {
        memcpy(personName + i, data + 4 + i, 1);
        if (personName[i] == 0) // null termination
            break;
    }
    logDebugP("personName: %s", personName);

    uint32_t storageOffset = FIN_CaclStorageOffset(fingerId);
    logDebugP("storageOffset: %d", storageOffset);
    _fingerprintStorage.writeByte(storageOffset, personFinger); // only 4 bits used
    _fingerprintStorage.write(storageOffset + 1, *personName, 28);
    _fingerprintStorage.commit();

    //bool success = enrollFinger(fingerId);
    bool success = true;
    
    resultData[0] = success ? 0 : 1;
    resultLength = 1;
}

void FingerprintModule::handleFunctionPropertyDeleteFinger(uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    logInfoP("Function property: Delete request");

    uint16_t fingerId = (data[1] << 8) | data[2];
    logDebugP("fingerId: %d", fingerId);

    char personName[28] = {}; // empty

    uint32_t storageOffset = FIN_CaclStorageOffset(fingerId);
    _fingerprintStorage.writeByte(storageOffset, 0); // "0" for not set
    _fingerprintStorage.write(storageOffset + 1, *personName, 28);
    _fingerprintStorage.commit();

    bool success = deleteFinger(fingerId);
    
    resultData[0] = success ? 0 : 1;
    resultLength = 1;
}

void FingerprintModule::handleFunctionPropertySearchPersonByFingerId(uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    logInfoP("Function property: Search person by FingerId");
    logIndentUp();

    uint16_t fingerId = (data[1] << 8) | data[2];
    logDebugP("fingerId: %d", fingerId);

    uint8_t* personName[28] = {};

    uint32_t storageOffset = FIN_CaclStorageOffset(fingerId);
    uint8_t personFinger = _fingerprintStorage.readByte(storageOffset);
    if (personFinger > 0)
    {
        _fingerprintStorage.read(storageOffset + 1, *personName, 28);

        logDebugP("Found:");
        logIndentUp();
        logDebugP("personFinger: %d", personFinger);
        logDebugP("personName: %s", personName);
        logIndentDown();

        resultData[0] = 1;
        resultData[1] = personFinger;
        resultLength = 2;
        for (size_t i = 0; i < 28; i++)
        {
            memcpy(resultData + 2 + i, personName + i, 1);
            resultLength++;

            if (personName[i] == 0) // null termination
                break;
        }
    }
    else
    {
        logDebugP("Not found.");

        resultData[0] = 0;
        resultLength = 1;
    }

    logIndentDown();
}

void FingerprintModule::handleFunctionPropertySearchFingerIdByPerson(uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    logInfoP("Function property: Search FingerId(s) by person");
    logIndentUp();

    uint8_t searchPersonFinger = data[1];
    logDebugP("searchPersonFinger: %d", searchPersonFinger); // can be "0" if only by name should be searched

    char searchPersonName[28] = {};
    uint8_t searchPersonNameLength = 28;
    for (size_t i = 0; i < 28; i++)
    {
        memcpy(searchPersonName + i, data + 2 + i, 1);
        if (searchPersonName[i] == 0) // null termination
        {
            searchPersonNameLength = i + 1;
            break;
        }
    }
    logDebugP("searchPersonName: %s", searchPersonName);

    uint32_t storageOffset = 0;
    uint8_t personFinger = 0;
    uint8_t* personName[28] = {};
    uint8_t foundCount = 0;
    for (size_t fingerId = 0; fingerId < MAX_FINGERS; fingerId++)
    {
        storageOffset = FIN_CaclStorageOffset(fingerId);
        if (searchPersonFinger > 0)
        {
            personFinger = _fingerprintStorage.readByte(storageOffset);
            if (searchPersonFinger != personFinger)
                continue;
        }

        _fingerprintStorage.read(storageOffset + 1, *personName, 28);
        if (memcmp(personName, searchPersonName, searchPersonNameLength) == 0)
        {
            logDebugP("Found:");
            logIndentUp();
            logDebugP("personFinger: %d", personFinger);
            logDebugP("personName: %s", personName);
            logIndentDown();

            resultData[1 + foundCount * 31] = fingerId >> 8;
            resultData[1 + foundCount * 31 + 1] = fingerId;
            resultData[1 + foundCount * 31 + 2] = personFinger;
            memcpy(resultData + 1 + foundCount * 31 + 3, personName, 28);

            foundCount++;
            if (foundCount == 10)
                break; // we return max. 10 results
        }
    }
    
    resultData[0] = foundCount > 0;
    resultLength = 1 + foundCount * 31;

    logIndentDown();
}

void FingerprintModule::processAfterStartupDelay()
{
}

void FingerprintModule::delayCallback(uint32_t period)
{
    uint32_t start = delayTimerInit();
    delayCallbackActive = true;

    while (!delayCheck(start, period))
        openknx.loop();

    openknx.common.skipLooptimeWarning();
    delayCallbackActive = false;
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