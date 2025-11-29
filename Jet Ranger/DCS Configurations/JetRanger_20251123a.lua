_  = function(p) return p end
name = _('JetRanger 20251123a')
description = 'JetRanger'
PrmaryWidth = 1920
PrimaryHeight = 1080
Viewports = {
  LEFT = {
    x = 0;
    y = 0;
    width = 1920;
    height = 1080;
    aspect = 1.77777777777778;
    dx = 0;
    dy = 0;
	viewDx = -1.56;
	viewDy = 0;
	useAsoluteAnglesShift = true;
  },
  CHIN = {
		x = 1920;
		y = 1080;
		width = 1920;
		height = 1080;
		aspect = 1.777778;
		dx = 0;
		dy = 0;
		viewDx = 0.3;
		viewDy = -0.785;
		useAsoluteAnglesShift = true;
		useAbsoluteFOV = true;
		FOV = 1.56 ;
	},
	  CENTER = {
		x = 1920;
		y = 0;
		width = 1920;
		height = 1080;
		aspect = 1.777778;
		dx = 0;
		dy = 0
	},
	RIGHT = {
		x = 3840;
		y = 0;
		width = 1920;
		height = 1080;
		aspect = 1.777778;
		dx = 0;
		dy = 0;
		viewDx = 0.785;
		viewDy = 0;
		useAsoluteAnglesShift = true;
	},
}

--UIMainView = Viewports.P1
UI = { x = 1920, y = 0, width = 1920, height = 1080 }
UIMainView = UI
GUI_MAIN_VIEWPORT = Viewports.P1