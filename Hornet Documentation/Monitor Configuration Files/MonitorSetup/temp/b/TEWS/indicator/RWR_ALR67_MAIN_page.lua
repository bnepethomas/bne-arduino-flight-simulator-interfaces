dofile(LockOn_Options.script_path.."TEWS/indicator/RWR_ALR67_definitions.lua")

--addStrokeCircle("TEST_CIRCLE", 1, {0, 0})
--addStrokeCircle("TEST_CIRCLE_1", 0.74, {0, 0})
--addStrokeCircle("TEST_CIRCLE_2", 0.56, {0, 0})
--addStrokeCircle("TEST_CIRCLE_3", 0.19, {0, 0})
--addStrokeCircle("TEST_CIRCLE_0", 0.075, {0, 0})

AddPrioritySetting(STROKE_FNT_DFLT, {-0.1, 0.05})
AddBitFailures(STROKE_FNT_DFLT, {0, -0.05})
AddLimitModeLabel(STROKE_FNT_DFLT, {0.1, 0.05})

local boat_y = -0.075
local specScaleX = 0.0065
local specScaleY = 0.006
for i = 1, 16 do
	local num = i-1
	AddThreatPlacer("RWR", num, nil, true)
	AddThreatSymbol("RWR", STROKE_FNT_DFLT, num)

	AddBoatHarmSymbols("RWR", "stroke_symbols_MDI_AMPCD", num, boat_y, {"scale", specScaleX, specScaleY})
end
