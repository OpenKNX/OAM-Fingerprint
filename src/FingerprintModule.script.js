function FIN_sort(device, online, progress, context) {
    var parFingerActionCount = device.getParameterByName("FINACT_FingerActionCount");
    // first get rid of zero lines
    var endIndex = parFingerActionCount.value;
    var startIndex = 0;
    while (endIndex - 1 > startIndex + 1) {
        do {
            endIndex--;
            var parEndAction = device.getParameterByName("FINACT_fa" + endIndex + "ActionId");
        } while (parEndAction.value == 0);
        do {
            startIndex++;
            var parStartAction = device.getParameterByName("FINACT_fa" + startIndex + "ActionId");
        } while (parStartAction.value != 0);
        // now startIndex points to a zero entry and endIndex to a non-zero entry, we move end to start
        if (endIndex > startIndex) {
            var parStartFinger = device.getParameterByName("FINACT_fa" + startIndex + "FingerId");
            var parEndFinger = device.getParameterByName("FINACT_fa" + endIndex + "FingerId");
            parStartAction.value = parEndAction.value;
            parStartFinger.value = parEndFinger.value;
            parEndAction.value = 0;
            parEndFinger.value = 0;
        }
    }
    // now do bubble sort
    do {
        var continueSort = false;
        for (var current = 1; current < parFingerActionCount.value; current++) {
            var parCurrAction = device.getParameterByName("FINACT_fa" + current + "ActionId");
            var parNextAction = device.getParameterByName("FINACT_fa" + (current + 1) + "ActionId");
            var parCurrFinger = device.getParameterByName("FINACT_fa" + current + "FingerId");
            var parNextFinger = device.getParameterByName("FINACT_fa" + (current + 1) + "FingerId");
            if (parNextAction.value == 0) { break; }
            var swap = (parCurrAction.value > parNextAction.value || (parCurrAction.value == parNextAction.value && parCurrFinger.value > parNextFinger.value));
            if (swap) {
                continueSort = true;
                var tmpAction = parCurrAction.value;
                var tmpFinger = parCurrFinger.value;
                parCurrAction.value = parNextAction.value;
                parCurrFinger.value = parNextFinger.value;
                parNextAction.value = tmpAction;
                parNextFinger.value = tmpFinger;
            }
        }
    } while (continueSort);
}

function FIN_assignFingerId(device, online, progress, context) {
    var parFingerId = device.getParameterByName("FINACT_FingerId");
    var parFingerActionLine = device.getParameterByName("FINACT_FingerActionLine");
    var parTargetId = device.getParameterByName("FINACT_fa" + parFingerActionLine.value + "FingerId");
    parTargetId.value = parFingerId.value;
}

function FIN_checkFingerAction(device, online, progress, context) {
    var parActionId = device.getParameterByName("FINACT_fa" + context.Channel + "ActionId");
    var parFingerId = device.getParameterByName("FINACT_fa" + context.Channel + "FingerId");
    var parFingerActionInfo = device.getParameterByName("FINACT_fa" + context.Channel + "FingerActionInfo");
    var parVisibleActions = device.getParameterByName("FIN_VisibleActions");
    if (parActionId.value <= parVisibleActions.value) {
        var parActionDescription = device.getParameterByName("FIN_Action" + parActionId.value + "Description");
        parFingerActionInfo.value = (parActionDescription.value + "; Waldemar; Zeigefinger rechts").substring(0, 80);
    } else {
        parFingerActionInfo.value = "Aktion ist nicht definiert, Finger wurde nicht ermittelt";
    }
}

function FIN_clear(input, output, context) {
    output.FingerActionInfo = "";
}

function FIN_checkFingerIdRange(input, changed, prevValue, context) {
    var limit = [149, 199, 1499];
    if (input.FingerID > limit[input.Scanner]) {
        if (changed == "FingerID") {
            return "FingerId ist " + input.FingerID + ", aber der Fingerscanner kann nur " + limit[input.Scanner] + " Finger verwalten.";
        } else {
            return "Auf Finger-Seite gibt es FingerIds > " + limit[input.Scanner] + ". Hardware-Scanner kann erst geändert werden, wenn es keine ungültigen FingerIds für die neue Hardwareauswahl mehr gibt.";
        }
    }
    return true;
}

function FIN_dummy(input, output, context) { }

