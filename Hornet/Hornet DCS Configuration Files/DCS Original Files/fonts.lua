dofile(LockOn_Options.common_script_path.."Fonts/symbols_locale.lua")
dofile(LockOn_Options.common_script_path.."Fonts/fonts_cmn.lua")
dofile(LockOn_Options.common_script_path.."tools.lua")

local ResourcesPath = LockOn_Options.script_path.."../IndicationResources/"

fontdescription = {}

IFEI_xsize = 120
IFEI_ysize = 160
fontdescription["font_IFEI"] = {
	texture    = ResourcesPath.."IFEI/FONT_IFEI_F18.dds",
	size       = {6, 6},
	resolution = {1024, 1024},	
	default    = {IFEI_xsize, IFEI_ysize},
	chars	   = {

		 [1]   = {symbol[' '], IFEI_xsize, IFEI_ysize},
		 [2]   = {symbol['0'], IFEI_xsize, IFEI_ysize},
		 [3]   = {symbol['1'], IFEI_xsize, IFEI_ysize},
		 [4]   = {symbol['2'], IFEI_xsize, IFEI_ysize},
		 [5]   = {symbol['3'], IFEI_xsize, IFEI_ysize},
		 [6]   = {symbol['4'], IFEI_xsize, IFEI_ysize},

		 [7]   = {symbol['5'], IFEI_xsize, IFEI_ysize},
		 [8]   = {symbol['6'], IFEI_xsize, IFEI_ysize},
		 [9]   = {symbol['7'], IFEI_xsize, IFEI_ysize},
		 [10]  = {symbol['8'], IFEI_xsize, IFEI_ysize},
		 [11]  = {symbol['9'], IFEI_xsize, IFEI_ysize},
		 [12]  = {symbol[':'], IFEI_xsize, IFEI_ysize},

		 [13]  = {latin['C'], IFEI_xsize, IFEI_ysize},
		 [14]  = {latin['D'], IFEI_xsize, IFEI_ysize},
		 [15]  = {latin['F'], IFEI_xsize, IFEI_ysize},
		 [16]  = {latin['H'], IFEI_xsize, IFEI_ysize},
		 [17]  = {latin['I'], IFEI_xsize, IFEI_ysize},
		 [18]  = {latin['M'], IFEI_xsize, IFEI_ysize},

		 [19]  = {latin['P'], IFEI_xsize, IFEI_ysize},
		 [20]  = {latin['S'], IFEI_xsize, IFEI_ysize},
		 [21]  = {latin['T'], IFEI_xsize, IFEI_ysize},
		 [22]  = {latin['W'], IFEI_xsize, IFEI_ysize},
		 [23]  = {latin['X'], IFEI_xsize, IFEI_ysize},
		 [24]  = {latin['Y'], IFEI_xsize, IFEI_ysize},

		 [25]  = {symbol['+'], IFEI_xsize, IFEI_ysize},
		 [26]  = {symbol['-'], IFEI_xsize, IFEI_ysize},
		 [27]  = {latin['d'], IFEI_xsize, IFEI_ysize},
		 [28]  = {latin['A'], IFEI_xsize, IFEI_ysize},
		 [29]  = {latin['C'], IFEI_xsize, IFEI_ysize},
		 [30]  = {latin['E'], IFEI_xsize, IFEI_ysize},

		 [31]  = {latin['F'], IFEI_xsize, IFEI_ysize},
		 [32]  = {latin['L'], IFEI_xsize, IFEI_ysize},
		 [33]  = {latin['O'], IFEI_xsize, IFEI_ysize},

	}
}

