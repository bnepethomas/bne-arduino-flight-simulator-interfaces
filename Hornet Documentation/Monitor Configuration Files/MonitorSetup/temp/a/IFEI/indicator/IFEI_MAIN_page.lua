dofile(LockOn_Options.script_path.."IFEI/indicator/Common/Common_definitions.lua")

addStrPoly("txt_RPM_R", {XPositionRPM, YPositionRPM}, "RightCenter", {{"txt_RPM", IFEI.RIGHT}})
addStrPoly("txt_RPM_L", {OffsetToLeft, YPositionRPM}, "RightCenter", {{"txt_RPM", IFEI.LEFT}})

addStrPoly("txt_TEMP_R", {XPositionTEMP, YPositionTEMP}, "RightCenter", {{"txt_TEMP", IFEI.RIGHT}})
addStrPoly("txt_TEMP_L", {OffsetToLeft, YPositionTEMP}, "RightCenter", {{"txt_TEMP", IFEI.LEFT}})

addStrPoly("txt_FF_R", {XPositionTEMP, YPositionFF}, "RightCenter", {{"txt_FF", IFEI.RIGHT}})
addStrPoly("txt_FF_L", {OffsetToLeft, YPositionFF}, "RightCenter", {{"txt_FF", IFEI.LEFT}})

addStrPoly("txt_OilPress_R", {XPositionRPM, YPositionOilPress}, "RightCenter", {{"txt_OilPress", IFEI.RIGHT}})
addStrPoly("txt_OilPress_L", {OffsetToLeft, YPositionOilPress}, "RightCenter", {{"txt_OilPress", IFEI.LEFT}})

addStrPoly("txt_BINGO", {XPossitionBingo, YPossitionBingo}, "RightCenter", {{"txt_BINGO"}})
addStrPoly("txt_FUEL_UP", {XPossitionFuel, YPossitionFuelUp}, "RightCenter", {{"txt_FUEL_UP"}})
addStrPoly("txt_FUEL_DOWN", {XPossitionFuel, YPossitionFuelDown}, "RightCenter", {{"txt_FUEL_DOWN"}})

--addStrPoly("txt_FUEL_L", {XPossitionRL, YPossitionFuelL}, "LeftTop", {{"txt_DrawChar", IFEI_CHARS.L}}, {VerticalSize / 2, HorizontalSize / 2})
--addStrPoly("txt_FUEL_R", {XPossitionRL, YPossitionFuelR}, "LeftTop", {{"txt_DrawChar", IFEI_CHARS.R}}, {VerticalSize / 2, HorizontalSize / 2})
--addStrPoly("txt_Z", {XPositionClock, YPositionZ}, "LeftTop", {{"txt_DrawChar", IFEI_CHARS.Z}}, {VerticalSize / 2, HorizontalSize / 2})

addStrPoly("txt_CLOCK_S", {XPositionClock, YPositionClock}, "RightCenter", {{"txt_CLOCK_S"}}, {VerticalSizeClock, HorizontalSizeClock})
addStrPoly("txt_CLOCK_M", {XPositionClock - OffsetToDoubleDot - OffsetToNextNumeral, YPositionClock}, "RightCenter", {{"txt_CLOCK_M"}}, {VerticalSizeClock, HorizontalSizeClock})
addStrPoly("txt_CLOCK_H", {XPositionClock - OffsetToDoubleDot * 2 - OffsetToNextNumeral * 2, YPositionClock}, "RightCenter", {{"txt_CLOCK_H"}}, {VerticalSizeClock, HorizontalSizeClock})

addStrPoly("txt_TIMER_S", {XPositionClock, YPositionTimer}, "RightCenter", {{"txt_TIMER_S"}}, {VerticalSizeClock, HorizontalSizeClock})
addStrPoly("txt_TIMER_M", {XPositionClock - OffsetToDoubleDot - OffsetToNextNumeral, YPositionTimer}, "RightCenter", {{"txt_TIMER_M"}}, {VerticalSizeClock, HorizontalSizeClock})
addStrPoly("txt_TIMER_H", {XPositionClock - OffsetToDoubleDot * 2 - OffsetToNextNumeral * 2, YPositionTimer}, "RightCenter", {{"txt_TIMER_H"}}, {VerticalSizeClock, HorizontalSizeClock})

drawDD()

