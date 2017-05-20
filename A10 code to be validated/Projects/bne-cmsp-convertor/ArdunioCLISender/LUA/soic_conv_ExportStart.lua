-- SSOIC_CONV_EXPORTSTART.LUA
--
-- Called from export.lua
-- Declares and sets up variables
	soic_conv_port = 7777
	soic_conv_host = "127.0.0.1"
	soic_conv_socket = require("socket")
	soic_conv_con = socket.try(soic_conv_socket.udp())
	soic_conv_socket.try(soic_conv_con:settimeout(.001))
	soic_conv_socket.try(soic_conv_con:setpeername(soic_conv_host,soic_conv_port))

	arduino_display_port = 7781
	arduino_display_host = "127.0.0.1"
	arduino_display_socket = require("socket")
	arduino_display_con = socket.try(arduino_display_socket.udp())
	arduino_display_socket.try(arduino_display_con:settimeout(.001))
	arduino_display_socket.try(arduino_display_con:setpeername(arduino_display_host,arduino_display_port))	
	
	cdu_receive_port = 7780
	c_socket = require("socket")
	cdu_socket = c_socket.udp()
	cdu_socket:setsockname("*",7780)
	cdu_socket:setoption('broadcast',true)
	cdu_socket:settimeout(.001) -- set timeout for reading socket
	
