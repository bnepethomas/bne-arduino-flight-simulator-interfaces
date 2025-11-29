echo Copy Configuration files
echo Had to change file attributes from read only and change permissions to mods sub directories
echo CDU
del "C:\Program Files\Eagle Dynamics\DCS World\Mods\aircraft\A-10C\Cockpit\Scripts\CDU\indicator\CDU_init.lua"
copy .\CDU_init.lua "C:\Program Files\Eagle Dynamics\DCS World\Mods\aircraft\A-10C\Cockpit\Scripts\CDU\indicator"
echo ALR69V
del "C:\Program Files\Eagle Dynamics\DCS World\Mods\aircraft\A-10C\Cockpit\Scripts\AN_ALR69V\indicator" /F /Q
copy .\AN_ALR69V_init.lua  "C:\Program Files\Eagle Dynamics\DCS World\Mods\aircraft\A-10C\Cockpit\Scripts\AN_ALR69V\indicator\AN_ALR69V_init.lua"
echo CSMC
del "C:\Program Files\Eagle Dynamics\DCS World\Mods\aircraft\A-10C\Cockpit\Scripts\CMSC\indicator\CMSC_init.lua" /F /Q
copy .\CMSC_init.lua "C:\Program Files\Eagle Dynamics\DCS World\Mods\aircraft\A-10C\Cockpit\Scripts\CMSC\indicator"
echo CMSP
del "C:\Program Files\Eagle Dynamics\DCS World\Mods\aircraft\A-10C\Cockpit\Scripts\CMSP\indicator\CMSP_init.lua"
copy .\CMSP_init.lua "C:\Program Files\Eagle Dynamics\DCS World\Mods\aircraft\A-10C\Cockpit\Scripts\CMSP\indicator"
pause