fontdescription["font_stroke_MDG"] = {
	class     = "ceSLineFont",
	symb_storage = "stroke_font",
	thickness  = stroke_thickness,
	fuzziness  = stroke_fuzziness,
	draw_as_wire = dbg_drawStrokesAsWire,
	default    = {12, 20}, -- F/A-18 MDG DIs (display increments)
	chars	   = {
		 [1]   = {latin['A'], "A"},
		 [2]   = {latin['B'], "B"},
		 [3]   = {latin['C'], "C"},
		 [4]   = {latin['D'], "D"},
		 [5]   = {latin['E'], "E"},
		 [6]   = {latin['F'], "F"},
		 [7]   = {latin['G'], "G"},
		 [8]   = {latin['H'], "H"},
		 [9]   = {latin['I'], "I"},
		 [10]  = {latin['J'], "J"},
		 [11]  = {latin['K'], "K"},
		 [12]  = {latin['L'], "L"},
		 [13]  = {latin['M'], "M"},
		 [14]  = {latin['N'], "N"},
		 [15]  = {latin['O'], "O"},
		 [16]  = {latin['P'], "P"},
		 [17]  = {latin['Q'], "Q"},
		 [18]  = {latin['R'], "R"},
		 [19]  = {latin['S'], "S"},
		 [20]  = {latin['T'], "T"},
		 [21]  = {latin['U'], "U"},
		 [22]  = {latin['V'], "V"},
		 [23]  = {latin['W'], "W"},
		 [24]  = {latin['X'], "X"},
		 [25]  = {latin['Y'], "Y"},
		 [26]  = {latin['Z'], "Z"},
		 
		 [27]  = {symbol['0'], "0"},
		 [28]  = {symbol['1'], "1"},
		 [29]  = {symbol['2'], "2"},
		 [30]  = {symbol['3'], "3"},
		 [31]  = {symbol['4'], "4"},
		 [32]  = {symbol['5'], "5"},
		 [33]  = {symbol['6'], "6"},
		 [34]  = {symbol['7'], "7"},
		 [35]  = {symbol['8'], "8"},
		 [36]  = {symbol['9'], "9"},
		 
		 [37]  = {symbol['-'], "symbol-minus"},
		 [38]  = {symbol['+'], "symbol-plus"},
		 [39]  = {symbol['\''], "symbol-apostrophe"},
		 [40]  = {symbol['('], "symbol-parenthesis-left"},
		 [41]  = {symbol[')'], "symbol-parenthesis-right"},
		 [42]  = {symbol['*'], "symbol-asterisk"},
		 [43]  = {symbol['%'], "symbol-percent"},
		 [44]  = {symbol[','], "symbol-comma"},
		 [45]  = {symbol['°'], "symbol-degree"},
		 [46]  = {symbol['.'], "symbol-period"},
		 [47]  = {symbol['/'], "symbol-slash"},
		 [48]  = {symbol['\\'], "symbol-backslash"},
		 [49]  = {symbol['\"'], "symbol-quote"},
		 [50]  = {symbol['?'], "symbol-question"},
		 [51]  = {symbol[':'], "symbol-colon"},
		 [52]  = {symbol['#'], "symbol-octothorpe"},
		 [53]  = {symbol['='], "symbol-equal"},
		 [54]  = {symbol['_'], "symbol-underscore"},
		 [55]  = {symbol['^'], "symbol-lambda"},
	}
}

-- DMC generated font outline
fontdescription["font_stroke_MDG_DMC_outline"] = {}
copyTable(fontdescription["font_stroke_MDG_DMC_outline"], fontdescription["font_stroke_MDG"])
fontdescription["font_stroke_MDG_DMC_outline"].thickness = DMC_outline_thickness
fontdescription["font_stroke_MDG_DMC_outline"].fuzziness = DMC_outline_fuzziness

-- DMC generated font main
fontdescription["font_stroke_MDG_DMC_main"] = {}
copyTable(fontdescription["font_stroke_MDG_DMC_main"], fontdescription["font_stroke_MDG"])
fontdescription["font_stroke_MDG_DMC_main"].thickness = DMC_stroke_thickness
fontdescription["font_stroke_MDG_DMC_main"].fuzziness = DMC_stroke_fuzziness

UFC_xsize = 146
UFC_ysize = 146
fontdescription["font_UFC"] = {
	class		= "ceMultiTexPolyFont",
	size		= {7, 7},
	resolution	= {1024, 1024},
	default		= {UFC_xsize, UFC_ysize},
}

