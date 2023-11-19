# This script is just a template and has to be copied and modified per project
# This script should be called from .vscode/tasks.json with
#
#   scripts/Build-Release.ps1            - for Beta builds
#   scripts/Build-Release.ps1 Release    - for Release builds
#
# {
#     "label": "Build-Release",
#     "type": "shell",
#     "command": "scripts/Build-Release.ps1 Release",
#     "args": [],
#     "problemMatcher": [],
#     "group": "test"
# },
# {
#     "label": "Build-Beta",
#     "type": "shell",
#     "command": "scripts/Build-Release.ps1 ",
#     "args": [],
#     "problemMatcher": [],
#     "group": "test"
# }

# set product names, allows mapping of (devel) name in Project to a more consistent name in release
# $settings = scripts/OpenKNX-Build-Settings.ps1

# execute generic pre-build steps
../OGM-Common/scripts/setup/reusable/Build-Release-Preprocess.ps1 $args[0]
if (!$?) { exit 1 }

# build firmware based on generated headerfile for SAMD
lib/OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_SAMD firmware-SAMD bin
if (!$?) { exit 1 }

# build firmware for PiPico-BCU-Connector
lib/OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_PiPico_BCU_Connector firmware-PiPico-BCU-Connector uf2
if (!$?) { exit 1 }

# build firmware for Masifi-Sensor_Breakout
lib/OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_MASIFI_SENSOR_BREAKOUT firmware-Masifi-Sensor_Breakout uf2
if (!$?) { exit 1 }

# build firmware based on generated headerfile for ESP32
../OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_ESP32 firmware-ESP32 bin
if (!$?) { exit 1 }

# execute generic post-build steps
../OGM-Common/scripts/setup/reusable/Build-Release-Postprocess.ps1 $args[0]
if (!$?) { exit 1 }
