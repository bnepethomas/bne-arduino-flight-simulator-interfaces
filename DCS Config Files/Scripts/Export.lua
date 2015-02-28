-- Export Script for Simmeters Workspace DCS Wrapper
-- For complete information please refer to Workspace Manual

function LuaExportStart()

	package.path  = package.path..";.\\LuaSocket\\?.lua"
	package.cpath = package.cpath..";.\\LuaSocket\\?.dll"

	dofile("C:\\Users\\admin\\Saved Games\\DCS\\Scripts\\soic_conv_ExportStart.lua")
	
	port = 6061
	host = "127.0.0.1"
	socket = require("socket")
	con = socket.try(socket.udp())
	socket.try(con:settimeout(.001))
	socket.try(con:setpeername(host,port))

end

function LuaExportBeforeNextFrame()

	dofile("C:\\Users\\admin\\Saved Games\\DCS\\Scripts\\soic_conv_BeforeNextFrame.lua")

	local packet = ""
	local flightData = {}
	local selfData = LoGetSelfData();

	--*********************************************************************** EXPORT SECTION FOR SU-25T ***********************************************************************
	if selfData and selfData.Name == "Su-25T" then	
	
		table.insert(flightData,"XXX="..LoGetSelfData().LatLongAlt.Lat)  								-- LATITUDE
		table.insert(flightData,"XXX="..LoGetSelfData().LatLongAlt.Long) 								-- LONGITUDE
		table.insert(flightData,"XXX="..LoGetAltitudeAboveSeaLevel() * 3.28) 							-- ALTITUDE SEA LEVEL(MTS TO FT)
		table.insert(flightData,"XXX="..LoGetAltitudeAboveGroundLevel() * 3.28)							-- ALTITUDE GROUND LEVEL(MTS TO FT)
		table.insert(flightData,"XXX="..LoGetBasicAtmospherePressure() * 0.03937)						-- BAROMETRIC PRESSURE(MM TO IN) 
		table.insert(flightData,"XXX="..LoGetTrueAirSpeed() * 1.94) 									-- TRUE AIRSPEED (M/S TO KNOTS)
		table.insert(flightData,"ASI00="..LoGetIndicatedAirSpeed() * 1.94) 								-- INDICATED AIRSPEED (M/S TO KNOTS)
		table.insert(flightData,"XXX="..LoGetMachNumber())	 											-- MACH
		table.insert(flightData,"XXX="..LoGetVerticalVelocity() * 196.85)								-- VERTICAL SPEED (M/S TO FPM)
		table.insert(flightData,"XXX="..pitch * 57.3) 													-- PITCH (RAD TO DEG)
		table.insert(flightData,"XXX="..bank * 57.3) 													-- BANK (RAD TO DEG)
		table.insert(flightData,"XXX="..LoGetSelfData().Heading * 57.3) 								-- HEADING (RAD TO DEG)
		table.insert(flightData,"XXX="..LoGetSlipBallPosition()) 										-- SLIP BALL (-1 +1)
		table.insert(flightData,"XXX="..LoGetAngleOfAttack() * 57.3)	 								-- ANGLE OF ATTACK AoA (RAD TO DEG)
		table.insert(flightData,"XXX="..LoGetAccelerationUnits().y)	 									-- G-LOAD
		table.insert(flightData,"XXX="..LoGetNavigationInfo().Requirements.pitch * 57.3)				-- AP REQUIRED PITCH (RAD TO DEG)
		table.insert(flightData,"XXX="..LoGetNavigationInfo().Requirements.roll * 57.3)					-- AP REQUIRED BANK (RAD TO DEG)
		table.insert(flightData,"XXX="..LoGetNavigationInfo().Requirements.speed * 1.94)				-- AP SPEED (M/S TO KNOTS)
		table.insert(flightData,"XXX="..LoGetNavigationInfo().Requirements.altitude * 3.28)				-- AP ALTITUDE (MTS TO FT)
		table.insert(flightData,"XXX="..LoGetNavigationInfo().Requirements.vertical_speed * 196.85)		-- AP VERTICAL SPEED (M/S TO FPM)
		table.insert(flightData,"XXX="..LoGetSideDeviation())											-- VOR1 HORIZONTAL DEFLECTION (-1 +1)
		table.insert(flightData,"XXX="..LoGetGlideDeviation())											-- VOR1 VERTICAL DEFLECTION (-1 +1)
		table.insert(flightData,"XXX="..LoGetControlPanel_HSI().RMI * 57.3)								-- VOR1 OBS (RAD TO DEG)
		table.insert(flightData,"XXX="..LoGetControlPanel_HSI().ADF * 57.3)								-- ADF OBS (RAD TO DEG)
		table.insert(flightData,"XXX="..LoGetEngineInfo().RPM.left)										-- ENG1 RPM %
		table.insert(flightData,"XXX="..LoGetEngineInfo().RPM.right)									-- ENG2 RPM %
		table.insert(flightData,"XXX="..LoGetEngineInfo().Temperature.left)								-- ENG1 EGT ºC
		table.insert(flightData,"XXX="..LoGetEngineInfo().Temperature.right)							-- ENG2 EGT ºC
		table.insert(flightData,"XXX="..LoGetEngineInfo().fuel_internal * 2.2)							-- TANK1 (INT) (KG TO LBS)
		table.insert(flightData,"XXX="..LoGetEngineInfo().fuel_external * 2.2)							-- TANK2 (EXT) (KG TO LBS)
	
	end

	--*********************************************************************** EXPORT SECTION FOR Ka-50 ***********************************************************************
	if selfData and selfData.Name == "Ka-50" then	
	
		local mp = GetDevice(0)
		if mp then
		
			-- Update Data
			---------------------------------------------------
			mp:update_arguments()

			-- ADP_4 Accelerometer
			---------------------------------------------------
			table.insert(flightData,"ACC00="..(mp:get_argument_value(97) * 6) -2) 						-- ADP_4_acceleration{-2.0,4.0}{0.0,1.0} G-Units

			-- IKP-81 (ADI)
			---------------------------------------------------
			table.insert(flightData,"ADI00="..mp:get_argument_value(101) * -90) 						-- ADI_Pitch {-math.pi/2.0,math.pi/2.0}{1.0,-1.0} Degrees
			table.insert(flightData,"ADI01="..mp:get_argument_value(100) * -360) 						-- ADI_Roll {0.0,2.0*math.pi}{-1.0,1.0} Degrees
			table.insert(flightData,"ADI03="..mp:get_argument_value(106) * -27) 						-- ADI_pitch_steering {-1.0,1.0}{-1.0,1.0} Degrees
			table.insert(flightData,"ADI04="..mp:get_argument_value(107) * -27) 						-- ADI_bank_steering {-1.0,1.0}{-1.0,1.0} Degrees
			table.insert(flightData,"ADI05="..mp:get_argument_value(526) * 27) 							-- ADI_height_deviation {-1.0,1.0}{-1.0,1.0} Degrees
			table.insert(flightData,"ADI06="..mp:get_argument_value(103) * 27) 							-- ADI_track_deviation {-1.0,1.0}{-1.0,1.0} Degrees
			table.insert(flightData,"ADI07="..mp:get_argument_value(111) * 27) 							-- ADI_airspeed_deviation {-1.0,1.0}{-1.0,1.0} Degrees
			table.insert(flightData,"ADI08=" .. mp:get_argument_value(102) * 1) 						-- ADI_steering_warning_flag {0.0,1.0}{0.0,1.0} Boolean
			table.insert(flightData,"ADI09=" .. mp:get_argument_value(109) * 1) 						-- ADI_attitude_warning_flag {0.0,1.0}{0.0,1.0} Boolean
			table.insert(flightData,"ADI0A=" .. mp:get_argument_value(108) * 27) 						-- ADI_sideslip {-1.0,1.0}{-1.0,1.0} Degrees

			-- AGR-81 backup ADI
			---------------------------------------------------
			table.insert(flightData,"ADI0B=" .. mp:get_argument_value(143) * 90)						-- AGR_81_Pitch{-math.pi / 2.0, math.pi / 2.0}{-1.0, 1.0}
			table.insert(flightData,"ADI0C=" .. mp:get_argument_value(142) * 180)						-- AGR_81_Roll{-math.pi, math.pi}{1.0, -1.0}
			table.insert(flightData,"ADI0D=" .. mp:get_argument_value(144) * 27)						-- AGR_81_sideslip	{-1.0, 1.0}{-1.0, 1.0}
			table.insert(flightData,"ADI0E=" .. mp:get_argument_value(599) * 27)						-- AGR_81_LongitudinalDeviationBar{-1.0, 1.0}{-1.0, 1.0}
			table.insert(flightData,"ADI0F=" .. mp:get_argument_value(613) * 27)						-- AGR_81_LateralDeviationBar{-1.0, 1.0}{-1.0, 1.0}
			table.insert(flightData,"ADI10=" .. mp:get_argument_value(145) * 1)							-- AGR_81_failure_flag{0.0, 1.0}{0.0, 1.0}

			-- A_036 Radar Altimeter
			---------------------------------------------------
			local AGL01 = mp:get_argument_value(94)														-- A_036_RALT {0,20,50,150,200,300,350}{0.0,0.1838,0.4631,0.7541,0.8330,0.9329,1.0} Meters
			if (AGL01 > 0      and AGL01 <= 0.1838) then table.insert(flightData,"AGL01=" .. (AGL01 * 144.92)) end
			if (AGL01 > 0.1838 and AGL01 <= 0.4631) then table.insert(flightData,"AGL01=" .. (20 +  ((AGL01 - 0.1838) * 107.41))) end
			if (AGL01 > 0.4631 and AGL01 <= 0.7541) then table.insert(flightData,"AGL01=" .. (50 +  ((AGL01 - 0.4631) * 343.64))) end
			if (AGL01 > 0.7541 and AGL01 <= 0.8330) then table.insert(flightData,"AGL01=" .. (150 + ((AGL01 - 0.7541) * 633.71))) end
			if (AGL01 > 0.8330 and AGL01 <= 0.9329) then table.insert(flightData,"AGL01=" .. (200 + ((AGL01 - 0.8330) * 1001.0))) end
			if (AGL01 > 0.9329 and AGL01 <= 	 1.0) then table.insert(flightData,"AGL01=" .. (300 + ((AGL01 - 0.9329) * 745.15))) end
			if (AGL01 > 1.0) then table.insert(flightData,"AGL01=350") end

			local AGL03 = mp:get_argument_value(93)														-- A_036_DangerRALT_index {0,20,50,150,200,300,350}{0.0,0.1838,0.4631,0.7541,0.8330,0.9329,1.0} Meters
			if (AGL03 > 0      and AGL03 <= 0.1838) then table.insert(flightData,"AGL03=" .. (AGL03 * 144.92)) end
			if (AGL03 > 0.1838 and AGL03 <= 0.4631) then table.insert(flightData,"AGL03=" .. (20 +  ((AGL03 - 0.1838) * 107.41))) end
			if (AGL03 > 0.4631 and AGL03 <= 0.7541) then table.insert(flightData,"AGL03=" .. (50 +  ((AGL03 - 0.4631) * 343.64))) end
			if (AGL03 > 0.7541 and AGL03 <= 0.8330) then table.insert(flightData,"AGL03=" .. (150 + ((AGL03 - 0.7541) * 633.71))) end
			if (AGL03 > 0.8330 and AGL03 <= 0.9329) then table.insert(flightData,"AGL03=" .. (200 + ((AGL03 - 0.8330) * 1001.0))) end
			if (AGL03 > 0.9329 and AGL03 <= 	 1.0) then table.insert(flightData,"AGL03=" .. (300 + ((AGL03 - 0.9329) * 745.15))) end
			if (AGL03 > 1.0) then table.insert(flightData,"AGL03=350") end

			table.insert(flightData,"AGL04="  .. mp:get_argument_value(92))								-- A_036_DangerRALT_lamp {0.0,1.0}{0.0,1.0} Boolean
			table.insert(flightData,"AGLXX="  .. mp:get_argument_value(95))								-- A_036_warning_flag {0.0,1.0}{0.0,1.0} Boolean

			-- VM_15PV Barometric Altimeter
			---------------------------------------------------
			table.insert(flightData,"ALT01=" .. (mp:get_argument_value(87) * 10000))					-- VM_15PV_BALT_thousands{0.0,10000.0}{0.0,1.0} Meters
			table.insert(flightData,"ALT03=" .. ((mp:get_argument_value(88) * 200) + 600)) 				-- VM_15PV_BaroPressure{600.0,800.0}{0.0,1.0} mmHG
			table.insert(flightData,"ALTXX="  .. (mp:get_argument_value(89) * 10000)) 					-- VM_15PV_BALT_CommandedAlt{0.0,10000.0}{0.0,1.0} Meters

			-- APU Temperature
			---------------------------------------------------
			table.insert(flightData,"APU01=" .. mp:get_argument_value(6) * 900)							-- APUTemperature{0.0,900.0}{0.0,1.0}

			-- Indicated Airspeed
			---------------------------------------------------
			table.insert(flightData,"ASI01=" .. (mp:get_argument_value(51) * 100) * 3.6) 				-- IAS {0.0,100.0}{0.0,1.0} ms to kph

			-- KI-13 (magnetic compass)
			---------------------------------------------------
			table.insert(flightData,"HDG00=" .. mp:get_argument_value(11) * 360)						-- KI_13_course {0,math.pi*2.0}{-1,1} Degrees
			table.insert(flightData,"HDGXX=" .. mp:get_argument_value(12) * 90)							-- KI_13_pitch {-math.pi/2.0,math.pi/2.0}{-1,1} Degrees
			table.insert(flightData,"HDGXX=" .. mp:get_argument_value(14) * 180)						-- KI_13_bank {-math.pi,math.pi}{-1,1} Degrees
		
			-- PNP-72-16 (HSI)
			---------------------------------------------------
			table.insert(flightData,"HSI00=" .. mp:get_argument_value(112) * 360) 						-- HSI_heading{0.0, math.pi * 2.0}{0.0, 1.0}
			table.insert(flightData,"HSI01=" .. mp:get_argument_value(124) * 360) 						-- HSI_commanded_heading_needle{0.0, math.pi * 2.0}{0.0, 1.0}
			table.insert(flightData,"HSI02=" .. mp:get_argument_value(118) * 360) 						-- HSI_commanded_course_needle{0.0, math.pi * 2.0}{0.0, 1.0}
			table.insert(flightData,"HSI03=" .. mp:get_argument_value(128) * 27) 						-- HSI_lateral_deviation{-1.0, 1.0}{-1.0, 1.0}
			table.insert(flightData,"HSI05=" .. mp:get_argument_value(115) * 360) 						-- HSI_bearing_needle{0.0, math.pi * 2.0}{0.0, 1.0}
			--table.insert(flightData,"HSI07=" .. mp:get_argument_value(117) * 10) 						-- HSI_range_counter_hundreds{0.0,10.0}{0.0,1.0}
			--table.insert(flightData,"HSI07=" .. mp:get_argument_value(527) * 10) 						-- HSI_range_counter_tenth{0.0,10.0}{0.0,1.0}
			--table.insert(flightData,"HSI07=" .. mp:get_argument_value(528) * 10) 						-- HSI_range_counter_units{0.0,10.0}{0.0,1.0}
			table.insert(flightData,"HSI08=" .. mp:get_argument_value(127) * 27) 						-- HSI_longitudinal_deviation{-1.0, 1.0}{-1.0, 1.0}
			table.insert(flightData,"HSI09=" .. LoGetSelfData().LatLongAlt.Lat) 						-- LATITUDE
			table.insert(flightData,"HSI0A=" .. LoGetSelfData().LatLongAlt.Long) 						-- LONGITUDE
			table.insert(flightData,"HSI0C=" .. mp:get_argument_value(116) * 1) 						-- HSI_range_unavailable_flag{0.0, 1.0}{0.0, 1.0}
			table.insert(flightData,"HSI0D=" .. mp:get_argument_value(121) * 1) 						-- HSI_course_unavailable_flag{0.0, 1.0}{0.0, 1.0}
			table.insert(flightData,"HSI0E=" .. mp:get_argument_value(114) * 1) 						-- HSI_course_warning_flag{0.0, 1.0}{0.0, 1.0}
			table.insert(flightData,"HSI0F=" .. mp:get_argument_value(125) * 1) 						-- HSI_glideslope_warning_flag{0.0, 1.0}{0.0, 1.0}
			table.insert(flightData,"HSI10=" .. mp:get_argument_value(119) * 1) 						-- HSI_heading_warning_flag{0.0,1.0}{0.0,1.0}

			-- Rotor Pitch
			---------------------------------------------------
			table.insert(flightData,"RPI00=" .. (mp:get_argument_value(53) * 15) - 1)					-- RotorPitch {1.0,15.0}{0.0,1.0} Degrees

			-- Left Engine RPM
			---------------------------------------------------
			table.insert(flightData,"RPM00=" .. mp:get_argument_value(135) * 110)						-- LeftEngineRPM{0.0, 110.0}{0.0,1.0} Percent%

			-- Right Engine RPM
			---------------------------------------------------
			table.insert(flightData,"RPM01=" .. mp:get_argument_value(136) * 110)						-- RightEngineRPM{0.0, 110.0}{0.0,1.0} Percent%
		
			-- Rotor RPM
			---------------------------------------------------
			table.insert(flightData,"RPM02=" .. mp:get_argument_value(52) * 110)						-- RotorRPM {0.0,110.0}{0.0,1.0} Percent%
		
			-- Engines temperature
			---------------------------------------------------
			table.insert(flightData,"TMP00=" .. mp:get_argument_value(133) * 1200)						-- LeftEngineTemperatureHund{0.0,1200.0}{0.0,1.0} Celsius
			--table.insert(flightData,"566=" .. mp:get_argument_value(566) * 100)						-- LeftEngineTemperatureTenth{0.0,100.0}{0.0,1.0} Celsius
			table.insert(flightData,"TMP01=" .. mp:get_argument_value(134) * 1200)						-- RightEngineTemperatureHund{0.0,1200.0}{0.0,1.0} Celsius
			--table.insert(flightData,"567=" .. mp:get_argument_value(567) * 100)						-- RightEngineTemperatureTenth{0.0,100.0}{0.0,1.0} Celsius
		
			-- Fuel tanks (TODO CHECK)
			---------------------------------------------------
			table.insert(flightData,"TNK00=" .. mp:get_argument_value(137) * 800)						-- ForwardTankAmount {0.0,1.0}{0.0,1.0} Lbs ??
			table.insert(flightData,"TNK01=" .. mp:get_argument_value(138) * 800)						-- RearTankAmount {0.0,1.0}{0.0,1.0} Lbs ??
			--table.insert(flightData,"139=" .. mp:get_argument_value(139) * 1)							-- lamp_ForwardTankTest {0.0,1.0}{0.0,1.0} Boolean
			--table.insert(flightData,"140=" .. mp:get_argument_value(140) * 1)							-- lamp_RearTankTest {0.0,1.0}{0.0,1.0} Boolean

			-- VVI Indicator
			---------------------------------------------------
			table.insert(flightData,"VVI01=" .. (mp:get_argument_value(24) * 30)) 						-- VVI {-30,30}{-1.0,1.0} M/S

			-- Mechanic Clock
			---------------------------------------------------
			--table.insert(flightData,"68="  .. mp:get_argument_value(68) * 12)							-- CLOCK_currtime_hours{0.0,12.0}{0.0,1.0} Hours
			--table.insert(flightData,"69="  .. mp:get_argument_value(69) * 60)							-- CLOCK_currtime_minutes{0.0,60.0}{0.0,1.0} Minutes
			--table.insert(flightData,"70="  .. mp:get_argument_value(70) * 60)							-- CLOCK_currtime_seconds{0.0,60.0}{0.0,1.0} Seconds
			--table.insert(flightData,"75="  .. mp:get_argument_value(75))								-- CLOCK_flight_time_meter_status{0.0,0.2}{0.0,0.2} 
			--table.insert(flightData,"72="  .. mp:get_argument_value(72) * 12)							-- CLOCK_flight_hours{0.0,12.0}{0.0,1.0} Hours
			--table.insert(flightData,"531=" .. mp:get_argument_value(531) * 60)						-- CLOCK_flight_minutes{0.0,60.0}{0.0,1.0} Minutes
			--table.insert(flightData,"73="  .. mp:get_argument_value(73) * 30)							-- CLOCK_seconds_meter_time_minutes{0.0,30.0}{0.0,1.0} Minutes
			--table.insert(flightData,"532=" .. mp:get_argument_value(532) * 30)						-- CLOCK_seconds_meter_time_seconds{0.0,30.0}{0.0,1.0} Minutes

			-- Oil pressure and temperature indicators group (Right Panel)
			---------------------------------------------------
			--table.insert(flightData,"252=" .. mp:get_argument_value(252))								-- NeedleOilPressureEngineLeft {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"253=" .. mp:get_argument_value(253))								-- NeedleOilPressureEngineRight {0.0, 1.0}{0.0,1.0}
			--table.insert(flightData,"254=" .. mp:get_argument_value(254))								-- NeedleOilPressureGearBox {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"255=" .. mp:get_argument_value(255))								-- NeedleOilTemperatureEngineLeft {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"256=" .. mp:get_argument_value(256))								-- NeedleOilTemperatureEngineRight {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"257=" .. mp:get_argument_value(257))								-- NeedleOilTemperatureGearBox {0.0,1.0}{0.0,1.0}

			-- Hydraulics Gauges (Back Panel)
			---------------------------------------------------
			--table.insert(flightData,"471=" .. mp:get_argument_value(471))								-- hydro_common_pressure {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"472=" .. mp:get_argument_value(472))								-- hydro_main_pressure {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"473=" .. mp:get_argument_value(473))								-- hydro_acc_brake_pressure {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"474=" .. mp:get_argument_value(474))								-- hydro_gear_brake_pressure {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"475=" .. mp:get_argument_value(475))								-- hydro_common_temperature {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"476=" .. mp:get_argument_value(476))								-- hydro_main_temperature {0.0,1.0}{0.0,1.0}
		
			-- OUTPUT LAMPS & ILLUMINATION (TODO ORDER LAMPS)
			---------------------------------------------------
			--table.insert(flightData,"803=" .. mp:get_argument_value(803))								-- illumination_panels {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"799=" .. mp:get_argument_value(799)) 							-- illumination_panels2 {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"798=" .. mp:get_argument_value(798)) 							-- ADI_illumination {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"801=" .. mp:get_argument_value(801))								-- ADI_illumination2 {0.0,1.0}{0.0,1.0}		
			--table.insert(flightData,"802=" .. mp:get_argument_value(802)) 							-- AHR_illumination {0.0,1.0}{0.0,1.0}		
			--table.insert(flightData,"800=" .. mp:get_argument_value(800)) 							-- Control_Panel_illumination {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"1000=".. mp:get_argument_value(1000))							-- Plafond {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"545=" .. mp:get_argument_value(545)) 							-- mirrors_draw {0,1}{1,0}
			--table.insert(flightData,"540=" .. mp:get_argument_value(540)) 							-- pilot_draw {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"266=" .. mp:get_argument_value(266)) 							-- rudder {-0.0815,0.0815}{1,-1}
			--table.insert(flightData,"71="  .. mp:get_argument_value(71))								-- stick_roll {-0.13,0.13}{1,-1}
			--table.insert(flightData,"74="  .. mp:get_argument_value(74))								-- stick_pitch {-0.115,0.1816}{-1,1}
			--table.insert(flightData,"104=" .. mp:get_argument_value(104))								-- collective_position {0,1}{0,1}
			--table.insert(flightData,"560=" .. mp:get_argument_value(560))								-- coll_ROUTE_DESCENT_sw {0,1}{0,1}
			--table.insert(flightData,"558=" .. mp:get_argument_value(558))								-- coll_ReadjustFreeTurbRPM_sw {0,1}{0,1}
			--table.insert(flightData,"580=" .. mp:get_argument_value(580))								-- collective_stopper {0,1}{0,1}
			--table.insert(flightData,"533=" .. mp:get_argument_value(533))								-- left_door {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"578=" .. mp:get_argument_value(578))								-- left_engine_throttle {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"557=" .. mp:get_argument_value(557))								-- right_engine_throttle {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"579=" .. mp:get_argument_value(579))								-- throttle_cover {0.0,1.0}{0.0,1.0}

			-- Laser warning system
			---------------------------------------------------
			--table.insert(flightData,"582=" .. mp:get_argument_value(582))								-- LWS_LampReady {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"25="  .. mp:get_argument_value(25))								-- LWS_LampAzimuth_0 {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"28="  .. mp:get_argument_value(28))								-- LWS_LampAzimuth_90 {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"26="  .. mp:get_argument_value(26))								-- LWS_LampAzimuth_180 {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"27="  .. mp:get_argument_value(27))								-- LWS_LampAzimuth_270 {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"31="  .. mp:get_argument_value(31))								-- LWS_LampAzimuth_UpperHemisphere {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"32="  .. mp:get_argument_value(32))								-- LWS_LampAzimuth_LowerHemisphere {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"33="  .. mp:get_argument_value(33))								-- LWS_LampAzimuth_LaserRangefinder {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"34="  .. mp:get_argument_value(34))								-- LWS_LampAzimuth_LaserTargetingSystem {0.0,1.0}{0.0,1.0}

			-- LAMPS
			---------------------------------------------------
			--table.insert(flightData,"170=" .. mp:get_argument_value(170))								-- lamp_H_RALT_stab {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"175=" .. mp:get_argument_value(175))								-- lamp_Hover {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"172=" .. mp:get_argument_value(172))								-- lamp_Descent {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"165=" .. mp:get_argument_value(165))								-- lamp_RouteHeadingSteering {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"171=" .. mp:get_argument_value(171))								-- lamp_RouteCourseSteering {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"176=" .. mp:get_argument_value(176))								-- lamp_Waypoint {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"166=" .. mp:get_argument_value(166))								-- lamp_EndOfRoute {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"164=" .. mp:get_argument_value(164))								-- lamp_RoughNAVcalc {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"178=" .. mp:get_argument_value(178))								-- lamp_Burst {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"173=" .. mp:get_argument_value(173))								-- lamp_CannonMovingBarr {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"177=" .. mp:get_argument_value(177))								-- lamp_CannonMovingBarr2 {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"211=" .. mp:get_argument_value(211))								-- lamp_FuelCrossfeedOn {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"187=" .. mp:get_argument_value(187))								-- lamp_CouplerOff {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"204=" .. mp:get_argument_value(204))								-- lamp_ActuatorsOilPress {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"213=" .. mp:get_argument_value(213))								-- lamp_LockOff {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"167=" .. mp:get_argument_value(167))								-- lamp_WeapSysBUS_ON {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"180=" .. mp:get_argument_value(180))								-- lamp_TrainingMode {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"179=" .. mp:get_argument_value(179))								-- lamp_OBZOR_800 {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"188=" .. mp:get_argument_value(188))								-- lamp_RANET {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"189=" .. mp:get_argument_value(189))								-- lamp_CC_test {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"206=" .. mp:get_argument_value(206))								-- lamp_CC_failure {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"212=" .. mp:get_argument_value(212))								-- lamp_DC_AC_Inverter {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"205=" .. mp:get_argument_value(205))								-- lamp_I_251V {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"181=" .. mp:get_argument_value(181))								-- lamp_AntiIceLeftEngine {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"190=" .. mp:get_argument_value(190))								-- lamp_DustProtectLeftEngine {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"207=" .. mp:get_argument_value(207))								-- lamp_BackupModeLeftEngine {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"183=" .. mp:get_argument_value(183))								-- lamp_AntiIceRotors {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"182=" .. mp:get_argument_value(182))								-- lamp_AntiIceRightEngine {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"191=" .. mp:get_argument_value(191))								-- lamp_DustProtectRightEngine {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"208=" .. mp:get_argument_value(208))								-- lamp_BackupModeRightEngine {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"184=" .. mp:get_argument_value(184))								-- lamp_VUO_Heat {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"200=" .. mp:get_argument_value(200))								-- lamp_ForwardTank {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"209=" .. mp:get_argument_value(209))								-- lamp_LeftValveClosed {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"185=" .. mp:get_argument_value(185))								-- lamp_LeftOuterTank {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"202=" .. mp:get_argument_value(202))								-- lamp_LeftInnerTank {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"201=" .. mp:get_argument_value(201))								-- lamp_RearTank {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"210=" .. mp:get_argument_value(210))								-- lamp_RightValveClosed {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"186=" .. mp:get_argument_value(186))								-- lamp_RightOuterTank {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"203=" .. mp:get_argument_value(203))								-- lamp_RightInnerTank {0.0,1.0}{0.0,1.0}

			-- HUD
			---------------------------------------------------
			--table.insert(flightData,"509=" .. mp:get_argument_value(509))								-- filter {0,1}{0,1}
			--table.insert(flightData,"510=" .. mp:get_argument_value(510))								-- filter_handle {0,1}{0,1}

			-- UV-26
			---------------------------------------------------
			--table.insert(flightData,"541=" .. mp:get_argument_value(541))								-- UV26_lampLeftBoard {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"542=" .. mp:get_argument_value(542))								-- UV26_lampRightBoard {0.0,1.0}{0.0,1.0}

			-- DATALINK
			---------------------------------------------------
			--table.insert(flightData,"159=" .. mp:get_argument_value(159))								-- PRC_button_SEND {0.0,0.3}{0.0,0.3}
			--table.insert(flightData,"150=" .. mp:get_argument_value(150))								-- PRC_button_ESCAPE {0.0,0.3}{0.0,0.3}
			--table.insert(flightData,"161=" .. mp:get_argument_value(161))								-- PRC_button_ERASE {0.0,0.3}{0.0,0.3}
			--table.insert(flightData,"15="  .. mp:get_argument_value(15))								-- PRC_button_CLEAN {0.0,0.3}{0.0,0.3}
			--table.insert(flightData,"16="  .. mp:get_argument_value(16))								-- PRC_button_WINGMAN_ALL {0.0,0.3}{0.0,0.3}
			--table.insert(flightData,"17="  .. mp:get_argument_value(17))								-- PRC_button_WINGMAN_1 {0.0,0.3}{0.0,0.3}
			--table.insert(flightData,"18="  .. mp:get_argument_value(18))								-- PRC_button_WINGMAN_2 {0.0,0.3}{0.0,0.3}
			--table.insert(flightData,"19="  .. mp:get_argument_value(19))								-- PRC_button_WINGMAN_3 {0.0,0.3}{0.0,0.3}
			--table.insert(flightData,"20="  .. mp:get_argument_value(20))								-- PRC_button_WINGMAN_4 {0.0,0.3}{0.0,0.3}
			--table.insert(flightData,"21="  .. mp:get_argument_value(21))								-- PRC_button_TARGET_1 {0.0,0.3}{0.0,0.3}
			--table.insert(flightData,"22="  .. mp:get_argument_value(22))								-- PRC_button_TARGET_2 {0.0,0.3}{0.0,0.3}
			--table.insert(flightData,"23="  .. mp:get_argument_value(23))								-- PRC_button_TARGET_3 {0.0,0.3}{0.0,0.3}
			--table.insert(flightData,"50="  .. mp:get_argument_value(50))								-- PRC_button_TARGET_POINT {0.0,0.3}{0.0,0.3}

			-- WEAPON  INTERFACE
			---------------------------------------------------
			--table.insert(flightData,"437=" .. mp:get_argument_value(437))								-- PRC_button_AUTO_TURN {0.0,0.3}{0.0,0.3}
			--table.insert(flightData,"438=" .. mp:get_argument_value(438))								-- PRC_button_AIRBORNE_TARGET {0.0,0.3}{0.0,0.3}
			--table.insert(flightData,"440=" .. mp:get_argument_value(440))								-- PRC_button_GROUND_MOVING_TARGET {0.0,0.3}{0.0,0.3}
			--table.insert(flightData,"439=" .. mp:get_argument_value(439))								-- PRC_button_FORWARD_HEMISPHERE {0.0,0.3}{0.0,0.3}
			--table.insert(flightData,"441=" .. mp:get_argument_value(441))								-- PRC_button_CLEAR {0.0,0.3}{0.0,0.3}

			-- LAMPS
			---------------------------------------------------
			--table.insert(flightData,"44=" .. mp:get_argument_value(44))								-- lamp_MasterWarning {0.0,0.3}{0.0,0.3}
			--table.insert(flightData,"46=" .. mp:get_argument_value(46))								-- lamp_RotorRPM {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"47=" .. mp:get_argument_value(47))								-- lamp_UNDERFIREwarning {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"48=" .. mp:get_argument_value(48))								-- lamp_LowerGear {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"78=" .. mp:get_argument_value(78))								-- lamp_RPM_leftEngineMax {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"79=" .. mp:get_argument_value(79))								-- lamp_RPM_rightEngineMax {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"80=" .. mp:get_argument_value(80))								-- lamp_NyMax {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"81=" .. mp:get_argument_value(81))								-- lamp_vibr_leftEngine {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"82=" .. mp:get_argument_value(82))								-- lamp_vibr_rightEngine {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"83=" .. mp:get_argument_value(83))								-- lamp_IAS_max {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"84=" .. mp:get_argument_value(84))								-- lamp_mainTransmission {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"85=" .. mp:get_argument_value(85))								-- lamp_Fire {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"86=" .. mp:get_argument_value(86))								-- lamp_IFFfailure {0.0,1.0}{0.0,1.0}

			-- RadioCommunicator R-800
			---------------------------------------------------
			--table.insert(flightData,"419=" .. mp:get_argument_value(419))								-- Radio_Test_lamp {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"424=" .. mp:get_argument_value(424))								-- R800Revolve1 {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"577=" .. mp:get_argument_value(577))								-- R800Rotary1 {0.0,1.0}{0.0,1.0}	
			--table.insert(flightData,"425=" .. mp:get_argument_value(425))								-- R800Revolve2 {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"574=" .. mp:get_argument_value(574))								-- R800Rotary2 {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"426=" .. mp:get_argument_value(426))								-- R800Revolve3 {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"575=" .. mp:get_argument_value(575))								-- R800Rotary3 {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"427=" .. mp:get_argument_value(427))								-- R800Revolve4 {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"576=" .. mp:get_argument_value(576))								-- R800Rotary4 {0.0,1.0}{0.0,1.0}

			--RadioCommunicator R-828
			---------------------------------------------------
			--table.insert(flightData,"375=" .. mp:get_argument_value(375))								-- Radio_ASU_lamp{0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"581=" .. mp:get_argument_value(581)) 							-- R828_channel_selector{0.0,0.9}{0.0,1.0}

			-- LGCP - Landing gear control panel
			---------------------------------------------------
			--table.insert(flightData,"63=" .. mp:get_argument_value(63))								-- LGCP_Lamp_NoseGearUp {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"64=" .. mp:get_argument_value(64))								-- LGCP_Lamp_NoseGearDown {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"61=" .. mp:get_argument_value(61))								-- LGCP_Lamp_RightMainGearUp {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"62=" .. mp:get_argument_value(62))								-- LGCP_Lamp_RightMainGearDown {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"59=" .. mp:get_argument_value(59))								-- LGCP_Lamp_LeftMainGearUp {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"60=" .. mp:get_argument_value(60))								-- LGCP_Lamp_LeftMainGearDown{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"65=" .. mp:get_argument_value(65))								-- LGCP_gear_handle{0.0, 1.0}{0.0, 1.0}

			-- ABRIS
			---------------------------------------------------
			--table.insert(flightData,"561=" .. mp:get_argument_value(561))								-- lamp_ABRIS_button1{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"562=" .. mp:get_argument_value(562))								-- lamp_ABRIS_button2{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"563=" .. mp:get_argument_value(563))								-- lamp_ABRIS_button3	{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"564=" .. mp:get_argument_value(564))								-- lamp_ABRIS_button4{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"565=" .. mp:get_argument_value(565))								-- lamp_ABRIS_button5{0.0, 1.0}{0.0, 1.0}

			-- Engines mode indicator
			---------------------------------------------------
			--table.insert(flightData,"592=" .. mp:get_argument_value(592))								-- EnginesMode	{0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"234=" .. mp:get_argument_value(234))								-- LeftEngineMode{0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"235=" .. mp:get_argument_value(235))								-- RightEngineMode	{0.0,1.0}{0.0,1.0}

			-- Fire Extinguishers panel
			---------------------------------------------------
			--table.insert(flightData,"237=" .. mp:get_argument_value(237))								-- lamp_Exting_LeftEngineFire{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"239=" .. mp:get_argument_value(239))								-- lamp_Exting_APU_Fire{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"568=" .. mp:get_argument_value(568))								-- lamp_Exting_HydraulicsFire{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"241=" .. mp:get_argument_value(241))								-- lamp_Exting_RightEngineFire{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"243=" .. mp:get_argument_value(243))								-- lamp_Exting_VentilatorFire{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"244=" .. mp:get_argument_value(244))								-- lamp_Exting_Lamp1{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"245=" .. mp:get_argument_value(245))								-- lamp_Exting_Lamp2{0.0, 1.0}{0.0, 1.0}

			-- Panels buttons light
			---------------------------------------------------
			--table.insert(flightData,"551=" .. mp:get_argument_value(551))								-- Panels_buttons_light{0.0, 1.0}{0.0, 1.0}

			-- PVI (Nav Control Panel)
			---------------------------------------------------
			--table.insert(flightData,"313=" .. mp:get_argument_value(313))								-- PVI_button_ENTER{0.0, 0.3}{0.0, 0.3}
			--table.insert(flightData,"314=" .. mp:get_argument_value(314))								-- PVI_button_CANCEL{0.0, 0.3}{0.0, 0.3}
			--table.insert(flightData,"315=" .. mp:get_argument_value(315))								-- PVI_button_WPT{0.0, 0.3}{0.0, 0.3}
			--table.insert(flightData,"316=" .. mp:get_argument_value(316))								-- PVI_button_FIXPT{0.0, 0.3}{0.0, 0.3}
			--table.insert(flightData,"317=" .. mp:get_argument_value(317))								-- PVI_button_AERDR{0.0, 0.3}{0.0, 0.3}
			--table.insert(flightData,"318=" .. mp:get_argument_value(318))								-- PVI_button_TGT{0.0, 0.3}{0.0, 0.3}
			--table.insert(flightData,"319=" .. mp:get_argument_value(319))								-- PVI_button_FILAMBDA{0.0, 0.3}{0.0, 0.3}	
			--table.insert(flightData,"320=" .. mp:get_argument_value(320))								-- PVI_button_FIZ{0.0, 0.3}{0.0, 0.3}
			--table.insert(flightData,"321=" .. mp:get_argument_value(321))								-- PVI_button_DU{0.0, 0.3}{0.0, 0.3}
			--table.insert(flightData,"322=" .. mp:get_argument_value(322))								-- PVI_button_FII{0.0, 0.3}{0.0, 0.3}
			--table.insert(flightData,"323=" .. mp:get_argument_value(323))								-- PVI_button_BRGRNG{0.0, 0.3}{0.0, 0.3}
			--table.insert(flightData,"519=" .. mp:get_argument_value(519))								-- PVI_button_INSREALN{0.0, 0.3}{0.0, 0.3}
			--table.insert(flightData,"520=" .. mp:get_argument_value(520))								-- PVI_button_PRECALN{0.0, 0.3}{0.0, 0.3}
			--table.insert(flightData,"521=" .. mp:get_argument_value(521))								-- PVI_button_NORMALN{0.0, 0.3}{0.0, 0.3}
			--table.insert(flightData,"522=" .. mp:get_argument_value(522))								-- PVI_button_INITCOORD{0.0, 0.3}{0.0, 0.3}
			--table.insert(flightData,"330=" .. mp:get_argument_value(330))								-- PPR_button_K{0.0, 0.3}{0.0, 0.3}
			--table.insert(flightData,"332=" .. mp:get_argument_value(332))								-- PPR_button_H{0.0, 0.3}{0.0, 0.3}
			--table.insert(flightData,"333=" .. mp:get_argument_value(333))								-- PPR_button_B{0.0, 0.3}{0.0, 0.3}
			--table.insert(flightData,"331=" .. mp:get_argument_value(331))								-- PPR_button_T{0.0, 0.3}
			--table.insert(flightData,"334=" .. mp:get_argument_value(334))								-- PPR_button_DIR{0.0, 0.3}

			-- Engines start panel
			---------------------------------------------------
			--table.insert(flightData,"163=" .. mp:get_argument_value(163))								-- lamp_EnginesStartValve{0.0, 1.0}{0.0, 1.0}

			-- Engines breaks
			---------------------------------------------------
			--table.insert(flightData,"554=" .. mp:get_argument_value(554))								-- left_engine_break_hangle{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"555=" .. mp:get_argument_value(555))								-- right_engine_break_hangle{0.0, 1.0}{0.0, 1.0}

			-- APU control panel
			---------------------------------------------------
			--table.insert(flightData,"162=" .. mp:get_argument_value(162))								-- lamp_APUValveOpen{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"168=" .. mp:get_argument_value(168))								-- lamp_APUOilPres{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"169=" .. mp:get_argument_value(169))								-- lamp_APUStoppedByRPM{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"174=" .. mp:get_argument_value(174))								-- lamp_APU_IsON{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"392=" .. mp:get_argument_value(392))								-- lamp_PUI800_Sta1_WeapIsPresent{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"393=" .. mp:get_argument_value(393))								-- lamp_PUI800_Sta2_WeapIsPresent{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"394=" .. mp:get_argument_value(394))								-- lamp_PUI800_Sta3_WeapIsPresent{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"395=" .. mp:get_argument_value(395))								-- lamp_PUI800_Sta4_WeapIsPresent{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"388=" .. mp:get_argument_value(388))								-- lamp_PUI800_Sta1_WeapIsReady{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"389=" .. mp:get_argument_value(389))								-- lamp_PUI800_Sta2_WeapIsReady{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"390=" .. mp:get_argument_value(390))								-- lamp_PUI800_Sta3_WeapIsReady{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"391=" .. mp:get_argument_value(391))								-- lamp_PUI800_Sta4_WeapIsReady{0.0, 1.0}{0.0, 1.0}

			-- Right Side Panel
			---------------------------------------------------
			--table.insert(flightData,"586=" .. mp:get_argument_value(586))								-- lamp_AC_Ground_Power{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"261=" .. mp:get_argument_value(261))								-- lamp_DC_Ground_Power{0.0, 1.0}{0.0, 1.0}

			-- Right Back Panel
			---------------------------------------------------
			--table.insert(flightData,"469=" .. mp:get_argument_value(469))								-- lamp_flap_hydro_1{0.0, 1.0}{0.0, 1.0}
			--table.insert(flightData,"470=" .. mp:get_argument_value(470))								-- lamp_flap_hydro_2{0.0, 1.0}{0.0, 1.0}

			-- Eject system
			---------------------------------------------------
			--table.insert(flightData,"461=" .. mp:get_argument_value(461))								-- EjectSystemTestLamp{0.0, 1.0}{0.0, 1.0}

			-- PShK_7 (Latitude Entry Panel)
			---------------------------------------------------
			--table.insert(flightData,"339=" .. mp:get_argument_value(339))								-- PShK7_Latitude_counter_tenth {0.0,10.0}{0.0,1.0}
			--table.insert(flightData,"594=" .. mp:get_argument_value(594))								-- PShK7_Latitude_counter_units {0.0,10.0}{0.0,1.0}
			--table.insert(flightData,"342=" .. mp:get_argument_value(342))								-- PShK7_LampAuto {0.0,1.0}{0.0,1.0}

			-- ZMS_3 (Magnetic Variation Entry Panel)
			---------------------------------------------------
			--table.insert(flightData,"337=" .. mp:get_argument_value(337))								-- ZMS3_MagVar_counter_hundreds_tenth {-18.0,18.0}{-1.0,1.0}
			--table.insert(flightData,"596=" .. mp:get_argument_value(596))								-- ZMS3_MagVar_counter_units {0.0,10.0}{0.0,1.0}

			-- Mechanical stuff
			---------------------------------------------------
			--table.insert(flightData,"571=" .. mp:get_argument_value(571))								-- wheel_brakes {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"615=" .. mp:get_argument_value(615))								-- gun_trigger {-1.0,1.0}{1.0,-1.0}	
			--table.insert(flightData,"614=" .. mp:get_argument_value(614))								-- missile_trigger {-1.0,1.0}{1.0,-1.0}
			--table.insert(flightData,"546=" .. mp:get_argument_value(546))								-- windscreen_wiper { 0.0,1.0}{0.0,1.0}
	
		end

	end
	
	--*********************************************************************** EXPORT SECTION FOR A-10C ***********************************************************************
	if selfData and selfData.Name == "A-10C" then	

		local mp = GetDevice(0)
		if mp then
		
			-- Update Data
			---------------------------------------------------
			mp:update_arguments()
		
			-- Accelerometer
			---------------------------------------------------
			table.insert(flightData,"ACC00=" .. (mp:get_argument_value(15)  * 15) - 5) 					-- Accelerometer_main {-5.0,10.0}{0.0,1.0}
			table.insert(flightData,"ACC01=" .. (mp:get_argument_value(903) * 15) - 5) 					-- Accelerometer_max {-5.0,10.0}{0.0,1.0}
			table.insert(flightData,"ACC02=" .. (mp:get_argument_value(902) * 15) - 5) 					-- Accelerometer_min {-5.0,10.0}{0.0,1.0}

			-- Attitude Direction Indicator
			---------------------------------------------------
			table.insert(flightData,"ADI00=" .. mp:get_argument_value(17) * -90) 						-- ADI_Pitch {-math.pi/2.0,math.pi/2.0}{1.0,-1.0}
			table.insert(flightData,"ADI01=" .. mp:get_argument_value(18) * -180) 						-- ADI_Bank {0.0,math.pi*2.0}{-1.0,1.0}
			table.insert(flightData,"ADI03=" .. mp:get_argument_value(21) * -100) 						-- ADI_pitch_steering_bar {-1.0,1.0}{-1.0,1.0}
			table.insert(flightData,"ADI04=" .. mp:get_argument_value(20) * 100) 						-- ADI_bank_steering_bar {-1.0,1.0}{-1.0,1.0}
			table.insert(flightData,"ADI05=" .. mp:get_argument_value(27) * -50) 						-- ADI_glide_slope_indicator {-1.0,1.0}{-1.0,1.0}
			table.insert(flightData,"ADI08=" .. mp:get_argument_value(19)) 							    -- ADI_course_warning_flag {0.0,1.0}{0.0,1.0}
			table.insert(flightData,"ADI09=" .. mp:get_argument_value(25)) 								-- ADI_attitude_warning_flag {0.0,1.0}{0.0,1.0}
			table.insert(flightData,"ADI0A=" .. mp:get_argument_value(24) * 27) 						-- ADI_Slipball {-math.pi,math.pi}{-1.0,1.0}
			table.insert(flightData,"ADIXX=" .. mp:get_argument_value(26)) 								-- ADI_glide_slope_flag {0.0,1.0}{0.0,1.0}
		
			-- Standby Attitude Indicator
			---------------------------------------------------
			table.insert(flightData,"ADI0B=" .. mp:get_argument_value(63) * 90) 						-- SAI_Pitch {-math.pi/2.0,math.pi/2.0}{-1.0,1.0}
			table.insert(flightData,"ADI0C=" .. mp:get_argument_value(64) * 180) 						-- SAI_Bank {-math.pi,math.pi}{-1.0,1.0}
			table.insert(flightData,"ADI10=" .. mp:get_argument_value(65)) 								-- SAI_attitude_warning_flag {0.0,1.0}{0.0,1.0}

			-- Radar Altimeter
			---------------------------------------------------
			table.insert(flightData,"AGL00=" .. LoGetSelfData().LatLongAlt.Alt) 						-- Altitude Considered RALT for EADI

			-- Barometric Altimeter
			---------------------------------------------------
			local Altimeter_10000 = math.floor(mp:get_argument_value(52) * 10) * 10000 					-- Altimeter_10000_footCount
			local Altimeter_1000  = math.floor((mp:get_argument_value(53) + 0.0000001) * 10) * 1000 	-- Altimeter_10000_footCount
			local Altimeter_100   = mp:get_argument_value(54) * 1000									-- Altimeter_100_footCount
			table.insert(flightData,"ALT00=" .. (Altimeter_10000 + Altimeter_1000 + Altimeter_100)) 
			local pressure_setting_3 = math.floor(mp:get_argument_value(56) * 100) 						-- pressure_setting_3 
			local pressure_setting_2 = math.floor(mp:get_argument_value(57) * 10) 						-- pressure_setting_2 
			local pressure_setting_1 = mp:get_argument_value(58) 										-- pressure_setting_1 
			local pressure_setting_0 = mp:get_argument_value(59) * 0.1 									-- pressure_setting_0 
			table.insert(flightData,"ALT02=" .. (pressure_setting_3 + pressure_setting_2 + pressure_setting_1 + pressure_setting_0)) 

			-- Angle of attack indicator
			---------------------------------------------------
			table.insert(flightData,"AOA00=" .. mp:get_argument_value(4) * 30) 							-- AOA_Units {0.0,30.0}{0.0,1.0}
			table.insert(flightData,"AOA01=" .. mp:get_argument_value(540)) 							-- AOA_INDEXER_HIGH
			table.insert(flightData,"AOA02=" .. mp:get_argument_value(541)) 							-- AOA_INDEXER_NORM
			table.insert(flightData,"AOA03=" .. mp:get_argument_value(542)) 							-- AOA_INDEXER_LOW
		
			-- Apu RPM
			---------------------------------------------------		
			table.insert(flightData,"APU00=" .. mp:get_argument_value(13) * 120)						-- APU_RPM {0.0,120.0}{0.0,1.0}
		
			-- APU Temperature
			---------------------------------------------------		
			table.insert(flightData,"APU01=" .. mp:get_argument_value(14) * 1000) 						-- APUTemperature {0.0,1000.0}{0.0,1.0}
		
			-- Airspeed Indicator
			---------------------------------------------------
			local ASI00 = mp:get_argument_value(48)														-- AirspeedNeedle {0,50,100,150,200,250,300,350,400,450,500,550}{0.0,0.053,0.146,0.234,0.322,0.412,0.517,0.620,0.719,0.811,0.906,1.0}
			if (ASI00 > 0     and ASI00 <= 0.053) then table.insert(flightData,"ASI00=" .. (ASI00 * 943.39)) end
			if (ASI00 > 0.053 and ASI00 <= 0.146) then table.insert(flightData,"ASI00=" .. (50  + ((ASI00 - 0.053) * 537.63))) end
			if (ASI00 > 0.146 and ASI00 <= 0.234) then table.insert(flightData,"ASI00=" .. (100 + ((ASI00 - 0.146) * 568.18))) end
			if (ASI00 > 0.234 and ASI00 <= 0.322) then table.insert(flightData,"ASI00=" .. (150 + ((ASI00 - 0.234) * 568.18))) end
			if (ASI00 > 0.322 and ASI00 <= 0.412) then table.insert(flightData,"ASI00=" .. (200 + ((ASI00 - 0.322) * 555.55))) end
			if (ASI00 > 0.412 and ASI00 <= 0.517) then table.insert(flightData,"ASI00=" .. (250 + ((ASI00 - 0.412) * 476.19))) end
			if (ASI00 > 0.517 and ASI00 <= 0.620) then table.insert(flightData,"ASI00=" .. (300 + ((ASI00 - 0.517) * 485.43))) end
			if (ASI00 > 0.620 and ASI00 <= 0.719) then table.insert(flightData,"ASI00=" .. (350 + ((ASI00 - 0.620) * 505.05))) end
			if (ASI00 > 0.719 and ASI00 <= 0.811) then table.insert(flightData,"ASI00=" .. (400 + ((ASI00 - 0.719) * 543.47))) end
			if (ASI00 > 0.811 and ASI00 <= 0.906) then table.insert(flightData,"ASI00=" .. (450 + ((ASI00 - 0.811) * 526.31))) end
			if (ASI00 > 0.906 and ASI00 <= 1.0)   then table.insert(flightData,"ASI00=" .. (500 + ((ASI00 - 0.906) * 531.91))) end
			if (ASI00 > 1.0) then table.insert(flightData,"ASI00=550") end
			table.insert(flightData,"ASI04=" .. mp:get_argument_value(50)) 								-- maximum_IAS_needle {0,50,100,150,200,250,300,350,400,450,500,550}{0.0,0.053,0.146,0.234,0.322,0.412,0.517,0.620,0.719,0.811,0.906,1.0}
			table.insert(flightData,"ASIXX=" .. mp:get_argument_value(49)) 								-- AirspeedDial {0.0,100.0}{0.0,1.0}
		
			-- Left Engine Fan %RPM
			---------------------------------------------------
			local FAN00 = mp:get_argument_value(76)														-- EngineLeftFanSpeed {0,25,50,75,80,85,90,95,100}{0.0,1/8,2/8,3/8,4/8,5/8,6/8,7/8,1}
			if (FAN00 > 0     and FAN00 <= 0.375) then table.insert(flightData,"FAN00=" .. (FAN00 * 200)) end
			if (FAN00 > 0.375 and FAN00 <= 1)     then table.insert(flightData,"FAN00=" .. (75 + ((FAN00 - 0.375) * 40))) end
			if (FAN00 > 1.0) then table.insert(flightData,"FAN00=100") end
		
			-- Right Engine Fan %RPM
			---------------------------------------------------
			local FAN01 = mp:get_argument_value(77)														-- EngineRightFanSpeed {0,25,50,75,80,85,90,95,100}{0.0,1/8,2/8,3/8,4/8,5/8,6/8,7/8,1}
			if (FAN01 > 0     and FAN01 <= 0.375) then table.insert(flightData,"FAN01=" .. (FAN01 * 200)) end
			if (FAN01 > 0.375 and FAN01 <= 1)     then table.insert(flightData,"FAN01=" .. (75 + ((FAN01 - 0.375) * 40))) end
			if (FAN01 > 1.0) then table.insert(flightData,"v704=100") end

			-- Left Engine Fuel Flow
			---------------------------------------------------		
			table.insert(flightData,"FFW00=" .. mp:get_argument_value(84) * 5000) 						-- EngineLeftFuelFlow {0.0,5000.0}{0.0,1.0}

			-- Right Engine Fuel Flow
			---------------------------------------------------		
			table.insert(flightData,"FFW01=" .. mp:get_argument_value(85) * 5000) 						-- EngineRightFuelFlow {0.0,5000.0}{0.0,1.0}

			-- Flap Indicator
			---------------------------------------------------
			local FLP00 = mp:get_argument_value(653)													-- FlapPositionIndicator {0,20,30}{0,2/3,1}
			if (FLP00 > 0    and FLP00 <= 0.6)  then table.insert(flightData,"FLP00=" .. (FLP00 * 33.3)) end
			if (FLP00 > 0.6  and FLP00 <= 1)    then table.insert(flightData,"FLP00=" .. (20 + ((FLP00 - 0.6) * 25))) end
			if (FLP00 > 1.0) then table.insert(flightData,"FLP00=30") end
		
			-- Wet Compass
			---------------------------------------------------
			table.insert(flightData,"HDG00=" .. mp:get_argument_value(6) * 360) 						-- Standby magnetic compass {0.0,math.pi*2.0}{-1.0,1.0}
		
			-- Horizontal Situation Indicator
			---------------------------------------------------
			table.insert(flightData,"HSI00=" .. mp:get_argument_value(34) * 360) 						-- HSI_heading {0.0,math.pi*2.0}{0.0,1.0}
			table.insert(flightData,"HSI01=" .. mp:get_argument_value(36) * 360) 						-- HSI_heading_marker {0.0,math.pi*2.0}{0.0,1.0}
			table.insert(flightData,"HSI02=" .. mp:get_argument_value(47) * 360) 						-- HSI_course_arrow {0.0,math.pi*2.0}{0.0,1.0}
			table.insert(flightData,"HSI03=" .. mp:get_argument_value(41) * 60) 						-- HSI_deviation {-1.0,1.0}{-1.0,1.0}
			table.insert(flightData,"HSI04=" .. mp:get_argument_value(42)) 								-- HSI_to_from_1 {0.0,1.0}{0.0,1.0}
			table.insert(flightData,"HSI05=" .. mp:get_argument_value(33) * 360) 						-- HSI_bearing_no1 {0.0,math.pi*2.0}{0.0,1.0}
			table.insert(flightData,"HSI06=" .. mp:get_argument_value(35) * 360) 						-- HSI_bearing_no2 {0.0,math.pi*2.0}{0.0,1.0}
			--table.insert(flightData,"HSI07=" .. mp:get_argument_value(28)) 							-- HSI_range_counter_1000 {0.0,1.0}{0.0,1.0}
			--table.insert(flightData,"HSI07=" .. mp:get_argument_value(29)) 							-- HSI_range_counter_100 {0.0,10.0}{0.0,1.0}
			--table.insert(flightData,"HSI07=" .. mp:get_argument_value(30)) 							-- HSI_range_counter_10 {0.0,10.0}{0.0,1.0}
			--table.insert(flightData,"HSI07=" .. mp:get_argument_value(31)) 							-- HSI_range_counter_1 {0.0,10.0}{0.0,1.0}
			table.insert(flightData,"HSI09=" .. LoGetSelfData().LatLongAlt.Lat) 						-- LATITUDE
			table.insert(flightData,"HSI0A=" .. LoGetSelfData().LatLongAlt.Long) 						-- LONGITUDE
			table.insert(flightData,"HSI0B=" .. mp:get_argument_value(46)) 								-- HSI_bearing_flag {0.0,1.0}{0.0,1.0}
			table.insert(flightData,"HSI0C=" .. mp:get_argument_value(32)) 								-- HSI_range_flag {0.0,1.0}{0.0,1.0}
		
			-- Left Engine Oil Pressure
			---------------------------------------------------		
			table.insert(flightData,"PSI00=" .. mp:get_argument_value(82) * 100) 						-- EngineLeftOilPressure{0.0,100.0}{0.0,1.0}
		
			-- Right Engine Oil Pressure
			---------------------------------------------------		
			table.insert(flightData,"PSI01=" .. mp:get_argument_value(83) * 100) 						-- EngineRightOilPressure{0.0,100.0}{0.0,1.0}
		
			-- Left Engine RPM
			---------------------------------------------------		
			table.insert(flightData,"RPM00=" .. mp:get_argument_value(78) * 100) 						-- EngineLeftCoreSpeedTenth {0.0,100.0}{0.0,1.0}

			-- Right Engine RPM
			---------------------------------------------------		
			table.insert(flightData,"RPM01=" .. mp:get_argument_value(80) * 100) 						-- EngineRightCoreSpeedTenth {0.0,100.0}{0.0,1.0}

			-- Left Engine Temperature
			---------------------------------------------------		
			local TMP00 = mp:get_argument_value(70)														-- EngineLeftTemperatureTenth {100,200,300,400,500,600,700,800,900,1000,1100}{0.005,0.095,0.183,0.275,0.365,0.463,0.560,0.657,0.759,0.855,0.995}
			if (TMP00 > 0     and TMP00 <= 0.005) then table.insert(flightData,"TMP00=" .. (TMP00 * 20000)) end
			if (TMP00 > 0.005 and TMP00 <= 0.095) then table.insert(flightData,"TMP00=" .. (100  + ((TMP00 - 0.005) * 1111.11))) end
			if (TMP00 > 0.095 and TMP00 <= 0.183) then table.insert(flightData,"TMP00=" .. (200  + ((TMP00 - 0.095) * 1136.36))) end
			if (TMP00 > 0.183 and TMP00 <= 0.275) then table.insert(flightData,"TMP00=" .. (300  + ((TMP00 - 0.183) * 1086.95))) end
			if (TMP00 > 0.275 and TMP00 <= 0.365) then table.insert(flightData,"TMP00=" .. (400  + ((TMP00 - 0.275) * 1111.11))) end
			if (TMP00 > 0.365 and TMP00 <= 0.463) then table.insert(flightData,"TMP00=" .. (500  + ((TMP00 - 0.365) * 1020.40))) end
			if (TMP00 > 0.463 and TMP00 <= 0.560) then table.insert(flightData,"TMP00=" .. (600  + ((TMP00 - 0.463) * 1030.92))) end
			if (TMP00 > 0.560 and TMP00 <= 0.657) then table.insert(flightData,"TMP00=" .. (700  + ((TMP00 - 0.560) * 1030.92))) end
			if (TMP00 > 0.657 and TMP00 <= 0.759) then table.insert(flightData,"TMP00=" .. (800  + ((TMP00 - 0.657) * 980.39))) end
			if (TMP00 > 0.759 and TMP00 <= 0.855) then table.insert(flightData,"TMP00=" .. (900  + ((TMP00 - 0.759) * 1041.66))) end
			if (TMP00 > 0.855 and TMP00 <= 0.995) then table.insert(flightData,"TMP00=" .. (1000 + ((TMP00 - 0.855) * 714.28))) end
			if (TMP00 > 0.995) then table.insert(flightData,"TMP00=1100") end
		
			-- Right Engine Temperature
			---------------------------------------------------		
			local TMP01 = mp:get_argument_value(73)														-- EngineRightTemperatureTenth {100,200,300,400,500,600,700,800,900,1000,1100}{0.005,0.095,0.183,0.275,0.365,0.463,0.560,0.657,0.759,0.855,0.995}
			if (TMP01 > 0     and TMP01 <= 0.005) then table.insert(flightData,"TMP01=" .. (TMP01 * 20000)) end
			if (TMP01 > 0.005 and TMP01 <= 0.095) then table.insert(flightData,"TMP01=" .. (100  + ((TMP01 - 0.005) * 1111.11))) end
			if (TMP01 > 0.095 and TMP01 <= 0.183) then table.insert(flightData,"TMP01=" .. (200  + ((TMP01 - 0.095) * 1136.36))) end
			if (TMP01 > 0.183 and TMP01 <= 0.275) then table.insert(flightData,"TMP01=" .. (300  + ((TMP01 - 0.183) * 1086.95))) end
			if (TMP01 > 0.275 and TMP01 <= 0.365) then table.insert(flightData,"TMP01=" .. (400  + ((TMP01 - 0.275) * 1111.11))) end
			if (TMP01 > 0.365 and TMP01 <= 0.463) then table.insert(flightData,"TMP01=" .. (500  + ((TMP01 - 0.365) * 1020.40))) end
			if (TMP01 > 0.463 and TMP01 <= 0.560) then table.insert(flightData,"TMP01=" .. (600  + ((TMP01 - 0.463) * 1030.92))) end
			if (TMP01 > 0.560 and TMP01 <= 0.657) then table.insert(flightData,"TMP01=" .. (700  + ((TMP01 - 0.560) * 1030.92))) end
			if (TMP01 > 0.657 and TMP01 <= 0.759) then table.insert(flightData,"TMP01=" .. (800  + ((TMP01 - 0.657) * 980.39))) end
			if (TMP01 > 0.759 and TMP01 <= 0.855) then table.insert(flightData,"TMP01=" .. (900  + ((TMP01 - 0.759) * 1041.66))) end
			if (TMP01 > 0.855 and TMP01 <= 0.995) then table.insert(flightData,"TMP01=" .. (1000 + ((TMP01 - 0.855) * 714.28))) end
			if (TMP01 > 0.995) then table.insert(flightData,"TMP01=1100") end
		
			-- Fuel Indicator
			---------------------------------------------------		
			table.insert(flightData,"TNK00=" .. mp:get_argument_value(88) * 6000) 						-- FuelQuantityLeft {0.0,6000.0}{0.0,1.0} KG
			table.insert(flightData,"TNK01=" .. mp:get_argument_value(89) * 6000) 						-- FuelQuantityRight {0.0,6000.0}{0.0,1.0} KG
			--table.insert(flightData,"90=" .. mp:get_argument_value(90)) 								-- FuelQuantityCounterTenthOfThous{0.0,10.0}{0.0,1.0}
			--table.insert(flightData,"91=" .. mp:get_argument_value(91)) 								-- FuelQuantityCounterThousands{0.0,10.0}{0.0,1.0}
			--table.insert(flightData,"92=" .. mp:get_argument_value(92)) 								-- FuelQuantityCounterHundreds{0.0,10.0}{0.0,1.0}

			-- Variometer
			---------------------------------------------------
			local VVI00 = mp:get_argument_value(12)														-- Variometer FPM
			if (VVI00 > 0    and VVI00 <= 0.29) or (VVI00 < 0 and VVI00 >= -0.29) then table.insert(flightData,"VVI00=" .. (VVI00 * 3448.2758)) end
			if (VVI00 > 0.29 and VVI00 <= 0.5) 	then table.insert(flightData,"VVI00=" .. (1000 + ((VVI00 - 0.29) * 4761.9))) 	end
			if (VVI00 > 0.5  and VVI00 <= 1.0) 	then table.insert(flightData,"VVI00=" .. (2000 + ((VVI00 - 0.5) * 8000))) 	end
			if (VVI00 < -0.29 and VVI00 >= -0.5) 	then table.insert(flightData,"VVI00=" .. (-1000 + ((VVI00 + 0.29) * 4761.9))) end
			if (VVI00 < -0.5  and VVI00 >= -1) 	then table.insert(flightData,"VVI00=" .. (-2000 + ((VVI00 + 0.5) * 8000))) 	end

			-- Hydraulic Pressure
			---------------------------------------------------		
			--table.insert(flightData,"647=" .. mp:get_argument_value(647) * 4000) 						-- HydraulicPressureLeft{0.0,4000.0}{0.0,1.0}
			--table.insert(flightData,"648=" .. mp:get_argument_value(648) * 4000) 						-- HydraulicPressureRight{0.0,4000.0}{0.0,1.0}

			-- LAMPS Indicators
			---------------------------------------------------

			-- Master Warning UFCP
			---------------------------------------------------
			--table.insert(flightData,"v900=" .. mp:get_argument_value(404)) 							-- MASTER_WARNING_STUB MASTER WARNING
			
			-- Caution Panel LAMPS
			---------------------------------------------------
			--table.insert(flightData,"480=" .. mp:get_argument_value(480)) 							-- CAUTION LIGHT PANEL (ENG_START_CYCLE) 
			--table.insert(flightData,"481=" .. mp:get_argument_value(481)) 							-- CAUTION LIGHT PANEL (L_HYD_PRESS)
			--table.insert(flightData,"482=" .. mp:get_argument_value(482)) 							-- CAUTION LIGHT PANEL (R_HYD_PRESS) 
			--table.insert(flightData,"483=" .. mp:get_argument_value(483))								-- CAUTION LIGHT PANEL (GUN_UNSAFE) 
			--table.insert(flightData,"484=" .. mp:get_argument_value(484)) 							-- CAUTION LIGHT PANEL (ANTISKID) 
			--table.insert(flightData,"485=" .. mp:get_argument_value(485)) 							-- CAUTION LIGHT PANEL (L_HYD_RES) 
			--table.insert(flightData,"486=" .. mp:get_argument_value(486)) 							-- CAUTION LIGHT PANEL (R_HYD_RES) 
			--table.insert(flightData,"487=" .. mp:get_argument_value(487)) 							-- CAUTION LIGHT PANEL (OXY_LOW) 
			--table.insert(flightData,"488=" .. mp:get_argument_value(488)) 							-- CAUTION LIGHT PANEL (ELEV_DISENG) 
			--table.insert(flightData,"489=" .. mp:get_argument_value(489)) 							-- CAUTION LIGHT PANEL (VOID1) 
			--table.insert(flightData,"490=" .. mp:get_argument_value(490)) 							-- CAUTION LIGHT PANEL (SEAT_NOT_ARMED)
			--table.insert(flightData,"491=" .. mp:get_argument_value(491)) 							-- CAUTION LIGHT PANEL (BLEED_AIR_LEAK) 
			--table.insert(flightData,"492=" .. mp:get_argument_value(492)) 							-- CAUTION LIGHT PANEL (AIL_DISENG) 
			--table.insert(flightData,"493=" .. mp:get_argument_value(493)) 							-- CAUTION LIGHT PANEL (L_AIL_TAB) 
			--table.insert(flightData,"494=" .. mp:get_argument_value(494)) 							-- CAUTION LIGHT PANEL (R_AIL_TAB) 
			--table.insert(flightData,"495=" .. mp:get_argument_value(495)) 							-- CAUTION LIGHT PANEL (SERVICE_AIR_HOT)
			--table.insert(flightData,"496=" .. mp:get_argument_value(496))								-- CAUTION LIGHT PANEL (PITCH_SAS)
			--table.insert(flightData,"497=" .. mp:get_argument_value(497)) 							-- CAUTION LIGHT PANEL (L_ENG_HOT)
			--table.insert(flightData,"498=" .. mp:get_argument_value(498)) 							-- CAUTION LIGHT PANEL (R_ENG_HOT)
			--table.insert(flightData,"499=" .. mp:get_argument_value(499)) 							-- CAUTION LIGHT PANEL (WINDSHIELD_HOT)
			--table.insert(flightData,"500=" .. mp:get_argument_value(500)) 							-- CAUTION LIGHT PANEL (YAW_SAS) 
			--table.insert(flightData,"501=" .. mp:get_argument_value(501)) 							-- CAUTION LIGHT PANEL (L_ENG_OIL_PRESS)
			--table.insert(flightData,"502=" .. mp:get_argument_value(502)) 							-- CAUTION LIGHT PANEL (R_ENG_OIL_PRESS)
			--table.insert(flightData,"503=" .. mp:get_argument_value(503)) 							-- CAUTION LIGHT PANEL (CICU)
			--table.insert(flightData,"504=" .. mp:get_argument_value(504)) 							-- CAUTION LIGHT PANEL (GCAS)
			--table.insert(flightData,"505=" .. mp:get_argument_value(505)) 							-- CAUTION LIGHT PANEL (L_MAIN_PUMP) 
			--table.insert(flightData,"506=" .. mp:get_argument_value(506)) 							-- CAUTION LIGHT PANEL (R_MAIN_PUMP)
			--table.insert(flightData,"507=" .. mp:get_argument_value(507)) 							-- CAUTION LIGHT PANEL (VOID2)
			--table.insert(flightData,"508=" .. mp:get_argument_value(508)) 							-- CAUTION LIGHT PANEL (LASTE)
			--table.insert(flightData,"509=" .. mp:get_argument_value(509)) 							-- CAUTION LIGHT PANEL (L_WING_PUMP)
			--table.insert(flightData,"510=" .. mp:get_argument_value(510)) 							-- CAUTION LIGHT PANEL (R_WING_PUMP) 
			--table.insert(flightData,"511=" .. mp:get_argument_value(511))								-- CAUTION LIGHT PANEL (HARS) 
			--table.insert(flightData,"512=" .. mp:get_argument_value(512)) 							-- CAUTION LIGHT PANEL (IFF_MODE_4) 
			--table.insert(flightData,"513=" .. mp:get_argument_value(513)) 							-- CAUTION LIGHT PANEL (L_MAIN_FUEL_LOW) 
			--table.insert(flightData,"514=" .. mp:get_argument_value(514)) 							-- CAUTION LIGHT PANEL (R_MAIN_FUEL_LOW) 
			--table.insert(flightData,"515=" .. mp:get_argument_value(515)) 							-- CAUTION LIGHT PANEL (L_R_TKS_UNEQUAL)
			--table.insert(flightData,"516=" .. mp:get_argument_value(516)) 							-- CAUTION LIGHT PANEL (EAC)
			--table.insert(flightData,"517=" .. mp:get_argument_value(517)) 							-- CAUTION LIGHT PANEL (L_FUEL_PRESS)
			--table.insert(flightData,"518=" .. mp:get_argument_value(518))								-- CAUTION LIGHT PANEL (R_FUEL_PRESS)
			--table.insert(flightData,"519=" .. mp:get_argument_value(519)) 							-- CAUTION LIGHT PANEL (NAV)
			--table.insert(flightData,"520=" .. mp:get_argument_value(520)) 							-- CAUTION LIGHT PANEL (STALL_SYS)
			--table.insert(flightData,"521=" .. mp:get_argument_value(521)) 							-- CAUTION LIGHT PANEL (L_CONV) 
			--table.insert(flightData,"522=" .. mp:get_argument_value(522)) 							-- CAUTION LIGHT PANEL (R_CONV) 
			--table.insert(flightData,"523=" .. mp:get_argument_value(523)) 							-- CAUTION LIGHT PANEL (CADC) 
			--table.insert(flightData,"524=" .. mp:get_argument_value(524)) 							-- CAUTION LIGHT PANEL (APU_GEN) 
			--table.insert(flightData,"525=" .. mp:get_argument_value(525)) 							-- CAUTION LIGHT PANEL (L_GEN) 
			--table.insert(flightData,"526=" .. mp:get_argument_value(526)) 							-- CAUTION LIGHT PANEL (R_GEN) 
			--table.insert(flightData,"527=" .. mp:get_argument_value(527)) 							-- CAUTION LIGHT PANEL (INST_INV) 

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
			--table.insert(flightData,"663=" .. mp:get_argument_value(663)) 							-- NOSEWHEEL_STEERING
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
			--table.insert(flightData,"662=" .. mp:get_argument_value(662)) 							-- GUN_READY
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
	
	--*********************************************************************** EXPORT SECTION FOR P-51D ***********************************************************************
	if selfData and selfData.Name == "P-51D" then	

		local mp = GetDevice(0)
		if mp then		
		
			-- Update Data
			---------------------------------------------------
			mp:update_arguments()
		
			-- Lat/Lon/Alt
			-------------------------------------------------------
			table.insert(flightData,"XXX=" .. LoGetSelfData().LatLongAlt.Lat)			-- Lat
			table.insert(flightData,"XXX=" .. LoGetSelfData().LatLongAlt.Long)			-- Long
			table.insert(flightData,"XXX=" .. LoGetSelfData().LatLongAlt.Alt)			-- Alt
		
			-- Canopy
			---------------------------------------------------
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(162)))			-- Canopy_Trucks {0,1}{0,1}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(163)))			-- Canopy_Visibility {0,1}{0,1}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(147)))			-- Canopy_Crank {0.0,1.0}{0.0,1.0}

			-- Stick
			---------------------------------------------------
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(50)))			-- StickPitch {-1,1}{1,-1}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(51)))			-- StickBank {-1,1}{1,-1}

			-- RudderPedals
			---------------------------------------------------
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(54)))			-- RudderPedals {-1,1}{-1,1}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(55)))			-- Left_Wheel_Brake {0,1}{0,1}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(56)))			-- Right_Wheel_Brake {0,1}{0,1}

			--Throttle
			---------------------------------------------------
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(43)))			-- Throttle.input {0,1}{0,1}

			--Propeller control
			---------------------------------------------------
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(46))) 			-- PropellerRPM {0,1}{0,1}

			-- Flight Instruments
			---------------------------------------------------
			table.insert(flightData,"ASI00=" .. (mp:get_argument_value(11))* 1000)	-- AirspeedNeedle {0,50,100,150,200,250,300,350,400,450,500,550,600,650,700}{0.0,0.05,0.10,0.15,0.2,0.25,0.3,0.35,0.4,0.45,0.5,0.55,0.6,0.65,0.7}
			table.insert(flightData,"VVI00=" .. (mp:get_argument_value(29))* 10000) -- Variometer {-6000,-4000,-2000,2000,4000,6000}{-0.6,-0.4,-0.2,0.2,0.4,0.6}

			-- Altimeter
			---------------------------------------------------
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(96))) 			-- Altimeter_10000_footPtr {0.0,100000.0}{0.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(24)))			-- Altimeter_1000_footPtr {0.0,10000.0}{0.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(25)))			-- Altimeter_100_footPtr {0.0,1000.0}{0.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(97)))			-- Altimeter_Pressure {28.1,31.0}{0.0,1.0}

			-- Artificial Horizon
			---------------------------------------------------
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(15)))			-- AHorizon_Pitch {-math.pi/3.0,math.pi/3.0}{1.0,-1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(14)))			-- AHorizon_Bank {-math.pi,math.pi}{1.0,-1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(16)))			-- AHorizon_PitchShift {-10.0 * math.pi/180.0,10.0 * math.pi/180.0}{-1.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(20)))			-- AHorizon_Caged {0.0,1.0}{0.0,1.0}

			-- Directional Gyro
			---------------------------------------------------
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(12)))			-- GyroHeading {0.0,2.0 * math.pi}{0.0,1.0}

			-- Turn Indicator
			---------------------------------------------------
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(27)))			-- TurnNeedle {-3.0 * math.pi/180.0,3.0 * math.pi/180.0}{-1.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(28)))			-- Slipball {-1.0,1.0}{-1.0,1.0}

			-- Oxygen Pressure Indicator
			---------------------------------------------------
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(34)))			-- Oxygen_Pressure {0.0,500.0}{0.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(33)))			-- Oxygen_Flow_Blinker {0.0,1.0}{0.0,1.0}

			-- Fuel System
			---------------------------------------------------
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(155)))			-- Fuel_Tank_Left US GAL {0.0,5.0,15.0,30.0,45.0,60.0,75.0,92.0}{0.0,0.2,0.36,0.52,0.65,0.77,0.92,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(156)))			-- Fuel_Tank_Right US GAL {0.0,5.0,15.0,30.0,45.0,60.0,75.0,92.0}{0.0,0.2,0.36,0.52,0.65,0.77,0.92,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(160)))			-- Fuel_Tank_Fuselage US GAL {0.0,10.0,20.0,30.0,40.0,50.0,60.0,70.0,80.0,85.0}{0.0,0.12,0.28,0.40,0.51,0.62,0.72,0.83,0.96,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(32)))			-- Fuel_Pressure PSI {0.0,25.0}{0.0,1.0}

			-- A-11 clock
			---------------------------------------------------
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(4)))			-- CLOCK_currtime_hours {0.0,12.0}{0.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(5)))			-- CLOCK_currtime_minutes {0.0,60.0}{0.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(6)))			-- CLOCK_currtime_seconds {0.0,60.0}{0.0,1.0}

			-- AN5730 remote compass
			---------------------------------------------------
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(1)))			-- CompassHeading {0.0,math.pi * 2.0}{0.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(2)))			-- CommandedCourse {0.0,math.pi * 2.0}{0.0,1.0}

			-- K-14 gunsight
			---------------------------------------------------
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(36))) 			-- RangeGraticule {600.0,700.0,800.0,900.0,1000.0,1100.0,1200.0,1300.0,1400.0,1500.0,1600.0,1700.0,1800.0,1900.0,2000.0,2100.0,2200.0,2300.0,2400.0}{0.0,0.102,0.189,0.264,0.335,0.392,0.449,0.506,0.563,0.62,0.667,0.71,0.757,0.80,0.844,0.885,0.923,0.961,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(45)))			-- ThrottleTwistGrip {600.0,700.0,800.0,900.0,1000.0,1100.0,1200.0,1300.0,1400.0,1500.0,1600.0,1700.0,1800.0,1900.0,2000.0,2100.0,2200.0,2300.0,2400.0}{0.0,0.102,0.189,0.264,0.335,0.392,0.449,0.506,0.563,0.62,0.667,0.71,0.757,0.80,0.844,0.885,0.923,0.961,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(161)))			-- TailRadarWarning {0.0,1.0}{0.0,1.0}

			--SCR-522A Control panel 
			---------------------------------------------------
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(122)))			-- A_channel_light {0.0,1.0}{0.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(123)))			-- B_channel_light {0.0,1.0}{0.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(124)))			-- C_channel_light {0.0,1.0}{0.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(125)))			-- D_channel_light {0.0,1.0}{0.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(126)))			-- Transmit_light {0.0,1.0}{0.0,1.0}

			-- Hydraulic Pressure
			-------------------------------------------------------
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(78)))			-- Hydraulic_Pressure PSI{0.0,2000.0}{0.0,1.0}
		
			-- Landing Gears
			-------------------------------------------------------
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(150)))			-- Landing_Gear_Handle {0.0,1.0}{0.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(151)))			-- Landing_Gear_Handle_Indoor {0.0,1.0}{0.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(80)))			-- LandingGearGreenLight {0.0,1.0}{0.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(82)))			-- LandingGearRedLight {0.0,1.0}{0.0,1.0}

			-- RPM / Manifold Eng Indicators
			-------------------------------------------------------
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(10)))			-- Manifold_Pressure {10.0,75.0}{0.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(23)))			-- Engine_RPM {0.0,4500.0}{0.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(9)))			-- Vacuum_Suction {0.0,10.0}{0.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(21)))			-- Carb_Temperature {-80.0,150.0}{-0.533,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(22)))			-- Coolant_Temperature {-80.0,150.0}{-0.533,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(30)))			-- Oil_Temperature.input {0.0,100.0}{0,1.0}

			-- Accelerometer
			-------------------------------------------------------
			table.insert(flightData,"ACC00=" .. (mp:get_argument_value(175)))		-- Accelerometer_main {-5.0,12.0}{0.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(177)))			-- Accelerometer_min {-5.0,12.0}{0.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(178)))			-- Accelerometer_max {-5.0,12.0}{0.0,1.0}

			-- Other
			-------------------------------------------------------
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(77)))			-- Rocket_Counter {0.0,1.0}{0.0, 1.0 - 1.0/12.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(164)))			-- Left_Fluor_Light {0.0,1.0}{0.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(165)))			-- Right_Fluor_Light {0.0,1.0}{0.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(59)))			-- Hight_Blower_Lamp {0.0,1.0}{0.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(170)))			-- Aileron_Trimmer {-1.0,1.0}{-1.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(172)))			-- Rudder_Trimmer {-1.0,1.0}{-1.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(171)))			-- Elevator_Trimmer {-1.0,1.0}{-0.42,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(174)))			-- Control_Lock_Bracket {0.0,1.0}{0.0,1.0}
			table.insert(flightData,"XXX=" .. (mp:get_argument_value(101)))			-- Ammeter {0.0,150.0}{0.0,1.0}

		end

	end
	
	-- TX Data
	---------------------------------------------------
	for i = 1, #flightData do packet = packet .. flightData[i] .. ";" end
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
	dofile("C:\\Users\\admin\\Saved Games\\DCS\\Scripts\\soic_conv_Stop.lua")
	
	-- Move Simmeters Dials back to Zero Pos
	local endPacket = ""
	local endFlightData = {}
	table.insert(endFlightData,"ASI00=0")
	table.insert(endFlightData,"ALT00=0")
	table.insert(endFlightData,"VVI00=0")
		for i = 1, #endFlightData do endPacket = endPacket .. endFlightData[i] .. ";" end
	socket.try(con:send(endPacket))
	socket.try(con:close())
	
	-- 20141214a Added shutdown for Stepper Gauges
	Fuel_packet = "S0:0:0:0"
	Fuel_socket.try(Fuel_con:send(Fuel_packet))
	-- 20141215a Added shutdown for Fuel OLED
	Fuel_packet = "   00000    "
	Fuel_Display_socket.try(Fuel_Display_con:send(Fuel_packet))
	-- 20150221c Added shutdown for Compass
	Compass_Packet = "S0:0"
	Compass_socket.try(Compass_con:send(Compass_Packet))	

	-- 20150228 Added Shutdown for Clock Digits
	Clock_Digits_Packet = "S0:0"
	Clock_Digits_socket.try(Clock_Digits_con:send(Clock_Digits_Packet))
	
	-- 20150228 Added Shutdown for general gauges	
	General_Stepper_Packet = "S3120:0"
	General_Stepper_socket.try(General_Stepper_con:send(General_Stepper_Packet))		
	
end