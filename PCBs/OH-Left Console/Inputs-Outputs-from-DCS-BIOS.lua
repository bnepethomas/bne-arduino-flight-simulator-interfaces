This from comes from DCS-BIOS
Inner file is FA-18C_hornet.lua 
Outer files is module-fa-18c-hornet-2019-11-05
It doesn't exactly jump out - https://github.com/dcs-bios/module-fa-18c-hornet/releases/tag/2019-11-05

Plan is to grab the blocks. compare against the NeoEngress picture
NeoEngress_FA-18C_Full_Layout_-_4220x5200

Start from front and work to the back 


-- 1. Fire Test Panel
-- Will just connect this to Ground Power Panel as they are neighbouring
defineRockerSwitch("FIRE_TEST_SW", 12, 3006, 3006, 3007, 3007, 331, "Fire Test Panel", "Fire and Bleed Air Test Switch, (RMB) TEST A/(LMB) TEST B")

-- 2. Ground Power Panel
defineRockerSwitch("EXT_PWR_SW", 3, 3005, 3005, 3004, 3004, 336, "Ground Power Panel", "External Power Switch, RESET/NORM/OFF")
defineRockerSwitch("GND_PWR_1_SW", 3, 3008, 3008, 3009, 3009, 332, "Ground Power Panel", "Ground Power Switch 1, A ON/AUTO/B ON")
defineRockerSwitch("GND_PWR_2_SW", 3, 3010, 3010, 3011, 3011, 333, "Ground Power Panel", "Ground Power Switch 2, A ON/AUTO/B ON")
defineRockerSwitch("GND_PWR_3_SW", 3, 3012, 3012, 3013, 3013, 334, "Ground Power Panel", "Ground Power Switch 3, A ON/AUTO/B ON")
defineRockerSwitch("GND_PWR_4_SW", 3, 3014, 3014, 3015, 3015, 335, "Ground Power Panel", "Ground Power Switch 4, A ON/AUTO/B ON")


-- 4. Exterior Lights Panel
definePotentiometer("POSITION_DIMMER", 8, 3001, 338, {0, 1}, "Exterior Lights Panel", "Position Lights Dimmer")
definePotentiometer("FORMATION_DIMMER", 8, 3002, 337, {0, 1}, "Exterior Lights Panel", "Formation Lights Dimmer")
define3PosTumb("STROBE_SW", 8, 3003, 339, "Exterior Lights Panel", "Strobe Lights Switch")
defineToggleSwitch("INT_WNG_TANK_SW", 6, 3001, 340, "Exterior Lights Panel", "Internal Wing Tank Fuel Control Switch, INHIBIT/NORM")


