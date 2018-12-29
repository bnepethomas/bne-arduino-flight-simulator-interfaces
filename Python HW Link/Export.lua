
package.path  = package.path..";.\\LuaSocket\\?.lua"
package.cpath = package.cpath..";.\\LuaSocket\\?.dll"
  
socket = require("socket")

gps_export_port = 13136
gps_export_host = "192.168.1.134"
gps_export_socket = require("socket")
gps_export_con = socket.try(gps_export_socket.udp())
gps_export_socket.try(gps_export_con:settimeout(.001))
gps_export_socket.try(gps_export_con:setpeername(gps_export_host,gps_export_port))



-- Lua Export Functions
LuaExportStart = function()
	

end

LuaExportStop = function()
	

	
end

function LuaExportBeforeNextFrame()
	

	
end

function LuaExportAfterNextFrame()
	
	

	
end
function LuaExportActivityNextEvent(t)

	local tNext = t
	local gps_export_flightData = {}
	
	table.insert(gps_export_flightData,"latitude:"..LoGetSelfData().LatLongAlt.Lat)       -- LATITUDE
    table.insert(gps_export_flightData,"longitude:"..LoGetSelfData().LatLongAlt.Long)      -- LONGITUDE
    table.insert(gps_export_flightData,"altitude:"..LoGetAltitudeAboveSeaLevel())          -- ALTITUDE SEA LEVEL(MTS TO FT)
    table.insert(gps_export_flightData,"airspeed:"..LoGetTrueAirSpeed() * 1.94)            -- TRUE AIRSPEED (M/S TO KNOTS)
    table.insert(gps_export_flightData,"magheading:"..LoGetSelfData().Heading * 57.3)         -- HEADING (RAD TO DEG)
	gps_export_packet =  gps_export_flightData[1] .. "," .. gps_export_flightData[2] .. "," .. gps_export_flightData[3] .. ","
	gps_export_packet =  gps_export_packet .. gps_export_flightData[4] .. "," .. gps_export_flightData[5]  
	gps_export_socket.try(gps_export_con:send(gps_export_packet))

	tNext = tNext + 0.5 --repeat every 1/2 second
	return tNext
end

