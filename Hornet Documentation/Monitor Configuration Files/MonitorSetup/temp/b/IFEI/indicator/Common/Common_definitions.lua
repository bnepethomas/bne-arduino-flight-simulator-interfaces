dofile(LockOn_Options.script_path.."IFEI/indicator/Common/Common_functions.lua")

IFEI = 
{
	RIGHT = 0,
	LEFT = 1
}

--[[IFEI_CHARS = 
{
	R = 0,
	L = 1,
	Z = 2,
	SP = 3
}]]--

scale = 0.75
HorizontalSize			= scale * 0.0099
VerticalSize			= scale * 0.009
OffsetToLeft			= -0.008025

XPositionRPM			= 0.0197
YPositionRPM			= 0.03145

XPositionTEMP			= 0.0252
YPositionTEMP			= 0.02135

YPositionFF				= 0.0116
YPositionOilPress		= -0.0296

XPossitionBingo 		= 0.0897
YPossitionBingo 		= 0.00027
XPossitionFuel 			= 0.090125
YPossitionFuelUp 		= 0.0301
YPossitionFuelDown 		= 0.01845

XPositionClock 			= 0.0895
YPositionClock 			= -0.0191
YPositionTimer 			= -0.0295
HorizontalSizeClock		= scale * 0.00875
VerticalSizeClock		= scale * 0.009
OffsetToDoubleDot 		= HorizontalSizeClock + 0.0005
OffsetToNextNumeral  	= OffsetToDoubleDot / 2.5

RPM_XPos 				= -0.0035
RPM_YPos				= 0.0295
RPM_XLBPos				= 54
RPM_YLBPos				= 146
RPM_Height				= 123
RPM_Width				= 237
RPM_Attitude			= RPM_Height / RPM_Width
RPM_XSize				= 0.007
RPM_YSize				= RPM_XSize * RPM_Attitude

Temp_XPos 				= -0.0044
Temp_YPos				= 0.019725
Temp_XLBPos				= 25
Temp_YLBPos				= 317
Temp_Width				= 294
Temp_Height				= 123
Temp_Attitude			= Temp_Height / Temp_Width
Temp_XSize				= 0.009
Temp_YSize				= Temp_XSize * Temp_Attitude

FF_XPos 				= -0.00335
FF_YPos					= 0.00745
FF_XLBPos				= 738
FF_YLBPos				= 994
FF_Width				= 234
FF_Height				= 194
FF_Attitude				= FF_Height / FF_Width
FF_XSize				= 0.00715
FF_YSize				= FF_XSize * FF_Attitude

NOZ_XPos 				= -0.0026
NOZ_YPos				= -0.01125
NOZ_XLBPos				= 55
NOZ_YLBPos				= 482
NOZ_Width				= 214
NOZ_Height				= 110
NOZ_Attitude			= NOZ_Height / NOZ_Width
NOZ_XSize				= 0.005
NOZ_YSize				= NOZ_XSize * NOZ_Attitude

OIL_XPos 				= -0.0024725
OIL_YPos				= -0.03171
OIL_XLBPos				= 87
OIL_YLBPos				= 654
OIL_Width				= 168
OIL_Height				= 113
OIL_Attitude			= OIL_Height / OIL_Width
OIL_XSize				= 0.0055
OIL_YSize				= OIL_XSize * OIL_Attitude

BINGO_XPos 				= 0.0723
BINGO_YPos				= 0.00555
BINGO_XLBPos			= 13
BINGO_YLBPos			= 823
BINGO_Width				= 318
BINGO_Height			= 110
BINGO_Attitude			= BINGO_Height / BINGO_Width
BINGO_XSize				= 0.01
BINGO_YSize				= BINGO_XSize * BINGO_Attitude

L_XPos 					= 0.0905
L_YPos					= 0.027
L_XLBPos				= 391
L_YLBPos				= 825
L_Width					= 74
L_Height				= 113
L_Attitude				= L_Height / L_Width
L_XSize					= 0.0022
L_YSize					= L_XSize * L_Attitude

