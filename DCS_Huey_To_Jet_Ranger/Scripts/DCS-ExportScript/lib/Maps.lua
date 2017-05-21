-- Ikarus and D.A.C. Export Script
-- Version 1.0.0 BETA
--
-- Map Config File
--
-- Copyright by Michael aka McMicha 2014
-- Contact dcs2arcaze.micha@farbpigmente.org
--
-- This file contains the data to identify the loaded map.
--
-- The data can be read in the editor. 
-- Load the corresponding map and read the latitude and longitude values and write specify the decimal degree format (convert from degrees, decimal to decimal degrees).
-- Lat1 and Long1 contain the coordinates of the upper left corner.
-- Lat2 and Long2 contain the coordinates of the lower right corner.
--

-- Maps
ExportScript.Maps                    = {}
-- Caucasus Map
ExportScript.Maps.CaucasusBase       = {}
ExportScript.Maps.CaucasusBase.Lat1  = 48.384867  -- high left Latitude 48° 23. 92 N
ExportScript.Maps.CaucasusBase.Long1 = 26.779467  -- high left Longitude 26° 46.768 E
ExportScript.Maps.CaucasusBase.Lat2  = 38.865183  -- low right Latitude 38° 51.911 N
ExportScript.Maps.CaucasusBase.Long2 = 47.14225   -- low right Longitude 47° 8.535 E
-- Nevada (NTTR) Map
ExportScript.Maps.Nevada       = {} -- Nevada Map
ExportScript.Maps.Nevada.Lat1  = 37.578333   -- high left Latitude  37° 34' 42" N
ExportScript.Maps.Nevada.Long1 = -119.964722 -- high left Longitude 119° 57' 53" W
ExportScript.Maps.Nevada.Lat2  = 34.651667   -- low right Latitude 34° 39' 06" N
ExportScript.Maps.Nevada.Long2 = -114.536111 -- low right Longitude 114° 32' 10" W