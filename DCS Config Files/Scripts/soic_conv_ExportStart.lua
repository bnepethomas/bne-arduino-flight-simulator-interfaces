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
	
	
	Fuel_Port = 13135
	Fuel_Host = "192.168.1.105"
	Fuel_socket = require("socket")
	Fuel_con = socket.try(Fuel_socket.udp())
	Fuel_socket.try(Fuel_con:settimeout(.001))
	Fuel_socket.try(Fuel_con:setpeername(Fuel_Host,Fuel_Port))	

	Fuel_Display_Port = 13135
	Fuel_Display_Host = "192.168.1.106"
	Fuel_Display_socket = require("socket")
	Fuel_Display_con = socket.try(Fuel_Display_socket.udp())
	Fuel_Display_socket.try(Fuel_Display_con:settimeout(.001))
	Fuel_Display_socket.try(Fuel_Display_con:setpeername(Fuel_Display_Host,Fuel_Display_Port))	
	
	-- For both Compass and Analog Clock Hands
	Compass_Port = 13135
	Compass_Host = "192.168.1.107"
	Compass_socket = require("socket")
	Compass_con = socket.try(Compass_socket.udp())
	Compass_socket.try(Compass_con:settimeout(.001))
	Compass_socket.try(Compass_con:setpeername(Compass_Host,Compass_Port))	
	
	Clock_Digits_Port = 13135
	Clock_Digits_Host = "192.168.1.108"
	Clock_Digits_socket = require("socket")
	Clock_Digits_con = socket.try(Clock_Digits_socket.udp())
	Clock_Digits_socket.try(Clock_Digits_con:settimeout(.001))
	Clock_Digits_socket.try(Clock_Digits_con:setpeername(Clock_Digits_Host,Clock_Digits_Port))	