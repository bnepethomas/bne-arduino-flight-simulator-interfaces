dofile(LockOn_Options.script_path.."UFC/indicator/UFC_usefull_definitions.lua")

UFC_MainDummy			= CreateElement "ceSimple"
UFC_MainDummy.name		= "UFC_MainDummy"
UFC_MainDummy.init_pos	= {0.0, 0.0, -0.00005}
UFC_MainDummy.isvisible	= true
Add(UFC_MainDummy)

UFC_mask				= CreateElement "ceMeshPoly"
UFC_mask.name			= "UFC_mask"
UFC_mask.primitivetype	= "triangles"
UFC_mask.vertices		= {	{  -x_size_05, -y_size_05},
								{  -x_size_05,  y_size_05},
								{ x_size_05,  y_size_05},
								{ x_size_05, -y_size_05}}
UFC_mask.indices		= default_box_indices
UFC_mask.material		= "DBG_UFC"
UFC_mask.parent_element	= UFC_MainDummy.name
UFC_mask.isvisible		= false
Add(UFC_mask)


for display_id = 1, 5 do
	UFC_OptionDisplay					= CreateElement "ceStringPoly"
	UFC_OptionDisplay.name				= "UFC_OptionDisplay"..string.format("%d", display_id)
	UFC_OptionDisplay.material			= "font_UFC"
	UFC_OptionDisplay.init_pos			= {option_display_pos_x, option_display_pos_y0 - option_display_pos_dy * (display_id - 1), 0}
	UFC_OptionDisplay.alignment			= "LeftCenter"
	UFC_OptionDisplay.stringdefs		= UFC_OptionDisplay_font
	UFC_OptionDisplay.value				= ""
	UFC_OptionDisplay.controllers		= {{"OptionDisplay", display_id}}
	UFC_OptionDisplay.parent_element	= UFC_mask.name
	Add(UFC_OptionDisplay)
	use_mipfilter(UFC_OptionDisplay)

	UFC_OptionCueing					= CreateElement "ceStringPoly"
	UFC_OptionCueing.name				= "UFC_OptionCueing"..string.format("%d", display_id)
	UFC_OptionCueing.material			= "font_UFC"
	UFC_OptionCueing.init_pos			= {option_cueing_pos_x, option_display_pos_y0 - option_display_pos_dy * (display_id - 1), 0}
	UFC_OptionCueing.alignment			= "LeftCenter"
	UFC_OptionCueing.stringdefs		= UFC_OptionDisplay_font
	UFC_OptionCueing.value				= ":"
	UFC_OptionCueing.controllers		= {{"OptionCueing", display_id}}
	UFC_OptionCueing.parent_element	= UFC_mask.name
	Add(UFC_OptionCueing)
	use_mipfilter(UFC_OptionCueing)
end


UFC_ScratchPadString1Display					= CreateElement "ceStringPoly"
UFC_ScratchPadString1Display.name			= "UFC_ScratchPadString1Display"
UFC_ScratchPadString1Display.material		= "font_UFC"
UFC_ScratchPadString1Display.init_pos		= {scratch_pad_string1_display_pos_x, scratch_pad_string_display_pos_y, 0}
UFC_ScratchPadString1Display.alignment		= "LeftCenter"
UFC_ScratchPadString1Display.stringdefs		= UFC_ScratchPadStringDisplay_font
UFC_ScratchPadString1Display.value			= ""
UFC_ScratchPadString1Display.controllers		= {{"ScratchPadStringDisplay", 1}}
UFC_ScratchPadString1Display.parent_element	= UFC_mask.name
Add(UFC_ScratchPadString1Display)
use_mipfilter(UFC_ScratchPadString1Display)

UFC_ScratchPadString2Display					= CreateElement "ceStringPoly"
UFC_ScratchPadString2Display.name			= "UFC_ScratchPadString2Display"
UFC_ScratchPadString2Display.material		= "font_UFC"
UFC_ScratchPadString2Display.init_pos		= {scratch_pad_string2_display_pos_x, scratch_pad_string_display_pos_y, 0}
UFC_ScratchPadString2Display.alignment		= "LeftCenter"
UFC_ScratchPadString2Display.stringdefs		= UFC_ScratchPadStringDisplay_font
UFC_ScratchPadString2Display.value			= ""
UFC_ScratchPadString2Display.controllers		= {{"ScratchPadStringDisplay", 2}}
UFC_ScratchPadString2Display.parent_element	= UFC_mask.name
Add(UFC_ScratchPadString2Display)
use_mipfilter(UFC_ScratchPadString2Display)

UFC_ScratchPadNumberDisplay					= CreateElement "ceStringPoly"
UFC_ScratchPadNumberDisplay.name			= "UFC_ScratchPadNumberDisplay"
UFC_ScratchPadNumberDisplay.material		= "font_UFC"
UFC_ScratchPadNumberDisplay.init_pos		= {scratch_pad_number_display_pos_x, scratch_pad_number_display_pos_y, 0}
UFC_ScratchPadNumberDisplay.alignment		= "RightCenter"
UFC_ScratchPadNumberDisplay.stringdefs		= UFC_ScratchPadNumberDisplay_font
UFC_ScratchPadNumberDisplay.value			= ""
UFC_ScratchPadNumberDisplay.controllers		= {{"ScratchPadNumberDisplay"}}
UFC_ScratchPadNumberDisplay.parent_element	= UFC_mask.name
Add(UFC_ScratchPadNumberDisplay)
use_mipfilter(UFC_ScratchPadNumberDisplay)

UFC_Comm1Display				= CreateElement "ceStringPoly"
UFC_Comm1Display.name			= "UFC_Comm1Display"
UFC_Comm1Display.material		= "font_UFC"
UFC_Comm1Display.init_pos		= {comm_1_display_pos_x, comm_1_display_pos_y, 0}
UFC_Comm1Display.alignment		= "LeftCenter"
UFC_Comm1Display.stringdefs		= UFC_Comm1Display_font
UFC_Comm1Display.value			= ""
UFC_Comm1Display.controllers	= {{"CommChannel", 1}}
UFC_Comm1Display.parent_element	= UFC_mask.name
Add(UFC_Comm1Display)
use_mipfilter(UFC_Comm1Display)

UFC_Comm2Display				= CreateElement "ceStringPoly"
UFC_Comm2Display.name			= "UFC_Comm2Display"
UFC_Comm2Display.material		= "font_UFC"
UFC_Comm2Display.init_pos		= {comm_2_display_pos_x, comm_2_display_pos_y, 0}
UFC_Comm2Display.alignment		= "LeftCenter"
UFC_Comm2Display.stringdefs		= UFC_Comm2Display_font
UFC_Comm2Display.value			= ""
UFC_Comm2Display.controllers	= {{"CommChannel", 2}}
UFC_Comm2Display.parent_element	= UFC_mask.name
Add(UFC_Comm2Display)
use_mipfilter(UFC_Comm2Display)