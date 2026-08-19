# Nexora Studio Bridge

The Nexora Studio Bridge is the Roblox Studio plugin side of the Nexora desktop agent.

## Responsibilities

- Establish a local connection with the Nexora desktop application.
- Report Studio/plugin health with heartbeat messages.
- Receive validated commands from the desktop agent.
- Execute DataModel mutations inside Roblox Studio without mouse automation.
- Return structured results and errors.

## Initial command families

Core primitives:
- inspect_project
- inspect_instance
- create_instance
- delete_instance
- clone_instance
- get_property
- set_property
- move_instance
- resize_instance
- rotate_instance
- create_script
- edit_script
- delete_script
- create_model
- create_ui
- run_test
- read_output
- get_errors

High-level builders will be layered above these primitives later (maps, NPCs, animation, skins/characters, combat, gameplay systems, UI, environment and audio).

## Safety

The bridge must bind only to localhost, authenticate the desktop session, validate every command and its arguments, and never expose a public network listener. Commands must execute on Roblox Studio's main execution context as required by the Roblox plugin API.

## Connection state

The desktop client should receive explicit states such as `connected`, `disconnected`, and `reconnecting`, plus periodic heartbeats. The UI must never block while waiting for Studio.