R_XPos 					= 0.0905
R_YPos					= 0.0151
R_XLBPos				= 558
R_YLBPos				= 825
R_Width					= 74
R_Height				= 113
R_Attitude				= R_Height / R_Width
R_XSize					= 0.0022
R_YSize					= R_XSize * R_Attitude

Z_XPos 					= 0.09
Z_YPos					= -0.02215
Z_XLBPos				= 558
Z_YLBPos				= 993
Z_Width					= 74
Z_Height				= 113
Z_Attitude				= Z_Height / Z_Width
Z_XSize					= 0.0022
Z_YSize					= Z_XSize * Z_Attitude

LScale_XPos 			= -0.024
LScale_YPos				= -0.0184
LScale_XLBPos			= 341
LScale_YLBPos			= 683
LScale_Width			= 684
LScale_Height			= 684
LScale_Attitude			= LScale_Height / LScale_Width
LScale_XSize			= 0.0211
LScale_YSize			= LScale_XSize * LScale_Attitude

RScale_XPos 			= 0.024
RScale_YPos				= -0.0184
RScale_XLBPos			= 341
RScale_YLBPos			= 683
RScale_Width			= 684
RScale_Height			= 684
RScale_Attitude			= RScale_Height / RScale_Width
RScale_XSize			= 0.0211
RScale_YSize			= RScale_XSize * RScale_Attitude

L0_XPos 				= -0.0025
L0_YPos					= 0.0011
L0_XLBPos				= 55
L0_YLBPos				= 978
L0_Width				= 62
L0_Height				= 78
L0_Attitude				= L0_Height / L0_Width
L0_XSize				= 0.00175
L0_YSize				= L0_XSize * L0_Attitude

R0_XPos 				= 0.000825
R0_YPos					= 0.0011
R0_XLBPos				= 55
R0_YLBPos				= 978
R0_Width				= 62
R0_Height				= 78
R0_Attitude				= R0_Height / R0_Width
R0_XSize				= 0.00175
R0_YSize				= R0_XSize * R0_Attitude

L50_XPos 				= -0.009
L50_YPos				= -0.0148
L50_XLBPos				= 200
L50_YLBPos				= 978
L50_Width				= 110
L50_Height				= 78
L50_Attitude			= L50_Height / L50_Width
L50_XSize				= 0.003
L50_YSize				= L50_XSize * L50_Attitude

R50_XPos 				= 0.0065
R50_YPos				= L50_YPos
R50_XLBPos				= 200
R50_YLBPos				= 978
R50_Width				= 110
R50_Height				= 78
R50_Attitude			= R50_Height / R50_Width
R50_XSize				= 0.003
R50_YSize				= R50_XSize * R50_Attitude

L100_XPos 				= -0.0243
L100_YPos				= -0.0213
L100_XLBPos				= 352
L100_YLBPos				= 978
L100_Width				= 149
L100_Height				= 78
L100_Attitude			= L100_Height / L100_Width
L100_XSize				= 0.0045
L100_YSize				= L100_XSize * L100_Attitude

R100_XPos 				= 0.0195
R100_YPos				= L100_YPos
R100_XLBPos				= 352
R100_YLBPos				= 978
R100_Width				= 149
R100_Height				= 78
R100_Attitude			= R100_Height / R100_Width
R100_XSize				= 0.0045
R100_YSize				= R100_XSize * R100_Attitude

OffsetPointer			= 0.007
value = 0

LPointer_XPos 			= -0.0235
LPointer_YPos			= 0.0022
LPointer_XLBPos			= 690
LPointer_YLBPos			= 746
LPointer_Width			= 327
LPointer_Height			= 43
LPointer_Attitude		= LPointer_Height / LPointer_Width
LPointer_XSize			= 0.0085
LPointer_YSize			= LPointer_XSize * LPointer_Attitude

RPointer_XPos 			= 0.0235
RPointer_YPos			= 0.0022
RPointer_XLBPos			= 690
RPointer_YLBPos			= 746
RPointer_Width			= 327
RPointer_Height			= 43
RPointer_Attitude		= RPointer_Height / RPointer_Width
RPointer_XSize			= 0.0085
RPointer_YSize			= RPointer_XSize * RPointer_Attitude