local charsUFC_set1 =
{
	[1]		= {symbol[' '],		UFC_xsize, UFC_ysize},
	[2]		= {symbol['0'],		UFC_xsize, UFC_ysize},
	[3]		= {symbol['1'],		UFC_xsize, UFC_ysize},
	[4]		= {symbol['2'],		UFC_xsize, UFC_ysize},
	[5]		= {symbol['3'],		UFC_xsize, UFC_ysize},
	[6]		= {symbol['4'],		UFC_xsize, UFC_ysize},
	[7]		= {symbol['5'],		UFC_xsize, UFC_ysize},
	[8]		= {symbol['6'],		UFC_xsize, UFC_ysize},
	[9]		= {symbol['7'],		UFC_xsize, UFC_ysize},
	[10]	= {symbol['8'],		UFC_xsize, UFC_ysize},
	[11]	= {symbol['9'],		UFC_xsize, UFC_ysize},
	[12]	= {symbol['~'],		UFC_xsize, UFC_ysize},	-- '2' от двадцатки
	[13]	= {latin['A'],		UFC_xsize, UFC_ysize},
	[14]	= {latin['B'],		UFC_xsize, UFC_ysize},
	[15]	= {latin['C'],		UFC_xsize, UFC_ysize},
	[16]	= {latin['D'],		UFC_xsize, UFC_ysize},
	[17]	= {latin['E'],		UFC_xsize, UFC_ysize},
	[18]	= {latin['F'],		UFC_xsize, UFC_ysize},
	[19]	= {latin['G'],		UFC_xsize, UFC_ysize},
	[20]	= {latin['H'],		UFC_xsize, UFC_ysize},
	[21]	= {latin['I'],		UFC_xsize, UFC_ysize},
	[22]	= {latin['J'],		UFC_xsize, UFC_ysize},
	[23]	= {latin['K'],		UFC_xsize, UFC_ysize},
	[24]	= {latin['L'],		UFC_xsize, UFC_ysize},
	[25]	= {latin['M'],		UFC_xsize, UFC_ysize},
	[26]	= {latin['N'],		UFC_xsize, UFC_ysize},
	[27]	= {latin['O'],		UFC_xsize, UFC_ysize},
	[28]	= {latin['P'],		UFC_xsize, UFC_ysize},
	[29]	= {latin['Q'],		UFC_xsize, UFC_ysize},
	[30]	= {latin['R'],		UFC_xsize, UFC_ysize},
	[31]	= {latin['S'],		UFC_xsize, UFC_ysize},
	[32]	= {latin['T'],		UFC_xsize, UFC_ysize},
	[33]	= {latin['U'],		UFC_xsize, UFC_ysize},
	[34]	= {latin['V'],		UFC_xsize, UFC_ysize},
	[35]	= {latin['W'],		UFC_xsize, UFC_ysize},
	[36]	= {latin['X'],		UFC_xsize, UFC_ysize},
	[37]	= {latin['Y'],		UFC_xsize, UFC_ysize},
	[38]	= {latin['Z'],		UFC_xsize, UFC_ysize},
	[39]	= {symbol['/'],		UFC_xsize, UFC_ysize},
	[40]	= {symbol['_'],		UFC_xsize, UFC_ysize},	-- тире
	[41]	= {symbol[':'],		UFC_xsize, UFC_ysize},
	[42]	= {symbol['*'],		UFC_xsize, UFC_ysize},
	[43]	= {symbol[','],		UFC_xsize, UFC_ysize},
	[44]	= {symbol['@'],		UFC_xsize, UFC_ysize},	-- градус
	[45]	= {symbol['.'],		UFC_xsize, UFC_ysize},
	[46]	= {symbol['\''],	UFC_xsize, UFC_ysize},
	[47]	= {symbol['-'],		UFC_xsize, UFC_ysize},
	[48]	= {symbol['`'],		UFC_xsize, UFC_ysize},	-- '1' от десятков
	[49]	= {symbol['|'],		UFC_xsize, UFC_ysize},
}

