   
   --PT Export
   
   -- PT Start GPS
    local gps_export_flightData = {}
    table.insert(gps_export_flightData,"Latitude="..LoGetSelfData().LatLongAlt.Lat)        -- LATITUDE
    table.insert(gps_export_flightData,"Longitude="..LoGetSelfData().LatLongAlt.Long)      -- LONGITUDE
    table.insert(gps_export_flightData,"Altitude="..LoGetAltitudeAboveSeaLevel())          -- ALTITUDE SEA LEVEL(MTS TO FT)
    table.insert(gps_export_flightData,"Airspeed="..LoGetTrueAirSpeed() * 1.94)            -- TRUE AIRSPEED (M/S TO KNOTS)
    table.insert(gps_export_flightData,"Heading="..LoGetSelfData().Heading * 57.3)         -- HEADING (RAD TO DEG)
   gps_export_packet =  gps_export_flightData[1] .. ":" .. gps_export_flightData[2] .. ":" .. gps_export_flightData[3] .. ":"
   gps_export_packet =  gps_export_packet .. gps_export_flightData[4] .. ":" .. gps_export_flightData[5] .. ":" 
   gps_export_socket.try(gps_export_con:send(gps_export_packet))
   -- PT End GPS

   
   -- PT Start SOIC
    local soic_export_flightData = {}
   
    local lSOICDevice = GetDevice(0)
    lSOICDevice:update_arguments()
   
    table.insert(soic_export_flightData,"100=" .. lSOICDevice:get_argument_value(56))      -- Com1 TX
    table.insert(soic_export_flightData,"101=" .. lSOICDevice:get_argument_value(56))      -- Com2 TX
    table.insert(soic_export_flightData,"102=" .. lSOICDevice:get_argument_value(56))      -- Com RX Both
    table.insert(soic_export_flightData,"103=" .. lSOICDevice:get_argument_value(56))      -- Nav 1 Sound
    table.insert(soic_export_flightData,"104=" .. lSOICDevice:get_argument_value(56))      -- Nav 2 Sound  
    table.insert(soic_export_flightData,"105=" .. lSOICDevice:get_argument_value(56))      -- Marker Sound
    table.insert(soic_export_flightData,"106=" .. lSOICDevice:get_argument_value(56))      -- DME Sound    
    table.insert(soic_export_flightData,"107=" .. lSOICDevice:get_argument_value(56))      -- ADF 1 Sound
     
    table.insert(soic_export_flightData,"110=" .. lSOICDevice:get_argument_value(56))       -- Inner Marker 
    table.insert(soic_export_flightData,"111=" .. lSOICDevice:get_argument_value(56))       -- Middle Marker
    table.insert(soic_export_flightData,"112=" .. lSOICDevice:get_argument_value(56))       -- Outer Marker
   
    table.insert(soic_export_flightData,"120=" .. lSOICDevice:get_argument_value(277))      -- Master Caution (Engine Out)
    table.insert(soic_export_flightData,"121=" .. lSOICDevice:get_argument_value(276))      -- Rotor Low
    table.insert(soic_export_flightData,"122=" .. lSOICDevice:get_argument_value(98))       -- 20 Minute Fuel (Fuel Low)
    table.insert(soic_export_flightData,"123=" .. lSOICDevice:get_argument_value(106))      -- Inst Inverter (Generator Fail)
    table.insert(soic_export_flightData,"124=" .. lSOICDevice:get_argument_value(107))      -- DC Generator (Battery Warm)
    table.insert(soic_export_flightData,"125=" .. lSOICDevice:get_argument_value(108))      -- External Power (Battery Hot)
    table.insert(soic_export_flightData,"126=" .. lSOICDevice:get_argument_value(98))       -- Fuel Pump Fail      
    table.insert(soic_export_flightData,"127=" .. lSOICDevice:get_argument_value(102))      -- Transmission Pressure Fail
    table.insert(soic_export_flightData,"128=" .. lSOICDevice:get_argument_value(103))      -- Transmission Temperature Fail     
    table.insert(soic_export_flightData,"129=" .. lSOICDevice:get_argument_value(94))       -- Engine Chip      
    table.insert(soic_export_flightData,"130=" .. lSOICDevice:get_argument_value(109))      -- Chip Detector (Tail Rotor Chip)
    table.insert(soic_export_flightData,"131=" .. lSOICDevice:get_argument_value(109))      -- Chip Detector (TransmissionChip)
    table.insert(soic_export_flightData,"132=" .. lSOICDevice:get_argument_value(99))       -- Fuel Filter Fail      
    table.insert(soic_export_flightData,"133=" .. lSOICDevice:get_argument_value(100))      -- Govv Emergency (Baggage Door)
    table.insert(soic_export_flightData,"134=" .. lSOICDevice:get_argument_value(275))      -- Fire (SimConnet Fail)
    table.insert(soic_export_flightData,"135=" .. lSOICDevice:get_argument_value(104))      -- Hyd Pessure (Spare 1)
    table.insert(soic_export_flightData,"136=" .. lSOICDevice:get_argument_value(92))       -- Engine Icing (Spare 2)      
    table.insert(soic_export_flightData,"137=" .. lSOICDevice:get_argument_value(105))      -- Engine Inlet Air (Spare 3)
    table.insert(soic_export_flightData,"138=" .. lSOICDevice:get_argument_value(91))       -- Engine Oil Pressure (Spare 4)  
    table.insert(soic_export_flightData,"139=" .. lSOICDevice:get_argument_value(95))       -- Left Fuel Boost (Spare 5)    

 
   
   soic_export_packet = ""
   for ltmp = 1, 31, 1 do
      soic_export_packet = soic_export_packet .. ":" .. soic_export_flightData[ltmp]
   end
   soic_export_socket.try(soic_export_con:send(soic_export_packet))
   -- PT End GPS