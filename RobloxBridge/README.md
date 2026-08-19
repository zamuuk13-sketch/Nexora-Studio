# Roblox Studio Bridge

The desktop client expects a localhost bridge at `127.0.0.1:38471` with:
- `GET /ping`
- `POST /createPart` body `{ "name": "..." }`

The Studio-side implementation must use a supported Roblox Studio plugin transport and should validate every command before mutating the DataModel. Do not expose this bridge publicly.
