dofile(LockOn_Options.common_script_path.."Fonts/symbols_locale.lua")
dofile(LockOn_Options.common_script_path.."Fonts/fonts_cmn.lua")
dofile(LockOn_Options.script_path.."Multipurpose_Display_Group/Common/indicator/MDG_materials.lua")
dofile(LockOn_Options.script_path.."Multipurpose_Display_Group/Common/indicator/MDG_strokesDefs.lua")

-- variables used in stroke font description as well as in MDG stroke symbology
dbg_drawStrokesAsWire = false

dofile(LockOn_Options.script_path.."fonts.lua")

-------MATERIALS-------

-- X/Y CIE color coordinate system is described in MIL-C-25050 'Colors, Aeronautical Lights and Lighting Equipment'.
-- For RGB convertion, Yxy color space was used with maximum possible luminance (Y) set.
-- HOWEVER it does not work well, and some colors were replaced with custom ones, picked from color-balanced photos.

-- red							{255, 93, 0, 255}  -- X - 0.633, Y - 0.366 (radius - 0.05)
-- green (per documents) 		{204, 255, 0, 255} -- X - 0.396, Y - 0.597 (radius - 0.07)
-- yellow						{255, 225, 0, 255} -- X - 0.513, Y - 0.483 (radius - 0.06)

-- Only green colors are described here. Red and yellow will be set via dedicated controllers
local MDG_materials = {}
MDG_materials[MDG_SELF_IDS.HUD] 	= {2, 255, 20, 385}
--MDG_materials[MDG_SELF_IDS.HUD] 	= {5, 255, 76, 400}
-- {94, 202, 0, 255} -- MDI original
-- {57, 224, 32, 255} -- MDI test
-- {69, 224, 45, 255} -- MDI test 2
--MDG_materials[MDG_SELF_IDS.LMDI] 	= {94, 202, 0, 255} -- MDI original
MDG_materials[MDG_SELF_IDS.LMDI] 	= {94, 202, 0, 350} -- MDI original
MDG_materials[MDG_SELF_IDS.RMDI] 	= MDG_materials[MDG_SELF_IDS.LMDI]
MDG_materials[MDG_SELF_IDS.HI] 		= MDG_materials[MDG_SELF_IDS.LMDI]

local MDG_fonts = {}
MDG_fonts[MDG_SELF_IDS.HUD] 		= {fontdescription["font_stroke_MDG"], 10, MDG_materials[MDG_SELF_IDS.HUD]}
MDG_fonts[MDG_SELF_IDS.LMDI] 		= {fontdescription["font_stroke_MDG"], 10, MDG_materials[MDG_SELF_IDS.LMDI]}
MDG_fonts[MDG_SELF_IDS.RMDI] 		= {fontdescription["font_stroke_MDG"], 10, MDG_materials[MDG_SELF_IDS.RMDI]}
MDG_fonts[MDG_SELF_IDS.HI] 			= {fontdescription["font_stroke_MDG"], 10, MDG_materials[MDG_SELF_IDS.HI]}

materials = {}

local function add_MDG_material(ID)
	materials[MDG_material_name(ID)] = MDG_materials[ID]
end

materials["INDICATION_COMMON_RED"]		= {255, 0, 0, 255}
materials["INDICATION_COMMON_WHITE"]	= {255, 255, 255, 255}
materials["INDICATION_COMMON_GREEN"]	= {0, 255, 0, 255}
materials["INDICATION_COMMON_AMBER"]	= {255,161,45,255}
materials["MASK_MATERIAL_PURPLE"]		= {255, 0, 255, 30}
materials["MASK_MATERIAL_PURPLE_2"]		= {255, 0, 255, 100}
materials["MASK_MATERIAL_2"]			= {0, 255, 255, 30}

materials["MAV_COLOR_0"]			= materials["INDICATION_COMMON_GREEN"]
materials["MAV_COLOR_1"]			= materials["INDICATION_COMMON_GREEN"]
materials["MAV_COLOR_2"]			= materials["INDICATION_COMMON_WHITE"]

materials["TGP_STBY_BLACK"] 		= {0, 0, 0, 255}
materials["TGP_STBY_DGRAY"] 		= {5, 5, 5, 255}

materials["DBG_GREY"]					= {25, 25, 25, 255}
materials["DBG_BLACK"]					= {0, 0, 0, 100}
materials["DBG_RED"]					= {255, 0, 0, 100}
materials["DBG_GREEN"]					= {0, 255, 0, 100}
materials["BLACK"]						= {0, 0, 0, 255}
materials["SIMPLE_WHITE"]				= {255, 255, 255, 255}
materials["PURPLE"]						= {255, 0, 255, 255}

materials["IFEI_FONT"]					= {255, 220, 220, 255}
materials["UFC_FONT"]					= {50, 255, 30, 255}
materials["DBG_UFC"]					= {0, 255, 100, 5}

add_MDG_material(MDG_SELF_IDS.HUD)
add_MDG_material(MDG_SELF_IDS.LMDI)
add_MDG_material(MDG_SELF_IDS.RMDI)
add_MDG_material(MDG_SELF_IDS.HI)

