# Installing Nexora Studio Bridge

## Development installation

1. Open Roblox Studio.
2. Open the **Plugins** tab.
3. Use **Plugins > Plugin Folder** to open Roblox Studio's local plugin folder.
4. Copy the NexoraStudioBridge plugin package into that folder.
5. Restart Roblox Studio.
6. The **Nexora** toolbar should appear under the Plugins tab.

> The repository currently contains the source layout for the bridge. A packaged `.rbxm`/`.rbxmx` plugin artifact will be produced once the bridge UI and desktop transport are implemented.

## Important

The bridge is intended to communicate only with the local Nexora desktop application. It should not expose a public network endpoint.
