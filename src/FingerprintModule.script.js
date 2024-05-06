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

function FIN_assignFingerId(device, online, progress, context) {
    var parFingerId = device.getParameterByName("FINACT_FingerId");
    var parTargetId = device.getParameterByName("FINACT_fa" + context.Channel + "FingerId");
    parTargetId.value = parFingerId.value;
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

// function FIN_dummy(input, output, context) { }

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