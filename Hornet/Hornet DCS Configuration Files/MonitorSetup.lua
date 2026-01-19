_ = function(p) return p; end;
name = _('SIM18_0_0_11520_2160');
Description = 'Exported configuration with 3 projectors'

-- First value is top left corner of the screen

CENTER_MFCD = { x = 1600 ; y = 2160; width = 727; height = 750;}
DDIOFFSET = 10
LEFT_MFCD = { x = 0+DDIOFFSET; y = 2160; width = 755; height = 755;}
RIGHT_MFCD = { x = 800+DDIOFFSET; y = 2160; width = 755; height = 755; }
--F18_RWR = { x = 11520+1024 + 25, y = 800 - 130 , width = 430, height = 450 }
--F18_UFC = { x = 11522  , y = 2160, width = 2, height = 2 }
--F18_IFEI = { x = 3480; y = 2160; width = 1480; height = 595; }
F18_IFEI = { x = 3005; y = 2180; width = 1480; height = 595; }
HUDOFFSET=140
F18_HUD = { x = 2400+HUDOFFSET; y = 2160; width = 800; height = 800*1.4;}

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


