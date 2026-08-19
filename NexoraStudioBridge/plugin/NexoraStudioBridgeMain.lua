--!strict
-- Single-source Nexora Studio Bridge entry point.
-- Install this source as the plugin's Script entry point when packaging.

local HttpService = game:GetService("HttpService")
local Selection = game:GetService("Selection")
local ChangeHistoryService = game:GetService("ChangeHistoryService")

local BASE_URL = "http://127.0.0.1:28473"
local VERSION = "0.3.0"
local PROTOCOL = "NEXORA_BRIDGE_V1"
local sessionId = HttpService:GenerateGUID(false)

local function pathFind(path: {string}): Instance?
	local current: Instance = game
	for _, name in ipairs(path) do
		local child = current:FindFirstChild(name)
		if not child then return nil end
		current = child
	end
	return current
end

local function snapshot(instance: Instance, depth: number): {[string]: any}
	local out = {name = instance.Name, className = instance.ClassName, children = {}}
	if depth <= 0 then return out end
	for _, child in ipairs(instance:GetChildren()) do
		table.insert(out.children, snapshot(child, depth - 1))
	end
	return out
end

local function result(id: string, success: boolean, data: any?, errorText: string?): {[string]: any}
	return {type = "result", requestId = id, ok = success, data = data, error = errorText}
end

