function layout()
return {
keyCommands = {
{combos = {{key = "MOUSE_BTN3"}, }, down = iCommandViewTransposeModeOn, up = iCommandViewTransposeModeOff, name = "Camera transpose mode on/off", category = "View Cockpit"},
{combos = {{key = "MOUSE_BTN2"}, }, down = iCommand_ExplorationStart, name = "Enable exploration mode", category = "View Cockpit"},
},
axisCommands = {
{combos = {{key = "MOUSE_X"}, }, action = iCommandPlaneViewHorizontal, name = "Camera Horizontal View"},
{combos = {{key = "MOUSE_Y"}, }, action = iCommandPlaneViewVertical, name = "Camera Vertical View"},
{combos = {{key = "MOUSE_Z"}, }, action = iCommandPlaneZoomView, name = "Camera Zoom View"},
},
}
end