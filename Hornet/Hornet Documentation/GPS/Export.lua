-- run Helios export script supporting drivers generated by Helios Profile Editor and Capt Zeen modules
dofile(lfs.writedir()..[[Scripts\Helios\HeliosExport16.lua]])
dofile(lfs.writedir()..[[Scripts\DCS-BIOS\BIOS.lua]])
-- example calls for other programs you may have:
-- dofile(lfs.writedir()..[[Scripts\SimShaker.lua]])
-- dofile(lfs.writedir()..[[Scripts\SimShaker-export-core\ExportCore.lua]])
-- dofile(lfs.writedir()..[[Scripts\VAICOMPRO\VAICOMPRO.export.lua]])
-- dofile(lfs.writedir()..[[Scripts\TacviewGameExport.lua]])
-- dofile(lfs.writedir()..[[Mods\Services\DCS-SRS\Scripts\DCS-SimpleRadioStandalone.lua]])

package.path  = package.path..";.\\LuaSocket\\?.lua"
package.cpath = package.cpath..";.\\LuaSocket\\?.dll"
  
socket = require("socket")



gps_export_port = 13136
gps_export_host = "192.168.2.30"
gps_export_socket = require("socket")
gps_export_con = socket.try(gps_export_socket.udp())
gps_export_socket.try(gps_export_con:settimeout(.001))
gps_export_socket.try(gps_export_con:setpeername(gps_export_host,gps_export_port))



function LuaExportActivityNextEvent(t)

	local tNext = t

	
	local gps_export_flightData = {}
	-- LATITUDE
	table.insert(gps_export_flightData,"latitude:"..LoGetSelfData().LatLongAlt.Lat) 
	-- LONGITUDE
    table.insert(gps_export_flightData,"longitude:"..LoGetSelfData().LatLongAlt.Long)   
	-- ALTITUDE SEA LEVEL(MTS TO FT)
    table.insert(gps_export_flightData,"altitude:"..LoGetAltitudeAboveSeaLevel())
	-- TRUE AIRSPEED (M/S TO KNOTS)
    table.insert(gps_export_flightData,"airspeed:"..LoGetTrueAirSpeed() * 1.94)  
	-- HEADING (RAD TO DEG)
    table.insert(gps_export_flightData,"magheading:"..LoGetSelfData().Heading * 57.3)         
	gps_export_packet =  gps_export_flightData[1] .. "," .. gps_export_flightData[2] .. "," 
	gps_export_packet =  gps_export_packet .. gps_export_flightData[3] .. ","
	gps_export_packet =  gps_export_packet .. gps_export_flightData[4] .. "," 
	gps_export_packet =  gps_export_packet .. gps_export_flightData[5]
	gps_export_socket.try(gps_export_con:send(gps_export_packet))
	

	tNext = tNext + 1.0

	return tNext

	
end