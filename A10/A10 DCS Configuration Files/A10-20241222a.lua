_  = function(p) return p end
name = _('A10-22')
description = 'Generated from compatible Helios Profiles'
Viewports = {
  P2 = {
    x = 0,
    y = 0,
    width = 1920,
    height = 1080,
    aspect = 1.77777777777778,
    dx = 0,
    dy = 0
  }
}
LEFT_MFCD =
{
     x = 367;
     y = 1151;
     width = 800;
     height = 600;
}
RIGHT_MFCD =
{
     x = 67;
     y = 1872;
     width = 800;
     height = 600;
}
ED_A10C_CMSP =
{
	x = 1;
	y = 1;
	width = 50;
	height = 50;
}
ED_A10C_CMSC =
{
	x = 1;
	y = 51;
	width = 50;
	height = 50;
}
ED_A10C_CDU =
{
	--x = 1264;
	--y = 1088;
	--x = 1280;
	--y = 1200;
	x = 1310;
	y = 1115;
	width = 630;
	height = 450;
}	
ED_A10C_RWR =
{
	x = 1257;
	y = 1560;
	width = 500;
	height = 4800;
}
--UIMainView = Viewports.P1
UI = { x = 0, y = 0, width = 1920, height = 1080 }
UIMainView = UI
--GUI_MAIN_VIEWPORT = Viewports.P2