local function execute(request: {[string]: any}): {[string]: any}
	local id = tostring(request.requestId or HttpService:GenerateGUID(false))
	local command = tostring(request.command or "")
	local args = request.args or {}

	local success, value = pcall(function()
		if command == "ping" then
			return result(id, true, {pong = true, sessionId = sessionId, version = VERSION})
		elseif command == "inspect_project" or command == "inspect_instance" then
			local root = pathFind(args.path or {})
			if not root then error("Instance path not found") end
			return result(id, true, snapshot(root, tonumber(args.depth) or 2))
		elseif command == "create_instance" then
			local parent = pathFind(args.parentPath or {})
			if not parent then error("Parent path not found") end
			local instance = Instance.new(tostring(args.className))
			instance.Name = tostring(args.name or instance.Name)
			instance.Parent = parent
			ChangeHistoryService:SetWaypoint("Nexora: Create " .. instance.Name)
			return result(id, true, {path = instance:GetFullName(), className = instance.ClassName})
		elseif command == "delete_instance" then
			local instance = pathFind(args.path or {})
			if not instance or instance == game then error("Invalid instance") end
			local name = instance.Name
			instance:Destroy()
			ChangeHistoryService:SetWaypoint("Nexora: Delete " .. name)
			return result(id, true, {deleted = true})
		elseif command == "clone_instance" then
			local source = pathFind(args.path or {})
			local parent = pathFind(args.parentPath or {})
			if not source or not parent then error("Source or parent path not found") end
			local clone = source:Clone()
			clone.Name = tostring(args.name or clone.Name)
			clone.Parent = parent
			ChangeHistoryService:SetWaypoint("Nexora: Clone " .. clone.Name)
			return result(id, true, {path = clone:GetFullName()})
		elseif command == "get_property" or command == "set_property" then
			local instance = pathFind(args.path or {})
			if not instance then error("Instance path not found") end
			local property = tostring(args.property)
			if command == "get_property" then
				return result(id, true, {property = property, value = (instance :: any)[property]})
			end
			(instance :: any)[property] = args.value
			ChangeHistoryService:SetWaypoint("Nexora: Set " .. property)
			return result(id, true, {property = property, updated = true})
		elseif command == "create_script" then
			local parent = pathFind(args.parentPath or {})
			if not parent then error("Parent path not found") end
			local scriptClass = tostring(args.scriptClass or "Script")
			local scriptObject = Instance.new(scriptClass)
			scriptObject.Name = tostring(args.name or "NexoraScript")
			if scriptObject:IsA("LuaSourceContainer") then scriptObject.Source = tostring(args.source or "") end
			scriptObject.Parent = parent
			ChangeHistoryService:SetWaypoint("Nexora: Create script " .. scriptObject.Name)
			return result(id, true, {path = scriptObject:GetFullName()})
		elseif command == "edit_script" then
			local scriptObject = pathFind(args.path or {})
			if not scriptObject or not scriptObject:IsA("LuaSourceContainer") then error("Script not found") end
			scriptObject.Source = tostring(args.source or "")
			ChangeHistoryService:SetWaypoint("Nexora: Edit script " .. scriptObject.Name)
			return result(id, true, {updated = true})
		elseif command == "delete_script" then
			local scriptObject = pathFind(args.path or {})
			if not scriptObject or not scriptObject:IsA("LuaSourceContainer") then error("Script not found") end
			scriptObject:Destroy()
			ChangeHistoryService:SetWaypoint("Nexora: Delete script")
			return result(id, true, {deleted = true})
		elseif command == "select" then
			local selected = {}
			for _, path in ipairs(args.paths or {}) do
				local instance = pathFind(path)
				if instance then table.insert(selected, instance) end
			end
			Selection:Set(selected)
			return result(id, true, {selected = #selected})
		end
		error("Unknown command: " .. command)
	end)

	if success then return value end
	return result(id, false, nil, tostring(value))
end

local toolbar = plugin:CreateToolbar("Nexora")
local button = toolbar:CreateButton("Nexora", "Nexora Studio Bridge", "")
button.ClickableWhenViewportHidden = true

local widgetInfo = DockWidgetPluginGuiInfo.new(Enum.InitialDockState.Float, true, false, 360, 220, 280, 160)
local widget = plugin:CreateDockWidgetPluginGui("NexoraStudioBridge", widgetInfo)
widget.Title = "Nexora Studio Bridge"

local status = Instance.new("TextLabel")
status.Size = UDim2.fromScale(1, 1)
status.BackgroundColor3 = Color3.fromRGB(18, 18, 18)
status.TextColor3 = Color3.fromRGB(235, 235, 235)
status.Font = Enum.Font.Gotham
status.TextSize = 14
status.TextWrapped = true
status.Text = "NEXORA\n\nBridge: starting...\nStudio: waiting for Nexora"
status.Parent = widget

button.Click:Connect(function()
	widget.Enabled = not widget.Enabled
end)

local function http(method: string, endpoint: string, body: any?): (boolean, any)
	local okRequest, response = pcall(function()
		return HttpService:RequestAsync({
			Url = BASE_URL .. endpoint,
			Method = method,
			Headers = { ["Content-Type"] = "application/json" },
			Body = body and HttpService:JSONEncode(body) or nil,
		})
	end)
	if not okRequest or not response.Success then return false, nil end
	local okDecode, decoded = pcall(function() return HttpService:JSONDecode(response.Body) end)
	if not okDecode then return false, nil end
	return true, decoded
end

local running = true

task.spawn(function()
	local helloOk = http("POST", "/hello", {protocol = PROTOCOL, version = VERSION, sessionId = sessionId})
	if helloOk then
		status.Text = "NEXORA\n\nBridge: ● Connected\nProtocol: " .. PROTOCOL .. "\nVersion: " .. VERSION
	else
		status.Text = "NEXORA\n\nBridge: ○ Disconnected\nAbra o Nexora Desktop para conectar."
	end

	while running do
		local okPoll, message = http("GET", "/next")
		if okPoll then
			status.Text = "NEXORA\n\nBridge: ● Connected\nProtocol: " .. PROTOCOL .. "\nVersion: " .. VERSION
			if message and message.command then
				local response = execute(message)
				http("POST", "/result", response)
			end
		else
			status.Text = "NEXORA\n\nBridge: ○ Reconnecting...\nAbra o Nexora Desktop para continuar."
		end
		task.wait(0.5)
	end
end)

plugin.Unloading:Connect(function()
	running = false
end)

print("[Nexora] Studio Bridge " .. VERSION .. " loaded")
