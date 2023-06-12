dofile(LockOn_Options.common_script_path.."elements_defs.lua")
dofile(LockOn_Options.script_path.."symbology_defs.lua")

-- RWR
function AddThreatPlacer(name, number, parent, flashing, radius)
	local placer		= CreateElement "ceSimple"
	if parent ~= nil then
		placer.parent_element	= parent
	end
	placer.name			= name.."_ThreatPlacer"..number
	placer.controllers	= {{name.."_ThreatPos", number, radius}}
--	SetCenterOrigin(placer)
	Add(placer)
	
	local flasher			= CreateElement "ceSimple"
	flasher.name			= name.."_ThreatFlasher"..number
	if flashing then
		flasher.controllers	= {{name.."_ThreatFlash", number}}
	end
	flasher.parent_element	= placer.name
	Add(flasher)
end

function AddThreatSymbol(name, string_def, number)
	addStrokeText(name.."_ThreatSymbol"..number, "", string_def, "CenterCenter", nil, name.."_ThreatFlasher"..number, {{name.."_ThreatSymbol", number}})
end

function AddSttSpecialSymbol(name, file, number, stt_y)
	addStrokeSymbol(name.."_SpecialSymbolsStt"..number, 	{file, "rwr-stt"},  "CenterCenter",
					{0,stt_y}, name.."_ThreatFlasher"..number, {{name.."_SttSymbols", number}})
end

local SpecialSymbol = 
{
	CROSS		= 0,
	OCTO		= 1,
	SQUARE		= 2,
	BOAT		= 3,
	AAA			= 4,
	SAM			= 5,
	AQUISITION	= 6,
	HOSTILE_AI	= 7,
	FRIENDLY_AI	= 8,
	AMBIGUOUS_AI= 9,
	E_WARNING	= 10,
	AI			= 11,
	FLIR		= 12,
	HARM		= 13,
}

function AddBoatHarmSymbols(name, file, number, boat_y, scale_controller)
	addStrokeSymbol(name.."_SpecialSymbolsSquare"..number, 	{file, "rwr-square"},"CenterCenter",
					{0,0}, name.."_ThreatPlacer"..number, {{name.."_SpecialSymbols", number, SpecialSymbol.SQUARE}, scale_controller})
					
	addStrokeSymbol(name.."_SpecialSymbolsBoat"..number, 	{file, "rwr-boat"},  "CenterCenter",
					{0,boat_y}, name.."_ThreatFlasher"..number, {{name.."_SpecialSymbols", number, SpecialSymbol.BOAT}, scale_controller})
end

function AddSpecialSymbols(name, file, number, stt_y, aaa_y, sam_y, ewarn_x)
	addStrokeSymbol(name.."_SpecialSymbolsCross"..number, 	{file, "rwr-cross"}, "CenterCenter",
					{0,0}, name.."_ThreatFlasher"..number, {{name.."_SpecialSymbols", number, SpecialSymbol.CROSS}})
					
	addStrokeSymbol(name.."_SpecialSymbolsOcto"..number, 	{file, "rwr-octo"},  "CenterCenter",
					{0,0}, name.."_ThreatFlasher"..number, {{name.."_SpecialSymbols", number, SpecialSymbol.OCTO}})
					
	addStrokeSymbol(name.."_SpecialSymbolsAaa"..number, 	{file, "rwr-aaa"},  "CenterCenter",
					{0,aaa_y}, name.."_ThreatFlasher"..number, {{name.."_SpecialSymbols", number, SpecialSymbol.AAA}})
					
	addStrokeSymbol(name.."_SpecialSymbolsSam"..number, 	{file, "rwr-sam"},  "CenterCenter",
					{0,sam_y}, name.."_ThreatFlasher"..number, {{name.."_SpecialSymbols", number, SpecialSymbol.SAM}})
					
	addStrokeSymbol(name.."_SpecialSymbolsAquisition"..number, 	{file, "rwr-aquisition"},  "CenterCenter",
					{0,sam_y + 10}, name.."_ThreatFlasher"..number, {{name.."_SpecialSymbols", number, SpecialSymbol.AQUISITION}})
					
	addStrokeSymbol(name.."_SpecialSymbolsHostile"..number, 	{file, "rwr-hostile"},  "CenterCenter",
					{0,-stt_y}, name.."_ThreatFlasher"..number, {{name.."_SpecialSymbols", number, SpecialSymbol.HOSTILE_AI}})
					
	addStrokeSymbol(name.."_SpecialSymbolsFriendly"..number, 	{file, "rwr-friendly"},  "CenterCenter",
					{0,-stt_y}, name.."_ThreatFlasher"..number, {{name.."_SpecialSymbols", number, SpecialSymbol.FRIENDLY_AI}})
					
	addStrokeSymbol(name.."_SpecialSymbolsAmbiguous"..number, 	{file, "rwr-ambiguous"},  "CenterCenter",
					{2,-stt_y - 1}, name.."_ThreatFlasher"..number, {{name.."_SpecialSymbols", number, SpecialSymbol.AMBIGUOUS_AI}})
					
	addStrokeSymbol(name.."_SpecialSymbolsWarning"..number, 	{file, "rwr-warning"},  "CenterCenter",
					{ewarn_x,0}, name.."_ThreatFlasher"..number, {{name.."_SpecialSymbols", number, SpecialSymbol.E_WARNING}})
					
	addStrokeSymbol(name.."_SpecialSymbolsFlir"..number, 	{file, "rwr-flir"},  "CenterCenter",
					{0, 0}, name.."_ThreatFlasher"..number, {{name.."_SpecialSymbols", number, SpecialSymbol.FLIR}})
					
	addStrokeSymbol(name.."_SpecialSymbolsHarm"..number, 	{file, "rwr-harm"},  "CenterCenter",
					{0, -20}, name.."_ThreatFlasher"..number, {{name.."_SpecialSymbols", number, SpecialSymbol.HARM}})
					
