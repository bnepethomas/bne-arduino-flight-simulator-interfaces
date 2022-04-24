_  = function(p) return p; end;
name = _('Petes Projector');
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
          aspect = screen.aspect;
     }
}

LEFT_MFCD = 
{
    x = 1952;
    y = 57;
    width = 520;
    height = 520;
}

UIMainView = Viewports.Center