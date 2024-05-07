// function FIN_calcChannelText(input, output, context) {
//     var finger;
//     switch (input.Finger) {
//         case 0:
//             finger = "Daumen rechts";
//             break;
//         case 1:
//             finger = "Daumen links";
//             break;
//         case 2:
//             finger = "Zeigefinger rechts";
//             break;
//         case 3:
//             finger = "Zeigefinger links";
//             break;
//         case 4:
//             finger = "Mittelfinger rechts";
//             break;
//         case 5:
//             finger = "Mittelfinger links";
//             break;
//         case 6:
//             finger = "Ringfinger rechts";
//             break;
//         case 7:
//             finger = "Ringfinger links";
//             break;
//         case 8:
//             finger = "Kleiner Finger rechts";
//             break;
//         case 9:
//             finger = "Kleiner Finger links";
//             break;
//         default:
//         // code block
//     }
//     output.Channel = input.Name + ' - ' + finger;
// }

// function FIN_refreshFingerAction(input, output, context) {
//     // var numActionsMax = device.getParameterByName("FIN_VisibleChannels").value;
//     info(input.ActionActive);
//     info(output.FingerActionCount);
//     info(context);
//     var parFingerActionCount = output.FingerActionCount;
//     var numFingerActionCount = parFingerActionCount.value;
//     info(numFingerActionCount);
//     var fingerActionIterator = 1;
//     // info(context.Action);
//     var lastAction = 0;
//     for (var i = 1; i < context.Action; i++) {
//         info("FIN_Channel%C%Active".replace("%C%", i));
//         var actionActive = device.getParameterByName("FIN_Channel%C%Active".replace("%C%", i)).value;
//         if (actionActive > 0) {
//             var parActionId = device.getParameterByName("FINACT_fa%C%ActionId".replace("%C%", fingerActionIterator));
//             // first check, if we are at the end of the list
//             if (fingerActionIterator > numFingerActionCount) {
//                 // we add a new line to finger-action array
//                 parFingerActionCount.value = fingerActionIterator;
//                 parActionId.value = i;
//                 break;
//             }
//             if (parActionId.value == i) {
//                 lastAction = i;
//                 fingerActionIterator++;
//             } else if (parActionId.value > i) {
//                 fingerActionIterator++;
//             } else {
//                 fingerActionIterator++;
//             }
//         }
//     }
// }

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

// function FIN_calcFingerIdsToDataBlock(device, input, output, context) {
//     //var channelBytes = [(context.Channel & 0x0000ff00) >> 8, (context.Channel & 0x000000ff)];
//     var channelBytes = [(1 & 0x0000ff00) >> 8, (1 & 0x000000ff)];

//     info(input.FingerIds);
//     info(input);
//     var fingerIds = input.FingerIds.split(',');
//     info(fingerIds.length);

//     var dataBlock = device.getParameterByName("FIN_DataBlock").value;
//     info(dataBlock.length);

//     // first clear all entries for current channel
//     for (var i = 0; i < 2048; ++i) {
//         var offset = i * 4;

//         if (dataBlock.charCodeAt(offset) == channelBytes[0] &&
//             dataBlock.charCodeAt(offset + 1) == channelBytes[1]) {
//             dataBlock = dataBlock.substring(0, offset) + "0000" + dataBlock.substring(offset + 4);
//             i += 4;
//         }
//     }

//     if (fingerIds.length > 0) {
//         // now add new channel entries where space is left
//         var currentFinger = 0;
//         var currentFingerBytes = [(fingerIds[currentFinger] & 0x0000ff00) >> 8, (fingerIds[currentFinger] & 0x000000ff)];
//         for (var i = 0; i < 2048; ++i) {
//             var offset = i * 4;

//             // if second byte is 0 (LSB of channel number), we found a space
//             if (dataBlock.charCodeAt(offset + 1) == 0) {
//                 dataBlock = dataBlock.substring(0, offset)
//                     + String.fromCharCode(channelBytes[0])
//                     + String.fromCharCode(channelBytes[1])
//                     + String.fromCharCode(currentFingerBytes[0])
//                     + String.fromCharCode(currentFingerBytes[1])
//                     + dataBlock.substring(offset + 4);

//                 currentFinger++;
//                 if (currentFinger == fingerIds.length)
//                     break;

//                 currentFingerBytes = [(fingerIds[currentFinger] & 0x0000ff00) >> 8, (fingerIds[currentFinger] & 0x000000ff)];
//                 i += 4;
//             }
//         }
//     }

//     device.getParameterByName("FIN_DataBlock").value = dataBlock;
// }

function FIN_clear(input, output, context) {
    output.FingerActionInfo = "";
}

// function FIN_syncR503Pro(input, output, context) {
//     if (input.R503Pro.value <= 149) {
//         if (output.R503S.value != input.R503Pro.value)
//             output.R503S.value = input.R503Pro.value;
//         if (output.R503.value != input.R503Pro.value)
//             output.R503.value = input.R503Pro.value;
//     } else if (input.R503Pro.value <= 199) {
//         if (output.R503.value != input.R503Pro.value)
//             output.R503.value = input.R503Pro.value;
//     }
// }

// function FIN_syncR503(input, output, context) {
//     if (output.R503Pro.value != input.R503.value)
//         output.R503Pro.value = input.R503.value;
// }

// function FIN_syncR503S(input, output, context) {
//     if (output.R503Pro.value != input.R503S.value)
//         output.R503Pro.value = input.R503S.value;
// }

function FIN_syncFingerIdLR(input, output, context) {
    info("syncFingerIdLR");
    info(output.Scanner);
    info(input.R503Pro);
    info(output.R503S);
    info(output.R503);
    if (output.Scanner == 0 && input.R503Pro <= 149)
        output.R503S = input.R503Pro;
    else if (output.Scanner == 1 && input.R503Pro <= 199)
        output.R503 = input.R503Pro;
}

function FIN_syncFingerIdRL(input, output, context) {
    info("syncFingerIdRL");
    info(input.Scanner);
    info(output.R503Pro);
    info(input.R503S);
    info(input.R503);
    if (input.Scanner == 0)
        output.R503Pro = input.R503S;
    else if (input.Scanner == 1)
        output.R503Pro = input.R503;

}

function FIN_checkFingerIdRange(input, changed, prevValue, context) {
    // if (changed == "FingerID") {
    var limit = [149, 199, 1499];
    if (input.FingerID > limit[input.Scanner])
        return "FingerId ist " + input.FingerID + ", aber der Fingerscanner kann nur " + limit[input.Scanner] + " Finger verwalten.";
    // }
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