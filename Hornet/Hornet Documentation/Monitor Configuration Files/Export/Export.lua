local wwtlfs=require('lfs')
dofile(wwtlfs.writedir()..'Scripts/wwt/wwtExport.lua')

dofile(lfs.writedir()..[[Scripts\DCS-BIOS\BIOS.lua]])

dofile(lfs.writedir()..[[Scripts\Helios\HeliosExport16.lua]])
