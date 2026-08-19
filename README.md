# LemonadeAI V1

Desktop Roblox creation agent built with Qt/C++ and qmake.

## Architecture
- Qt/QML desktop UI
- Gemini API client using the user's own API key
- Configurable AI profile (name, instructions)
- Agent layer and tool registry
- Local Roblox Studio bridge endpoint
- Build/doctor workflow through `compilar.bat`

## Build
Run `compilar.bat` on Windows with Qt + MinGW/qmake available in PATH.

Commands:
- `compilar.bat` — build Release
- `compilar.bat doctor` — inspect qmake/Qt/g++/make
- `compilar.bat clean` — remove build directory
- `compilar.bat rebuild` — clean then build
- `compilar.bat run` — build and launch

## Important
The Google account/cloud project system and the Studio-side HTTP transport need real OAuth/backend credentials and Studio transport integration before they can be called production-ready. This repository deliberately does not contain secrets or pretend those integrations are complete.
