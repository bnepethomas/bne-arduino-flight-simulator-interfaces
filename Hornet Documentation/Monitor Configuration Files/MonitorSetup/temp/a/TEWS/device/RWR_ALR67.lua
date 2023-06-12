dofile(LockOn_Options.common_script_path..'AN_ALR_SymbolsBase.lua')
local gettext = require("i_18n")
_ = gettext.translate

device_timer_dt     = 0.2
MaxThreats          = 16
EmitterLiveTime     = 11
EmitterSoundTime    = 0.5
LaunchSoundDelay    = 15.0

RWR_detection_coeff = 0.85

-- RWR sensors: F/A-18C has four sensors - 2 on nose and 2 in tail
eyes ={}

eyes[1] =
{
    position      = {x = 6.65,y = -0.07,z =  0.4},
    orientation   = {azimuth  = math.rad(45),elevation = math.rad(0.0)},
    field_of_view = math.rad(120) 
}
eyes[2] =
{
    position      = {x = 6.65,y = -0.07,z = -0.4},
    orientation   = {azimuth  = math.rad(-45),elevation = math.rad(0.0)},
    field_of_view = math.rad(120) 
}
eyes[3] =
{
    position      = {x = -6.3,y = 2.37,z =  1.6},
    orientation   = {azimuth  = math.rad(135),elevation = math.rad(0.0)},
    field_of_view = math.rad(120) 
}
eyes[4] =
{
    position      = {x = -6.3,y = 2.37,z = -1.6},
    orientation   = {azimuth  = math.rad(-135),elevation = math.rad(0.0)},
    field_of_view = math.rad(120) 
}


need_to_be_closed = true -- lua_state  will be closed in post_initialize()