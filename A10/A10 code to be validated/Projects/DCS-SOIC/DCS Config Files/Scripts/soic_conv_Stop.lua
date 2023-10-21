-- SSOIC_CONV_STOP
--
-- Called from export.lua
-- Closes UDP socket
	soic_conv_socket.try(soic_conv_con:close())
