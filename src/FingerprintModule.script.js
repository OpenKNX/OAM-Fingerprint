function FIN_calcChannelText(input, output, context) {
    var finger;
    switch (input.Finger) {
        case 0:
            finger = "Daumen rechts";
            break;
        case 1:
            finger = "Daumen links";
            break;
        case 2:
            finger = "Zeigefinger rechts";
            break;
        case 3:
            finger = "Zeigefinger links";
            break;
        case 4:
            finger = "Mittelfinger rechts";
            break;
        case 5:
            finger = "Mittelfinger links";
            break;
        case 6:
            finger = "Ringfinger rechts";
            break;
        case 7:
            finger = "Ringfinger links";
            break;
        case 8:
            finger = "Kleiner Finger rechts";
            break;
        case 9:
            finger = "Kleiner Finger links";
            break;
        default:
        // code block
    }
    output.Channel = input.Name + ' - ' + finger;
}

function FIN_dummy(input, output, context) { }

function FIN_enrollFinger(device, online, progress, context) {
    //device.getParameterByName("MTR_Channel" + context.Channel + "Counter").value;
    //var fingerId = 10; // 2 bytes!

    progress.setText("Fingerprint: Finger ID " + fingerId + " anlernen...");
    online.connect();

    // internal function ID
    var data = [1];

    //var data = [1, (fingerId & 0x0000ff00) >> 8, (fingerId & 0x000000ff)];

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

    //var data = [1, 10];
    var resp = online.invokeFunctionProperty(160, 3, data);
    if (resp[0] != 0) {
        throw new Error("Fingerprint: Es ist ein unbekannter Fehler aufgetreten!");
    }

    /*var counterSigned = resp[1];
    var counter = (resp[2] << 24) | (resp[3] << 16) | (resp[4] << 8) | resp[5];
    var referenceSigned = resp[6];
    var reference = (resp[7] << 24) | (resp[8] << 16) | (resp[9] << 8) | resp[10];

    // Convert int-> uint
    if (!counterSigned) { counter = counter >>> 0; }
    if (!referenceSigned) { reference = reference >>> 0; }

    if (counterSigned) {
        device.getParameterByName("MTR_Channel" + context.Channel + "Counter").value = 0;
        device.getParameterByName("MTR_Channel" + context.Channel + "CounterSigned").value = counter;
    } else {
        device.getParameterByName("MTR_Channel" + context.Channel + "Counter").value = counter;
        device.getParameterByName("MTR_Channel" + context.Channel + "CounterSigned").value = 0;
    }
    device.getParameterByName("MTR_Channel" + context.Channel + "Reference").value = reference.toString();
*/
    online.disconnect();
    progress.setText("Fingerprint: Finger ID " + fingerId + " angelernt.");

/*
    // Start read devicetype
    progress.setText(device.getMessage(2));

    var data = [2, context.Channel];
    online.connect();
    var resp = online.invokeFunctionProperty(160, 3, data); //invoke readdevicetype

    if (resp[0] != 0) {
        // Dali Error:
        throw new Error(device.getMessage(1) + String(resp[0]));
    }

    var para = device.getParameterByName("deviceType");
    if (resp[1] == 255)
        throw new Error(device.getMessage(14)) // Unknown DeviceType

    para.value = resp[1].toString();

    if (resp[1] == 6 || resp[1] == 8) {
        var byte = resp[2];

        para = device.getParameterByName("colorSpace");
        para.value = (byte & amp; 1) ?"1" : "0";


        para = device.getParameterByName("colorType");
        para.value = (byte & amp; 2) ?"2" : "1";
    }

    // Read Successfully devicetype
    progress.setProgress(100);
    progress.setText(device.getMessage(3));
*/
}