end


-- RWR BIT
-- BIT Page 1
function AddBitPage1Fixed(name, string_def, pos_lib, pos_ver, radius, parent)
	addStrokeText(name.."_BIT_Page1_THREAT_LIB", "13", string_def, "CenterCenter", pos_lib, parent, {{name.."_BIT_CheckPage", 1}})
	addStrokeText(name.."_BIT_Page1_FULL_SYSTEM", "F", string_def, "CenterCenter", {math.sin(math.rad(-135)) * radius, math.cos(math.rad(-135)) * radius},
					parent, {{name.."_BIT_CheckPage", 1}})
	addStrokeText(name.."_BIT_Page1_SOFTWARE_VERSION", "GM", string_def, "CenterCenter", pos_ver, parent, {{name.."_BIT_CheckPage", 1}})
end

function AddBitPage1ReceiverFail(name, string_def, box_size, radius, parent)
	for i = 1, 4 do
		addStrokeText(name.."_BIT_Page1_RECEIVER_FAIL_"..i, "R", string_def,	 "CenterCenter", {math.sin(math.rad(i * 90 + 45)) * radius, math.cos(math.rad(i * 90 + 45)) * radius},
					parent, {{name.."_BIT_CheckPage", 1}, {name.."_BIT_ReceiverFail", i}})
		addStrokeBox(name.."_BIT_Page1_BIT_ENHANCEMENT_"..i, box_size, box_size, "CenterCenter", {math.sin(math.rad(i * 90 + 45)) * radius, math.cos(math.rad(i * 90 + 45)) * radius},
					parent, {{name.."_BIT_CheckPage", 1}, {name.."_BIT_ReceiverBox", i}})
	end
end

ALR_BIT1_FAIL_CI = 1
ALR_BIT1_FAIL_SR = 2
ALR_BIT1_FAIL_AN = 3
ALR_BIT1_FAIL_IA = 4
ALR_BIT1_FAIL_TO = 5

local function addAlrBitPage1Fails(name, text, pos)
	failTable = {}
	failTable.name			= name
	failTable.text			= text
	failTable.pos			= pos
	return failTable
end

AlrBitPage1Fails = {}
AlrBitPage1Fails[ALR_BIT1_FAIL_CI] = addAlrBitPage1Fails("_CONTROL_INDICATOR_FAIL",	"C", {0, 0.44})
AlrBitPage1Fails[ALR_BIT1_FAIL_SR] = addAlrBitPage1Fails("_SPECIAL_RECEIVER_FAIL",	"S", {0, 0.22})
AlrBitPage1Fails[ALR_BIT1_FAIL_AN] = addAlrBitPage1Fails("_ANALYZER_FAIL",			"A", {0, 0.0})
AlrBitPage1Fails[ALR_BIT1_FAIL_IA] = addAlrBitPage1Fails("_INTEGRATED_ANTENNA_FAIL","L", {0, -0.22})
AlrBitPage1Fails[ALR_BIT1_FAIL_TO] = addAlrBitPage1Fails("_THERMAL_OVERLOAD",		"T", {0, -0.44})

function AddBitPage1Fails(num, name, string_def, parent)
	addStrokeText(name.."_BIT_Page1"..AlrBitPage1Fails[num].name,	AlrBitPage1Fails[num].text, string_def, "CenterCenter",
				AlrBitPage1Fails[num].pos,  parent, {{name.."_BIT_CheckPage", 1}, {name.."_BIT_Page1_Fails", num}})
end

-- BIT Page 2
function AddBitPage2Fails(name, string_def, string_def_ib, pos_126, pos_ib, parent)
	addStrokeText(name.."_BIT_Page2_126_FAIL",	"1 2 6", string_def, "CenterCenter", pos_126,	parent, {{name.."_BIT_CheckPage", 2}, {name.."_BIT_Page1_Fails", 1}})
	addStrokeText(name.."_BIT_Page2_HARM_FAIL",	"H R M", string_def, "CenterCenter", {0, 0}, 	parent, {{name.."_BIT_CheckPage", 2}, {name.."_BIT_Page1_Fails", 2}})
	addStrokeText(name.."_BIT_Page2_IB_FAIL",	"IB", string_def_ib, "CenterCenter",  pos_ib,	parent, {{name.."_BIT_CheckPage", 2}, {name.."_BIT_Page1_Fails", 3}})
end

-- BIT Page 3, 4, 5, 6
function AddBitFixedFormatText(name, string_def, pos, parent)
	addStrokeText(name.."_BIT_Page3_6_FIXED_FORMATS", "", string_def, "CenterCenter", pos, parent, {{name.."_BIT_GetPage"}}, { "",
					"",
					"",
					"THE\n\nQUICK\n\nBROWN",
					"FOXES\n\nJUMPED\n\nOVER",
					"THE\n\nLAZY\n\nDOG",
					"01234\n\n56789\n\n ",
					"",
					"" })
end

-- BIT Page 6
-- TODO: add symbols

-- BIT Page 7

-- BIT Page 8

-- BIT Page 1A Special

