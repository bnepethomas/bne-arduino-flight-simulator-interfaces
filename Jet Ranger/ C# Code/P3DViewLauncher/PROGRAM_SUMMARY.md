# P3DViewLauncher

Opens Prepar3D v5 views with chosen cameras, undocks each one, and moves it to a screen, as an ordinary window by default or full screen when a view asks for it. It is a console program and never shows a dialog box.

## Use

1. Build: `dotnet build -c Release`. The program is in `bin\Release\net48`.
2. List the screens: `P3DViewLauncher.exe --screens`.
3. Edit `views.txt` next to the program: one line per view, `camera name | window title | screen number | size`. A build copies the project's `views.txt` over that one when the project's file is newer, so keep lasting changes in the project folder.
4. Start Prepar3D, load a flight, then run `P3DViewLauncher.exe`.

| Option | Effect |
|---|---|
| (none) | open the views listed in `views.txt`; a view window already open is only moved |
| `--wait` | keep trying until Prepar3D is running (it does not wait for a flight to be loaded) |
| `--close` | close the listed views |
| `--screens` | list the screen numbers (the order of the Windows device names, which may differ from the numbers in Windows Settings) |
| `--identify` | show each screen's number on that screen for 30 seconds |
| `--windows` | list Prepar3D's window titles |
| a file name | use that file in place of `views.txt` |

The exit code is 0 when everything worked, 1 on a failure and 2 for a wrong option.

Each view keeps its frame and is centred on its screen (inside the taskbar). The size is optional (`1280x720`, limited to the screen); left out, the window keeps the size Prepar3D gave it. `fullscreen` in the size field instead removes the frame and covers the whole screen, through Windows only (Prepar3D's own full screen mode, which has caused trouble in earlier versions, is not used). Close a full screen view before changing it back to a window. The screen numbers change if screens are added, removed or rearranged, so check them with `--screens`.

Run the program on the simulator PC. If Prepar3D runs as administrator, run this as administrator too.

## Cameras

The camera name is the `Title` of a `[CameraDefinition.N]` entry in `%AppData%\Lockheed Martin\Prepar3D v5\cameras.cfg`, for example `Cockpit`, `Virtual Cockpit` or `Spot`. For a view looking in another direction, add an entry with the next free number, its own title and a new GUID (PowerShell: `[guid]::NewGuid()`), and restart Prepar3D. For example, looking 90 degrees left (back up `cameras.cfg` first; this entry has not been tried):

```ini
[CameraDefinition.028]
Title = Left Window
Guid = {put-a-new-guid-here}
Description = Virtual cockpit looking left
Origin = Virtual Cockpit
Category = Cockpit
InitialZoom = 0.5
InitialPbh = 0, 0, -90
```

`InitialPbh` is pitch, bank, heading in degrees.

For a custom camera made inside Prepar3D (saved in `CustomCameras.xml`), no `cameras.cfg` change is needed: write `base camera > custom camera` in `views.txt`, for example `Cockpit > LEFT`. The base camera is the one the custom camera was made from (`ParentGUID` in `CustomCameras.xml` matches its `Guid` in `cameras.cfg`).

## How it works

SimConnect (`Simulator.cs`) opens each view (`OpenView`) and undocks it (`UndockView`). SimConnect's window position takes no negative numbers (screens left of or above the primary one), so the program then finds the Prepar3D window with that title and moves it through Windows (`NativeWindows.cs`).

`lib\LockheedMartin.Prepar3D.SimConnect.dll` is version 4.4 of the library, copied from `Control_P3d\Managed Data Request` (no Prepar3D v5 SDK was installed). It needs the Microsoft Visual C++ 2015-2022 x64 runtime. The program targets .NET Framework 4.8 because the library does not load in modern .NET.

## Tested (26 November 2026)

Current version, against Prepar3D v5: both `views.txt` views opened as ordinary windows with title bars, not maximised, centred on their screens at the size Prepar3D gave them (573 x 298 in that run), and `--close` closed them. The size field has not been tried against Prepar3D. The screen numbering on that PC changed between runs (a different monitor became screen 1 and primary).

The supplied `views.txt` now puts `RIGHT` on screen 3 (the right-hand monitor) and `LEFT` on screen 4 (the left-hand monitor), both full screen. The launcher's numbers differ from the ones the owner uses for these monitors.

`fullscreen` against Prepar3D v5: both views covered screens 3 and 4 exactly (1920 x 1080) without a title bar, each showing its own side view (checked by screen capture), and Prepar3D stayed responsive. Longer running, flight reloads and closing Prepar3D with full screen views open have not been tried.

Current `views.txt`: custom camera `FORWARD` on the main view (`Cockpit > FORWARD | main`, window not moved), `CHIN` full screen on screen 1 (the lower monitor), `RIGHT` on screen 3 and `LEFT` on screen 4. Against Prepar3D v5 all ran without errors and screen captures showed each picture on its screen. A full screen view is raised above other programs' windows once as it opens (the chin view first opened behind another program). The chin view capture had an unexplained pale band across the bottom fifth of the screen.

The results below up to the custom cameras were with an earlier version that made views full screen.

Builds clean. Without a simulator: `--screens`, `--windows`, a bad views file and the "not running" message work, and moving a test window to a second screen in `borderless` and `window` modes works.

Against Prepar3D v5 with a flight loaded: `Virtual Cockpit` and `Spot` views opened, undocked and covered their screens exactly (1920 x 1080) in `borderless` mode; a second run moved the open windows without duplicating them; `--close` closed them. An Alt+Enter full screen mode was tried and removed: it did not make an undocked view full screen. Not checked against Prepar3D: the `window` mode.

The supplied `views.txt` opens the custom cameras `RIGHT` (screen 1) and `LEFT` (screen 2), which are defined in `CustomCameras.xml` on the simulator PC (made inside Prepar3D). Giving such a name to `OpenView` opens a window showing the forward view, with no error. The line must be `base camera > custom camera` (`Cockpit > LEFT`): the program opens the base camera and then calls `CameraPanToView` on the window. Checked by capturing both screens: each showed its own side view.