local charsUFC_set2 =
{
	[1]		= {latin['r'],		UFC_xsize, UFC_ysize},	-- comm receive indicator
	[2]		= {latin['d'],		UFC_xsize, UFC_ysize},	-- inverted triangle
	[3]		= {latin['u'],		UFC_xsize, UFC_ysize},	-- upright triangle

	[4]		= {latin['a'],		UFC_xsize, UFC_ysize},	-- 0 without right lines
	[5]		= {latin['b'],		UFC_xsize, UFC_ysize},	-- 0 without right upper line
	[6]		= {latin['c'],		UFC_xsize, UFC_ysize},	-- 0 without right lower line
	[7]		= {latin['e'],		UFC_xsize, UFC_ysize},	-- 3 without right lines
	[8]		= {latin['f'],		UFC_xsize, UFC_ysize},	-- 3 without right upper line
	[9]		= {latin['g'],		UFC_xsize, UFC_ysize},	-- 3 without right lower line
	[10]	= {latin['h'],		UFC_xsize, UFC_ysize},	-- 4 without right lines
	[11]	= {latin['i'],		UFC_xsize, UFC_ysize},	-- 4 without right upper line
	[12]	= {latin['j'],		UFC_xsize, UFC_ysize},	-- 4 without right lower line
	[13]	= {latin['k'],		UFC_xsize, UFC_ysize},	-- 5 without right lower line
	[14]	= {latin['l'],		UFC_xsize, UFC_ysize},	-- 6 without right lower line
	[15]	= {latin['m'],		UFC_xsize, UFC_ysize},	-- 7 without right lines
	[16]	= {latin['n'],		UFC_xsize, UFC_ysize},	-- 7 without right upper line
	[17]	= {latin['o'],		UFC_xsize, UFC_ysize},	-- 7 without right lower line
	[18]	= {latin['p'],		UFC_xsize, UFC_ysize},	-- 8 without right lines
	[19]	= {latin['q'],		UFC_xsize, UFC_ysize},	-- 8 without right upper line
	[20]	= {latin['s'],		UFC_xsize, UFC_ysize},	-- 8 without right lower line
	[21]	= {latin['t'],		UFC_xsize, UFC_ysize},	-- 9 without right lines
	[22]	= {latin['v'],		UFC_xsize, UFC_ysize},	-- 9 without right upper line
	[23]	= {latin['w'],		UFC_xsize, UFC_ysize},	-- 'R' for "error"
}

addCharsetMultitex(fontdescription["font_UFC"], ResourcesPath.."UFC/FONT_UFC_F18.dds",		0, charsUFC_set1)
addCharsetMultitex(fontdescription["font_UFC"], ResourcesPath.."UFC/FONT_UFC_F18_2.dds",	1, charsUFC_set2)

fontdescription["font_stroke_RWR"] = {
	class     = "ceSLineFont",
	symb_storage = "FONT_RWR_F18",
	thickness  = stroke_thickness,
	fuzziness  = stroke_fuzziness,
	draw_as_wire = dbg_drawStrokesAsWire,
	default    = {1, 1},
	chars	   = {
		 [1]   = {latin['A'], "A"},
		 [2]   = {latin['B'], "B"},
		 [3]   = {latin['C'], "C"},
		 [4]   = {latin['D'], "D"},
		 [5]   = {latin['E'], "E"},
		 [6]   = {latin['F'], "F"},
		 [7]   = {latin['G'], "G"},
		 [8]   = {latin['H'], "H"},
		 [9]   = {latin['I'], "I"},
		 [10]  = {latin['J'], "J"},
		 [11]  = {latin['K'], "K"},
		 [12]  = {latin['L'], "L"},
		 [13]  = {latin['M'], "M"},
		 [14]  = {latin['N'], "N"},
		 [15]  = {latin['O'], "O"},
		 [16]  = {latin['P'], "P"},
		 [17]  = {latin['Q'], "Q"},
		 [18]  = {latin['R'], "R"},
		 [19]  = {latin['S'], "S"},
		 [20]  = {latin['T'], "T"},
		 [21]  = {latin['U'], "U"},
		 [22]  = {latin['V'], "V"},
		 [23]  = {latin['W'], "W"},
		 [24]  = {latin['X'], "X"},
		 [25]  = {latin['Y'], "Y"},
		 [26]  = {latin['Z'], "Z"},
		 
		 [27]  = {symbol['0'], "0"},
		 [28]  = {symbol['1'], "1"},
		 [29]  = {symbol['2'], "2"},
		 [30]  = {symbol['3'], "3"},
		 [31]  = {symbol['4'], "4"},
		 [32]  = {symbol['5'], "5"},
		 [33]  = {symbol['6'], "6"},
		 [34]  = {symbol['7'], "7"},
		 [35]  = {symbol['8'], "8"},
		 [36]  = {symbol['9'], "9"},
	}
}

