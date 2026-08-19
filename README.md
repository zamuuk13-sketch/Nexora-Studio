# Nexora

Nexora is a Windows desktop Roblox creation agent plus a Roblox Studio plugin.

## Repository layout

```text
app/        Qt/C++ desktop application and local bridge
plugin/     Roblox Studio plugin source
compilar.bat
```

The plugin and desktop application are deliberately separate.

## Desktop

- Qt Widgets + C++17
- qmake project: `app/Nexora.pro`
- local bridge bound to `127.0.0.1:28473`
- black/white professional workspace UI
- AI profile model (name/provider/model/API key/endpoint/system prompt)
- tool registry for project, model, NPC, UI, animation, scripting and testing operations

## Studio plugin

`plugin/NexoraPlugin.lua` is the standalone Studio plugin entry point. It provides the Nexora toolbar/window and executes bridge commands directly in the connected Studio session.

## Build

Run `compilar.bat` from Windows:

- `compilar.bat doctor` — detect qmake/Qt/g++
- `compilar.bat` — Release build
- `compilar.bat rebuild` — clean + build
- `compilar.bat clean` — remove build output
- `compilar.bat run` — build + launch

The script detects the installed qmake/Qt/g++ toolchain instead of assuming a fixed version.

## Security boundary

The desktop bridge binds only to loopback (`127.0.0.1`). It is not intended to be exposed to the network. API keys are user-provided configuration and are not committed to this repository.

## Current v1 transport flow

```text
Nexora Desktop
    ↓ localhost HTTP
127.0.0.1:28473
    ↓
Nexora Studio Plugin
    ↓
Roblox Studio DataModel
```

Production OAuth/account sync, cloud project storage, and marketplace publication are intentionally separate from the local v1 build and require their own credentials/configuration.
