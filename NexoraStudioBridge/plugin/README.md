# Nexora Studio Bridge plugin

`NexoraStudioBridge.lua` is the current functional Studio-side executor. It provides:

- toolbar button and dock widget
- protocol/version/session handshake
- project/instance inspection
- instance creation/deletion/cloning
- property get/set
- script source editing/deletion
- selection control
- ping/health response
- structured command results and error handling
- ChangeHistory waypoints for mutations

## Transport boundary

The plugin intentionally does not expose a public listener. The desktop transport adapter must provide a local, authenticated transport and invoke the plugin command entry point with decoded requests. This keeps network concerns separate from DataModel mutation logic.

The next desktop integration step will provide the local transport and connect it to the Nexora UI/agent.
