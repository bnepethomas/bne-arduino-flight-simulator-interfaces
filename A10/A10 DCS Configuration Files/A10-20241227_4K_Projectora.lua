_  = function(p) return p end
name = _('A10-27 with 4K Projector')
description = 'A10 with 1080 Primary'
PrmaryWidth = 3840
PrimaryHeight = 2160
Viewports = {
  P2 = {
    x = 0,
    y = 0,
    width = PrmaryWidth,
    height = PrimaryHeight,
    aspect = 1.77777777777778,
    dx = 0,
    dy = 0
  }
}
LEFT_MFCD =
{
     x = 367;
     y = PrimaryHeight +71;
     width = 800;
     height = 600;
}
RIGHT_MFCD =
{
     x = 67;
     y = PrimaryHeight + 792;
     width = 800;
     height = 600;
}
ED_A10C_CMSP =
{
	x = -1;
	y = -1;
	width = 1;
	height = 1;
}
ED_A10C_CMSC =
{
	x = -1;
	y = -2;
	width = 1;
	height = 1;
}
ED_A10C_CDU =
{
	--x = 1264;
	--y = 1088;
	--x = 1280;
	--y = 1200;
	x = 1310;
	y = PrimaryHeight + 35;
	width = 630;
	height = 450;
}	
ED_A10C_RWR =
{
	x = 1267;
	y = PrimaryHeight + 480;
	width = 500;
	height = 480;
}
--UIMainView = Viewports.P1
UI = { x = 0, y = 0, width = 1280, height = 720 }
UIMainView = UI
--GUI_MAIN_VIEWPORT = Viewports.P2