_ = function(p) return p; end;
name = _('DCS WIP 20230430');
Description = 'Exported configuration with 3 projectors'
CENTER_MFCD = { x = 2040, y = 3200, width = 595, height = 590 }
F18_IFEI = { x = 0, y =  1081, width = 1, height = 1 }
--F18_RWR = { x = 2362, y = 2764, width = 270, height = 270 }
--F18_RWR = { x = 1000, y = 2000, width = 270, height = 270 }
F18_UFC = { x = 0, y = 1081, width = 2, height = 2 }
LEFT_MFCD = { x = 2058, y = 2175, width = 560, height = 590 }
RIGHT_MFCD = { x = 3530, y = 2190, width = 560, height = 580 }
Viewports =
{
	P4 = 
	{
		x = 0;
		y = 2161;
		width = 1920;
		height = 1080;
		aspect = 1.777778;
		useAbsoluteFOV = true;
		useAbsoluteAnglesShift = true;
		FOV = 2.5;
		viewDx = 0;
		viewDy = -1.5;
	},
	P1 =
	{
		x = 0;
		y = 0;
		width = 3840;
		height = 2160;
		aspect = 1.777778;
		useAbsoluteFOV = true;
		useAbsoluteAnglesShift = true;
		FOV = 1.944007;
		viewDx = 1.153382;
		viewDy = 0.172730;
	},
	P2 =
	{
		x = 3840;
		y = 0;
		width = 3840;
		height = 2160;
		aspect = 1.777778;
		useAbsoluteFOV = true;
		useAbsoluteAnglesShift = true;
		FOV = 2.189753;
		viewDx = 0.082084;
		viewDy = 0.170320;
	},
	P3 =
	{
		x = 7680;
		y = 0;
		width = 3840;
		height = 2160;
		aspect = 1.777778;
		useAbsoluteFOV = true;
		useAbsoluteAnglesShift = true;
		FOV = 1.976795;
		viewDx = -1.236236;
		viewDy = 0.212489;
	},

}

--UIMainView = Viewports.P1
UI = { x = 5300, y = 100, width = 1400, height = 900 }
UIMainView = UI
GUI_MAIN_VIEWPORT = Viewports.P1

