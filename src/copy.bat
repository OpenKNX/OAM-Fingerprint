copy *.cpp C:\Austausch\AB-SmartHouse\FingerprintScanner\client\PlatformIO-KNX\src
for %%i in (*.h) do if not "%%i"=="secrets.h" copy "%%i" C:\Austausch\AB-SmartHouse\FingerprintScanner\client\PlatformIO-KNX\src
copy *.xml C:\Austausch\AB-SmartHouse\FingerprintScanner\client\PlatformIO-KNX\src