_ = function(p) return p; end;
name = _('SIM18_0_0_11520_2160');
Description = 'Exported configuration with 3 projectors'

-- First value is top left corner of the screen

CENTER_MFCD = { x = 11520 + 0 , y = 0 + 445, width = 595, height = 590 }
LEFT_MFCD = { x = 12120 + 17, y = 0 + 30, width = 560, height = 590 }
RIGHT_MFCD = { x = 11520 + 460, y = 1050 + 30, width = 560, height = 580 }
F18_RWR = { x = 12097, y = 1814, width = 430, height = 450 }
F18_UFC = { x = 11522  , y = 0, width = 2, height = 2 }
F18_IFEI = { x = 11522, y =  3, width = 1, height = 1 }

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
		FOV = 2.365660;
		viewDx = 1.230527;
		viewDy = 0.035436;
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
		viewDx = 0.027740;
		viewDy = 0.018762;
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
		FOV = 2.365660;
		viewDx = -1.268399;
		viewDy = 0.050927;
	}
}
--UIMainView = Viewports.P1
UI = { x = 5100, y = 100, width = 1400, height = 900 }
UIMainView = UI
GUI_MAIN_VIEWPORT = Viewports.P1


