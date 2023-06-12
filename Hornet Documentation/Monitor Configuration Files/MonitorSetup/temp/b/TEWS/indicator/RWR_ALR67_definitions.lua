dofile(LockOn_Options.script_path.."TEWS/indicator/RWR_ALR67_Common_definitions.lua")

maxDisplayedEmitters = 15

-- see 'use_mipfilter' in symbology_defs.lua
mip_filter_used	= true

Radius    = GetScale() -- half_width
Radius    = Radius * 0.0158
Diameter  = 2 * Radius
SetCustomScale(Diameter)

stroke_font 		= "font_RWR"
default_material  	= "RWR_STROKE"

--stringdefs = {20 * GetScale(), 12 * GetScale(), 4 * GetScale(), 5 * GetScale()}
local fontSz = 0.000025 / 0.0158  -- font size/screen size

MPDtoALR = 0.000025 / 0.0158  * GetScale()

fontScaleY_dflt	 = fontSz * GetScale()		-- 59/0.112
fontScaleY_small = 0.5 * fontSz * GetScale()

fontScaleX_dflt	 = fontSz * GetScale()		-- 59/0.112
fontScaleX_small = 0.5 * fontSz * GetScale()

STROKE_FNT_DFLT		= 1
STROKE_FNT_LONG		= 2
STROKE_FNT_SMALL	= 3
STROKE_FNT_SHORT	= 4

stringdefs[STROKE_FNT_DFLT]		= {fontScaleY_dflt, fontScaleX_dflt, 74 * fontSz * GetScale(), 74 * fontSz * GetScale()}
stringdefs[STROKE_FNT_LONG]		= {fontScaleY_dflt, fontScaleX_dflt, 104 * fontSz * GetScale(), 74 * fontSz * GetScale()}
stringdefs[STROKE_FNT_SMALL]	= {fontScaleY_small, fontScaleX_small, 29 * fontSz * GetScale(), 31 * fontSz * GetScale()}
stringdefs[STROKE_FNT_SHORT]	= {fontScaleY_dflt, fontScaleX_dflt, 59 * fontSz * GetScale(), 74 * fontSz * GetScale()}

-- RWR Azimuth Indicator functions
function AddPrioritySetting(string_def, pos)
	addStrokeText("RWR_PrioritySetting", "", string_def, "CenterCenter", pos, nil, {{"RWR_PrioritySetting"}}, { "",
					"N",
					"I",
					"A",
					"U",
					"F"})
end

function AddBitFailures(string_def, pos)
	addStrokeText("RWR_BitFailures", "", string_def, "CenterCenter", pos, nil, {{"RWR_BitFailures"}}, { "",
					"B",
					"T"})
end

function AddLimitModeLabel(string_def, pos)
	addStrokeText("RWR_LimitMode", "L", string_def, "CenterCenter", pos, nil, {{"RWR_LimitMode"}})
end
