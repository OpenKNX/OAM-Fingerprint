function FIN_searchFingerId(device, online, progress, context) {
    var parPersonName = device.getParameterByName("FINACT_PersonName");
    var parPersonFinger = device.getParameterByName("FINACT_PersonFinger");
    var parFingerId = device.getParameterByName("FINACT_FingerId");
    parFingerId.value = 123;
}

function FIN_searchUser(device, online, progress, context) {
    var parPersonName = device.getParameterByName("FINACT_PersonName");
    var parPersonFinger = device.getParameterByName("FINACT_PersonFinger");
    parPersonName.value = "Waldemar";
    parPersonFinger.value = 2;
}

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

function FIN_enrollFinger(device, online, progress, context) {
    progress.setText("Fingerprint: Finger ID " + fingerId + " anlernen...");
    online.connect();

    // internal function ID
    var data = [1];

    // finger ID
    var fingerId = device.getParameterByName("FIN_FingerId").value;
    data = data.concat((fingerId & 0x0000ff00) >> 8, (fingerId & 0x000000ff));

    // person finger
    var personFinger = device.getParameterByName("FIN_PersonFinger").value;
    data = data.concat((personFinger & 0x000000ff));

    // person name
    var personName = device.getParameterByName("FIN_PersonName").value;
    for (var i = 0; i < personName.length; ++i) {
      var code = personName.charCodeAt(i);
      data = data.concat([code]);
    }
    data = data.concat(0); // null-terminated string

    //info(personName);
    //info(personFinger);
    //info(fingerId);

    var resp = online.invokeFunctionProperty(160, 3, data);
    if (resp[0] != 0) {
        throw new Error("Fingerprint: Es ist ein unbekannter Fehler aufgetreten!");
    }

    online.disconnect();
    progress.setText("Fingerprint: Finger ID " + fingerId + " angelernt.");
}