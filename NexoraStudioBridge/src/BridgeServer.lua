-- Nexora Studio Bridge
-- Initial Roblox Studio plugin-side bridge.
-- Networking transport will be connected to the desktop app in the next integration step.

local HttpService = game:GetService("HttpService")

local BridgeServer = {}
BridgeServer.VERSION = "0.1.0"
BridgeServer.PROTOCOL = "NEXORA_BRIDGE_V1"
BridgeServer.State = "disconnected"
BridgeServer.SessionId = HttpService:GenerateGUID(false)

function BridgeServer:getHandshake()
	return {
		protocol = self.PROTOCOL,
		version = self.VERSION,
		sessionId = self.SessionId,
		state = "connected",
		studio = true,
	}
end

function BridgeServer:heartbeat()
	return {
		type = "heartbeat",
		timestamp = os.clock(),
		sessionId = self.SessionId,
	}
end

function BridgeServer:result(requestId, ok, data, errorMessage)
	return {
		type = "result",
		requestId = requestId,
		ok = ok,
		data = data,
		error = errorMessage,
	}
end

return BridgeServer
