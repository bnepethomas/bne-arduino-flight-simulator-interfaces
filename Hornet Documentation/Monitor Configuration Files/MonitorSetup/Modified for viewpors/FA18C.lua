--***DO NOT MODIFY THESE COMMENT LINES***
--A10CVirtualCockpitServer v0.0.0.0
--LOCATION Config\MonitorSetup
--HOOKTO
--Monitor lua
-- 6488*1080
_  = function(p) return p; end;
name = _('Ben_Pit'); -- ADD YOUR PIT NAME HERE - DCS WILL SHOW THIS IN THE MENU
Description = 'Setup for f18 Virtual Cockpit Server MFCD and CDU Exports.'
Viewports =
{
     Center =
     {
          x = 0;
          y = 0;
          width = 1920;
          height = 1200;
          viewDx = 0;
          viewDy = 0;
          aspect = 1.7777777777777777777777777778;
     }
}

CENTER_MFCD = 
{
    x = 2960;
    y = 450;
    width = 600;
    height = 600;
}

LEFT_MFCD = 
{
    x = 1920;
    y = 45;
    width = 570;
    height = 570;
}

RIGHT_MFCD = 

{
    x = 2380;
    y = 658;
    width = 570;
    height = 570;
}
F18_RWR = 
{
    x = 4785;
    y = 30;
    width = 520;
    height = 520;
}
F18_UFC =
{
    x = 3800;
    y = 25;
    width = 2;
    height = 2;
}

F18_IFEI =
{
    x = 3810;
    y = 15;
    width = 2;
    height = 2;
}


--CDU_EXPORT

UIMainView = Viewports.Center