-- HMD
fontdescription["font_stroke_HMD"] = {
	class     = "ceSLineFont",
	symb_storage = "stroke_font_HMD",
	thickness  = stroke_thickness,
	fuzziness  = stroke_fuzziness,
	draw_as_wire = dbg_drawStrokesAsWire,
	default    = {12, 20}, -- F/A-18 MDG DIs (display increments)
	chars	   = {
		 [1]   = {latin['A'], "A"},
		 [2]   = {latin['B'], "B"},
		 [3]   = {latin['C'], "C"},
		 [4]   = {latin['D'], "D"},
		 [5]   = {latin['E'], "E"},
		 [6]   = {latin['F'], "F"},
		 [7]   = {latin['G'], "G"},
		 [8]   = {latin['H'], "H"},
		 [9]   = {latin['I'], "I"},
		 [10]  = {latin['J'], "J"},
		 [11]  = {latin['K'], "K"},
		 [12]  = {latin['L'], "L"},
		 [13]  = {latin['M'], "M"},
		 [14]  = {latin['N'], "N"},
		 [15]  = {latin['O'], "O"},
		 [16]  = {latin['P'], "P"},
		 [17]  = {latin['Q'], "Q"},
		 [18]  = {latin['R'], "R"},
		 [19]  = {latin['S'], "S"},
		 [20]  = {latin['T'], "T"},
		 [21]  = {latin['U'], "U"},
		 [22]  = {latin['V'], "V"},
		 [23]  = {latin['W'], "W"},
		 [24]  = {latin['X'], "X"},
		 [25]  = {latin['Y'], "Y"},
		 [26]  = {latin['Z'], "Z"},
		 
		 [27]  = {symbol['0'], "0"},
		 [28]  = {symbol['1'], "1"},
		 [29]  = {symbol['2'], "2"},
		 [30]  = {symbol['3'], "3"},
		 [31]  = {symbol['4'], "4"},
		 [32]  = {symbol['5'], "5"},
		 [33]  = {symbol['6'], "6"},
		 [34]  = {symbol['7'], "7"},
		 [35]  = {symbol['8'], "8"},
		 [36]  = {symbol['9'], "9"},
		 
		 [37]  = {symbol['-'], "symbol-minus"},
		 [38]  = {symbol['+'], "symbol-plus"},
		 [39]  = {symbol['\''], "symbol-apostrophe"},
		 [40]  = {symbol['('], "symbol-parenthesis-left"},
		 [41]  = {symbol[')'], "symbol-parenthesis-right"},
		 [42]  = {symbol['*'], "symbol-asterisk"},
		 [43]  = {symbol['%'], "symbol-percent"},
		 [44]  = {symbol[','], "symbol-comma"},
		 [45]  = {symbol['°'], "symbol-degree"},
		 [46]  = {symbol['.'], "symbol-period"},
		 [47]  = {symbol['/'], "symbol-slash"},
		 [48]  = {symbol['\\'], "symbol-backslash"},
		 [49]  = {symbol['\"'], "symbol-quote"},
		 [50]  = {symbol['?'], "symbol-question"},
		 [51]  = {symbol[':'], "symbol-colon"},
		 [52]  = {symbol['#'], "symbol-octothorpe"},
		 [53]  = {symbol['='], "symbol-equal"},
	}
}

dofile(LockOn_Options.common_script_path.."Fonts/font_TGP_Litening_AT.lua")
fontdescription["font_TGP"] = fontdescription_TGP_Litening_AT

dofile(LockOn_Options.common_script_path.."Fonts/font_AGM_65E.lua")
fontdescription["font_AGM_65E"] = fontdescription_AGM_65E

