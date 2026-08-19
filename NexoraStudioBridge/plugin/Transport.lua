--!strict
-- Nexora Studio Bridge local transport adapter.
-- Roblox Studio polls the desktop HTTP server asynchronously.

local HttpService = game:GetService("HttpService")

local Transport = {}
Transport.__index = Transport

local DEFAULT_BASE_URL = "http://127.0.0.1:28473"

function Transport.new(baseUrl: string?, bearerToken: string?, onState: ((string) -> ())?, onCommand: ((any) -> any)?): any
	local self = setmetatable({}, Transport)
	self.baseUrl = baseUrl or DEFAULT_BASE_URL
	self.bearerToken = bearerToken or ""
	self.onState = onState
	self.onCommand = onCommand
	self.running = false
	self.busy = false
	return self
end

function Transport:_headers(): {[string]: string}
	local headers = { ["Content-Type"] = "application/json" }
	if self.bearerToken ~= "" then
		headers["Authorization"] = "Bearer " .. self.bearerToken
	end
	return headers
end

function Transport:_setState(state: string)
	if self.onState then
		self.onState(state)
	end
end

function Transport:poll()
	if self.busy then return end
	self.busy = true

	task.spawn(function()
		local okRequest, response = pcall(function()
			return HttpService:RequestAsync({
				Url = self.baseUrl .. "/next",
				Method = "GET",
				Headers = self:_headers(),
			})
		end)

		if not okRequest or not response.Success then
			self:_setState("disconnected")
			self.busy = false
			return
		end

		self:_setState("connected")

		local decodeOk, message = pcall(function()
			return HttpService:JSONDecode(response.Body)
		end)

		if decodeOk and message and message.command and self.onCommand then
			local resultOk, result = pcall(function()
				return self.onCommand(message)
			end)
			if not resultOk then
				result = {
					type = "result",
					requestId = tostring(message.requestId or ""),
					ok = false,
					error = tostring(result),
				}
			end

			pcall(function()
				HttpService:RequestAsync({
					Url = self.baseUrl .. "/result",
					Method = "POST",
					Headers = self:_headers(),
					Body = HttpService:JSONEncode(result),
				})
			end)
		end

		self.busy = false
	end)
end

function Transport:start(intervalSeconds: number?)
	if self.running then return end
	self.running = true
	local interval = intervalSeconds or 0.5

	task.spawn(function()
		while self.running do
			self:poll()
			task.wait(interval)
		end
	end)
end

function Transport:stop()
	self.running = false
	self:_setState("disconnected")
end

return Transport
