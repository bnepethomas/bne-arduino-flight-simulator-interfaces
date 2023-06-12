dofile(LockOn_Options.script_path.."TEWS/indicator/RWR_ALR67_definitions.lua")


---------------------------- RWR BIT ---------------------------------
-- BIT Page 1
AddBitPage1Fixed("RWR", STROKE_FNT_LONG, {0, 0.87}, {0, -0.87}, 0.87)
AddBitPage1ReceiverFail("RWR", STROKE_FNT_DFLT, 0.16,  0.65)

AlrBitPage1Fails[ALR_BIT1_FAIL_CI].pos = {0, 0.44}
AlrBitPage1Fails[ALR_BIT1_FAIL_SR].pos = {0, 0.22}
AlrBitPage1Fails[ALR_BIT1_FAIL_AN].pos = {0, 0}
AlrBitPage1Fails[ALR_BIT1_FAIL_IA].pos = {0, -0.22}
AlrBitPage1Fails[ALR_BIT1_FAIL_TO].pos = {0, -0.44}

AddBitPage1Fails(ALR_BIT1_FAIL_CI, "RWR", STROKE_FNT_DFLT)
AddBitPage1Fails(ALR_BIT1_FAIL_SR, "RWR", STROKE_FNT_DFLT)
AddBitPage1Fails(ALR_BIT1_FAIL_AN, "RWR", STROKE_FNT_DFLT)
AddBitPage1Fails(ALR_BIT1_FAIL_IA, "RWR", STROKE_FNT_DFLT)
AddBitPage1Fails(ALR_BIT1_FAIL_TO, "RWR", STROKE_FNT_DFLT)

-- BIT Page 2
AddBitPage2Fails("RWR", STROKE_FNT_SHORT, STROKE_FNT_DFLT, {0, 0.4}, {0, -0.4})

-- BIT Page 3, 4, 5, 6
AddBitFixedFormatText("RWR", STROKE_FNT_DFLT, {0, 0})

-- BIT Page 6
-- TODO: add symbols

-- BIT Page 7

-- BIT Page 8
