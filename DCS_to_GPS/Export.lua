
package.path  = package.path..";.\\LuaSocket\\?.lua"
package.cpath = package.cpath..";.\\LuaSocket\\?.dll"
  
socket = require("socket")

gps_export_port = 7783
gps_export_host = "127.0.0.1"
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
	
	
	local gps_export_flightData = {}
	table.insert(gps_export_flightData,"Latitude="..LoGetSelfData().LatLongAlt.Lat)       -- LATITUDE
    table.insert(gps_export_flightData,"Longitude="..LoGetSelfData().LatLongAlt.Long)      -- LONGITUDE
    table.insert(gps_export_flightData,"Altitude="..LoGetAltitudeAboveSeaLevel())          -- ALTITUDE SEA LEVEL(MTS TO FT)
    table.insert(gps_export_flightData,"Airspeed="..LoGetTrueAirSpeed() * 1.94)            -- TRUE AIRSPEED (M/S TO KNOTS)
    table.insert(gps_export_flightData,"Heading="..LoGetSelfData().Heading * 57.3)         -- HEADING (RAD TO DEG)
	gps_export_packet =  gps_export_flightData[1] .. ":" .. gps_export_flightData[2] .. ":" .. gps_export_flightData[3] .. ":"
	gps_export_packet =  gps_export_packet .. gps_export_flightData[4] .. ":" .. gps_export_flightData[5] .. ":" 
	gps_export_socket.try(gps_export_con:send(gps_export_packet))

	
end


