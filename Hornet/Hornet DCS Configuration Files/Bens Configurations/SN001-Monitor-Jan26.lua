_ = function(p) return p; end;
name = _('SN001-Simulator-HUD-Jan26');
Description = 'Exported configuration with 3 projectors'
Viewports =
{
	P1 =
	{
		x = 0;
		y = 0;
		width = 1920;
		height = 1080;
		aspect = 1.777778;
		useAbsoluteFOV = true;
		useAbsoluteAnglesShift = true;
		FOV = 2.071449;
		viewDx = 1.256637;
		viewDy = 0.122173;
	},
	P2 =
	{
		x = 1920;
		y = 0;
		width = 1920;
		height = 1080;
		aspect = 1.777778;
		useAbsoluteFOV = true;
		useAbsoluteAnglesShift = true;
		FOV = 2.131663;
		viewDx = 0.034907;
		viewDy = 0.087266;
	},
	P3 =
	{
		x = 3840;
		y = 0;
		width = 1920;
		height = 1080;
		aspect = 1.777778;
		useAbsoluteFOV = true;
		useAbsoluteAnglesShift = true;
		FOV = 2.146379;
		viewDx = -1.134464;
		viewDy = 0.104720;
	}
}
offset = 45;
LEFT_MFCD = 
{
    x = 5760+1080+offset;
    y = offset;
    width = 800-offset*2;
    height = 800-offset*2;
}

CENTER_MFCD = 
{
    x = 5760+1080+800;
    y = 30;
    width = 780;
    height = 780;
}

RIGHT_MFCD = 
{
    x = 5760+1080+800+800+offset-5;
    y = offset;
    width = 800-offset*2;
    height = 800-offset*2;
}
F18_RWR = 
{
    x = 5760+1080+1024;
    y = 800;
    width = 480;
    height = 480;
}

F18_IFEI = 
{
    x = 5760+1080-480;
    y = 820;
    width = 1480;
    height = 590;
}
local HUDScale = 1.6
local HUDSsize = 750
F18_HUD = 
{ 

    x = 5760+(1080-HUDSsize)/2,
	y = 0,
    width = HUDSsize,
    height = HUDSsize*HUDScale+50, 
}

F18_UFC = { 
x = 0, 
y = 1080, 
width = 5, 
height = 5, 
} -- NOT DISPLAYED BUT REQUIRED

local UIScale = 0.70
UIMainView = 
{
--
x = 1920+(1920-(1920*UIScale))/2,
y = 30,
    width = 1920*UIScale,
    height = 1080*UIScale,
}
