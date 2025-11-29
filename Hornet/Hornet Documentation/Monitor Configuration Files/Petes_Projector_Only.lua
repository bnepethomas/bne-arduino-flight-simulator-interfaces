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
    x = 1932;
    y = 25;
    width = 560;
    height = 590;
}

RIGHT_MFCD = 
{
    x = 3407;
    y = 23;
    width = 560;
    height = 590;
}

CENTER_MFCD = 
{
    x = 1902;
    y = 1035;
    width = 635;
    height = 595;
}

UIMainView = Viewports.Center