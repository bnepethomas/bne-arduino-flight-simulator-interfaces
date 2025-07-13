_  = function(p) return p; end;
name = _('PT IEFI');
Description = 'One monitor configuration'
Viewports =
{
     Center =
     {
          x = 0;
          y = 0;
          width = 1920;
          height = 1080;
          viewDx = 0;
          viewDy = 0;
          aspect = 1.9;
     }

}
F18_IFEI = 
{
    x = 1920-480;
    y = 20;
    width = 1480;
    height = 590;
}
--UIMainView = Viewports.P1
UI = { x = 0, y = 0, width = 1400, height = 900 }
UIMainView = UI

GU_MAIN_VIEWPORT = Viewports.Center