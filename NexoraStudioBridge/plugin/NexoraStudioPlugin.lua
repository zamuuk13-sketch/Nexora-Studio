-- Nexora Studio Plugin
-- Publish this Script as the plugin's main source in Roblox Studio.
-- The plugin talks only to the local Nexora Desktop bridge.

local HttpService = game:GetService("HttpService")

local BASE_URL = "http://127.0.0.1:28473"
local VERSION = "1.0.0"
local PROTOCOL = "NEXORA_BRIDGE_V1"
local sessionId = HttpService:GenerateGUID(false)
local running = true

local toolbar = plugin:CreateToolbar("Nexora")
local button = toolbar:CreateButton(
	"Nexora",
	"Connect Nexora to Roblox Studio",
	""
)
button.ClickableWhenViewportHidden = true

local widgetInfo = DockWidgetPluginGuiInfo.new(
	Enum.InitialDockState.Float,
	true,
	false,
	380,
	240,
	300,
	180
)

local widget = plugin:CreateDockWidgetPluginGui("NexoraStudioPlugin", widgetInfo)
widget.Title = "Nexora"

local root = Instance.new("Frame")
root.Size = UDim2.fromScale(1, 1)
root.BackgroundColor3 = Color3.fromRGB(12, 12, 14)
root.BorderSizePixel = 0
root.Parent = widget

local title = Instance.new("TextLabel")
title.Size = UDim2.new(1, -32, 0, 38)
title.Position = UDim2.fromOffset(16, 12)
title.BackgroundTransparency = 1
title.Text = "NEXORA"
title.TextColor3 = Color3.fromRGB(245, 245, 248)
title.Font = Enum.Font.GothamBold
title.TextSize = 22
title.TextXAlignment = Enum.TextXAlignment.Left
title.Parent = root

local status = Instance.new("TextLabel")
status.Size = UDim2.new(1, -32, 0, 110)
status.Position = UDim2.fromOffset(16, 58)
status.BackgroundTransparency = 1
status.Text = "○  Procurando o Nexora Desktop..."
status.TextColor3 = Color3.fromRGB(190, 190, 198)
status.Font = Enum.Font.Gotham
status.TextSize = 14
status.TextWrapped = true
status.TextXAlignment = Enum.TextXAlignment.Left
status.TextYAlignment = Enum.TextYAlignment.Top
status.Parent = root

button.Click:Connect(function()
	widget.Enabled = not widget.Enabled
end)

local function request(method: string, path: string, body: any?): (boolean, any)
	local okCall, response = pcall(function()
		return HttpService:RequestAsync({
			Url = BASE_URL .. path,
			Method = method,
			Headers = { ["Content-Type"] = "application/json" },
			Body = body and HttpService:JSONEncode(body) or nil,
		})
	end)

	if not okCall or not response.Success then
		return false, nil
	end

	local okDecode, decoded = pcall(function()
		return HttpService:JSONDecode(response.Body)
	end)
	if not okDecode then
		return false, nil
	end
	return true, decoded
end

local function sendHello()
	return request("POST", "/hello", {
		protocol = PROTOCOL,
		version = VERSION,
		sessionId = sessionId,
	})
end

local function sendResult(result: any)
	request("POST", "/result", result)
end

local function poll()
	local okPoll, message = request("GET", "/next")
	if not okPoll then
		status.Text = "○  Nexora Desktop desconectado\n\nAbra o Nexora Desktop para conectar ao Roblox Studio."
		return
	end

	status.Text = "●  Nexora conectado\n\nProtocolo: " .. PROTOCOL .. "\nPlugin: " .. VERSION .. "\nSessão: " .. sessionId

	if message and message.command then
		-- The desktop sends validated bridge commands. The full executor is loaded
		-- by NexoraStudioBridgeMain.lua in the development source tree.
		local response = {
			type = "result",
			requestId = message.requestId,
			ok = false,
			error = "Plugin executor is not bundled in this entry source yet.",
		}
		sendResult(response)
	end
end

local helloOk = sendHello()
if helloOk then
	status.Text = "●  Nexora conectado\n\nAguardando comandos da IA..."
end

task.spawn(function()
	while running do
		poll()
		task.wait(0.5)
	end
end)

plugin.Unloading:Connect(function()
	running = false
end)

print("[Nexora] Studio Plugin " .. VERSION .. " loaded")