-- ATFLIR font
local TGP_xsize = 50
local TGP_ysize = 70
fontdescription["font_ATFLIR"] = {
	texture    = ResourcesPath.."MDG/font_TGP_ATFLIR.tga",
	size      = {7, 7},
	resolution = {512, 512},
	default    = {TGP_xsize, TGP_ysize},
	chars	    = {
		 [1]  = {32, TGP_xsize, TGP_ysize},				-- [space]
		 [2]  = {42, TGP_xsize, TGP_ysize},				-- *
		 [3]  = {45, TGP_xsize, TGP_ysize},				-- -
		 [4]  = {47, TGP_xsize, TGP_ysize},				-- /
		 [5]  = {48, TGP_xsize, TGP_ysize},				-- 0
		 [6]  = {49, TGP_xsize, TGP_ysize},				-- 1
		 [7]  = {50, TGP_xsize, TGP_ysize},				-- 2
		 [8]  = {51, TGP_xsize, TGP_ysize},				-- 3
		 [9]  = {52, TGP_xsize, TGP_ysize},				-- 4
		 [10]  = {53, TGP_xsize, TGP_ysize},			-- 5
		 [11]  = {54, TGP_xsize, TGP_ysize},			-- 6
		 [12]  = {55, TGP_xsize, TGP_ysize},			-- 7
		 [13]  = {56, TGP_xsize, TGP_ysize},			-- 8
		 [14]  = {57, TGP_xsize, TGP_ysize},			-- 9
		 [15]  = {58, TGP_xsize, TGP_ysize},			-- :
		 [16]  = {60, TGP_xsize, TGP_ysize},			-- <
		 [17]  = {62, TGP_xsize, TGP_ysize},			-- >
		 [18]  = {65, TGP_xsize, TGP_ysize},			-- A
		 [19]  = {66, TGP_xsize, TGP_ysize},			-- B
		 [20]  = {67, TGP_xsize, TGP_ysize},			-- C
		 [21]  = {68, TGP_xsize, TGP_ysize},			-- D
		 [22]  = {69, TGP_xsize, TGP_ysize},			-- E
		 [23]  = {70, TGP_xsize, TGP_ysize},			-- F
		 [24]  = {71, TGP_xsize, TGP_ysize},			-- G
		 [25]  = {72, TGP_xsize, TGP_ysize},			-- H
		 [26]  = {73, TGP_xsize, TGP_ysize},			-- I
		 [27]  = {74, TGP_xsize, TGP_ysize},			-- J
		 [28]  = {75, TGP_xsize, TGP_ysize},			-- K
		 [29]  = {76, TGP_xsize, TGP_ysize},			-- L
		 [30]  = {77, TGP_xsize, TGP_ysize},			-- M
		 [31]  = {78, TGP_xsize, TGP_ysize},			-- N
		 [32]  = {79, TGP_xsize, TGP_ysize},			-- O
		 [33]  = {80, TGP_xsize, TGP_ysize},			-- P
		 [34]  = {81, TGP_xsize, TGP_ysize},			-- Q
		 [35]  = {82, TGP_xsize, TGP_ysize},			-- R
		 [36]  = {83, TGP_xsize, TGP_ysize},			-- S
		 [37]  = {84, TGP_xsize, TGP_ysize},			-- T
		 [38]  = {85, TGP_xsize, TGP_ysize},			-- U
		 [39]  = {86, TGP_xsize, TGP_ysize},			-- V
		 [40]  = {87, TGP_xsize, TGP_ysize},			-- W
		 [41]  = {88, TGP_xsize, TGP_ysize},			-- X
		 [42]  = {89, TGP_xsize, TGP_ysize},			-- Y
		 [43]  = {90, TGP_xsize, TGP_ysize},			-- Z
		 [44]  = {37, TGP_xsize, TGP_ysize},			-- %
		 [45]  = {46, TGP_xsize, TGP_ysize},			-- .
		 [46]  = {symbol['\''], TGP_xsize, TGP_ysize},	-- '
		 [47]  = {symbol['\"'], TGP_xsize, TGP_ysize},	-- "
		 [48]  = {symbol['°'],  TGP_xsize, TGP_ysize}}	-- °
}

fontdescription["font_general_loc"] = fontdescription_cmn["font_general_loc"]