function FIN_searchFingerId(device, online, progress, context) {
    var parPersonName = device.getParameterByName("FINACT_PersonName");
    var parPersonFinger = device.getParameterByName("FINACT_PersonFinger");
    var parFingerId = device.getParameterByName("FINACT_FingerId");

    progress.setText("Fingerprint: Finger ID zu Person " + parPersonName + " (" + parPersonFinger + ") suchen...");
    online.connect();

    var data = [12]; // internal function ID

    // person finger
    data = data.concat((parPersonFinger & 0x000000ff));

    // person name
    for (var i = 0; i < parPersonName.length; ++i) {
        var code = parPersonName.charCodeAt(i);
        data = data.concat([code]);
    }
    data = data.concat(0); // null-terminated string

    info(parPersonFinger);
    info(parPersonName);

    var resp = online.invokeFunctionProperty(160, 3, data);
    if (resp[0] != 0) {
        throw new Error("Fingerprint: Finger ID zu Person " + parPersonName + " (" + parPersonFinger + ") nicht gefunden.");
    }

    online.disconnect();
    progress.setText("Fingerprint: Finger ID zu Person " + parPersonName + " (" + parPersonFinger + ") gefunden.");

    var fingerId = resp[1] << 8 | resp[2];

    parFingerId.value = fingerId;
}

function FIN_searchUser(device, online, progress, context) {
    var parPersonName = device.getParameterByName("FINACT_PersonName");
    var parPersonFinger = device.getParameterByName("FINACT_PersonFinger");
    var parFingerId = device.getParameterByName("FINACT_FingerId");

    progress.setText("Fingerprint: Person zu Finger ID " + parFingerId + " suchen...");
    online.connect();

    var data = [11]; // internal function ID
    data = data.concat((parFingerId & 0x0000ff00) >> 8, (parFingerId & 0x000000ff));

    info(parFingerId);

    var resp = online.invokeFunctionProperty(160, 3, data);
    if (resp[0] != 0) {
        progress.setText("Fingerprint: Person zu Finger ID " + parFingerId + " nicht gefunden.");

        parPersonName.value = "";
        parPersonFinger.value = 0;
    }

    online.disconnect();
    progress.setText("Fingerprint: Person zu Finger ID " + parFingerId + " gefunden.");

    var personFinger = resp[1];
    var personName = "";
    for (var i = 2; i < resp.length; ++i) {
        if (resp[i] == 0)
            break; // null-termination
      
        personName += String.fromCharCode(resp[i]);
    }

    parPersonName.value = personName;
    parPersonFinger.value = personFinger;
}

function FIN_enrollFinger(device, online, progress, context) {
    var parFingerId = device.getParameterByName("FIN_FingerId").value;

    progress.setText("Fingerprint: Finger ID " + parFingerId + " anlernen...");
    online.connect();

    var data = [1]; // internal function ID

    // finger ID
    data = data.concat((parFingerId & 0x0000ff00) >> 8, (parFingerId & 0x000000ff));

    // person finger
    var parPersonFinger = device.getParameterByName("FIN_PersonFinger").value;
    data = data.concat((parPersonFinger & 0x000000ff));

    // person name
    var parPersonName = device.getParameterByName("FIN_PersonName").value;
    for (var i = 0; i < parPersonName.length; ++i) {
        var code = parPersonName.charCodeAt(i);
        data = data.concat([code]);
    }
    data = data.concat(0); // null-terminated string

    //info(parPersonName);
    //info(parPersonFinger);
    //info(parFingerId);

    var resp = online.invokeFunctionProperty(160, 3, data);
    if (resp[0] != 0) {
        throw new Error("Fingerprint: Es ist ein unbekannter Fehler aufgetreten!");
    }

    online.disconnect();
    progress.setText("Fingerprint: Finger ID " + parFingerId + " angelernt.");
}

function FIN_deleteFinger(device, online, progress, context) {
    var parFingerId = device.getParameterByName("FIN_FingerId").value;

    progress.setText("Fingerprint: Finger ID " + parFingerId + " löschen...");
    online.connect();

    // internal function ID
    var data = [2];
    data = data.concat((parFingerId & 0x0000ff00) >> 8, (parFingerId & 0x000000ff));

    var resp = online.invokeFunctionProperty(160, 3, data);
    if (resp[0] != 0) {
        throw new Error("Fingerprint: Es ist ein unbekannter Fehler aufgetreten!");
    }

    online.disconnect();
    progress.setText("Fingerprint: Finger ID " + parFingerId + " gelöscht.");
}