--!strict
-- Nexora Studio Bridge
-- Roblox Studio plugin-side command executor.
-- Transport is intentionally local-only and is provided by the desktop integration layer.

local HttpService = game:GetService("HttpService")
local Selection = game:GetService("Selection")
local ChangeHistoryService = game:GetService("ChangeHistoryService")

local VERSION = "0.2.0"
local PROTOCOL = "NEXORA_BRIDGE_V1"
local sessionId = HttpService:GenerateGUID(false)

local function ok(requestId: string, data: any): {[string]: any}
	return { type = "result", requestId = requestId, ok = true, data = data }
end

local function fail(requestId: string, message: string): {[string]: any}
	return { type = "result", requestId = requestId, ok = false, error = message }
end

local function findByPath(path: {string}): Instance?
	local current: Instance = game
	for _, name in ipairs(path) do
		local nextInstance = current:FindFirstChild(name)
		if not nextInstance then return nil end
		current = nextInstance
	end
	return current
end

local function serializeInstance(instance: Instance, depth: number?): {[string]: any}
	local maxDepth = depth or 1
	local result = {
		name = instance.Name,
		className = instance.ClassName,
		children = {} :: {any},
	}
	if maxDepth > 0 then
		for _, child in ipairs(instance:GetChildren()) do
			table.insert(result.children, serializeInstance(child, maxDepth - 1))
		end
	end
	return result
end

local function execute(request: {[string]: any}): {[string]: any}
	local requestId = tostring(request.requestId or HttpService:GenerateGUID(false))
	local command = request.command
	local args = request.args or {}

	local okCall, result = pcall(function()
		if command == "inspect_project" then
			local root = args.path and findByPath(args.path) or game
			if not root then error("Instance path not found") end
			return ok(requestId, serializeInstance(root, tonumber(args.depth) or 2))
		end

		if command == "inspect_instance" then
			local instance = findByPath(args.path)
			if not instance then error("Instance path not found") end
			return ok(requestId, serializeInstance(instance, tonumber(args.depth) or 1))
		end

		if command == "create_instance" then
			local parent = findByPath(args.parentPath or {})
			if not parent then error("Parent path not found") end
			local instance = Instance.new(tostring(args.className))
			instance.Name = tostring(args.name or instance.Name)
			instance.Parent = parent
			ChangeHistoryService:SetWaypoint("Nexora: Create " .. instance.Name)
			return ok(requestId, {path = instance:GetFullName(), className = instance.ClassName})
		end

		if command == "delete_instance" then
			local instance = findByPath(args.path)
			if not instance or instance == game then error("Instance cannot be deleted") end
			local name = instance.Name
			instance:Destroy()
			ChangeHistoryService:SetWaypoint("Nexora: Delete " .. name)
			return ok(requestId, {deleted = true, name = name})
		end

		if command == "clone_instance" then
			local source = findByPath(args.path)
			local parent = findByPath(args.parentPath or {})
			if not source or not parent then error("Source or parent path not found") end
			local clone = source:Clone()
			clone.Name = tostring(args.name or clone.Name)
			clone.Parent = parent
			ChangeHistoryService:SetWaypoint("Nexora: Clone " .. clone.Name)
			return ok(requestId, {path = clone:GetFullName()})
		end

		if command == "set_property" then
			local instance = findByPath(args.path)
			if not instance then error("Instance path not found") end
			local property = tostring(args.property)
			(instance :: any)[property] = args.value
			ChangeHistoryService:SetWaypoint("Nexora: Set " .. property)
			return ok(requestId, {path = instance:GetFullName(), property = property})
		end

		if command == "get_property" then
			local instance = findByPath(args.path)
			if not instance then error("Instance path not found") end
			local property = tostring(args.property)
			return ok(requestId, {property = property, value = (instance :: any)[property]})
		end

		if command == "create_script" or command == "edit_script" then
			local target = findByPath(args.path)
			if not target then error("Script path not found") end
			if not target:IsA("LuaSourceContainer") then error("Target is not a script") end
			(target :: LuaSourceContainer).Source = tostring(args.source or "")
			ChangeHistoryService:SetWaypoint("Nexora: Edit script " .. target.Name)
			return ok(requestId, {path = target:GetFullName(), updated = true})
		end

		if command == "delete_script" then
			local target = findByPath(args.path)
			if not target or not target:IsA("LuaSourceContainer") then error("Script not found") end
			local name = target.Name
			target:Destroy()
			ChangeHistoryService:SetWaypoint("Nexora: Delete script " .. name)
			return ok(requestId, {deleted = true})
		end

		if command == "select" then
			local items = {}
			for _, path in ipairs(args.paths or {}) do
				local instance = findByPath(path)
				if instance then table.insert(items, instance) end
			end
			Selection:Set(items)
			return ok(requestId, {selected = #items})
		end

		if command == "ping" then
			return ok(requestId, {pong = true, timestamp = os.clock()})
		end

		error("Unknown command: " .. tostring(command))
	end)

	if not okCall then return fail(requestId, tostring(result)) end
	return result
end

local toolbar = plugin:CreateToolbar("Nexora")
local button = toolbar:CreateButton("NexoraBridge", "Toggle Nexora Studio Bridge", "")
button.ClickableWhenViewportHidden = true

local widgetInfo = DockWidgetPluginGuiInfo.new(
	Enum.InitialDockState.Float,
	true,
	false,
	360,
	220,
	280,
	160
)
local widget = plugin:CreateDockWidgetPluginGui("NexoraStudioBridge", widgetInfo)
widget.Title = "Nexora Studio Bridge"

local status = Instance.new("TextLabel")
status.Size = UDim2.fromScale(1, 1)
status.BackgroundColor3 = Color3.fromRGB(20, 20, 20)
status.TextColor3 = Color3.fromRGB(240, 240, 240)
status.Font = Enum.Font.Gotham
status.TextSize = 14
status.TextWrapped = true
status.Text = "NEXORA\n\nStudio Bridge loaded\nProtocol: " .. PROTOCOL .. "\nVersion: " .. VERSION .. "\n\nWaiting for Nexora Desktop..."
status.Parent = widget

button.Click:Connect(function()
	widget.Enabled = not widget.Enabled
end)

-- Expose a plugin-local command entry point for the desktop transport layer.
-- The transport adapter should decode a request and call this function.
_G.NexoraStudioBridgeExecute = execute
_G.NexoraStudioBridgeHandshake = function()
	return {
		protocol = PROTOCOL,
		version = VERSION,
		sessionId = sessionId,
		studio = true,
	}
end

print("[Nexora] Studio Bridge " .. VERSION .. " loaded")
