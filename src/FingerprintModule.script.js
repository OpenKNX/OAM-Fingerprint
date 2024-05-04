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

    /*
    // Start read devicetype
    progress.setText(device.getMessage(2));

    var data = [2, context.Channel];
    online.connect();
    var resp = online.invokeFunctionProperty(160, 1, data); //invoke readdevicetype

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