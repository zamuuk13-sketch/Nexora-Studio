# Nexora local bridge protocol v1

The desktop application is the local HTTP server. Roblox Studio polls it; the plugin never opens a public listener.

Default endpoint: `http://127.0.0.1:28473`

Endpoints:

- `GET /health` — returns `{protocol,version,status}`.
- `GET /next` — plugin polls for the next queued command. Empty queue returns `{command:null}`.
- `POST /result` — plugin posts a command result.
- `POST /hello` — plugin announces its session.

Each command has:

```json
{"requestId":"...","command":"ping","args":{}}
```

Each result has:

```json
{"type":"result","requestId":"...","ok":true,"data":{}}
```

## Authentication

For v1, the desktop creates a random per-run bearer token and requires it in the `Authorization: Bearer <token>` header. The token is never committed to the repository. The plugin receives it through the local handshake configuration supplied by the desktop integration.

The server binds only to `127.0.0.1`, never `0.0.0.0`.

## Non-blocking behavior

The plugin polls asynchronously on a timer. If the desktop disappears, it changes to disconnected/reconnecting rather than blocking Studio. The desktop also reports the last heartbeat and connection state to the Nexora UI.
