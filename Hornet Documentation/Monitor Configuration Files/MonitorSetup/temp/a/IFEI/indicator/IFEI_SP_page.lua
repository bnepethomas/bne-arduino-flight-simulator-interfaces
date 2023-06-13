dofile(LockOn_Options.script_path.."IFEI/indicator/Common/Common_definitions.lua")

addStrPoly("txt_Codes", {XPositionTEMP, YPositionTEMP}, "RightCenter", {{"txt_Codes"}})
addStrPoly("txt_SP", {OffsetToLeft, YPositionTEMP}, "RightCenter", {{"txt_DrawChar"}})

drawDD()