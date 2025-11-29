-- SSOIC_CONV_BEFORENEXTFRAME
--
-- Called from export.lua
-- Updates variable, and then exports to UDP socket
	local soic_conv_packet = ""
	local soic_conv_flightData = {}
	local soic_conv_selfData = LoGetSelfData();

	local arduino_display_packet = ""
	local arduino_display_flightData = {}
	
	local gps_export_packet = ""
	local gps_export_flightData = {}

	
	--*********************************************************************** EXPORT SECTION FOR A-10C ***********************************************************************
	if soic_conv_selfData and soic_conv_selfData.Name == "A-10C" then	

		local soic_conv_mp = GetDevice(0)
		if soic_conv_mp then
		
			-- Update Data
			---------------------------------------------------
			soic_conv_mp:update_arguments()
		


			-- LAMPS Indicators
			---------------------------------------------------

			-- Master Warning UFCP
			---------------------------------------------------
			--table.insert(flightData,"v900=" .. soic_conv_mp:get_argument_value(404)) 						-- MASTER_WARNING_STUB MASTER WARNING
			
			-- Caution Panel LAMPS
			---------------------------------------------------
			table.insert(soic_conv_flightData,"480=" .. soic_conv_mp:get_argument_value(480)) 							-- CAUTION LIGHT PANEL (ENG_START_CYCLE) 
			table.insert(soic_conv_flightData,"481=" .. soic_conv_mp:get_argument_value(481)) 							-- CAUTION LIGHT PANEL (L_HYD_PRESS)
			table.insert(soic_conv_flightData,"482=" .. soic_conv_mp:get_argument_value(482)) 							-- CAUTION LIGHT PANEL (R_HYD_PRESS) 
			table.insert(soic_conv_flightData,"483=" .. soic_conv_mp:get_argument_value(483))							-- CAUTION LIGHT PANEL (GUN_UNSAFE) 
			table.insert(soic_conv_flightData,"484=" .. soic_conv_mp:get_argument_value(484)) 							-- CAUTION LIGHT PANEL (ANTISKID) 
			table.insert(soic_conv_flightData,"485=" .. soic_conv_mp:get_argument_value(485)) 							-- CAUTION LIGHT PANEL (L_HYD_RES) 
			table.insert(soic_conv_flightData,"486=" .. soic_conv_mp:get_argument_value(486)) 							-- CAUTION LIGHT PANEL (R_HYD_RES) 
			table.insert(soic_conv_flightData,"487=" .. soic_conv_mp:get_argument_value(487)) 							-- CAUTION LIGHT PANEL (OXY_LOW) 
			table.insert(soic_conv_flightData,"488=" .. soic_conv_mp:get_argument_value(488)) 							-- CAUTION LIGHT PANEL (ELEV_DISENG) 
			table.insert(soic_conv_flightData,"489=" .. soic_conv_mp:get_argument_value(489)) 							-- CAUTION LIGHT PANEL (VOID1) 
			table.insert(soic_conv_flightData,"490=" .. soic_conv_mp:get_argument_value(490)) 							-- CAUTION LIGHT PANEL (SEAT_NOT_ARMED)
			table.insert(soic_conv_flightData,"491=" .. soic_conv_mp:get_argument_value(491)) 							-- CAUTION LIGHT PANEL (BLEED_AIR_LEAK) 
			table.insert(soic_conv_flightData,"492=" .. soic_conv_mp:get_argument_value(492)) 							-- CAUTION LIGHT PANEL (AIL_DISENG) 
			table.insert(soic_conv_flightData,"493=" .. soic_conv_mp:get_argument_value(493)) 							-- CAUTION LIGHT PANEL (L_AIL_TAB) 
			table.insert(soic_conv_flightData,"494=" .. soic_conv_mp:get_argument_value(494)) 							-- CAUTION LIGHT PANEL (R_AIL_TAB) 
			table.insert(soic_conv_flightData,"495=" .. soic_conv_mp:get_argument_value(495)) 							-- CAUTION LIGHT PANEL (SERVICE_AIR_HOT)
			table.insert(soic_conv_flightData,"496=" .. soic_conv_mp:get_argument_value(496))							-- CAUTION LIGHT PANEL (PITCH_SAS)
			table.insert(soic_conv_flightData,"497=" .. soic_conv_mp:get_argument_value(497)) 							-- CAUTION LIGHT PANEL (L_ENG_HOT)
			table.insert(soic_conv_flightData,"498=" .. soic_conv_mp:get_argument_value(498)) 							-- CAUTION LIGHT PANEL (R_ENG_HOT)
			table.insert(soic_conv_flightData,"499=" .. soic_conv_mp:get_argument_value(499)) 							-- CAUTION LIGHT PANEL (WINDSHIELD_HOT)
			table.insert(soic_conv_flightData,"500=" .. soic_conv_mp:get_argument_value(500)) 							-- CAUTION LIGHT PANEL (YAW_SAS) 
			table.insert(soic_conv_flightData,"501=" .. soic_conv_mp:get_argument_value(501)) 							-- CAUTION LIGHT PANEL (L_ENG_OIL_PRESS)
			table.insert(soic_conv_flightData,"502=" .. soic_conv_mp:get_argument_value(502)) 							-- CAUTION LIGHT PANEL (R_ENG_OIL_PRESS)
			table.insert(soic_conv_flightData,"503=" .. soic_conv_mp:get_argument_value(503)) 							-- CAUTION LIGHT PANEL (CICU)
			table.insert(soic_conv_flightData,"504=" .. soic_conv_mp:get_argument_value(504)) 							-- CAUTION LIGHT PANEL (GCAS)
			table.insert(soic_conv_flightData,"505=" .. soic_conv_mp:get_argument_value(505)) 							-- CAUTION LIGHT PANEL (L_MAIN_PUMP) 
			table.insert(soic_conv_flightData,"506=" .. soic_conv_mp:get_argument_value(506)) 							-- CAUTION LIGHT PANEL (R_MAIN_PUMP)
			table.insert(soic_conv_flightData,"507=" .. soic_conv_mp:get_argument_value(507)) 							-- CAUTION LIGHT PANEL (VOID2)
			table.insert(soic_conv_flightData,"508=" .. soic_conv_mp:get_argument_value(508)) 							-- CAUTION LIGHT PANEL (LASTE)
			table.insert(soic_conv_flightData,"509=" .. soic_conv_mp:get_argument_value(509)) 							-- CAUTION LIGHT PANEL (L_WING_PUMP)
			table.insert(soic_conv_flightData,"510=" .. soic_conv_mp:get_argument_value(510)) 							-- CAUTION LIGHT PANEL (R_WING_PUMP) 
			table.insert(soic_conv_flightData,"511=" .. soic_conv_mp:get_argument_value(511))							-- CAUTION LIGHT PANEL (HARS) 
			--table.insert(soic_conv_flightData,"512=" .. soic_conv_mp:get_argument_value(512)) 						-- CAUTION LIGHT PANEL (IFF_MODE_4) 
			table.insert(soic_conv_flightData,"513=" .. soic_conv_mp:get_argument_value(513)) 							-- CAUTION LIGHT PANEL (L_MAIN_FUEL_LOW) 
			table.insert(soic_conv_flightData,"514=" .. soic_conv_mp:get_argument_value(514)) 							-- CAUTION LIGHT PANEL (R_MAIN_FUEL_LOW) 
			table.insert(soic_conv_flightData,"515=" .. soic_conv_mp:get_argument_value(515)) 							-- CAUTION LIGHT PANEL (L_R_TKS_UNEQUAL)
			table.insert(soic_conv_flightData,"516=" .. soic_conv_mp:get_argument_value(516)) 							-- CAUTION LIGHT PANEL (EAC)
			table.insert(soic_conv_flightData,"517=" .. soic_conv_mp:get_argument_value(517)) 							-- CAUTION LIGHT PANEL (L_FUEL_PRESS)
			table.insert(soic_conv_flightData,"518=" .. soic_conv_mp:get_argument_value(518))							-- CAUTION LIGHT PANEL (R_FUEL_PRESS)
			table.insert(soic_conv_flightData,"519=" .. soic_conv_mp:get_argument_value(519)) 							-- CAUTION LIGHT PANEL (NAV)
			table.insert(soic_conv_flightData,"520=" .. soic_conv_mp:get_argument_value(520)) 							-- CAUTION LIGHT PANEL (STALL_SYS)
			table.insert(soic_conv_flightData,"521=" .. soic_conv_mp:get_argument_value(521)) 							-- CAUTION LIGHT PANEL (L_CONV) 
			table.insert(soic_conv_flightData,"522=" .. soic_conv_mp:get_argument_value(522)) 							-- CAUTION LIGHT PANEL (R_CONV) 
			table.insert(soic_conv_flightData,"523=" .. soic_conv_mp:get_argument_value(523)) 							-- CAUTION LIGHT PANEL (CADC) 
			table.insert(soic_conv_flightData,"524=" .. soic_conv_mp:get_argument_value(524)) 							-- CAUTION LIGHT PANEL (APU_GEN) 
			table.insert(soic_conv_flightData,"525=" .. soic_conv_mp:get_argument_value(525)) 							-- CAUTION LIGHT PANEL (L_GEN) 
			table.insert(soic_conv_flightData,"526=" .. soic_conv_mp:get_argument_value(526)) 							-- CAUTION LIGHT PANEL (R_GEN) 
			table.insert(soic_conv_flightData,"527=" .. soic_conv_mp:get_argument_value(527)) 							-- CAUTION LIGHT PANEL (INST_INV) 

			-- NMSP Panel lamps
			---------------------------------------------------
			table.insert(soic_conv_flightData,"606=" .. soic_conv_mp:get_argument_value(606)) 							-- HARS
			table.insert(soic_conv_flightData,"608=" .. soic_conv_mp:get_argument_value(608)) 							-- EGI
			table.insert(soic_conv_flightData,"610=" .. soic_conv_mp:get_argument_value(610)) 							-- TISL
			table.insert(soic_conv_flightData,"612=" .. soic_conv_mp:get_argument_value(612)) 							-- STRPT
			table.insert(soic_conv_flightData,"614=" .. soic_conv_mp:get_argument_value(614)) 							-- ANCHR
			table.insert(soic_conv_flightData,"616=" .. soic_conv_mp:get_argument_value(616)) 							-- TCN
			table.insert(soic_conv_flightData,"618=" .. soic_conv_mp:get_argument_value(618)) 							-- ILS
			table.insert(soic_conv_flightData,"619=" .. soic_conv_mp:get_argument_value(619)) 							-- UHF
			table.insert(soic_conv_flightData,"620=" .. soic_conv_mp:get_argument_value(620)) 							-- FM
			
			--IFF Panel lamps
			---------------------------------------------------
			--table.insert(soic_conv_flightData,"798=" .. soic_conv_mp:get_argument_value(798)) 							-- IFF_reply_lamp_{0, 1}{0, 1}
			--table.insert(soic_conv_flightData,"799=" .. soic_conv_mp:get_argument_value(799)) 							-- IFF_test_lamp_{0, 1}{0, 1}

			-- Misc lamps
			---------------------------------------------------
			table.insert(soic_conv_flightData,"191=" .. soic_conv_mp:get_argument_value(191)) 							-- TAKE_OFF_TRIM
			table.insert(soic_conv_flightData,"659=" .. soic_conv_mp:get_argument_value(659)) 							-- LANDING_GEAR_N_SAFE
			table.insert(soic_conv_flightData,"660=" .. soic_conv_mp:get_argument_value(660)) 							-- LANDING_GEAR_L_SAFE
			table.insert(soic_conv_flightData,"661=" .. soic_conv_mp:get_argument_value(661)) 							-- LANDING_GEAR_R_SAFE
			table.insert(soic_conv_flightData,"737=" .. soic_conv_mp:get_argument_value(737)) 							-- HANDLE_GEAR_WARNING
			table.insert(soic_conv_flightData,"663=" .. soic_conv_mp:get_argument_value(663)) 							-- NOSEWHEEL_STEERING
			table.insert(soic_conv_flightData,"215=" .. soic_conv_mp:get_argument_value(215)) 							-- L_ENG_FIRE
			table.insert(soic_conv_flightData,"216=" .. soic_conv_mp:get_argument_value(216)) 							-- APU_FIRE
			table.insert(soic_conv_flightData,"217=" .. soic_conv_mp:get_argument_value(217)) 							-- R_ENG_FIRE
			table.insert(soic_conv_flightData,"664=" .. soic_conv_mp:get_argument_value(664)) 							-- MARKER_BEACON
			--table.insert(soic_conv_flightData,"730=" .. soic_conv_mp:get_argument_value(730)) 							-- AIR_REFUEL_READY
			--table.insert(soic_conv_flightData,"731=" .. soic_conv_mp:get_argument_value(731)) 							-- AIR_REFUEL_LATCHED
			--table.insert(soic_conv_flightData,"732=" .. soic_conv_mp:get_argument_value(732)) 							-- AIR_REFUEL_DISCONNECT
			--table.insert(soic_conv_flightData,"178=" .. soic_conv_mp:get_argument_value(178)) 							-- L_AILERON_EMER_DISENGAGE
			--table.insert(soic_conv_flightData,"179=" .. soic_conv_mp:get_argument_value(179)) 							-- R_AILERON_EMER_DISENGAGE
			--table.insert(soic_conv_flightData,"181=" .. soic_conv_mp:get_argument_value(181)) 							-- L_ELEVATOR_EMER_DISENGAGE
			--table.insert(soic_conv_flightData,"182=" .. soic_conv_mp:get_argument_value(182)) 							-- R_ELEVATOR_EMER_DISENGAGE
			table.insert(soic_conv_flightData,"662=" .. soic_conv_mp:get_argument_value(662)) 							-- GUN_READY
			table.insert(soic_conv_flightData,"665=" .. soic_conv_mp:get_argument_value(665)) 							-- CANOPY_UNLOCKED
			
			-- HARS Sync
			---------------------------------------------------
			--table.insert(soic_conv_flightData,"269=" .. soic_conv_mp:get_argument_value(269)) 							-- HARS_sync{-1.0, 1.0}{-1.0, 1.0}

			-- CMSC
			---------------------------------------------------
			table.insert(soic_conv_flightData,"372=" .. soic_conv_mp:get_argument_value(372)) 							-- CMSC_MissileLaunchIndicator{0.0, 1.0}{0.0, 1.0}
			table.insert(soic_conv_flightData,"373=" .. soic_conv_mp:get_argument_value(373)) 							-- CMSC_PriorityStatusIndicator{0.0, 1.0}{0.0, 1.0}
			table.insert(soic_conv_flightData,"374=" .. soic_conv_mp:get_argument_value(374)) 							-- CMSC_UnknownStatusIndicator{0.0, 1.0}{0.0, 1.0}
			
			-- Misc Data
			---------------------------------------------------
			--table.insert(soic_conv_flightData,"7="   .. soic_conv_mp:get_argument_value(7)) 								-- canopy{0,1}{0,1}
			--table.insert(soic_conv_flightData,"8="   .. soic_conv_mp:get_argument_value(8)) 								-- LeftEngineThrottle{0.0, 1.0}{0.0, 1.0}
			--table.insert(soic_conv_flightData,"9="   .. soic_conv_mp:get_argument_value(9)) 								-- RightEngineThrottle{0.0, 1.0}{0.0, 1.0}
			--table.insert(soic_conv_flightData,"10="  .. soic_conv_mp:get_argument_value(10)) 								-- StickPitch{-1, 1}{ 1, -1}
			--table.insert(soic_conv_flightData,"11="  .. soic_conv_mp:get_argument_value(11))								-- StickBank{-1, 1}{0.45, -0.45}
			--table.insert(soic_conv_flightData,"93="  .. soic_conv_mp:get_argument_value(93)) 								-- RudderPedals{-1, 1}{-1, 1}
			--table.insert(soic_conv_flightData,"716=" .. soic_conv_mp:get_argument_value(716)) 							-- gear_handle{0.0, 1}{0.0, 1}
			--table.insert(soic_conv_flightData,"719=" .. soic_conv_mp:get_argument_value(719))								-- mirrors_draw {0.0, 1.0}{0.0, 1.0}
			--table.insert(soic_conv_flightData,"263=" .. soic_conv_mp:get_argument_value(263)) 							-- TACAN_digit_pos.hundreds){0.0, 10.0}{0.0, 1.0}
			--table.insert(soic_conv_flightData,"264=" .. soic_conv_mp:get_argument_value(264)) 							-- TACAN_digit_pos.tens){0.0, 10.0}{0.0, 1.0}
			--table.insert(soic_conv_flightData,"265=" .. soic_conv_mp:get_argument_value(265)) 							-- TACAN_digit_pos.ones){0.0, 10.0}{0.0, 1.0}
			--table.insert(soic_conv_flightData,"266=" .. soic_conv_mp:get_argument_value(266)) 							-- XYwheel{0.0, 1.0}{0.0, 1.0}
			--table.insert(soic_conv_flightData,"260=" .. soic_conv_mp:get_argument_value(260)) 							-- TACAN_test_light{0.0, 1.0}{0.0, 1.0}
			--table.insert(soic_conv_flightData,"251=" .. soic_conv_mp:get_argument_value(251)) 							-- ILS_window_wheel_MHz{8.0, 11.0}{0.0, 0.3}
			--table.insert(soic_conv_flightData,"252=" .. soic_conv_mp:get_argument_value(252)) 							-- ILS_window_wheel_KHz{10.0, 15.0, 30.0, 35.0, 50.0, 55.0, 70.0, 75.0, 90.0, 95.0}{0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9}
			--table.insert(soic_conv_flightData,"143=" .. soic_conv_mp:get_argument_value(143)) 							-- VHF_radio.AM, 139, 0.5, 0, 15)
			--table.insert(soic_conv_flightData,"144=" .. soic_conv_mp:get_argument_value(144)) 							-- VHF_radio.AM, 140, 0.9, 0, 9)
			--table.insert(soic_conv_flightData,"145=" .. soic_conv_mp:get_argument_value(145)) 							-- VHF_radio.AM, 141, 0.9, 0, 9)
			--table.insert(soic_conv_flightData,"146=" .. soic_conv_mp:get_argument_value(146)) 							-- VHF_radio.AM, 142, 0.75, 0, 75)
			--table.insert(soic_conv_flightData,"157=" .. soic_conv_mp:get_argument_value(157)) 							-- VHF_radio.FM, 153, 0.9, 0, 15)
			--table.insert(soic_conv_flightData,"158=" .. soic_conv_mp:get_argument_value(158)) 							-- VHF_radio.FM, 154, 0.9, 0, 9)
			--table.insert(soic_conv_flightData,"159=" .. soic_conv_mp:get_argument_value(159)) 							-- VHF_radio.FM, 155, 0.9, 0, 9)
			--table.insert(soic_conv_flightData,"160=" .. soic_conv_mp:get_argument_value(160)) 							-- VHF_radio.FM, 156, 0.75, 0, 75)
			--table.insert(soic_conv_flightData,"771=" .. soic_conv_mp:get_argument_value(771)) 							-- seat_adjustment{-2.5 * 0.0254, 2.5 * 0.0254}{1,-1}
			--table.insert(soic_conv_flightData,"61="  .. soic_conv_mp:get_argument_value(61)) 								-- AAU34_PNEU_flag{0.0, 0.4}{0.0, 1.0}
			--table.insert(soic_conv_flightData,"281=" .. soic_conv_mp:get_argument_value(281)) 							-- CabinPressAlt{0.0, 50000.0}{0.0, 1.0}
			--table.insert(soic_conv_flightData,"274=" .. soic_conv_mp:get_argument_value(274)) 							-- OxygenVolume{0.0, 10.0}{0.0, 1.0}
			--table.insert(soic_conv_flightData,"604=" .. soic_conv_mp:get_argument_value(604)) 							-- OxygenPress{0.0, 100.0, 500.0}{0.0, 0.5, 1.0}
			--table.insert(soic_conv_flightData,"600=" .. soic_conv_mp:get_argument_value(600)) 							-- BreathFlow{0.0, 1.0}{0.0, 1.0}
			--table.insert(soic_conv_flightData,"791=" .. soic_conv_mp:get_argument_value(791)) 							-- DVADR_end_of_tape{0, 1}{0, 1}
			--table.insert(soic_conv_flightData,"792=" .. soic_conv_mp:get_argument_value(792)) 							-- DVADR_record{0, 1}{0, 1}
			--table.insert(soic_conv_flightData,"793=" .. soic_conv_mp:get_argument_value(793)) 							-- DVADR_cp_end_of_tape {0, 1}{0, 1}
			--table.insert(soic_conv_flightData,"794=" .. soic_conv_mp:get_argument_value(794)) 							-- DVADR_cp_record{0, 1}{0, 1}
	
	
			-- Radios
	
			-- ILS Frequency
			-- Major Digits
			-- Look up table 
			ils_major_digit = {"108", "109", "110", "111"}
			local DigitPointer = math.floor((soic_conv_mp:get_argument_value(251)*10)) + 1
			table.insert(soic_conv_flightData,"8010=" .. ils_major_digit[DigitPointer])
			table.insert(arduino_display_flightData,"8010=" .. ils_major_digit[DigitPointer])
			-- Minor Digits
			-- Needed to add a 1.1 as the maths sometimes rounded to .999 which causes errors in lookup
			DigitPointer = (soic_conv_mp:get_argument_value(252) * 10) + 1.1
			DigitPointer = tonumber(DigitPointer)
			-- Lookup Table
			ils_minor_digit = {"10", "15", "30", "35", "50", "55", "70", "75", "90", "95"}
			table.insert(soic_conv_flightData,"8011=" .. ils_minor_digit[math.floor(DigitPointer)])
			table.insert(arduino_display_flightData,"8011=" .. ils_minor_digit[math.floor(DigitPointer)])
			
			-- TACAN
			-- 1st Major Digit
			table.insert(soic_conv_flightData,"8020=" .. math.floor(soic_conv_mp:get_argument_value(263) * 10 + 0.1))
			table.insert(arduino_display_flightData,"8020=" .. math.floor(soic_conv_mp:get_argument_value(263) * 10 + 0.1))
			-- 2nd Major Digit
			table.insert(soic_conv_flightData,"8021=" .. math.floor(soic_conv_mp:get_argument_value(264) * 10 + 0.1))
			table.insert(arduino_display_flightData,"8021=" .. math.floor(soic_conv_mp:get_argument_value(264) * 10 + 0.1))
			-- Minor Digits
			table.insert(soic_conv_flightData,"8022=" .. math.floor(soic_conv_mp:get_argument_value(265) * 10 + 0.1))
			table.insert(arduino_display_flightData,"8022=" .. math.floor(soic_conv_mp:get_argument_value(265) * 10 + 0.1))
			-- XY
			table.insert(soic_conv_flightData,"8023=" .. soic_conv_mp:get_argument_value(266))
			table.insert(arduino_display_flightData,"8023=" .. soic_conv_mp:get_argument_value(266))
			
			-- VHF AM
			-- Major has values 3 to 15
			table.insert(soic_conv_flightData,"143=" .. math.floor(soic_conv_mp:get_argument_value(143) * 20 + 0.1))							-- VHF_radio.AM, 139, 0.5, 0, 15)
			table.insert(arduino_display_flightData,"143=" .. math.floor(soic_conv_mp:get_argument_value(143) * 20 + 0.1))							-- VHF_radio.AM, 139, 0.5, 0, 15)
			table.insert(soic_conv_flightData,"144=" .. math.floor(soic_conv_mp:get_argument_value(144) * 10 + 0.1))							-- VHF_radio.AM, 140, 0.9, 0, 9)
			table.insert(arduino_display_flightData,"144=" .. math.floor(soic_conv_mp:get_argument_value(144) * 10 + 0.1))							-- VHF_radio.AM, 140, 0.9, 0, 9)
			-- Moves through incremental numbers so will return interesting values
			table.insert(soic_conv_flightData,"145=" .. math.floor(soic_conv_mp:get_argument_value(145) * 10 + 0.1)) 							-- VHF_radio.AM, 141, 0.9, 0, 9)
			table.insert(arduino_display_flightData,"145=" .. math.floor(soic_conv_mp:get_argument_value(145) * 10 + 0.1)) 							-- VHF_radio.AM, 141, 0.9, 0, 9)
			-- Moves through incremental numbers so will return interesting values
			table.insert(soic_conv_flightData,"146=" .. string.format("%0.0f", soic_conv_mp:get_argument_value(146) * 100)) 					-- VHF_radio.AM, 142, 0.75, 0, 75)
			table.insert(arduino_display_flightData,"146=" .. string.format("%0.0f", soic_conv_mp:get_argument_value(146) * 100)) 					-- VHF_radio.AM, 142, 0.75, 0, 75)
			
			
			--VHF FM
			table.insert(soic_conv_flightData,"157=" .. math.floor(soic_conv_mp:get_argument_value(157) * 20 + 0.1)) 							-- VHF_radio.FM, 153, 0.9, 0, 15)
			table.insert(arduino_display_flightData,"157=" .. math.floor(soic_conv_mp:get_argument_value(157) * 20 + 0.1)) 							-- VHF_radio.FM, 153, 0.9, 0, 15)
			table.insert(soic_conv_flightData,"158=" .. math.floor(soic_conv_mp:get_argument_value(158) * 10 + 0.1)) 							-- VHF_radio.FM, 154, 0.9, 0, 9)
			table.insert(arduino_display_flightData,"158=" .. math.floor(soic_conv_mp:get_argument_value(158) * 10 + 0.1)) 							-- VHF_radio.FM, 154, 0.9, 0, 9)
			table.insert(soic_conv_flightData,"159=" .. math.floor(soic_conv_mp:get_argument_value(159) * 10 + 0.1))  							-- VHF_radio.FM, 155, 0.9, 0, 9)
			table.insert(arduino_display_flightData,"159=" .. math.floor(soic_conv_mp:get_argument_value(159) * 10 + 0.1))  							-- VHF_radio.FM, 155, 0.9, 0, 9)
			table.insert(soic_conv_flightData,"160=" .. string.format("%0.0f", soic_conv_mp:get_argument_value(160) * 100))						-- VHF_radio.FM, 156, 0.75, 0, 75)
			table.insert(arduino_display_flightData,"160=" .. string.format("%0.0f", soic_conv_mp:get_argument_value(160) * 100))						-- VHF_radio.FM, 156, 0.75, 0, 75)
			
			
			
			table.insert(soic_conv_flightData,"185=" .. soic_conv_mp:get_argument_value(185))													-- SAS Left Yaw
			table.insert(soic_conv_flightData,"186=" .. soic_conv_mp:get_argument_value(186))													-- SAS Right Yaw
			table.insert(soic_conv_flightData,"187=" .. soic_conv_mp:get_argument_value(187))													-- SAS Left Pitch
			table.insert(soic_conv_flightData,"188=" .. soic_conv_mp:get_argument_value(188))													-- SAS right Pitch
	

			-- Chaff Flare
			local CM = LoGetSnares()
			table.insert(arduino_display_flightData,"8030=" .. string.format("%d", CM.chaff))
			table.insert(arduino_display_flightData,"8031=" .. string.format("%d", CM.flare))

			--table.insert(gps_export_flightData,"test=test")
			table.insert(gps_export_flightData,"Latitude="..LoGetSelfData().LatLongAlt.Lat)  				-- LATITUDE
			table.insert(gps_export_flightData,"Longitude="..LoGetSelfData().LatLongAlt.Long) 				-- LONGITUDE
			table.insert(gps_export_flightData,"Altitude="..LoGetAltitudeAboveSeaLevel()) 					-- ALTITUDE SEA LEVEL(MTS TO FT)
			table.insert(gps_export_flightData,"Airspeed="..LoGetTrueAirSpeed() * 1.94) 					-- TRUE AIRSPEED (M/S TO KNOTS)
			table.insert(gps_export_flightData,"Heading="..LoGetSelfData().Heading * 57.3) 					-- HEADING (RAD TO DEG)
	
		end
	
	end
	
	
	-- TX Data
	---------------------------------------------------
	for soic_conv_i = 1, #soic_conv_flightData do soic_conv_packet = soic_conv_packet .. soic_conv_flightData[soic_conv_i] .. ":" end
	soic_conv_socket.try(soic_conv_con:send(soic_conv_packet))
	
	for arduino_display_i = 1, #arduino_display_flightData do arduino_display_packet = arduino_display_packet .. arduino_display_flightData[arduino_display_i] .. ":" end
	arduino_display_socket.try(arduino_display_con:send(arduino_display_packet))

	for gps_export_i = 1, #gps_export_flightData do gps_export_packet = gps_export_packet .. gps_export_flightData[gps_export_i] .. ":" end
	gps_export_socket.try(gps_export_con:send(gps_export_packet))
	
	
	-- RX Data
	---------------------------------------------------

 

    --local lInput = soic_conv_con:receive()
	local lInput = cdu_socket:receive()
    local lCommand, lCommandArgs, lDevice, lArgument, lLastValue
	
    
    if lInput then
		-- If needing to log uncomment log file open in export.lua
		-- logfile:write("Have Input", "\n")
		-- logfile:write(lInput, "\n")
		-- logfile:flush()
        lCommand = string.sub(lInput,1,1)
        
		if lCommand == "R" then
			ResetChangeValues()
		end
	
		if (lCommand == "C") then

			lCommandArgs = StrSplit(string.sub(lInput,2),",")
			lDevice = GetDevice(lCommandArgs[1])
			lLastValue = tostring(lDevice)
			if type(lDevice) == "table" then
				--logfile:write(lLastValue, ":", lCommandArgs[0], ":",lCommandArgs[1],":",lCommandArgs[2],":",lCommandArgs[3], "\n")
				lDevice:performClickableAction(lCommandArgs[2],lCommandArgs[3])	
			end

		end
    end
