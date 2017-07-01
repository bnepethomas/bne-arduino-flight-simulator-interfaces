	-- Variable Declarations
	
	-- PT GPS Start
	gps_export_port = 7783
	gps_export_host = "127.0.0.1"
	gps_export_socket = require("socket")
	gps_export_con = socket.try(gps_export_socket.udp())
	gps_export_socket.try(gps_export_con:settimeout(.001))
	gps_export_socket.try(gps_export_con:setpeername(gps_export_host,gps_export_port))
	-- PT GPS End
	
	-- PT SOIC Start
	soic_export_port = 7777
	soic_export_host = "127.0.0.1"
	soic_export_socket = require("socket")
	soic_export_con = socket.try(soic_export_socket.udp())
	soic_export_socket.try(soic_export_con:settimeout(.001))
	soic_export_socket.try(soic_export_con:setpeername(soic_export_host,soic_export_port))
	-- PT SOIC End
	
	-- PT Huey Caution Start
	Huey_Caution_export_port = 7784
	Huey_Caution_export_host = "192.168.1.109"
	Huey_Caution_export_socket = require("socket")
	Huey_Caution_export_con = socket.try(Huey_Caution_export_socket.udp())
	Huey_Caution_export_socket.try(Huey_Caution_export_con:settimeout(.001))
	Huey_Caution_export_socket.try(Huey_Caution_export_con:setpeername(Huey_Caution_export_host,Huey_Caution_export_port))
	-- PT Huey Caution End