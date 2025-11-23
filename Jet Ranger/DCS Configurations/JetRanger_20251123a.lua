_  = function(p) return p end
name = _('JetRanger 20251123a')
description = 'JetRanger'
PrmaryWidth = 1920
PrimaryHeight = 1080
Viewports = {
  P1 = {
    x = 0;
    y = 0;
    width = 1920;
    height = 1080;
    aspect = 1.77777777777778;
    dx = 0;
    dy = 0
  },
  P2 = {
		x = 1920;
		y = 1080;
		width = 1920;
		height = 1080;
		aspect = 1.777778;
		dx = 0;
		dy = 0
	},
	  P3 = {
		x = 1920;
		y = 0;
		width = 1920;
		height = 1080;
		aspect = 1.777778;
		dx = 0;
		dy = 0
	},
	P4 = {
		x = 3840;
		y = 0;
		width = 1920;
		height = 1080;
		aspect = 1.777778;
		dx = 0;
		dy = 0
	},
}

--UIMainView = Viewports.P1
UI = { x = 1920, y = 0, width = 1920, height = 1080 }
UIMainView = UI
GUI_MAIN_VIEWPORT = Viewports.P1