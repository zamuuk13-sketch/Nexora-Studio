-- Nexora Roblox Studio Plugin v1.0.0
-- Local-only bridge. Install/publish this Script as the plugin entry point.

local HttpService = game:GetService("HttpService")
local Selection = game:GetService("Selection")
local ChangeHistoryService = game:GetService("ChangeHistoryService")

local BASE_URL = "http://127.0.0.1:28473"
local PROTOCOL = "NEXORA_BRIDGE_V1"
local VERSION = "1.0.0"
local running = true

local toolbar = plugin:CreateToolbar("Nexora")
local button = toolbar:CreateButton("Nexora", "Open Nexora", "")
local info = DockWidgetPluginGuiInfo.new(Enum.InitialDockState.Float, true, false, 390, 250, 300, 180)
local widget = plugin:CreateDockWidgetPluginGui("Nexora", info)
widget.Title = "Nexora"
local label = Instance.new("TextLabel")
label.Size = UDim2.fromScale(1,1)
label.BackgroundColor3 = Color3.fromRGB(12,12,14)
label.TextColor3 = Color3.fromRGB(245,245,248)
label.Font = Enum.Font.Gotham
label.TextSize = 14
label.TextWrapped = true
label.Text = "NEXORA\n\n○ Procurando o Nexora Desktop..."
label.Parent = widget
button.Click:Connect(function() widget.Enabled = not widget.Enabled end)

local function request(method, path, body)
	local ok, response = pcall(function()
		return HttpService:RequestAsync({Url=BASE_URL..path, Method=method, Headers={ ["Content-Type"]="application/json" }, Body=body and HttpService:JSONEncode(body) or nil})
	end)
	if not ok or not response.Success then return false,nil end
	local decodedOk, decoded = pcall(function() return HttpService:JSONDecode(response.Body) end)
	return decodedOk, decoded
end

local function findPath(path)
	local current = game
	for _, name in ipairs(path or {}) do
		current = current:FindFirstChild(name)
		if not current then return nil end
	end
	return current
end

local function ok(id, data) return {type="result",requestId=id,ok=true,data=data} end
local function fail(id, message) return {type="result",requestId=id,ok=false,error=message} end

local function execute(req)
	local id=tostring(req.requestId or "")
	local args=req.args or {}
	local command=tostring(req.command or "")
	local success,value=pcall(function()
		if command=="ping" then return ok(id,{pong=true,version=VERSION}) end
		if command=="inspect_project" or command=="inspect_instance" then
			local root=findPath(args.path or {}) or game
			local out={name=root.Name,className=root.ClassName,children={}}
			for _,child in ipairs(root:GetChildren()) do table.insert(out.children,{name=child.Name,className=child.ClassName}) end
			return ok(id,out)
		end
		if command=="create_instance" then
			local parent=findPath(args.parentPath or {"Workspace"}); if not parent then error("Parent not found") end
			local obj=Instance.new(tostring(args.className or "Folder")); obj.Name=tostring(args.name or obj.Name); obj.Parent=parent
			ChangeHistoryService:SetWaypoint("Nexora: Create "..obj.Name); return ok(id,{path=obj:GetFullName(),className=obj.ClassName})
		end
		if command=="delete_instance" then
			local obj=findPath(args.path or {}); if not obj or obj==game then error("Instance not found") end
			obj:Destroy(); ChangeHistoryService:SetWaypoint("Nexora: Delete"); return ok(id,{deleted=true})
		end
		if command=="clone_instance" then
			local source=findPath(args.path or {}); local parent=findPath(args.parentPath or {"Workspace"}); if not source or not parent then error("Source/parent not found") end
			local clone=source:Clone(); clone.Name=tostring(args.name or clone.Name); clone.Parent=parent; ChangeHistoryService:SetWaypoint("Nexora: Clone"); return ok(id,{path=clone:GetFullName()})
		end
		if command=="get_property" then
			local obj=findPath(args.path or {}); if not obj then error("Instance not found") end
			return ok(id,{property=args.property,value=(obj :: any)[tostring(args.property)]})
		end
		if command=="set_property" then
			local obj=findPath(args.path or {}); if not obj then error("Instance not found") end
			(obj :: any)[tostring(args.property)]=args.value; ChangeHistoryService:SetWaypoint("Nexora: Set property"); return ok(id,{updated=true})
		end
		if command=="create_script" then
			local parent=findPath(args.parentPath or {"ServerScriptService"}); if not parent then error("Parent not found") end
			local s=Instance.new(tostring(args.scriptClass or "Script")); s.Name=tostring(args.name or "NexoraScript"); s.Parent=parent
			if s:IsA("LuaSourceContainer") then s.Source=tostring(args.source or "") end
			ChangeHistoryService:SetWaypoint("Nexora: Create script"); return ok(id,{path=s:GetFullName()})
		end
		if command=="edit_script" then
			local s=findPath(args.path or {}); if not s or not s:IsA("LuaSourceContainer") then error("Script not found") end
			s.Source=tostring(args.source or ""); ChangeHistoryService:SetWaypoint("Nexora: Edit script"); return ok(id,{updated=true})
		end
		if command=="delete_script" then
			local s=findPath(args.path or {}); if not s or not s:IsA("LuaSourceContainer") then error("Script not found") end
			s:Destroy(); ChangeHistoryService:SetWaypoint("Nexora: Delete script"); return ok(id,{deleted=true})
		end
		if command=="select" then
			local list={}; for _,path in ipairs(args.paths or {}) do local obj=findPath(path); if obj then table.insert(list,obj) end end
			Selection:Set(list); return ok(id,{selected=#list})
		end
		error("Unknown command: "..command)
	end)
	if success then return value end
	return fail(id,tostring(value))
end

request("POST","/hello",{protocol=PROTOCOL,version=VERSION})

task.spawn(function()
	while running do
		local good,message=request("GET","/next")
		if good then
			label.Text="NEXORA\n\n● Studio conectado\nProtocolo: "..PROTOCOL.."\nPlugin: "..VERSION
			if message and message.command then request("POST","/result",execute(message)) end
		else
			label.Text="NEXORA\n\n○ Nexora Desktop desconectado\nAbra o aplicativo para conectar."
		end
		task.wait(0.5)
	end
end)

plugin.Unloading:Connect(function() running=false end)
print("[Nexora] Plugin "..VERSION.." loaded")
