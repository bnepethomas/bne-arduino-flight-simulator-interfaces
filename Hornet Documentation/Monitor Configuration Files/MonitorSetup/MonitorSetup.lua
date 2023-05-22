_ = function(p) return p; end;
name = _('SIM18_0_0_11520_2160');
Description = 'Exported configuration with 3 projectors'


CENTER_MFCD = { x = 11520, y = 445, width = 595, height = 590 }
LEFT_MFCD = { x = 12137, y = 30, width = 560, height = 590 }
RIGHT_MFCD = { x = 11980, y = 1080, width = 560, height = 580 }


Viewports =
{
	P1 =
	{
		x = 0;
		y = 0;
		width = 3840;
		height = 2160;
		aspect = 1.777778;
		useAbsoluteFOV = true;
		useAbsoluteAnglesShift = true;
		FOV = 2.116815;
		viewDx = 1.228586;
		viewDy = 0.037466;
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
		FOV = 2.365660;
		viewDx = 0.027072;
		viewDy = 0.025351;
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
		FOV = 2.146379;
		viewDx = -1.225961;
		viewDy = 0.029147;
	}
}
--UIMainView = Viewports.P1
UI = { x = 5100, y = 100, width = 1400, height = 900 }
UIMainView = UI
GUI_MAIN_VIEWPORT = Viewports.P1


