dofile(LockOn_Options.script_path.."IFEI/indicator/Common/Common_definitions.lua")

addStrPoly("txt_T", {XPossitionFuel, YPossitionFuelUp}, "RightCenter", {{"txt_T"}})
addStrPoly("txt_TimeSetMode", {XPossitionFuel, YPossitionFuelDown}, "RightCenter", {{"txt_TimeSetMode"}})

addStrPoly("txt_CLOCK_S", {XPositionClock, YPositionClock}, "RightCenter", {{"txt_CLOCK_S"}}, {VerticalSizeClock, HorizontalSizeClock})
addStrPoly("txt_CLOCK_M", {XPositionClock - OffsetToDoubleDot - OffsetToNextNumeral, YPositionClock}, "RightCenter", {{"txt_CLOCK_M"}}, {VerticalSizeClock, HorizontalSizeClock})
addStrPoly("txt_CLOCK_H", {XPositionClock - OffsetToDoubleDot * 2 - OffsetToNextNumeral * 2, YPositionClock}, "RightCenter", {{"txt_CLOCK_H"}}, {VerticalSizeClock, HorizontalSizeClock})

drawDD()

addTextPoly("RPMTexture", {RPM_XSize, RPM_YSize}, {RPM_XLBPos, RPM_YLBPos}, {RPM_Width, RPM_Height}, {RPM_XPos, RPM_YPos}, {{"ShowLabels", 0}})
addTextPoly("TempTexture", {Temp_XSize, Temp_YSize}, {Temp_XLBPos, Temp_YLBPos}, {Temp_Width, Temp_Height}, {Temp_XPos, Temp_YPos}, {{"ShowLabels", 0}})
addTextPoly("FFTexture", {FF_XSize, FF_YSize}, {FF_XLBPos, FF_YLBPos}, {FF_Width, FF_Height}, {FF_XPos, FF_YPos}, {{"ShowLabels", 0}})
addTextPoly("NOZTexture", {NOZ_XSize, NOZ_YSize}, {NOZ_XLBPos, NOZ_YLBPos}, {NOZ_Width, NOZ_Height}, {NOZ_XPos, NOZ_YPos}, {{"ShowLabels", 0}})
addTextPoly("OILTexture", {OIL_XSize, OIL_YSize}, {OIL_XLBPos, OIL_YLBPos}, {OIL_Width, OIL_Height}, {OIL_XPos, OIL_YPos}, {{"ShowLabels", 0}})
addTextPoly("LScaleTexture", {LScale_XSize, LScale_YSize}, {LScale_XLBPos, LScale_YLBPos}, {LScale_Width, LScale_Height}, {LScale_XPos, LScale_YPos}, {{"ShowLabels", 0}})
addTextPoly("RScaleTexture", {RScale_XSize, RScale_YSize}, {RScale_XLBPos, RScale_YLBPos}, {RScale_Width, RScale_Height}, {RScale_XPos, RScale_YPos}, {{"ShowLabels", 1}})
addTextPoly("L0Texture", {L0_XSize, L0_YSize}, {L0_XLBPos, L0_YLBPos}, {L0_Width, L0_Height}, {L0_XPos, L0_YPos}, {{"ShowLabels", 0}})
addTextPoly("R0Texture", {R0_XSize, R0_YSize}, {R0_XLBPos, R0_YLBPos}, {R0_Width, R0_Height}, {R0_XPos, R0_YPos}, {{"ShowLabels", 0}})
addTextPoly("L50Texture", {L50_XSize, L50_YSize}, {L50_XLBPos, L50_YLBPos}, {L50_Width, L50_Height}, {L50_XPos, L50_YPos}, {{"ShowLabels", 0}})
addTextPoly("R50Texture", {R50_XSize, R50_YSize}, {R50_XLBPos, R50_YLBPos}, {R50_Width, R50_Height}, {R50_XPos, R50_YPos}, {{"ShowLabels", 0}})
addTextPoly("L100Texture", {L100_XSize, L100_YSize}, {L100_XLBPos, L100_YLBPos}, {L100_Width, L100_Height}, {L100_XPos, L100_YPos}, {{"ShowLabels", 0}})
addTextPoly("R100Texture", {R100_XSize, R100_YSize}, {R100_XLBPos, R100_YLBPos}, {R100_Width, R100_Height}, {R100_XPos, R100_YPos}, {{"ShowLabels", 0}})