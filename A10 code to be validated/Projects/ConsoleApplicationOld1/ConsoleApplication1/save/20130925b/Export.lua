-- Export Script for Simmeters Workspace DCS Wrapper
-- For complete information please refer to Workspace Manual

function LuaExportStart()

	package.path  = package.path..";.\\LuaSocket\\?.lua"
	package.cpath = package.cpath..";.\\LuaSocket\\?.dll"

	port = 7777
	host = "127.0.0.1"
	socket = require("socket")
	con = socket.try(socket.udp())
	socket.try(con:settimeout(.001))
	socket.try(con:setpeername(host,port))

end

function LuaExportBeforeNextFrame()

	local packet = ""
	local flightData = {}
	local selfData = LoGetSelfData();


	
	--*********************************************************************** EXPORT SECTION FOR A-10C ***********************************************************************
	if selfData and selfData.Name == "A-10C" then	

		local mp = GetDevice(0)
		if mp then
		
			-- Update Data
			---------------------------------------------------
			mp:update_arguments()
		


			-- LAMPS Indicators
			---------------------------------------------------

			-- Master Warning UFCP
			---------------------------------------------------
			--table.insert(flightData,"v900=" .. mp:get_argument_value(404)) 							-- MASTER_WARNING_STUB MASTER WARNING
			
			-- Caution Panel LAMPS
			---------------------------------------------------
			table.insert(flightData,"480=" .. mp:get_argument_value(480)) 							-- CAUTION LIGHT PANEL (ENG_START_CYCLE) 
			table.insert(flightData,"481=" .. mp:get_argument_value(481)) 							-- CAUTION LIGHT PANEL (L_HYD_PRESS)
			table.insert(flightData,"482=" .. mp:get_argument_value(482)) 							-- CAUTION LIGHT PANEL (R_HYD_PRESS) 
			table.insert(flightData,"483=" .. mp:get_argument_value(483))								-- CAUTION LIGHT PANEL (GUN_UNSAFE) 
			table.insert(flightData,"484=" .. mp:get_argument_value(484)) 							-- CAUTION LIGHT PANEL (ANTISKID) 
			table.insert(flightData,"485=" .. mp:get_argument_value(485)) 							-- CAUTION LIGHT PANEL (L_HYD_RES) 
			table.insert(flightData,"486=" .. mp:get_argument_value(486)) 							-- CAUTION LIGHT PANEL (R_HYD_RES) 
			table.insert(flightData,"487=" .. mp:get_argument_value(487)) 							-- CAUTION LIGHT PANEL (OXY_LOW) 
			table.insert(flightData,"488=" .. mp:get_argument_value(488)) 							-- CAUTION LIGHT PANEL (ELEV_DISENG) 
			table.insert(flightData,"489=" .. mp:get_argument_value(489)) 							-- CAUTION LIGHT PANEL (VOID1) 
			table.insert(flightData,"490=" .. mp:get_argument_value(490)) 							-- CAUTION LIGHT PANEL (SEAT_NOT_ARMED)
			table.insert(flightData,"491=" .. mp:get_argument_value(491)) 							-- CAUTION LIGHT PANEL (BLEED_AIR_LEAK) 
			table.insert(flightData,"492=" .. mp:get_argument_value(492)) 							-- CAUTION LIGHT PANEL (AIL_DISENG) 
			table.insert(flightData,"493=" .. mp:get_argument_value(493)) 							-- CAUTION LIGHT PANEL (L_AIL_TAB) 
			table.insert(flightData,"494=" .. mp:get_argument_value(494)) 							-- CAUTION LIGHT PANEL (R_AIL_TAB) 
			table.insert(flightData,"495=" .. mp:get_argument_value(495)) 							-- CAUTION LIGHT PANEL (SERVICE_AIR_HOT)
			table.insert(flightData,"496=" .. mp:get_argument_value(496))								-- CAUTION LIGHT PANEL (PITCH_SAS)
			table.insert(flightData,"497=" .. mp:get_argument_value(497)) 							-- CAUTION LIGHT PANEL (L_ENG_HOT)
			table.insert(flightData,"498=" .. mp:get_argument_value(498)) 							-- CAUTION LIGHT PANEL (R_ENG_HOT)
			table.insert(flightData,"499=" .. mp:get_argument_value(499)) 							-- CAUTION LIGHT PANEL (WINDSHIELD_HOT)
			table.insert(flightData,"500=" .. mp:get_argument_value(500)) 							-- CAUTION LIGHT PANEL (YAW_SAS) 
			table.insert(flightData,"501=" .. mp:get_argument_value(501)) 							-- CAUTION LIGHT PANEL (L_ENG_OIL_PRESS)
			table.insert(flightData,"502=" .. mp:get_argument_value(502)) 							-- CAUTION LIGHT PANEL (R_ENG_OIL_PRESS)
			table.insert(flightData,"503=" .. mp:get_argument_value(503)) 							-- CAUTION LIGHT PANEL (CICU)
			table.insert(flightData,"504=" .. mp:get_argument_value(504)) 							-- CAUTION LIGHT PANEL (GCAS)
			table.insert(flightData,"505=" .. mp:get_argument_value(505)) 							-- CAUTION LIGHT PANEL (L_MAIN_PUMP) 
			table.insert(flightData,"506=" .. mp:get_argument_value(506)) 							-- CAUTION LIGHT PANEL (R_MAIN_PUMP)
			table.insert(flightData,"507=" .. mp:get_argument_value(507)) 							-- CAUTION LIGHT PANEL (VOID2)
			table.insert(flightData,"508=" .. mp:get_argument_value(508)) 							-- CAUTION LIGHT PANEL (LASTE)
			table.insert(flightData,"509=" .. mp:get_argument_value(509)) 							-- CAUTION LIGHT PANEL (L_WING_PUMP)
			table.insert(flightData,"510=" .. mp:get_argument_value(510)) 							-- CAUTION LIGHT PANEL (R_WING_PUMP) 
			table.insert(flightData,"511=" .. mp:get_argument_value(511))								-- CAUTION LIGHT PANEL (HARS) 
			--table.insert(flightData,"512=" .. mp:get_argument_value(512)) 							-- CAUTION LIGHT PANEL (IFF_MODE_4) 
			table.insert(flightData,"513=" .. mp:get_argument_value(513)) 							-- CAUTION LIGHT PANEL (L_MAIN_FUEL_LOW) 
			table.insert(flightData,"514=" .. mp:get_argument_value(514)) 							-- CAUTION LIGHT PANEL (R_MAIN_FUEL_LOW) 
			table.insert(flightData,"515=" .. mp:get_argument_value(515)) 							-- CAUTION LIGHT PANEL (L_R_TKS_UNEQUAL)
			table.insert(flightData,"516=" .. mp:get_argument_value(516)) 							-- CAUTION LIGHT PANEL (EAC)
			table.insert(flightData,"517=" .. mp:get_argument_value(517)) 							-- CAUTION LIGHT PANEL (L_FUEL_PRESS)
			table.insert(flightData,"518=" .. mp:get_argument_value(518))								-- CAUTION LIGHT PANEL (R_FUEL_PRESS)
			table.insert(flightData,"519=" .. mp:get_argument_value(519)) 							-- CAUTION LIGHT PANEL (NAV)
			table.insert(flightData,"520=" .. mp:get_argument_value(520)) 							-- CAUTION LIGHT PANEL (STALL_SYS)
			table.insert(flightData,"521=" .. mp:get_argument_value(521)) 							-- CAUTION LIGHT PANEL (L_CONV) 
			table.insert(flightData,"522=" .. mp:get_argument_value(522)) 							-- CAUTION LIGHT PANEL (R_CONV) 
			table.insert(flightData,"523=" .. mp:get_argument_value(523)) 							-- CAUTION LIGHT PANEL (CADC) 
			table.insert(flightData,"524=" .. mp:get_argument_value(524)) 							-- CAUTION LIGHT PANEL (APU_GEN) 
			table.insert(flightData,"525=" .. mp:get_argument_value(525)) 							-- CAUTION LIGHT PANEL (L_GEN) 
			table.insert(flightData,"526=" .. mp:get_argument_value(526)) 							-- CAUTION LIGHT PANEL (R_GEN) 
			table.insert(flightData,"527=" .. mp:get_argument_value(527)) 							-- CAUTION LIGHT PANEL (INST_INV) 

			-- NMSP Panel lamps
			---------------------------------------------------
			--table.insert(flightData,"606=" .. mp:get_argument_value(606)) 							-- HARS
			--table.insert(flightData,"608=" .. mp:get_argument_value(608)) 							-- EGI
			--table.insert(flightData,"610=" .. mp:get_argument_value(610)) 							-- TISL
			--table.insert(flightData,"612=" .. mp:get_argument_value(612)) 							-- STRPT
			--table.insert(flightData,"614=" .. mp:get_argument_value(614)) 							-- ANCHR
			--table.insert(flightData,"616=" .. mp:get_argument_value(616)) 							-- TCN
			--table.insert(flightData,"618=" .. mp:get_argument_value(618)) 							-- ILS
			--table.insert(flightData,"619=" .. mp:get_argument_value(619)) 							-- UHF
			--table.insert(flightData,"620=" .. mp:get_argument_value(620)) 							-- FM
			
			--IFF Panel lamps
			---------------------------------------------------
			--table.insert(flightData,"798=" .. mp:get_argument_value(798)) 							-- IFF_reply_lamp_{0, 1}{0, 1}
			--table.insert(flightData,"799=" .. mp:get_argument_value(799)) 							-- IFF_test_lamp_{0, 1}{0, 1}

			-- Misc lamps
			---------------------------------------------------
			--table.insert(flightData,"191=" .. mp:get_argument_value(191)) 							-- TAKE_OFF_TRIM
			--table.insert(flightData,"659=" .. mp:get_argument_value(659)) 							-- LANDING_GEAR_N_SAFE
			--table.insert(flightData,"660=" .. mp:get_argument_value(660)) 							-- LANDING_GEAR_L_SAFE
			--table.insert(flightData,"661=" .. mp:get_argument_value(661)) 							-- LANDING_GEAR_R_SAFE
			--table.insert(flightData,"737=" .. mp:get_argument_value(737)) 							-- HANDLE_GEAR_WARNING
			table.insert(flightData,"663=" .. mp:get_argument_value(663)) 							-- NOSEWHEEL_STEERING
			--table.insert(flightData,"215=" .. mp:get_argument_value(215)) 							-- L_ENG_FIRE
			--table.insert(flightData,"216=" .. mp:get_argument_value(216)) 							-- APU_FIRE
			--table.insert(flightData,"217=" .. mp:get_argument_value(217)) 							-- R_ENG_FIRE
			--table.insert(flightData,"664=" .. mp:get_argument_value(664)) 							-- MARKER_BEACON
			--table.insert(flightData,"730=" .. mp:get_argument_value(730)) 							-- AIR_REFUEL_READY
			--table.insert(flightData,"731=" .. mp:get_argument_value(731)) 							-- AIR_REFUEL_LATCHED
			--table.insert(flightData,"732=" .. mp:get_argument_value(732)) 							-- AIR_REFUEL_DISCONNECT
			--table.insert(flightData,"178=" .. mp:get_argument_value(178)) 							-- L_AILERON_EMER_DISENGAGE
			--table.insert(flightData,"179=" .. mp:get_argument_value(179)) 							-- R_AILERON_EMER_DISENGAGE
			--table.insert(flightData,"181=" .. mp:get_argument_value(181)) 							-- L_ELEVATOR_EMER_DISENGAGE
			--table.insert(flightData,"182=" .. mp:get_argument_value(182)) 							-- R_ELEVATOR_EMER_DISENGAGE
			table.insert(flightData,"662=" .. mp:get_argument_value(662)) 							-- GUN_READY
			--table.insert(flightData,"665=" .. mp:get_argument_value(665)) 							-- CANOPY_UNLOCKED
			
			-- HARS Sync
			---------------------------------------------------
			--table.insert(flightData,"269=" .. mp:get_argument_value(269)) 							-- HARS_sync{-1.0, 1.0}{-1.0, 1.0}

			-- CMSC
			---------------------------------------------------
			--table.insert(flightData,"372=" .. mp:get_argument_value(372)) 							-- CMSC_MissileLaunchIndicator{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"373=" .. mp:get_argument_value(373)) 							-- CMSC_PriorityStatusIndicator{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"374=" .. mp:get_argument_value(374)) 							-- CMSC_UnknownStatusIndicator{0.0, 1.0}{0.0, 1.0}
			
			-- Misc Data
			---------------------------------------------------
			--table.insert(flightData,"7="   .. mp:get_argument_value(7)) 								-- canopy{0,1}{0,1}
			--table.insert(flightData,"8="   .. mp:get_argument_value(8)) 								-- LeftEngineThrottle{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"9="   .. mp:get_argument_value(9)) 								-- RightEngineThrottle{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"10="  .. mp:get_argument_value(10)) 								-- StickPitch{-1, 1}{ 1, -1}
			--table.insert(flightData,"11="  .. mp:get_argument_value(11))								-- StickBank{-1, 1}{0.45, -0.45}
			--table.insert(flightData,"93="  .. mp:get_argument_value(93)) 								-- RudderPedals{-1, 1}{-1, 1}
			--table.insert(flightData,"716=" .. mp:get_argument_value(716)) 							-- gear_handle{0.0, 1}{0.0, 1}
			--table.insert(flightData,"719=" .. mp:get_argument_value(719))								-- mirrors_draw {0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"263=" .. mp:get_argument_value(263)) 							-- TACAN_digit_pos.hundreds){0.0, 10.0}{0.0, 1.0}
			--table.insert(flightData,"264=" .. mp:get_argument_value(264)) 							-- TACAN_digit_pos.tens){0.0, 10.0}{0.0, 1.0}
			--table.insert(flightData,"265=" .. mp:get_argument_value(265)) 							-- TACAN_digit_pos.ones){0.0, 10.0}{0.0, 1.0}
			--table.insert(flightData,"266=" .. mp:get_argument_value(266)) 							-- XYwheel{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"260=" .. mp:get_argument_value(260)) 							-- TACAN_test_light{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"251=" .. mp:get_argument_value(251)) 							-- ILS_window_wheel_MHz{8.0, 11.0}{0.0, 0.3}
			--table.insert(flightData,"252=" .. mp:get_argument_value(252)) 							-- ILS_window_wheel_KHz{10.0, 15.0, 30.0, 35.0, 50.0, 55.0, 70.0, 75.0, 90.0, 95.0}{0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9}
			--table.insert(flightData,"143=" .. mp:get_argument_value(143)) 							-- VHF_radio.AM, 139, 0.5, 0, 15)
			--table.insert(flightData,"144=" .. mp:get_argument_value(144)) 							-- VHF_radio.AM, 140, 0.9, 0, 9)
			--table.insert(flightData,"145=" .. mp:get_argument_value(145)) 							-- VHF_radio.AM, 141, 0.9, 0, 9)
			--table.insert(flightData,"146=" .. mp:get_argument_value(146)) 							-- VHF_radio.AM, 142, 0.75, 0, 75)
			--table.insert(flightData,"157=" .. mp:get_argument_value(157)) 							-- VHF_radio.FM, 153, 0.9, 0, 15)
			--table.insert(flightData,"158=" .. mp:get_argument_value(158)) 							-- VHF_radio.FM, 154, 0.9, 0, 9)
			--table.insert(flightData,"159=" .. mp:get_argument_value(159)) 							-- VHF_radio.FM, 155, 0.9, 0, 9)
			--table.insert(flightData,"160=" .. mp:get_argument_value(160)) 							-- VHF_radio.FM, 156, 0.75, 0, 75)
			--table.insert(flightData,"771=" .. mp:get_argument_value(771)) 							-- seat_adjustment{-2.5 * 0.0254, 2.5 * 0.0254}{1,-1}
			--table.insert(flightData,"61="  .. mp:get_argument_value(61)) 								-- AAU34_PNEU_flag{0.0, 0.4}{0.0, 1.0}
			--table.insert(flightData,"281=" .. mp:get_argument_value(281)) 							-- CabinPressAlt{0.0, 50000.0}{0.0, 1.0}
			--table.insert(flightData,"274=" .. mp:get_argument_value(274)) 							-- OxygenVolume{0.0, 10.0}{0.0, 1.0}
			--table.insert(flightData,"604=" .. mp:get_argument_value(604)) 							-- OxygenPress{0.0, 100.0, 500.0}{0.0, 0.5, 1.0}
			--table.insert(flightData,"600=" .. mp:get_argument_value(600)) 							-- BreathFlow{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"791=" .. mp:get_argument_value(791)) 							-- DVADR_end_of_tape{0, 1}{0, 1}
			--table.insert(flightData,"792=" .. mp:get_argument_value(792)) 							-- DVADR_record{0, 1}{0, 1}
			--table.insert(flightData,"793=" .. mp:get_argument_value(793)) 							-- DVADR_cp_end_of_tape {0, 1}{0, 1}
			--table.insert(flightData,"794=" .. mp:get_argument_value(794)) 							-- DVADR_cp_record{0, 1}{0, 1}
		
		end
	
	end
	
	
	-- TX Data
	---------------------------------------------------
	for i = 1, #flightData do packet = packet .. flightData[i] .. ":" end
	socket.try(con:send(packet))
	
	-- RX Data
	---------------------------------------------------
	local i = 0
	local args = {}
	local recv = con:receive()
    
	if recv then
		for arg in string.gmatch(recv,'(.-);()') do 
			table.insert(args,i,arg)
			i = i + 1
			if i == 3 then 
				i = 0 
				local dev = GetDevice(args[0])
				if dev then dev:performClickableAction(args[1],args[2]) end
			end
		end
	end
		
end

function LuaExportStop()
	socket.try(con:close())
end