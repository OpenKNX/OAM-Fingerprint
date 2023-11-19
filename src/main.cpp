#include "FingerprintModule.h"
#include "Logic.h"
#include "VirtualButtonModule.h"
#include "FileTransferModule.h"

void setup()
{
    const uint8_t firmwareRevision = 0;
    openknx.init(firmwareRevision);
    openknx.addModule(1, new Logic());
    openknx.addModule(2, openknxFingerprintModule);
    openknx.addModule(3, new VirtualButtonModule());
    openknx.addModule(9, new FileTransferModule());
    openknx.setup();

    // call direct for testing without KNX connected
    // openknx.modules.list[1]->setup();
}

void loop()
{
    openknx.loop();

    // call direct for testing without KNX connected
    // openknx.modules.list[1]->loop();
}