dofile(LockOn_Options.common_script_path.."devices_defs.lua")
dofile(LockOn_Options.script_path.."Multipurpose_Display_Group/Common/indicator/MDG_materials.lua")
dofile(LockOn_Options.script_path.."config.lua")

-- Parameters handling functions
dofile(LockOn_Options.script_path.."Multipurpose_Display_Group/Common/indicator/InitParams.lua")

indicator_type = indicator_types.COLLIMATOR

if bakeIndicators == true and bakeHUD == true then
	purposes	   				= {render_purpose.BAKE} -- direct render disabled
else
	purposes	   				= {render_purpose.GENERAL, render_purpose.HUD_ONLY_VIEW} -- HUD only was added for test purposes
	--purposes	   				= {render_purpose.GENERAL} -- HUD only was added for test purposes
	
	opacity_sensitive_materials = 
	{
		MDG_font_name(MDG_SELF_IDS.HUD),
		MDG_material_name(MDG_SELF_IDS.HUD)
	}
end

shaderLineParamsUpdatable  = true
shaderLineDefaultThickness = 0.8
shaderLineDefaultFuzziness = 0.5
shaderLineDrawAsWire 	   = false
shaderLineUseSpecularPass  = false

-- page specific for the indicator, implements indicator border/FOV
BasePage              = LockOn_Options.script_path.."Multipurpose_Display_Group/HUD_AVQ32/indicator/Pages/HUD_base.lua"
-- Add here any specific symbology which should not appear on the HUD repeater page
IndicatorSpecificPage = LockOn_Options.script_path.."Multipurpose_Display_Group/HUD_AVQ32/indicator/Pages/HUD_specific.lua"
isHUD                 = true

-- Parameters to customize common symbology properties
writeParameter("MDG_init_specifics", LockOn_Options.script_path.."Multipurpose_Display_Group/HUD_AVQ32/indicator/AVQ32_specifics.lua")
writeParameter("MDG_init_DEFAULT_LEVEL", 19) -- was 19
dofile(LockOn_Options.script_path.."Multipurpose_Display_Group/Common/indicator/Common_init.lua")

-- optical center is 4 degrees below the FRL
collimator_default_distance_factor = {auto_collimator_default_distance_factor[1], auto_collimator_default_distance_factor[1] * math.rad(-4), 0}
--collimator_default_distance_factor = {auto_collimator_default_distance_factor[1], auto_collimator_default_distance_factor[1] * math.rad(-4), 0.58} -- was 0. Removes HUD from forward, external view.
dofile(LockOn_Options.common_script_path.."ViewportHandling.lua")
try_find_assigned_viewport("F18_HUD")