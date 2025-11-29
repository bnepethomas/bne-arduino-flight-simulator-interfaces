--- DO NOT EDIT.
--- This file is for reference purposes only
--- All user modifications should go to $HOME\Saved Games\DCS\Scripts\Export.lua

-- Data export script for DCS, version 1.2.
-- Copyright (C) 2006-2014, Eagle Dynamics.
-- See http://www.lua.org for Lua script system info 
-- We recommend to use the LuaSocket addon (http://www.tecgraf.puc-rio.br/luasocket) 
-- to use standard network protocols in Lua scripts.
-- LuaSocket 2.0 files (*.dll and *.lua) are supplied in the Scripts/LuaSocket folder
-- and in the installation folder of the DCS. 

-- Expand the functionality of following functions for your external application needs.
-- Look into Saved Games\DCS\Logs\dcs.log for this script errors, please.

--[[	
-- Uncomment if using Vector class from the Scripts\Vector.lua file 
local lfs = require('lfs')
LUA_PATH = "?;?.lua;"..lfs.currentdir().."/Scripts/?.lua"
require 'Vector'
-- See the Scripts\Vector.lua file for Vector class details, please.
--]]



default_output_file = nil

function StrSplit(str, delim, maxNb)

    -- Called from Process Input from CDU

    -- Eliminate bad cases...
    if string.find(str, delim) == nil then
        return { str }
    end

    if maxNb == nil or maxNb < 1 then
       maxNb = 0    -- No limit
    end

    local result = {}
    local pat = "(.-)" .. delim .. "()"
    local nb = 0
    local lastPos

    for part, pos in string.gfind(str, pat) do
        nb = nb + 1
        result[nb] = part

        lastPos = pos

        if nb == maxNb then break end

    end

    -- Handle the last field

    if nb ~= maxNb then
        result[nb + 1] = string.sub(str, lastPos)
    end

    return result
end


function LuaExportStart()
-- Works once just before mission start.
-- Make initializations of your files or connections here.
-- For example:
-- 1) File
  default_output_file = io.open(lfs.writedir().."/Logs/PTExport.log", "w")
-- 2) Socket
   package.path  = package.path..";"..lfs.currentdir().."/LuaSocket/?.lua"
   package.cpath = package.cpath..";"..lfs.currentdir().."/LuaSocket/?.dll"
   socket = require("socket")
--  host = host or "localhost"
--  port = port or 8080
--  c = socket.try(socket.connect(host, port)) -- connect to the listener socket
--  c:setoption("tcp-nodelay",true) -- set immediate transmission mode
-- 
	local version = LoGetVersionInfo() --request current version info (as it showed by Windows Explorer fo DCS.exe properties)
	if version and default_output_file then
		
		default_output_file:write("ProductName: "..version.ProductName..'\n')
		default_output_file:write(string.format("FileVersion: %d.%d.%d.%d\n",
												version.FileVersion[1],
												version.FileVersion[2],
												version.FileVersion[3],
												version.FileVersion[4]))
		default_output_file:write(string.format("ProductVersion: %d.%d.%d.%d\n",
												version.ProductVersion[1],
												version.ProductVersion[2],
												version.ProductVersion[3],  -- head  revision (Continuously growth)
												version.ProductVersion[4])) -- build number   (Continuously growth)	
	end

	-- Using Same Receive port as XPlane 
	cdu_receive_port = 49000
	c_socket = require("socket")
	cdu_socket = c_socket.udp()
	cdu_socket:setsockname("*",49000)
	cdu_socket:setoption('broadcast',true)
	cdu_socket:settimeout(.001) -- set timeout for reading socket
	
	
	package.path  = package.path..";.\\LuaSocket\\?.lua"
	package.cpath = package.cpath..";.\\LuaSocket\\?.dll"
	  
	socket = require("socket")

	gps_export_port = 13136
	gps_export_host = "172.16.1.2"
	gps_export_socket = require("socket")
	gps_export_con = socket.try(gps_export_socket.udp())
	gps_export_socket.try(gps_export_con:settimeout(.001))
	gps_export_socket.try(gps_export_con:setpeername(gps_export_host,gps_export_port))
	
	

end


function LuaExportStop()
-- Works once just after mission stop.
-- Close files and/or connections here.
-- 1) File
   if default_output_file then
		default_output_file:write("Closing: "..'\n')
		default_output_file:close()
		default_output_file = nil
   end
-- 2) Socket
--	socket.try(c:send("quit")) -- to close the listener socket
--	c:close()
end

function LuaExportBeforeNextFrame()
-- Works just before every simulation frame.

-- Call Lo*() functions to set data to Lock On here
-- For example:
--	LoSetCommand(3, 0.25) -- rudder 0.25 right 
--	LoSetCommand(64) -- increase thrust


-- PT
	local lInput = cdu_socket:receive()
	local lCommand, lCommandArgs, lDevice, lArgument, lLastValue

-- Definitions from commands_def - hopefully reasonally generic	


	if lInput then
	
		lCommand = string.sub(lInput,1,1)
			
		if lCommand == "R" then
				ResetChangeValues()
		end
			
		if (lCommand == "C") then

			-- Using Panel Specific commands from clickabledata.lua
			lCommandArgs = StrSplit(string.sub(lInput,2),",")
			lDevice = GetDevice(lCommandArgs[1])
			lLastValue = tostring(lDevice)
			if type(lDevice) == "table" then
				lDevice:performClickableAction(lCommandArgs[2],lCommandArgs[3])
			end
		end
		
		-- If there is not a specific panel to use (eg HOTAS then use commands from command_defs.lua)
		if (lCommand == "P") then


			-- Plane_HOTAS_SpeedBrakeSwitchForward = 577,
			-- Plane_HOTAS_SpeedBrakeSwitchAft = 578,
			-- Plane_HOTAS_SpeedBrakeSwitchCenter = 579,

			--default_output_file:write("Got to P start: "..'\n')
			-- data is PXXX where XXX is command integer from command_defs
			lCommandArgs = StrSplit(string.sub(lInput,2),",")
			LoSetCommand(lCommandArgs[1])
			--default_output_file:write("Got to P end: "..'\n')
			
		end
		
	end
end

function LuaExportActivityNextEvent(t)

	local tNext = t
	local gps_export_flightData = {}

			

	table.insert(gps_export_flightData,"latitude:"..LoGetSelfData().LatLongAlt.Lat)       -- LATITUDE

	table.insert(gps_export_flightData,"longitude:"..LoGetSelfData().LatLongAlt.Long)      -- LONGITUDE

	table.insert(gps_export_flightData,"altitude:"..LoGetAltitudeAboveSeaLevel())          -- ALTITUDE SEA LEVEL(MTS TO FT)

	table.insert(gps_export_flightData,"airspeed:"..LoGetTrueAirSpeed() * 1.94)            -- TRUE AIRSPEED (M/S TO KNOTS)

	table.insert(gps_export_flightData,"magheading:"..LoGetSelfData().Heading * 57.3)         -- HEADING (RAD TO DEG)

	gps_export_packet =  gps_export_flightData[1] .. "," ..          
	gps_export_flightData[2] .. "," .. gps_export_flightData[3] .. ","
	gps_export_packet =  gps_export_packet .. gps_export_flightData[4] .. "," ..    
	gps_export_flightData[5]  

	gps_export_socket.try(gps_export_con:send(gps_export_packet))

	tNext = tNext + 0.5 --repeat every 1/2 second

	return tNext
end
