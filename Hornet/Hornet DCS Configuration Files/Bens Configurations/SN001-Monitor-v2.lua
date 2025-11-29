_ = function(p) return p; end;
name = _('SN001-Simulator-2');
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
		FOV = 1.893591;
		viewDx = 1.291544;
		viewDy = 0.017453;
		--tans of side angles
		projection_bounds = {
			left = 1.376382,
			right = 1.279942,
			top = 0.781286,
			bottom = 0.753554,
		}
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
		FOV = 2.116815;
		viewDx = 0.034907;
		viewDy = 0.034907;
		--tans of side angles
		projection_bounds = {
			left = 1.664279,
			right = 1.804048,
			top = 1.000000,
			bottom = 0.900404,
		}
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
		FOV = 1.910564;
		viewDx = -1.308997;
		viewDy = 0.034907;
		--tans of side angles
		projection_bounds = {
			left = 1.327045,
			right = 1.234897,
			top = 0.809784,
			bottom = 0.781286,
		}
	}
}

LEFT_MFCD = 
{
    x = 5760;
    y = 30;
    width = 580;
    height = 580;
}

CENTER_MFCD = 
{
    x = 5760+1024;
    y = 600;
    width = 780;
    height = 780;
}

RIGHT_MFCD = 
{
    x = 1920+1920+1920+1024-570;
    y = 630;
    width = 580;
    height = 580;
}
F18_RWR = 
{
    x = 5760+1024+800;
    y = 0;
    width = 480;
    height = 480;
}

F18_IFEI = 
{
    x = 5760+1024-480;
    y = 20;
    width = 1480;
    height = 590;
}

F18_UFC = { x = 5760+600, y = 600+600+20, width = 5, height = 5 } -- NOT DISPLAYED BUT REQUIRED


UIMainView = Viewports.P2

