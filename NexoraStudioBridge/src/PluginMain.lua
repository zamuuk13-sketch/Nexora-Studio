-- Nexora Studio Bridge plugin entry point

local toolbar = plugin:CreateToolbar("Nexora")
local toggle = toolbar:CreateButton(
	"NexoraBridge",
	"Connect Roblox Studio to the Nexora desktop app",
	""
)

toggle.Click:Connect(function()
	print("[Nexora] Studio Bridge enabled")
end)

print("[Nexora] Studio Bridge loaded")