-- Textures
addTextPoly("RPMTexture", {RPM_XSize, RPM_YSize}, {RPM_XLBPos, RPM_YLBPos}, {RPM_Width, RPM_Height}, {RPM_XPos, RPM_YPos}, {{"ShowLabels", 0}})
addTextPoly("TempTexture", {Temp_XSize, Temp_YSize}, {Temp_XLBPos, Temp_YLBPos}, {Temp_Width, Temp_Height}, {Temp_XPos, Temp_YPos}, {{"ShowLabels", 0}})
addTextPoly("FFTexture", {FF_XSize, FF_YSize}, {FF_XLBPos, FF_YLBPos}, {FF_Width, FF_Height}, {FF_XPos, FF_YPos}, {{"ShowLabels", 0}})
addTextPoly("NOZTexture", {NOZ_XSize, NOZ_YSize}, {NOZ_XLBPos, NOZ_YLBPos}, {NOZ_Width, NOZ_Height}, {NOZ_XPos, NOZ_YPos}, {{"ShowLabels", 0}})
addTextPoly("OILTexture", {OIL_XSize, OIL_YSize}, {OIL_XLBPos, OIL_YLBPos}, {OIL_Width, OIL_Height}, {OIL_XPos, OIL_YPos}, {{"ShowLabels", 0}})
addTextPoly("BINGOTexture", {BINGO_XSize, BINGO_YSize}, {BINGO_XLBPos, BINGO_YLBPos}, {BINGO_Width, BINGO_Height}, {BINGO_XPos, BINGO_YPos}, {{"ShowLabels", 2}})
addTextPoly("LTexture", {L_XSize, L_YSize}, {L_XLBPos, L_YLBPos}, {L_Width, L_Height}, {L_XPos, L_YPos}, {{"ShowLabels", 3}})
addTextPoly("RTexture", {R_XSize, R_YSize}, {R_XLBPos, R_YLBPos}, {R_Width, R_Height}, {R_XPos, R_YPos}, {{"ShowLabels", 3}})
addTextPoly("ZTexture", {Z_XSize, Z_YSize}, {Z_XLBPos, Z_YLBPos}, {Z_Width, Z_Height}, {Z_XPos, Z_YPos}, {{"ShowLabels", 4}})
addTextPoly("LScaleTexture", {LScale_XSize, LScale_YSize}, {LScale_XLBPos, LScale_YLBPos}, {LScale_Width, LScale_Height}, {LScale_XPos, LScale_YPos}, {{"ShowLabels", 0}})
addTextPoly("RScaleTexture", {RScale_XSize, RScale_YSize}, {RScale_XLBPos, RScale_YLBPos}, {RScale_Width, RScale_Height}, {RScale_XPos, RScale_YPos}, {{"ShowLabels", 1}})
addTextPoly("L0Texture", {L0_XSize, L0_YSize}, {L0_XLBPos, L0_YLBPos}, {L0_Width, L0_Height}, {L0_XPos, L0_YPos}, {{"ShowLabels", 0}})
addTextPoly("R0Texture", {R0_XSize, R0_YSize}, {R0_XLBPos, R0_YLBPos}, {R0_Width, R0_Height}, {R0_XPos, R0_YPos}, {{"ShowLabels", 0}})
addTextPoly("L50Texture", {L50_XSize, L50_YSize}, {L50_XLBPos, L50_YLBPos}, {L50_Width, L50_Height}, {L50_XPos, L50_YPos}, {{"ShowLabels", 0}})
addTextPoly("R50Texture", {R50_XSize, R50_YSize}, {R50_XLBPos, R50_YLBPos}, {R50_Width, R50_Height}, {R50_XPos, R50_YPos}, {{"ShowLabels", 0}})
addTextPoly("L100Texture", {L100_XSize, L100_YSize}, {L100_XLBPos, L100_YLBPos}, {L100_Width, L100_Height}, {L100_XPos, L100_YPos}, {{"ShowLabels", 0}})
addTextPoly("R100Texture", {R100_XSize, R100_YSize}, {R100_XLBPos, R100_YLBPos}, {R100_Width, R100_Height}, {R100_XPos, R100_YPos}, {{"ShowLabels", 0}})
addTextPoly("LPointerTexture", {LPointer_XSize, LPointer_YSize}, {LPointer_XLBPos, LPointer_YLBPos}, {LPointer_Width, LPointer_Height}, {LPointer_XPos, LPointer_YPos}, {{"ShowPointer", 0, OffsetPointer}}, "LeftCenter")
addTextPoly("RPointerTexture", {RPointer_XSize, RPointer_YSize}, {RPointer_XLBPos, RPointer_YLBPos}, {RPointer_Width, RPointer_Height}, {RPointer_XPos, RPointer_YPos}, {{"ShowPointer", 1, OffsetPointer}}, "LeftCenter")