materials["MPD_BACKGROUND"]       		= {0, 0, 0, 255}
materials["MPD_DMC_OUTLINE"]			= materials["BLACK"]
local HUD_mat = materials[MDG_material_name(MDG_SELF_IDS.HUD)]
materials["HUD_GREEN_FOV"]              = {HUD_mat[0], HUD_mat[1], HUD_mat[2], 100} -- used for FOV lens render
materials["RWR_STROKE"]					= {0, 255, 0, 350}

-- HMD
materials["HMD_SYMBOLOGY_MATERIAL"]		= {2, 255, 20, 255}

-------TEXTURES-------
textures = {}

local ResourcesPath = LockOn_Options.script_path.."../IndicationResources/"

textures["ARCADE"]							= {"arcade.tga",	materials["INDICATION_COMMON_RED"]}		-- Control Indicator
textures["ARCADE_WHITE"]					= {"arcade.tga",	materials["SIMPLE_WHITE"]}				-- Control Indicator
textures["IFEI_INDICATIONS"]				= {ResourcesPath.."IFEI/IFEI_Fixed_Symbology.dds", materials["IFEI_FONT"]}				-- Control Indicator
-- DBG
--textures["INDICATION_MPD_ADI_DBG"]		 	= {ResourcesPath.."MDG/DBG_ADI_background.jpg", materials["DBG_RED"]}

textures["INDICATION_RWR"]					= {ResourcesPath.."RWR/indication_RWR.tga", materials["INDICATION_COMMON_GREEN"]}
textures["INDICATION_RWR_LINE"]				= {"arcade.tga",							materials["INDICATION_COMMON_GREEN"]}
textures["INDICATION_TGP"]					= {ResourcesPath.."MDG/tgp_texture.tga",	materials["INDICATION_COMMON_WHITE"]}

textures["AGM_65E_GRID_0"]			 		 = {"agm_65e_grid.tga", materials["INDICATION_COMMON_GREEN"]}
textures["AGM_65E_GRID_1"]			 		 = {"agm_65e_grid.tga", materials["INDICATION_COMMON_GREEN"]}
textures["AGM_65E_GRID_2"]			 		 = {"agm_65e_grid.tga", materials["INDICATION_COMMON_WHITE"]}
-------FONTS----------

fonts = {}

local function add_MDG_font(ID)
	fonts[MDG_font_name(ID)] = MDG_fonts[ID]
end

fonts["font_MPD_DMC_outline"]			= {fontdescription["font_stroke_MDG_DMC_outline"], 10, materials["BLACK"]}
fonts["font_MPD_DMC_main"]				= {fontdescription["font_stroke_MDG_DMC_main"], 10, materials[MDG_material_name(MDG_SELF_IDS.HI)]}
fonts["font_IFEI"]						= {fontdescription["font_IFEI"], 10, materials["IFEI_FONT"]}
fonts["font_UFC"]						= {fontdescription["font_UFC"], 10, materials["UFC_FONT"]}
fonts["font_RWR"]						= {fontdescription["font_stroke_RWR"], 10, materials["RWR_STROKE"]}
fonts["font_AGM_65E_0"]               	= {fontdescription["font_AGM_65E"], 1, materials["INDICATION_COMMON_GREEN"]}
fonts["font_AGM_65E_1"]               	= {fontdescription["font_AGM_65E"], 1, materials["INDICATION_COMMON_GREEN"]}
fonts["font_AGM_65E_2"]               	= {fontdescription["font_AGM_65E"], 1, materials["INDICATION_COMMON_WHITE"]}

add_MDG_font(MDG_SELF_IDS.HUD)
add_MDG_font(MDG_SELF_IDS.LMDI)
add_MDG_font(MDG_SELF_IDS.RMDI)
add_MDG_font(MDG_SELF_IDS.HI)

fonts["font_TGP"]					= {fontdescription["font_TGP"], 10, materials["INDICATION_COMMON_WHITE"]}
fonts["font_ATFLIR"]				= {fontdescription["font_ATFLIR"], 10, materials["INDICATION_COMMON_WHITE"]}

-- HMD
fonts["HMD_FONT_MATERIAL"]			= {fontdescription["font_stroke_HMD"], 10, materials["HMD_SYMBOLOGY_MATERIAL"]}

-- path for stroke symbology
symbologyPaths = {LockOn_Options.script_path.."../IndicationResources/MDG",
					LockOn_Options.script_path.."../IndicationResources/RWR",
					LockOn_Options.script_path.."../IndicationResources/HMD",
					"Fonts/font_TGP_LITENING_AT.tga",
					}

-- BAKE MATERIALS
HUD_BAKE_MATERIAL 		= "HUD_Bake"
LMDI_BAKE_MATERIAL 		= "LMDI_Bake"
RMDI_BAKE_MATERIAL		= "RMDI_Bake"
AMPCD_BAKE_MATERIAL		= "AMPCD_Bake"
RWR_BAKE_MATERIAL		= "RWR_Bake"