function defineEmergencyParkingBrake(msg, device_id, emergency_command, park_command, arg_number, category, description)
	local alloc = moduleBeingDefined.memoryMap:allocateInt{ maxValue = 1 }
	moduleBeingDefined.exportHooks[#moduleBeingDefined.exportHooks+1] = function(dev0)
		if dev0:get_argument_value(arg_number) < 0.5 then
			alloc:setValue(0)
		else
			alloc:setValue(1)
		end
	end
	
	document {
		identifier = msg,
		category = category,
		description = description,
		control_type = "emergency_parking_brake",
		inputs = {
			{ interface = "set_state", max_value = 1, description = "set the switch position -- 0 = emergency, 1 = parking" }
		},
		outputs = {
			{ ["type"] = "integer",
			  suffix = "",
			  address = alloc.address,
			  mask = alloc.mask,
			  shift_by = alloc.shiftBy,
			  max_value = 1,
			  description = "switch position -- 0 = emergency, 1 = parking"
			}
		}
	}
	moduleBeingDefined.inputProcessors[msg] = function(toState)
		local dev = GetDevice(device_id)
		local fromState = GetDevice(0):get_argument_value(arg_number)
		if fromState > 0.5 then fromState = 1 end
		if fromState == 0 and toState == "1" then
			dev:performClickableAction(park_command, 0) 
			dev:performClickableAction(park_command, 1) 
			dev:performClickableAction(park_command, 0) 
		elseif fromState == 1 and toState == "0" then
			dev:performClickableAction(emergency_command, 0) 
			dev:performClickableAction(emergency_command, 1) 
			dev:performClickableAction(emergency_command, 0) 
		end
	end
end

function defineToggleSwitchToggleOnly2(msg, device_id, command, arg_number, category, description)
	local alloc = moduleBeingDefined.memoryMap:allocateInt{ maxValue = 1 }
	moduleBeingDefined.exportHooks[#moduleBeingDefined.exportHooks+1] = function(dev0)
		if dev0:get_argument_value(arg_number) < 0.5 then
			alloc:setValue(0)
		else
			alloc:setValue(1)
		end
	end
	
	document {
		identifier = msg,
		category = category,
		description = description,
		control_type = "toggle_switch",
		inputs = {
			{ interface = "set_state", max_value = 1, description = "set the switch position -- 0 = off, 1 = on" }
		},
		outputs = {
			{ ["type"] = "integer",
			  suffix = "",
			  address = alloc.address,
			  mask = alloc.mask,
			  shift_by = alloc.shiftBy,
			  max_value = 1,
			  description = "switch position -- 0 = off, 1 = on"
			}
		}
	}
	
	moduleBeingDefined.inputProcessors[msg] = function(toState)
		local fromState = GetDevice(0):get_argument_value(arg_number)
		if fromState == 0 and toState == "1" then
			GetDevice(device_id):performClickableAction(command, 1)
		end
		if fromState == 1 and toState == "0" then
			GetDevice(device_id):performClickableAction(command, 1)
		end
	end
end

function defineMissionComputerSwitch(msg, device_id, mc1_off_command, mc2_off_command, arg_number, category, description)
	local alloc = moduleBeingDefined.memoryMap:allocateInt{ maxValue = 2 }
	moduleBeingDefined.exportHooks[#moduleBeingDefined.exportHooks+1] = function(dev0)
	    local val = dev0:get_argument_value(arg_number)
		if val == -1 then
			alloc:setValue(0)
		elseif val == 0 then
			alloc:setValue(1)
		elseif val == 1 then
			alloc:setValue(2)
		end
	end
	
	document {
		identifier = msg,
		category = category,
		description = description,
		control_type = "mission_computer_switch",
		inputs = {
			{ interface = "set_state", max_value = 2, description = "set the switch position -- 0 = emergency, 1 = parking" }
		},
		outputs = {
			{ ["type"] = "integer",
			  suffix = "",
			  address = alloc.address,
			  mask = alloc.mask,
			  shift_by = alloc.shiftBy,
			  max_value = 2,
			  description = "switch position -- 0 = emergency, 1 = parking"
			}
		}
	}
	moduleBeingDefined.inputProcessors[msg] = function(toState)
		local dev = GetDevice(device_id)
		dev:performClickableAction(mc1_off_command, 0) 
		dev:performClickableAction(mc2_off_command, 0) 
		if toState == "0" then
			dev:performClickableAction(mc2_off_command, -1) 
		elseif toState == "2" then
			dev:performClickableAction(mc1_off_command, 1) 
		end
	end
end

n


-- radio freqs: by Capt Zeen
defineFrequency("COMM1_FREQ", 38, "Comms frequency", "COMM1 FREQ")
defineFrequency("COMM2_FREQ", 39, "Comms frequency", "COMM2 FREQ")
defineFloatFromUFCChannel("COMM1_CHANNEL_NUMERIC", 1, "Comms frequency", "Comm 1 Channel as number")   
defineFloatFromUFCChannel("COMM2_CHANNEL_NUMERIC", 2, "Comms frequency", "Comm 2 Channel as number")   




-- INSTRUMENT PANEL

-- 1. Lock/Shoot Lights
defineIndicatorLight("LS_LOCK", 1, "Lock Shoot Lights", "LOCK")
defineIndicatorLight("LS_SHOOT", 2, "Lock Shoot Lights", "SHOOT")
defineIndicatorLight("LS_SHOOT_STROBE", 3, "Lock Shoot Lights", "SHOOT STROBE")

-- 2. Head Up Display

-- 3. Angle of Attack Indexer Lights
defineIndicatorLight("AOA_INDEXER_HIGH", 4, "Angle of Attack Indexer Lights", "AOA Indexer High")
defineIndicatorLight("AOA_INDEXER_NORMAL", 5, "Angle of Attack Indexer Lights", "AOA Indexer Normal")
defineIndicatorLight("AOA_INDEXER_LOW", 6, "Angle of Attack Indexer Lights", "AOA Indexer Low")

-- 4. Left Engine Fire Warning / Extinguisher Light
defineIndicatorLight("FIRE_LEFT_LT", 10, "Left Engine Fire Warning Extinguisher Light", "FIRE LEFT")
definePushButton("LEFT_FIRE_BTN", 12, 3010, 11, "Left Engine Fire Warning Extinguisher Light", "Left Engine/AMAD Fire Warning/Extinguisher Light")
defineToggleSwitch("LEFT_FIRE_BTN_COVER", 12, 3012, 12, "Left Engine Fire Warning Extinguisher Light", "Left Engine/AMAD Fire Warning Cover")

-- 5. Master Caution Light
defineIndicatorLight("MASTER_CAUTION_LT", 13, "Master Caution Light", "MASTER CAUTION")
defineToggleSwitch("MASTER_CAUTION_RESET_SW", 9, 3008, 14, "Master Caution Light", "MASTER CAUTION Reset Button - Press to reset")

-- 6. LH Advisory and Threat Warning Indicator Panel
defineIndicatorLight("LH_ADV_L_BLEED", 17, "LH Advisory Panel", "L BLEED")
defineIndicatorLight("LH_ADV_R_BLEED", 18, "LH Advisory Panel", "R BLEED")
defineIndicatorLight("LH_ADV_SPD_BRK", 19, "LH Advisory Panel", "SPD BRK")
defineIndicatorLight("LH_ADV_STBY", 20, "LH Advisory Panel", "STBY")
defineIndicatorLight("LH_ADV_L_BAR_RED", 21, "LH Advisory Panel", "L BAR RED")
defineIndicatorLight("LH_ADV_REC", 22, "LH Advisory Panel", "REC")
defineIndicatorLight("LH_ADV_L_BAR_GREEN", 23, "LH Advisory Panel", "L BAR GREEN")
defineIndicatorLight("LH_ADV_XMIT", 24, "LH Advisory Panel", "XMIT")
defineIndicatorLight("LH_ADV_ASPJ_OH", 25, "LH Advisory Panel", "ASPJ OH")
defineIndicatorLight("LH_ADV_GO", 15, "LH Advisory Panel", "GO")
defineIndicatorLight("LH_ADV_NO_GO", 16, "LH Advisory Panel", "NO GO")

-- 7. HUD Video Bit Panel
definePushButton("HUD_VIDEO_BIT", 0, 3107, 7, "HUD Video Bit Panel", "HUD Video BIT Initiate Pushbutton - Push to initiate BIT")

-- 8. RH Advisory and Threat Warning Indicator Panel
defineIndicatorLight("RH_ADV_RCDR_ON", 31, "RH Advisory Panel", "RCDR ON")
defineIndicatorLight("RH_ADV_DISP", 32, "RH Advisory Panel", "DISP")
defineIndicatorLight("RH_ADV_SAM", 38, "RH Advisory Panel", "SAM")
defineIndicatorLight("RH_ADV_AI", 39, "RH Advisory Panel", "AI")
defineIndicatorLight("RH_ADV_AAA", 40, "RH Advisory Panel", "AAA")
defineIndicatorLight("RH_ADV_CW", 41, "RH Advisory Panel", "CW")
defineIndicatorLight("RH_ADV_SPARE_RH1", 33, "RH Advisory Panel", "SPARE RH1")
defineIndicatorLight("RH_ADV_SPARE_RH2", 34, "RH Advisory Panel", "SPARE RH2")
defineIndicatorLight("RH_ADV_SPARE_RH3", 35, "RH Advisory Panel", "SPARE RH3")
defineIndicatorLight("RH_ADV_SPARE_RH4", 36, "RH Advisory Panel", "SPARE RH4")
defineIndicatorLight("RH_ADV_SPARE_RH5", 37, "RH Advisory Panel", "SPARE RH5")

-- 9. APU Fire Warning / Extinguisher Light
defineIndicatorLight("FIRE_APU_LT", 29, "APU Fire Warning Extinguisher Light", "FIRE APU")
definePushButton("APU_FIRE_BTN", 12, 3009, 30, "APU Fire Warning Extinguisher Light", "APU Fire Warning/Extinguisher Light")

-- 10. Right Engine Fire Warning / Extinguisher Light
defineIndicatorLight("FIRE_RIGHT_LT", 26, " Right Engine Fire Warning Extinguisher Light", "FIRE RIGHT")
definePushButton("RIGHT_FIRE_BTN", 12, 3011, 27, " Right Engine Fire Warning Extinguisher Light", "Right Engine/AMAD Fire Warning/Extinguisher Light")
defineToggleSwitch("RIGHT_FIRE_BTN_COVER", 12, 3013, 28, " Right Engine Fire Warning Extinguisher Light", "Right Engine/AMAD Fire Warning Cover")

-- 11. Canopy Internal Jettison Handle
defineToggleSwitch("CANOPY_JETT_HANDLE_UNLOCK", 7, 3004, 43, "Canopy Internal Jettison Handle", "Canopy Jettison Handle Unlock Button - Press to unlock")
defineToggleSwitch("CANOPY_JETT_HANDLE_PULL", 7, 3003, 42, "Canopy Internal Jettison Handle", "Canopy Jettison Handle Unlock Button - Press to jettison")

-- 12. Master Arm Panel
defineIndicatorLight("MASTER_MODE_AA_LT", 47, "Master Arm Panel", "AA Light")
defineIndicatorLight("MASTER_MODE_AG_LT", 48, "Master Arm Panel", "AG Light")
definePushButton("MASTER_MODE_AA", 23, 3001, 458, "Master Arm Panel", "Master Mode Button, A/A")
definePushButton("MASTER_MODE_AG", 23, 3002, 459, "Master Arm Panel", "Master Mode Button, A/G")
defineToggleSwitch("MASTER_ARM_SW", 23, 3003, 49, "Master Arm Panel", "Master Arm Switch, ARM/SAFE")
defineIndicatorLight("MC_DISCH", 45, "Master Arm Panel", "DISCH Light")
defineIndicatorLight("MC_READY", 44, "Master Arm Panel", "READY Light")
definePushButton("FIRE_EXT_BTN", 12, 3008, 46, "Fire Systems", "Fire Extinguisher Pushbutton")

-- 13. Left DDI
defineTumb("LEFT_DDI_BRT_SELECT", 35, 3001, 51, 0.1, {0.0, 0.2}, nil, false, "Left DDI", "Brightness Selector Knob, OFF/NIGHT/DAY")
definePotentiometer("LEFT_DDI_BRT_CTL", 35, 3002, 52, {0, 1}, "Left DDI", "Brightness Control Knob")
definePotentiometer("LEFT_DDI_CONT_CTL", 35, 3003, 53, {0, 1}, "Left DDI", "Contrast Control Knob")
definePushButton("LEFT_DDI_PB_01", 35, 3011, 54, "Left DDI", "Pushbutton 1")
definePushButton("LEFT_DDI_PB_02", 35, 3012, 55, "Left DDI", "Pushbutton 2")
definePushButton("LEFT_DDI_PB_03", 35, 3013, 56, "Left DDI", "Pushbutton 3")
definePushButton("LEFT_DDI_PB_04", 35, 3014, 57, "Left DDI", "Pushbutton 4")
definePushButton("LEFT_DDI_PB_05", 35, 3015, 58, "Left DDI", "Pushbutton 5")
definePushButton("LEFT_DDI_PB_06", 35, 3016, 59, "Left DDI", "Pushbutton 6")
definePushButton("LEFT_DDI_PB_07", 35, 3017, 60, "Left DDI", "Pushbutton 7")
definePushButton("LEFT_DDI_PB_08", 35, 3018, 61, "Left DDI", "Pushbutton 8")
definePushButton("LEFT_DDI_PB_09", 35, 3019, 62, "Left DDI", "Pushbutton 9")
definePushButton("LEFT_DDI_PB_10", 35, 3020, 63, "Left DDI", "Pushbutton 10")
definePushButton("LEFT_DDI_PB_11", 35, 3021, 64, "Left DDI", "Pushbutton 11")
definePushButton("LEFT_DDI_PB_12", 35, 3022, 65, "Left DDI", "Pushbutton 12")
definePushButton("LEFT_DDI_PB_13", 35, 3023, 66, "Left DDI", "Pushbutton 13")
definePushButton("LEFT_DDI_PB_14", 35, 3024, 67, "Left DDI", "Pushbutton 14")
definePushButton("LEFT_DDI_PB_15", 35, 3025, 68, "Left DDI", "Pushbutton 15")
definePushButton("LEFT_DDI_PB_16", 35, 3026, 69, "Left DDI", "Pushbutton 16")
definePushButton("LEFT_DDI_PB_17", 35, 3027, 70, "Left DDI", "Pushbutton 17")
definePushButton("LEFT_DDI_PB_18", 35, 3028, 72, "Left DDI", "Pushbutton 18")
definePushButton("LEFT_DDI_PB_19", 35, 3029, 73, "Left DDI", "Pushbutton 19")
definePushButton("LEFT_DDI_PB_20", 35, 3030, 75, "Left DDI", "Pushbutton 20")

-- 14. Up Front Controller (UFC)
definePushButton("UFC_AP", 25, 3001, 128, "Up Front Controller (UFC)", "Function Selector Pushbutton, A/P")
definePushButton("UFC_IFF", 25, 3002, 129, "Up Front Controller (UFC)", "Function Selector Pushbutton, IFF")
definePushButton("UFC_TCN", 25, 3003, 130, "Up Front Controller (UFC)", "Function Selector Pushbutton, TCN")
definePushButton("UFC_ILS", 25, 3004, 131, "Up Front Controller (UFC)", "Function Selector Pushbutton, ILS")
definePushButton("UFC_DL", 25, 3005, 132, "Up Front Controller (UFC)", "Function Selector Pushbutton, D/L")
definePushButton("UFC_BCN", 25, 3006, 133, "Up Front Controller (UFC)", "Function Selector Pushbutton, BCN")
definePushButton("UFC_ONOFF", 25, 3007, 134, "Up Front Controller (UFC)", "Function Selector Pushbutton, ON/OFF")
definePushButton("UFC_COMM1_PULL", 25, 3008, 125, "Up Front Controller (UFC)", "COMM 1 Channel Selector Knob Pull")
definePushButton("UFC_COMM2_PULL", 25, 3009, 127, "Up Front Controller (UFC)", "COMM 2 Channel Selector Knob Pull")
definePushButton("UFC_OS1", 25, 3010, 100, "Up Front Controller (UFC)", "Option Select Pushbutton 1")
definePushButton("UFC_OS2", 25, 3011, 101, "Up Front Controller (UFC)", "Option Select Pushbutton 2")
definePushButton("UFC_OS3", 25, 3012, 102, "Up Front Controller (UFC)", "Option Select Pushbutton 3")
definePushButton("UFC_OS4", 25, 3013, 103, "Up Front Controller (UFC)", "Option Select Pushbutton 4")
definePushButton("UFC_OS5", 25, 3014, 106, "Up Front Controller (UFC)", "Option Select Pushbutton 5")
definePushButton("UFC_IP", 25, 3015, 99, "Up Front Controller (UFC)", "I/P Pushbutton")
define3PosTumb("UFC_ADF", 25, 3016, 107, "Up Front Controller (UFC)", "ADF Function Select Switch, 1/OFF/2")
definePushButton("UFC_EMCON", 25, 3017, 110, "Up Front Controller (UFC)", "Emission Control Pushbutton")
definePushButton("UFC_0", 25, 3018, 120, "Up Front Controller (UFC)", "UFC Keyboard Pushbutton, 0")
definePushButton("UFC_1", 25, 3019, 111, "Up Front Controller (UFC)", "UFC Keyboard Pushbutton, 1")
definePushButton("UFC_2", 25, 3020, 112, "Up Front Controller (UFC)", "UFC Keyboard Pushbutton, 2")
definePushButton("UFC_3", 25, 3021, 113, "Up Front Controller (UFC)", "UFC Keyboard Pushbutton, 3")
definePushButton("UFC_4", 25, 3022, 114, "Up Front Controller (UFC)", "UFC Keyboard Pushbutton, 4")
definePushButton("UFC_5", 25, 3023, 115, "Up Front Controller (UFC)", "UFC Keyboard Pushbutton, 5")
definePushButton("UFC_6", 25, 3024, 116, "Up Front Controller (UFC)", "UFC Keyboard Pushbutton, 6")
definePushButton("UFC_7", 25, 3025, 117, "Up Front Controller (UFC)", "UFC Keyboard Pushbutton, 7")
definePushButton("UFC_8", 25, 3026, 118, "Up Front Controller (UFC)", "UFC Keyboard Pushbutton, 8")
definePushButton("UFC_9", 25, 3027, 119, "Up Front Controller (UFC)", "UFC Keyboard Pushbutton, 9")
definePushButton("UFC_CLR", 25, 3028, 121, "Up Front Controller (UFC)", "Keyboard Pushbutton, CLR")
definePushButton("UFC_ENT", 25, 3029, 122, "Up Front Controller (UFC)", "Keyboard Pushbutton, ENT")
definePotentiometer("UFC_COMM1_VOL", 25, 3030, 108, {0, 1}, "Up Front Controller (UFC)", "COMM 1 Volume Control Knob")
definePotentiometer("UFC_COMM2_VOL", 25, 3031, 123, {0, 1}, "Up Front Controller (UFC)", "COMM 2 Volume Control Knob")
definePotentiometer("UFC_BRT", 25, 3032, 109, {0, 1}, "Up Front Controller (UFC)", "Brightness Control Knob")

defineRotary("UFC_COMM1_CHANNEL_SELECT", 25, 3033, 124, "Up Front Controller (UFC)", "COMM 1 Channel Select Knob")
defineRotary("UFC_COMM2_CHANNEL_SELECT", 25, 3034, 126, "Up Front Controller (UFC)", "COMM 2 Channel Select Knob")
BIOS.util.defineFixedStepInput("UFC_COMM1_CHANNEL_SELECT", 25, 3033, {-0.03, 0.03}, "Up Front Controller (UFC)", "COMM 1 Channel Select Knob")
BIOS.util.defineFixedStepInput("UFC_COMM2_CHANNEL_SELECT", 25, 3034, {-0.03, 0.03}, "Up Front Controller (UFC)", "COMM 2 Channel Select Knob")


-- 15. Right DDI
defineTumb("RIGHT_DDI_BRT_SELECT", 36, 3001, 76, 0.1, {0.0, 0.2}, nil, false, "Right DDI", "Brightness Selector Knob, OFF/NIGHT/DAY")
definePotentiometer("RIGHT_DDI_BRT_CTL", 36, 3002, 77, {0, 1}, "Right DDI", "Brightness Control Knob")
definePotentiometer("RIGHT_DDI_CONT_CTL", 36, 3003, 78, {0, 1}, "Right DDI", "Contrast Control Knob")
definePushButton("RIGHT_DDI_PB_01", 36, 3011, 79, "Right DDI", "Pushbutton 1")
definePushButton("RIGHT_DDI_PB_02", 36, 3012, 80, "Right DDI", "Pushbutton 2")
definePushButton("RIGHT_DDI_PB_03", 36, 3013, 81, "Right DDI", "Pushbutton 3")
definePushButton("RIGHT_DDI_PB_04", 36, 3014, 82, "Right DDI", "Pushbutton 4")
definePushButton("RIGHT_DDI_PB_05", 36, 3015, 83, "Right DDI", "Pushbutton 5")
definePushButton("RIGHT_DDI_PB_06", 36, 3016, 84, "Right DDI", "Pushbutton 6")
definePushButton("RIGHT_DDI_PB_07", 36, 3017, 85, "Right DDI", "Pushbutton 7")
definePushButton("RIGHT_DDI_PB_08", 36, 3018, 86, "Right DDI", "Pushbutton 8")
definePushButton("RIGHT_DDI_PB_09", 36, 3019, 87, "Right DDI", "Pushbutton 9")
definePushButton("RIGHT_DDI_PB_10", 36, 3020, 88, "Right DDI", "Pushbutton 10")
definePushButton("RIGHT_DDI_PB_11", 36, 3021, 89, "Right DDI", "Pushbutton 11")
definePushButton("RIGHT_DDI_PB_12", 36, 3022, 90, "Right DDI", "Pushbutton 12")
definePushButton("RIGHT_DDI_PB_13", 36, 3023, 91, "Right DDI", "Pushbutton 13")
definePushButton("RIGHT_DDI_PB_14", 36, 3024, 92, "Right DDI", "Pushbutton 14")
definePushButton("RIGHT_DDI_PB_15", 36, 3025, 93, "Right DDI", "Pushbutton 15")
definePushButton("RIGHT_DDI_PB_16", 36, 3026, 94, "Right DDI", "Pushbutton 16")
definePushButton("RIGHT_DDI_PB_17", 36, 3027, 95, "Right DDI", "Pushbutton 17")
definePushButton("RIGHT_DDI_PB_18", 36, 3028, 96, "Right DDI", "Pushbutton 18")
definePushButton("RIGHT_DDI_PB_19", 36, 3029, 97, "Right DDI", "Pushbutton 19")
definePushButton("RIGHT_DDI_PB_20", 36, 3030, 98, "Right DDI", "Pushbutton 20")

-- 16. Map Gain/Spin Recovery Panel
defineIndicatorLight("SPIN_LT", 137, "Map Gain/Spin Recovery Panel", "Spin Light")
defineToggleSwitch("SPIN_RECOVERY_COVER", 2, 3008, 139, "Map Gain/Spin Recovery Panel", "Spin Recovery Switch Cover, OPEN/CLOSE")
defineToggleSwitch("SPIN_RECOVERY_SW", 2, 3009, 138, "Map Gain/Spin Recovery Panel", "Spin Recovery Switch, RCVY/NORM")
definePotentiometer("HMD_OFF_BRT", 58, 3001, 136, {0, 0.75}, "Map Gain/Spin Recovery Panel", "HMD OFF/BRT Knob") -- From TODO, will change
defineTumb("IR_COOL_SW", 23, 3013, 135, 0.1, {0.0, 0.2}, nil, false, "Map Gain/Spin Recovery Panel", "IR Cooling Switch, ORIDE/NORM/OFF")

-- 17. Emergency Jettison Button
definePushButton("EMER_JETT_BTN", 23, 3004, 50, "Emergency Jettison Button", "Emergency Jettison Button")

-- 18. HUD Control Panel
defineTumb("HUD_SYM_REJ_SW", 34, 3001, 140, 0.1, {0.0, 0.2}, nil, false, "HUD Control Panel", "HUD Symbology Reject Switch, NORM/REJ 1/REJ 2")
definePotentiometer("HUD_SYM_BRT", 34, 3002, 141, {0, 1}, "HUD Control Panel", "HUD Symbology Brightness Control Knob")
defineToggleSwitch("HUD_SYM_BRT_SELECT", 34, 3003, 142, "HUD Control Panel", "HUD Symbology Brightness Selector Knob, DAY/NIGHT")
definePotentiometer("HUD_BLACK_LVL", 34, 3004, 143, {0, 1}, "HUD Control Panel", "Black Level Control Knob")
defineTumb("HUD_VIDEO_CONTROL_SW", 34, 3005, 144, 0.1, {0.0, 0.2}, nil, false, "HUD Control Panel", "HUD Video Control Switch, W/B /VID/OFF")
definePotentiometer("HUD_BALANCE", 34, 3006, 145, {0, 1}, "HUD Control Panel", "Balance Control Knob")
definePotentiometer("HUD_AOA_INDEXER", 34, 3007, 146, {0, 1}, "HUD Control Panel", "AOA Indexer Control Knob")
defineToggleSwitch("HUD_ALT_SW", 34, 3008, 147, "HUD Control Panel", "Altitude Switch, BARO/RDR")
define3PosTumb("HUD_ATT_SW", 34, 3009, 148, "HUD Control Panel", "Attitude Selector Switch, INS/AUTO/STBY")

-- 19. Standby Magnetic Compass

-- 20. Station Jettison Select
defineToggleSwitch("SJ_CTR", 23, 3005, 153, "Station Jettison Select", "Station Jettison Select Button, CENTER")
defineToggleSwitch("SJ_LI", 23, 3006, 155, "Station Jettison Select", "Station Jettison Select Button, LEFT IN")
defineToggleSwitch("SJ_LO", 23, 3007, 157, "Station Jettison Select", "Station Jettison Select Button, LEFT OUT")
defineToggleSwitch("SJ_RI", 23, 3008, 159, "Station Jettison Select", "Station Jettison Select Button, RIGHT IN")
defineToggleSwitch("SJ_RO", 23, 3009, 161, "Station Jettison Select", "Station Jettison Select Button, RIGHT OUT")
defineIndicatorLight("SJ_CTR_LT", 152, "Station Jettison Select", "CTR Light")
defineIndicatorLight("SJ_LI_LT", 154, "Station Jettison Select", "LI Light")
defineIndicatorLight("SJ_LO_LT", 156, "Station Jettison Select", "LO Light")
defineIndicatorLight("SJ_RI_LT", 158, "Station Jettison Select", "RI Light")
defineIndicatorLight("SJ_RO_LT", 160, "Station Jettison Select", "RO Light")

-- 21. Flaps, Landing Gear and Stores Indicator Panel
defineIndicatorLight("FLP_LG_NOSE_GEAR_LT", 166, "Flaps, Landing Gear, Stores Indicator Panel", "NOSE GEAR")
defineIndicatorLight("FLP_LG_LEFT_GEAR_LT", 165, "Flaps, Landing Gear, Stores Indicator Panel", "LEFT GEAR")
defineIndicatorLight("FLP_LG_RIGHT_GEAR_LT", 167, "Flaps, Landing Gear, Stores Indicator Panel", "RIGHT GEAR")
defineIndicatorLight("FLP_LG_HALF_FLAPS_LT", 163, "Flaps, Landing Gear, Stores Indicator Panel", "HALF FLAPS")
defineIndicatorLight("FLP_LG_FULL_FLAPS_LT", 164, "Flaps, Landing Gear, Stores Indicator Panel", "FULL FLAPS")
defineIndicatorLight("FLP_LG_FLAPS_LT", 162, "Flaps, Landing Gear, Stores Indicator Panel", "FLAPS")

-- 22. Integrated Fuel/Engine Indicator (IFEI)
definePushButton("IFEI_MODE_BTN", 33, 3001, 168, "Integrated Fuel/Engine Indicator (IFEI)", "Mode Button")
definePushButton("IFEI_QTY_BTN", 33, 3002, 169, "Integrated Fuel/Engine Indicator (IFEI)", "QTY Button")
definePushButton("IFEI_UP_BTN", 33, 3003, 170, "Integrated Fuel/Engine Indicator (IFEI)", "Up Arrow Button")
definePushButton("IFEI_DWN_BTN", 33, 3004, 171, "Integrated Fuel/Engine Indicator (IFEI)", "Down Arrow Button")
definePushButton("IFEI_ZONE_BTN", 33, 3005, 172, "Integrated Fuel/Engine Indicator (IFEI)", "ZONE Button")
definePushButton("IFEI_ET_BTN", 33, 3006, 173, "Integrated Fuel/Engine Indicator (IFEI)", "ET Button")


-- 23. HUD Video Record Panel
definePotentiometer("IFEI", 33, 3007, 174, {0, 1}, "HUD Video Record Panel", "Brightness Control Knob")
define3PosTumb("SELECT_HMD_LDDI_RDDI", 0, 3104, 175, "HUD Video Record Panel", "Selector Switch, HMD/LDDI/RDDI") -- From TODO, will change
define3PosTumb("SELECT_HUD_LDDI_RDDI", 0, 3105, 176, "HUD Video Record Panel", "Selector Switch, HUD/LDIR/RDDI") -- From TODO, will change
define3PosTumb("MODE_SELECTOR_SW", 0, 3106, 176, "HUD Video Record Panel", "Mode Selector Switch, MAN/OFF/AUTO") -- From TODO, will change

-- 24. AMPCD
definePotentiometer("AMPCD_BRT_CTL", 37, 3001, 203, {0, 1}, "AMPCD", "Brightness Control Knob")
define3PosTumb("AMPCD_NIGHT_DAY", 37, 3002, 177, "AMPCD", "Night/Day Brightness Selector")
define3PosTumb("AMPCD_SYM_SW", 37, 3004, 179, "AMPCD", "Symbology Control Switch")
define3PosTumb("AMPCD_CONT_SW", 37, 3006, 182, "AMPCD", "Contrast Control Switch")
define3PosTumb("AMPCD_GAIN_SW", 37, 3008, 180, "AMPCD", "Gain Control Switch")
definePushButton("AMPCD_PB_01", 37, 3011, 183, "AMPCD", "Pushbutton 1")
definePushButton("AMPCD_PB_02", 37, 3012, 184, "AMPCD", "Pushbutton 2")
definePushButton("AMPCD_PB_03", 37, 3013, 185, "AMPCD", "Pushbutton 3")
definePushButton("AMPCD_PB_04", 37, 3014, 186, "AMPCD", "Pushbutton 4")
definePushButton("AMPCD_PB_05", 37, 3015, 187, "AMPCD", "Pushbutton 5")
definePushButton("AMPCD_PB_06", 37, 3016, 188, "AMPCD", "Pushbutton 6")
definePushButton("AMPCD_PB_07", 37, 3017, 189, "AMPCD", "Pushbutton 7")
definePushButton("AMPCD_PB_08", 37, 3018, 190, "AMPCD", "Pushbutton 8")
definePushButton("AMPCD_PB_09", 37, 3019, 191, "AMPCD", "Pushbutton 9")
definePushButton("AMPCD_PB_10", 37, 3020, 192, "AMPCD", "Pushbutton 10")
definePushButton("AMPCD_PB_11", 37, 3021, 193, "AMPCD", "Pushbutton 11")
definePushButton("AMPCD_PB_12", 37, 3022, 194, "AMPCD", "Pushbutton 12")
definePushButton("AMPCD_PB_13", 37, 3023, 195, "AMPCD", "Pushbutton 13")
definePushButton("AMPCD_PB_14", 37, 3024, 196, "AMPCD", "Pushbutton 14")
definePushButton("AMPCD_PB_15", 37, 3025, 197, "AMPCD", "Pushbutton 15")
definePushButton("AMPCD_PB_16", 37, 3026, 198, "AMPCD", "Pushbutton 16")
definePushButton("AMPCD_PB_17", 37, 3027, 199, "AMPCD", "Pushbutton 17")
definePushButton("AMPCD_PB_18", 37, 3028, 200, "AMPCD", "Pushbutton 18")
definePushButton("AMPCD_PB_19", 37, 3029, 201, "AMPCD", "Pushbutton 19")
definePushButton("AMPCD_PB_20", 37, 3030, 202, "AMPCD", "Pushbutton 20")

-- 25. Standby Attitude Reference Indicator
definePushButton("SAI_TEST_BTN", 32, 3001, 215, "Standby Attitude Reference Indicator", "Test Button")
definePushButton("SAI_CAGE", 32, 3002, 213, "Standby Attitude Reference Indicator", "Pull to uncage")
defineRotary("SAI_SET", 32, 3003, 214, "Standby Attitude Reference Indicator", "Adjust Attitude")
defineFloat("SAI_PITCH", 205, {0, 1}, "Standby Attitude Reference Indicator", "Pitch")
defineFloat("SAI_BANK", 206, {0, 1}, "Standby Attitude Reference Indicator", "Bank")
defineFloat("SAI_ATT_WARNING_FLAG", 209, {0, 1}, "Standby Attitude Reference Indicator", "Attitude Warning Flag")
defineFloat("SAI_MAN_PITCH_ADJ", 210, {0, 1}, "Standby Attitude Reference Indicator", "Manual Pitch Adjustment")
defineFloat("SAI_SLIP_BALL", 207, {0, 1}, "Standby Attitude Reference Indicator", "Slip Ball")
defineFloat("SAI_RATE_OF_TURN", 208, {0, 1}, "Standby Attitude Reference Indicator", "Rate Of Turn")

-- 26. Azimuth Indicator

-- 27. Standby Airspeed Indicator
defineFloat("STBY_ASI_AIRSPEED", 217, {0, 1}, "Standby Airspeed Indicator", "Airspeed")

-- 28. Standby Altimeter
defineRotary("STBY_PRESS_ALT", 26, 3001, 224, "Standby Altimeter", "Pressure Setting Knob")
defineFloat("STBY_ALT_100_FT_PTR", 218, {0, 1}, "Standby Altimeter", "100 ft pointer")
defineFloat("STBY_ALT_10000_FT_CNT", 220, {0, 1}, "Standby Altimeter", "10000 ft count")
defineFloatWithValueConversion("STBY_ALT_1000_FT_CNT", 219, {0, 1}, {-1.0, 0.0, 0.0, 10.0}, {0.9, 1.0, 0.0, 1.0}, "Standby Altimeter", "1000 ft count")
defineFloat("STBY_PRESS_SET_0", 221, {0, 1}, "Standby Altimeter", "Pressure Setting 1")
defineFloat("STBY_PRESS_SET_1", 222, {0, 1}, "Standby Altimeter", "Pressure Setting 2")
defineFloatWithValueConversion("STBY_PRESS_SET_2", 223, {0, 1}, {26, 31}, {0.0, 1.0}, "Standby Altimeter", "Pressure Setting 3")

-- 29. Standby Rate of Climb Indicator
defineFloatWithValueConversion("VSI", 225, {0, 1}, {-6000.0, -4000.0, -3000.0, -2000.0, -1000.0, -500.0, 0.0, 500.0, 1000.0, 2000.0, 3000.0, 4000.0, 6000.0}, { -1.0,   -0.83,   -0.73,  -0.605,   -0.40,  -0.22, 0.0,  0.22,   0.40,  0.605,   0.73,   0.83,    1.0}, "Standby Rate of Climb Indicator", "Vertical Speed")

-- 30. Environment Control Louver
definePotentiometer("LEFT_LOUVER", 11, 3010, 505, {0, 1}, "Environment Control Louver", "Left Louver")
definePotentiometer("RIGHT_LOUVER", 11, 3011, 506, {0, 1}, "Environment Control Louver", "Right Louver") -- this is the same as left in clickabledata, seems incorrect

-- 31. Landing Gear Handle and Warning Tone Silence
defineIndicatorLight("LANDING_GEAR_HANDLE_LT", 227, "Landing Gear Handle and Warning Tone Silence", "Landing Gear Handle Light")
defineToggleSwitch("GEAR_LEVER", 5, 3001, 226, "Landing Gear Handle and Warning Tone Silence", "Gear Lever")
defineToggleSwitch("EMERGENCY_GEAR_ROTATE", 5, 3002, 228, "Landing Gear Handle and Warning Tone Silence", "Emergency Gear Rotate")
definePushButton("GEAR_DOWNLOCK_OVERRIDE_BTN", 5, 3003, 229, "Landing Gear Handle and Warning Tone Silence", "Landing Gear Override")
definePushButton("GEAR_SILENCE_BTN", 40, 3018, 230, "Landing Gear Handle and Warning Tone Silence", "Warning Tone Silence Button - Push to silence")

-- 32. Select Jettison Button
definePushButton("SEL_JETT_BTN", 23, 3010, 235, "Select Jettison Button", "Selective Jettison Pushbutton")
defineTumb("SEL_JETT_KNOB", 23, 3011, 236, 0.1, {-0.1, 0.3}, nil, false, "Select Jettison Button", "Selective Jettison Knob, L FUS MSL/SAFE/R FUS MSL/ RACK/LCHR /STORES")
defineToggleSwitch("ANTI_SKID_SW", 5, 3004, 238, "Select Jettison Button", "Anti Skid")
defineToggleSwitchToggleOnly2("LAUNCH_BAR_SW", 5, 3008, 233, "Select Jettison Button", "Launch Bar")
defineToggleSwitchToggleOnly2("HOOK_BYPASS_SW", 9, 3009, 239, "Select Jettison Button", "HOOK BYPASS Switch, FIELD/CARRIER")
define3PosTumb("FLAP_SW", 2, 3007, 234, "Select Jettison Button", "FLAP Switch, AUTO/HALF/FULL")
defineToggleSwitch("LDG_TAXI_SW", 8, 3004, 237, "Select Jettison Button", "LDG/TAXI LIGHT Switch")
defineFloat("HYD_IND_BRAKE", 242, {0, 1}, "Select Jettison Button", "HYD Indicator Brake")

-- 33. Brake Accumulator Pressure Gage

-- 34. Emergency and Parking Brake Handle
defineToggleSwitch("EMERGENCY_PARKING_BRAKE_PULL", 5, 3005, 240, "Emergency and Parking Brake Handle", "Emergency/Parking Brake Pull")
defineEmergencyParkingBrake("EMERGENCY_PARKING_BRAKE_ROTATE", 5, 3007, 3006, 241, "Emergency and Parking Brake Handle", "Emergency/Parking Brake Rotate")

-- 35. Dispenser/EMC Panel
defineTumb("CMSD_DISPENSE_SW", 54, 3001, 517, 0.1, {0.0, 0.2}, nil, false, "Dispenser/EMC Panel", "DISPENSER Switch, BYPASS/ON/OFF")
definePushButton("CMSD_JET_SEL_BTN", 54, 3003, 515, "Dispenser/EMC Panel", "ECM JETT JETT SEL Button - Push to jettison")
defineTumb("ECM_MODE_SW", 0, 3116, 248, 0.1, {0.0, 0.4}, nil, false, "Dispenser/EMC Panel", "ECM Mode Switch, XMIT/REC/BIT/STBY/OFF")
defineToggleSwitch("AUX_REL_SW", 23, 3012, 258, "Dispenser/EMC Panel", "Auxiliary Release Switch, ENABLE/NORM")

-- 36. RWR Control Indicator
definePushButton("RWR_POWER_BTN", 53, 3001, 277, "RWR Control Indicator", "ALR-67 POWER Pushbutton")
definePushButton("RWR_DISPLAY_BTN", 53, 3002, 275, "RWR Control Indicator", "ALR-67 DISPLAY Pushbutton")
definePushButton("RWR_SPECIAL_BTN", 53, 3003, 272, "RWR Control Indicator", "ALR-67 SPECIAL Pushbutton")
definePushButton("RWR_OFFSET_BTN", 53, 3004, 269, "RWR Control Indicator", "ALR-67 OFFSET Pushbutton")
definePushButton("RWR_BIT_BTN", 53, 3005, 266, "RWR Control Indicator", "ALR-67 BIT Pushbutton")
definePotentiometer("RWR_DMR_CTRL", 53, 3006, 263, {0, 1}, "RWR Control Indicator", "ALR-67 DMR Control Knob")
defineTumb("RWR_DIS_TYPE_SW", 53, 3007, 261, 0.1, {0.0, 0.4}, nil, false, "RWR Control Indicator", "ALR-67 DIS TYPE Switch, N/I/A/U/F")
definePotentiometer("RWR_RWR_INTESITY", 53, 3008, 216, {0, 1}, "RWR Control Indicator", "RWR Intensity Knob")
defineIndicatorLight("RWR_LOWER_LT", 276, "RWR Control Indicator", "ALR-67 LOWER Light (green)")
defineIndicatorLight("RWR_LIMIT_LT", 273, "RWR Control Indicator", "ALR-67 LIMIT Light (green)")
defineIndicatorLight("RWR_DISPLAY_LT", 274, "RWR Control Indicator", "ALR-67 DISPLAY Light (green)")
defineIndicatorLight("RWR_SPECIAL_EN_LT", 270, "RWR Control Indicator", "ALR-67 SPECIAL ENABLE Light (green)")
defineIndicatorLight("RWR_SPECIAL_LT", 271, "RWR Control Indicator", "ALR-67 SPECIAL Light (green)")
defineIndicatorLight("RWR_ENABLE_LT", 267, "RWR Control Indicator", "ALR-67 ENABLE Light (green)")
defineIndicatorLight("RWR_OFFSET_LT", 268, "RWR Control Indicator", "ALR-67 OFFSET Light (green)")
defineIndicatorLight("RWR_FAIL_LT", 264, "RWR Control Indicator", "ALR-67 FAIL Light (red)")
defineIndicatorLight("RWR_BIT_LT", 265, "RWR Control Indicator", "ALR-67 BIT Light (green)")

-- 37. Clock
defineFloat("CLOCK_HOURS", 278, {0, 1}, "Clock", "Hours")
defineFloat("CLOCK_MINUTES", 289, {0, 1}, "Clock", "Minutes")
defineFloat("CLOCK_ELAPSED_MINUTES", 281, {0, 1}, "Clock", "Elapsed Minutes")
defineFloat("CLOCK_ELAPSED_SECONDS", 280, {0, 1}, "Clock", "Elapsed Seconds")

-- 38. Rudder Pedal Adjust Level
defineToggleSwitch("RUDDER_PEDAL_ADJUST", 7, 3012, 260, "Rudder Pedal Adjust Level", "Rudder Pedal Adjust Lever")

-- 39. Cockpit Altimeter
defineFloat("PRESSURE_ALT", 285, {0, 1}, "Cockpit Altimeter", "Pressure Altitude")

-- 40. Static Source Select

-- 41. Radar Altimeter
definePushButton("RADALT_TEST_SW", 30, 3001, 292, "Radar Altimeter", "Push to Test Switch")
defineRotary("RADALT_HEIGHT", 30, 3002, 291, "Radar Altimeter", "Set low altitude pointer")
defineIndicatorLight("LOW_ALT_WARN_LT", 290, "Radar Altimeter", "Low Alt Warning")
defineFloat("RADALT_MIN_HEIGHT_PTR", 287, {0, 1}, "Radar Altimeter", "Min Height Pointer")
defineFloat("RADALT_ALT_PTR", 286, {0, 1}, "Radar Altimeter", "Altitude Pointer")
defineFloat("RADALT_OFF_FLAG", 288, {0, 1}, "Radar Altimeter", "OFF Flag")
defineIndicatorLight("RADALT_GREEN_LAMP", 289, "Radar Altimeter", "Green Lamp")

-- 42. Aircraft Bureau Number

-- 43. Arresting Hook Handle and Light
defineToggleSwitch("HOOK_LEVER", 5, 3009, 293, "Arresting Hook Handle and Light", "Hook Lever")
defineIndicatorLight("ARRESTING_HOOK_LT", 294, "Arresting Hook Handle and Light", "Hook Light")

-- 44. Wing Fold Switch
defineToggleSwitch("WING_FOLD_PULL", 2, 3010, 296, "Wing Fold Switch", "Wing Fold Control Handle Pull")
define3PosTumb("WING_FOLD_ROTATE", 2, 3011, 295, "Wing Fold Switch", "Wing Fold Control Handle, FOLD/HOLD/UNFOLD")

-- 45. Flight Computer Cool Switch
defineToggleSwitch("AV_COOL_SW", 11, 3008, 297, "Flight Computer Cool Switch", "AV COOL Switch, NORM/EMERG")

-- 46. Caution Lights Panel
defineIndicatorLight("CLIP_CK_SEAT_LT", 298, "Caution Light Panel", "CK SEAT")
defineIndicatorLight("CLIP_APU_ACC_LT", 299, "Caution Light Panel", "APU ACC")
defineIndicatorLight("CLIP_BATT_SW_LT", 300, "Caution Light Panel", "BATT SW")
defineIndicatorLight("CLIP_FCS_HOT_LT", 301, "Caution Light Panel", "FCS HOT")
defineIndicatorLight("CLIP_GEN_TIE_LT", 302, "Caution Light Panel", "GEN TIE")
defineIndicatorLight("CLIP_SPARE_CTN1_LT", 303, "Caution Light Panel", "SPARE CTN1")
defineIndicatorLight("CLIP_FUEL_LO_LT", 304, "Caution Light Panel", "FUEL LO`")
defineIndicatorLight("CLIP_FCES_LT", 305, "Caution Light Panel", "FCES")
defineIndicatorLight("CLIP_SPARE_CTN2_LT", 306, "Caution Light Panel", "SPARE CTN2")
defineIndicatorLight("CLIP_L_GEN_LT", 307, "Caution Light Panel", "L GEN")
defineIndicatorLight("CLIP_R_GEN_LT", 308, "Caution Light Panel", "R GEN")
defineIndicatorLight("CLIP_SPARE_CTN3_LT", 309, "Caution Light Panel", "SPARE CTN3")

-- 47. HYD 1 and HYD Pressure Indicator
defineFloat("HYD_IND_LEFT", 310, {0, 1}, "HYD 1 and HYD Pressure Indicator", "HYD Indicator Left")
defineFloat("HYD_IND_RIGHT", 311, {0, 1}, "HYD 1 and HYD Pressure Indicator", "HYD Indicator Right")

-- 48. Heading and Course Set Switches
defineRockerSwitch("LEFT_DDI_HDG_SW", 35, 3004, 3004, 3005, 3005, 312, "Heading and Course Set Switches", "Heading Set Switch")
defineRockerSwitch("LEFT_DDI_CRS_SW", 35, 3006, 3006, 3007, 3007, 313, "Heading and Course Set Switches", "Course Set Switch")

-- LEFT CONSOLE

-- 1. Fire Test Panel
defineRockerSwitch("FIRE_TEST_SW", 12, 3006, 3006, 3007, 3007, 331, "Fire Test Panel", "Fire and Bleed Air Test Switch, (RMB) TEST A/(LMB) TEST B")

-- 2. Ground Power Panel
defineRockerSwitch("EXT_PWR_SW", 3, 3005, 3005, 3004, 3004, 336, "Ground Power Panel", "External Power Switch, RESET/NORM/OFF")
defineRockerSwitch("GND_PWR_1_SW", 3, 3008, 3008, 3009, 3009, 332, "Ground Power Panel", "Ground Power Switch 1, A ON/AUTO/B ON")
defineRockerSwitch("GND_PWR_2_SW", 3, 3010, 3010, 3011, 3011, 333, "Ground Power Panel", "Ground Power Switch 2, A ON/AUTO/B ON")
defineRockerSwitch("GND_PWR_3_SW", 3, 3012, 3012, 3013, 3013, 334, "Ground Power Panel", "Ground Power Switch 3, A ON/AUTO/B ON")
defineRockerSwitch("GND_PWR_4_SW", 3, 3014, 3014, 3015, 3015, 335, "Ground Power Panel", "Ground Power Switch 4, A ON/AUTO/B ON")

-- 3. Throttle Quadrant
definePotentiometer("THROTTLE_FRICTION", 2, 3012, 504, {0, 1}, "Throttle Quadrant", "Throttles Friction Adjusting Lever")

-- 4. Exterior Lights Panel
definePotentiometer("POSITION_DIMMER", 8, 3001, 338, {0, 1}, "Exterior Lights Panel", "Position Lights Dimmer")
definePotentiometer("FORMATION_DIMMER", 8, 3002, 337, {0, 1}, "Exterior Lights Panel", "Formation Lights Dimmer")
define3PosTumb("STROBE_SW", 8, 3003, 339, "Exterior Lights Panel", "Strobe Lights Switch")
defineToggleSwitch("INT_WNG_TANK_SW", 6, 3001, 340, "Exterior Lights Panel", "Internal Wing Tank Fuel Control Switch, INHIBIT/NORM")

-- 5. Fuel Panel
define3PosTumb("PROBE_SW", 6, 3002, 341, "Fuel Panel", "Probe Control Switch, EXTEND/RETRACT/EMERG EXTD")
defineToggleSwitchToggleOnly2("FUEL_DUMP_SW", 6, 3003, 344, "Fuel Panel", "Fuel Dump Switch, ON/OFF")
define3PosTumb("EXT_CNT_TANK_SW", 6, 3004, 343, "Fuel Panel", "External Centerline Tank Fuel Control Switch, STOP/NORM/ORIDE")
define3PosTumb("EXT_WNG_TANK_SW", 6, 3005, 342, "Fuel Panel", "External Wing Tanks Fuel Control Switch, STOP/NORM/ORIDE")

-- 6. Flight Control System Panel
definePotentiometer("RUD_TRIM", 2, 3001, 345, {-1, 1}, "Flight Control System Panel", "RUD TRIM Control")
defineToggleSwitch("TO_TRIM_BTN", 2, 3002, 346, "Flight Control System Panel", "T/O TRIM Button")
defineToggleSwitch("FCS_RESET_BTN", 2, 3003, 349, "Flight Control System Panel", "FCS RESET Button")
defineToggleSwitch("GAIN_SWITCH_COVER", 2, 3005, 348, "Flight Control System Panel", "GAIN Switch Cover")
defineToggleSwitch("GAIN_SWITCH", 2, 3006, 347, "Flight Control System Panel", "GAIN Switch")

-- 7. Communication Panel
definePotentiometer("COM_VOX", 40, 3002, 357, {0, 1}, "Communication Panel", "VOX Volume Control Knob")
definePotentiometer("COM_ICS", 40, 3003, 358, {0, 1}, "Communication Panel", "ICS Volume Control Knob")
definePotentiometer("COM_RWR", 40, 3004, 359, {0, 1}, "Communication Panel", "RWR Volume Control Knob")
definePotentiometer("COM_WPN", 40, 3005, 360, {0, 1}, "Communication Panel", "WPN Volume Control Knob")
definePotentiometer("COM_MIDS_A", 40, 3006, 362, {0, 1}, "Communication Panel", "MIDS A Volume Control Knob")
definePotentiometer("COM_MIDS_B", 40, 3007, 361, {0, 1}, "Communication Panel", "MIDS B Volume Control Knob")
definePotentiometer("COM_TACAN", 40, 3008, 363, {0, 1}, "Communication Panel", "TACAN Volume Control Knob")
definePotentiometer("COM_AUX", 40, 3009, 364, {0, 1}, "Communication Panel", "AUX Volume Control Knob")
define3PosTumb("COM_COMM_RELAY_SW", 40, 3010, 350, "Communication Panel", "Comm Relay Switch, CIPHER/OFF/PLAIN")
define3PosTumb("COM_COMM_G_XMT_SW", 40, 3011, 351, "Communication Panel", "COMM G XMT Switch, COMM 1/OFF/COMM 2")
defineToggleSwitch("COM_IFF_MASTER_SW", 40, 3012, 356, "Communication Panel", "IFF Master Switch, EMER/NORM")
define3PosTumb("COM_IFF_MODE4_SW", 40, 3013, 355, "Communication Panel", "IFF Mode 4 Switch, DIS/AUD /DIS/OFF")
defineRockerSwitch("COM_CRYPTO_SW", 40, 3015, 3015, 3014, 3014, 354, "Communication Panel", "CRYPTO Switch, HOLD/NORM/ZERO")
defineToggleSwitch("COM_ILS_UFC_MAN_SW", 40, 3016, 353, "Communication Panel", "ILS UFC/MAN Switch, UFC/MAN")
defineTumb("COM_ILS_CHANNEL_SW", 40, 3017, 352, 0.05, {0.0, 0.95}, nil, false, "Communication Panel", "ILS Channel Selector Switch")

-- 8. LOX Indicator
defineToggleSwitch("OBOGS_SW", 10, 3001, 365, "LOX Indicator", "OBOGS Control Switch, ON/OFF")
definePotentiometer("OXY_FLOW", 10, 3002, 366, {0, 1}, "LOX Indicator", "OXY Flow Knob")

-- 9. Anti-G Valve

-- 10. Pilot Services Panel

-- 11. Communication Connect

-- 12. Mission Computer and Hydraulic Isolate Panel
defineMissionComputerSwitch("MC_SW", 3, 3025, 3026, 368, "Mission Computer and Hydraulic Isolate Panel", "MC Switch, 1 OFF/NORM/2 OFF")
defineToggleSwitch("HYD_ISOLATE_OVERRIDE_SW", 4, 3001, 369, "Mission Computer and Hydraulic Isolate Panel", "Hydraulic Isolate Override Switch, NORM/ORIDE")

-- 13. Antenna Select Panel
define3PosTumb("COMM1_ANT_SELECT_SW", 50, 3001, 373, "Antenna Select Panel", "COMM 1 Antenna Selector Switch, UPPER/AUTO/LOWER")
define3PosTumb("IFF_ANT_SELECT_SW", 50, 3002, 374, "Antenna Select Panel", "IFF Antenna Selector Switch, UPPER/BOTH/LOWER")

-- 14. Auxiliary Power Unit Panel
defineToggleSwitchToggleOnly2("APU_CONTROL_SW", 12, 3001, 375, "Auxiliary Power Unit Panel", "APU Control Switch, ON/OFF")
defineRockerSwitch("ENGINE_CRANK_SW", 12, 3003, 3003, 3002, 3002, 377, "Auxiliary Power Unit Panel", "Engine Crank Switch, LEFT/OFF/RIGHT")
defineIndicatorLight("APU_READY_LT", 376, "Auxiliary Power Unit Panel", "APU Ready Light")

-- 15. Generator Tie Control Switch
defineToggleSwitch("GEN_TIE_COVER", 3, 3007, 379, "Generator Tie Control Switch", "Generator TIE Control Switch Cover, OPEN/CLOSE")
defineToggleSwitch("GEN_TIE_SW", 3, 3006, 378, "Generator Tie Control Switch", "Generator TIE Control Switch, NORM/RESET")

-- 16. ECM Dispenser Button
definePushButton("CMSD_DISPENSE_BTN", 54, 3002, 380, "ECM Dispenser Button", "Dispense Button - Push to dispense flares and chaff")

-- 17. Ground Power Decal

-- 18. Left Essential Circuit Breakers
definePushButton("CB_FCS_CHAN1", 3, 3017, 381, "Left Essential Circuit Breakers", "CB FCS CHAN 1, ON/OFF")
definePushButton("CB_FCS_CHAN2", 3, 3018, 382, "Left Essential Circuit Breakers", "CB FCS CHAN 2, ON/OFF")
definePushButton("CB_SPD_BRK", 3, 3019, 383, "Left Essential Circuit Breakers", "CB SPD BRK, ON/OFF")
definePushButton("CB_LAUNCH_BAR", 3, 3020, 384, "Left Essential Circuit Breakers", "CB LAUNCH BAR, ON/OFF")

-- 19. Canopy Manual Handle and Drive

-- 20. HVI Stowage/Nuclear Weapon Switch

-- 21. Video Sensor Head

-- 22. OBOGS Monitor

-- RIGHT CONSOLE

-- 1. Electrical Power Panel
define3PosTumb("BATTERY_SW", 3, 3001, 404, "Electrical Power Panel", "Battery Switch, ON/OFF/ORIDE")
defineToggleSwitch("L_GEN_SW", 3, 3002, 402, "Electrical Power Panel", "Left Generator Control Switch, NORM/OFF")
defineToggleSwitch("R_GEN_SW", 3, 3003, 403, "Electrical Power Panel", "Right Generator Control Switch, NORM/OFF")
defineFloat("VOLT_U", 400, {0, 1}, "Electrical Power Panel", "Battery U Volts")
defineFloat("VOLT_E", 401, {0, 1}, "Electrical Power Panel", "Battery E Volts")

-- 2. Environment Control System Panel
defineTumb("BLEED_AIR_KNOB", 11, 3001, 411, 0.1, {0.0, 0.3}, nil, false, "Environment Control System Panel", "Bleed Air Knob, R OFF/NORM/L OFF/OFF")
defineToggleSwitch("BLEED_AIR_PULL", 11, 3002, 412, "Environment Control System Panel", "Bleed Air Knob, AUG PULL")
define3PosTumb("ECS_MODE_SW", 11, 3003, 405, "Environment Control System Panel", "ECS Mode Switch, AUTO/MAN/ OFF/RAM")
define3PosTumb("CABIN_PRESS_SW", 11, 3004, 408, "Environment Control System Panel", "Cabin Pressure Switch, NORM/DUMP/ RAM/DUMP")
definePotentiometer("CABIN_TEMP", 11, 3006, 407, {0, 1}, "Environment Control System Panel", "Cabin Temperature Knob")
definePotentiometer("SUIT_TEMP", 11, 3007, 406, {0, 1}, "Environment Control System Panel", "Suit Temperature Knob")
define3PosTumb("ENG_ANTIICE_SW", 12, 3014, 410, "Environment Control System Panel", "Engine Anti-Ice Switch, ON/OFF/TEST")
defineToggleSwitchToggleOnly2("PITOT_HEAT_SW", 3, 3016, 409, "Environment Control System Panel", "Pitot Heater Switch, ON/AUTO")

-- 3. Interior Lights Panel
definePotentiometer("CONSOLES_DIMMER", 9, 3001, 413, {0, 1}, "Interior Lights Panel", "CONSOLES Lights Dimmer")
definePotentiometer("INST_PNL_DIMMER", 9, 3002, 414, {0, 1}, "Interior Lights Panel", "INST PNL Dimmer")
definePotentiometer("FLOOD_DIMMER", 9, 3003, 415, {0, 1}, "Interior Lights Panel", "FLOOD Light Dimmer")
define3PosTumb("COCKKPIT_LIGHT_MODE_SW", 9, 3004, 419, "Interior Lights Panel", "MODE Switch, NVG/NITE/DAY")
definePotentiometer("CHART_DIMMER", 9, 3005, 418, {0, 1}, "Interior Lights Panel", "CHART Light Dimmer")
definePotentiometer("WARN_CAUTION_DIMMER", 9, 3006, 417, {0, 1}, "Interior Lights Panel", "WARN/CAUTION Light Dimmer")
defineToggleSwitch("LIGHTS_TEST_SW", 9, 3007, 416, "Interior Lights Panel", "Lights Test Switch, TEST/OFF")

-- 4. AMAC Control

-- 5. Sensor Panel
define3PosTumb("FLIR_SW", 62, 3001, 439, "Sensor Panel", "FLIR Switch, ON/STBY/OFF")
define3PosTumb("LTD_R_SW", 62, 3002, 441, "Sensor Panel", "LTD/R Switch, ---/SAFE/AFT") -- ARM position is handled by another parameter
defineToggleSwitch("LST_NFLR_SW", 62, 3004, 442, "Sensor Panel", "LST/NFLR Switch, ON/OFF")
defineToggleSwitch("LTD_R_ARM", 62, 3003, 441, "Sensor Panel", "LTD/R Switch, ARM/SAFE")
defineTumb("RADAR_SW", 42, 3001, 440, 0.1, {0.0, 0.3}, nil, false, "Sensor Panel", "RADAR Switch Change ,OFF/STBY/OPR/EMERG(PULL)")
definePushButton("RADAR_SW_PULL", 42, 3002, 440, "Sensor Panel", "RADAR Switch Pull (MW to pull), OFF/STBY/OPR/EMERG(PULL)")
defineTumb("INS_SW", 44, 3001, 443, 0.1, {0.0, 0.7}, nil, false, "Sensor Panel", "INS Switch, OFF/CV/GND/NAV/IFA/GYRO/GB/TEST")

-- 6. KY-58 Control
defineTumb("KY58_MODE_SELECT", 41, 3001, 444, 0.1, {0.0, 0.3}, nil, false, "KY-58 Control", "KY-58 Mode Select Knob, P/C/LD/RV")
definePotentiometer("KY58_VOLUME", 41, 3005, 445, {0, 1}, "KY-58 Control", "KY-58 Volume Control Knob")
defineTumb("KY58_FILL_SELECT", 41, 3002, 446, 0.1, {0.0, 0.7}, nil, false, "KY-58 Control", "KY-58 Fill Select Knob, Z 1-5/1/2/3/4/5/6/Z ALL")
defineTumb("KY58_POWER_SELECT", 41, 3004, 447, 0.1, {0.0, 0.2}, nil, false, "KY-58 Control", "KY-58 Power Select Knob, OFF/ON/TD")

-- 7. NVG Storage

-- 8. Fan Test Switch

-- 9. Map and Data Case

-- 10. Utility Light

-- 11. Defog Panel
definePotentiometer("DEFOG_HANDLE", 11, 3005, 451, {0, 1}, "Defog Panel", "Defog Handle")
define3PosTumb("WSHIELD_ANTI_ICE_SW", 11, 3009, 452, "Defog Panel", "Windshield Anti-Ice/Rain Switch, ANTI ICE/OFF/RAIN")

-- 12. Internal Canopy Switch
defineRockerSwitch("CANOPY_SW", 7, 3001, 3001, 3002, 3002, 453, "Internal Canopy Switch", "Canopy Control Switch, OPEN/HOLD/CLOSE")

-- 13. Right Essential Circuit Breakers
definePushButton("CB_FCS_CHAN3", 3, 3021, 454, "Right Essential Circuit Breakers", "CB FCS CHAN 3, ON/OFF")
definePushButton("CB_FCS_CHAN4", 3, 3022, 455, "Right Essential Circuit Breakers", "CB FCS CHAN 4, ON/OFF")
definePushButton("CB_HOOOK", 3, 3023, 456, "Right Essential Circuit Breakers", "CB HOOK, ON/OFF")
definePushButton("CB_LG", 3, 3024, 457, "Right Essential Circuit Breakers", "CB LG, ON/OFF")
defineToggleSwitch("FCS_BIT_SW", 2, 3004, 470, "Right Essential Circuit Breakers", "FCS BIT Switch")

-- 14. Video Sensor Head

-- 15. Video Camera


-- COCKPIT CONTROLS

defineFloat("CANOPY_POS", 181, {0, 1}, "Internal Canopy Switch", "Canopy Position")

-- 1. Ejection Seat

defineEjectionHandleSwitch("EJECTION_HANDLE_SW", 7, 3008, 510, "Ejection Seat", "Ejection Control Handle")
defineToggleSwitch("EJECTION_SEAT_ARMED", 7, 3006, 511, "Ejection Seat", "Ejection Seat SAFE/ARMED Handle, SAFE/ARMED")
defineToggleSwitch("EJECTION_SEAT_MNL_OVRD", 7, 3007, 512, "Ejection Seat", "Ejection Seat Manual Override Handle, PULL/PUSH")
defineToggleSwitch("SHLDR_HARNESS_SW", 7, 3009, 513, "Ejection Seat", "Shoulder Harness Control Handle, LOCK/UNLOCK")
defineRockerSwitch("SEAT_HEIGHT_SW", 7, 3011, 3011, 3010, 3010, 514, "Ejection Seat", "Seat Height Adjustment Switch, UP/HOLD/DOWN")
defineToggleSwitch("HIDE_STICK_TOGGLE", 7, 3013, 575, "Ejection Seat", "Hide Stick toggle")

-- 2. TODO
definePushButton("LEFT_VIDEO_BIT", 0, 3127, 315, "TODO", "Left Video Sensor BIT Initiate Pushbutton - Push to initiate BIT")
definePushButton("RIGHT_VIDEO_BIT", 0, 3128, 318, "TODO", "Right Video Sensor BIT Initiate Pushbutton - Push to initiate BIT")

