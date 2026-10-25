# Captures every form of the Jet Ranger C# programs for the User Guide.
#   powershell -ExecutionPolicy Bypass -File capture_all.ps1 [-Only swing|vsi|emulators|bridges|health|gps]
# Nothing here presses a control that sends to a real board: the programs are only started, and a few harmless
# controls are used. The GPS interface is run with its own (temporary) settings folder so the saved serial port is untouched.
param([string]$Only = "")
$ErrorActionPreference = "Stop"
. "$PSScriptRoot\shot.ps1"
$root = "C:\Users\admin\bne-arduino-flight-simulator-interfaces\Jet Ranger\ C# Code"
$out = $PSScriptRoot
$started = @()

function Want($n) { return ($Only -eq "" -or $Only -eq $n) }
function Run($exe, $secs = 3) { $p = Start-App $exe $secs; $script:started += $p; return $p }
function Cond($t) { New-Object System.Windows.Automation.PropertyCondition($script:A::ControlTypeProperty, $t) }
function Name-Cond($n) { New-Object System.Windows.Automation.PropertyCondition($script:A::NameProperty, $n) }
function Click-Button($win, $name) {
    $b = $win.FindFirst($script:TS::Descendants, (Name-Cond $name))
    if (-not $b) { throw "button not found: $name" }
    $h = $b.Current.NativeWindowHandle
    if ($h -ne 0) { [WinShot]::Click([IntPtr]$h) } else { ($b.GetCurrentPattern([System.Windows.Automation.InvokePattern]::Pattern)).Invoke() }
}
function Shot($p, $file, $title = $null, $w = 0, $h = 0) {
    $win = Find-Window $p.Id $title
    if (-not $win) { throw "no window for $file" }
    if ($w -gt 0) { Resize-Window $win 0 0 $w $h }     # big enough that no scroll bar is needed
    $r = Save-Window $win (Join-Path $out $file)
    "{0,-34} {1}" -f $file, $r
}
function Split-Image($src, $prefix, $parts) {
    $img = [System.Drawing.Image]::FromFile($src)
    $h = [int][Math]::Ceiling($img.Height / $parts)
    for ($i = 0; $i -lt $parts; $i++) {
        $y = $i * $h; $hh = [Math]::Min($h, $img.Height - $y)
        $bmp = New-Object System.Drawing.Bitmap $img.Width, $hh
        $g = [System.Drawing.Graphics]::FromImage($bmp)
        $g.DrawImage($img, (New-Object System.Drawing.Rectangle 0, 0, $img.Width, $hh), (New-Object System.Drawing.Rectangle 0, $y, $img.Width, $hh), [System.Drawing.GraphicsUnit]::Pixel)
        $g.Dispose()
        $name = "{0}_{1}.png" -f $prefix, ($i + 1)
        $bmp.Save((Join-Path $out $name), [System.Drawing.Imaging.ImageFormat]::Png); $bmp.Dispose()
        "{0,-34} {1}x{2}" -f $name, $img.Width, $hh
    }
    $img.Dispose()
}

try {
    if (Want "swing") {
        # SwingTester: main window, the Reset boards dialog (only opened and cancelled) and the COM ports window
        $p = Run "$root\SwingTester\bin\Release\net10.0-windows\SwingTester.exe"
        Shot $p "swing_tester.png"
        $main = Find-Window $p.Id "Jet Ranger Swing Tester"
        Click-Button $main "Reset boards..."
        Start-Sleep 1.5
        $dlg = Find-Window $p.Id "Reset boards"
        if ($dlg) { Shot $p "swing_tester_reset.png" "Reset boards"; Click-Button $dlg "Cancel" }
        # COM ports window (reads the PC's ports only; nothing is restarted)
        Click-Button $main "COM ports..."
        Start-Sleep 2.5
        $ports = Find-Window $p.Id "COM ports and USB devices"
        if ($ports) { Shot $p "swing_tester_ports.png" "COM ports and USB devices" }
        Stop-Process -Id $p.Id -Force
    }

    if (Want "vsi") {
        # StepperVSITester scrolls, so the pages are scrolled and stitched, then split in two for the page
        $p = Run "$root\StepperVSITester\bin\Release\net10.0-windows\StepperVSITester.exe"
        $win = Find-Window $p.Id $null
        Resize-Window $win 0 0 700 1040
        $r = Save-ScrollingWindow $win (Join-Path $out "stepper_vsi_tester_full.png")
        "stepper_vsi_tester_full.png       $r"
        Split-Image (Join-Path $out "stepper_vsi_tester_full.png") "stepper_vsi_tester" 2
        Stop-Process -Id $p.Id -Force
    }

    if (Want "emulators") {
        # Radio and Upper controller emulators (not clicked: that would send packets)
        $p = Run "$root\JetRangerRadioControllerEmulator\bin\Release\net10.0-windows\JetRangerRadioControllerEmulator.exe"
        Shot $p "radio_emulator.png" $null 700 800
        Stop-Process -Id $p.Id -Force
        $p = Run "$root\JetRangerUpperControllerEmulator\bin\Release\net10.0-windows\JetRangerUpperControllerEmulator.exe"
        Shot $p "upper_emulator.png" $null 1340 480
        Stop-Process -Id $p.Id -Force
    }

    if (Want "bridges") {
        # The two bridges (no simulator running, so they show the disconnected state)
        $p = Run "$root\FSUIPCWinformsAutoCS\bin\Release\net10.0-windows10.0.19041.0\FSUIPCWinformsAutoCS.exe"
        Shot $p "fsuipc_bridge.png"
        Stop-Process -Id $p.Id -Force
        $p = Run "$root\MSFSWinformsAutoCS\bin\Release\net10.0-windows10.0.19041.0\MSFSWinformsAutoCS.exe"
        Shot $p "msfs_bridge.png"
        Stop-Process -Id $p.Id -Force
    }

    if (Want "health") {
        # Health Monitor: heartbeats are sent to this PC's port 13137
        $hm = Run "$root\JetRangerHealthMonitor\bin\Release\net10.0-windows\JetRangerHealthMonitor.exe"
        $u = New-Object System.Net.Sockets.UdpClient
        function Beat($tokens) { foreach ($t in $tokens) { $b = [Text.Encoding]::ASCII.GetBytes($t); [void]$u.Send($b, $b.Length, "127.0.0.1", 13137) } }
        # heartbeats carry the 5 V rail (and the ADF also the 12 V rail) after the board name
        $all = "COMM_NAV V5:5.02", "SERVO V5:4.98", "JOYSTICK V5:5.01", "UPPER_INPUT V5:5.03", "STEPPER V5:5.00", "DUAL_STEPPER V5:4.97", "ADF_PANEL V5:5.03 V12:12.41"
        Beat $all
        Start-Sleep 2
        Shot $hm "health_monitor.png"
        $win = Find-Window $hm.Id $null
        # the voltage table: Show Voltages, then back to the log view
        Click-Button $win "Show Voltages"
        Start-Sleep 1
        Beat $all
        Start-Sleep 1
        Shot $hm "health_monitor_voltages.png"
        Click-Button $win "Hide Voltages"
        Start-Sleep 1
        $btn = $win.FindFirst($script:TS::Descendants, (Name-Cond "Hide Logs"))
        [WinShot]::Click([IntPtr]$btn.Current.NativeWindowHandle)
        Start-Sleep 1
        Beat $all
        Start-Sleep 1
        Shot $hm "health_monitor_logs_hidden.png"
        [WinShot]::Click([IntPtr]$btn.Current.NativeWindowHandle)
        # keep four boards alive, let Stepper and Dual Stepper go quiet: orange after 15 s, red after 30 s
        $alive = "COMM_NAV V5:5.02", "SERVO V5:4.98", "JOYSTICK V5:5.01", "UPPER_INPUT V5:5.03", "ADF_PANEL V5:5.03 V12:12.41"
        $t0 = Get-Date
        $warn = $false; $lost = $false
        while (-not $lost) {
            Beat $alive
            Start-Sleep 4
            $age = ((Get-Date) - $t0).TotalSeconds
            if (-not $warn -and $age -ge 18) { Shot $hm "health_monitor_warning.png"; $warn = $true }
            if ($age -ge 34) { Shot $hm "health_monitor_lost.png"; $lost = $true }
        }
        Stop-Process -Id $hm.Id -Force
    }

    if (Want "gps") {
        # GPS interface with the bench tester feeding it. A temporary APPDATA gives it a first-run (no saved port) state
        # without touching the real saved serial port.
        $realAppData = $env:APPDATA
        $env:APPDATA = Join-Path $env:TEMP "jr_gps_shots"
        New-Item -ItemType Directory -Force $env:APPDATA | Out-Null
        if (Test-Path "$env:APPDATA\LowranceAirMapGps") { Remove-Item "$env:APPDATA\LowranceAirMapGps" -Recurse -Force }
        $gps = Run "$root\LowranceAirMapGps\bin\Release\net10.0-windows\LowranceAirMapGps.exe" 3
        $env:APPDATA = $realAppData
        # first run: the serial port dialog is open
        $sd = Find-Window $gps.Id "Select GPS serial port"
        if ($sd) { Shot $gps "gps_serial_dialog.png" "Select GPS serial port"; Click-Button $sd "OK"; Start-Sleep 1 }
        $bt = Run "$root\LowranceAirMapGpsBenchTester\bin\Release\net10.0-windows\LowranceAirMapGpsBenchTester.exe" 4
        Shot $bt "gps_bench_tester.png"
        Start-Sleep 1
        Shot $gps "gps_interface.png" "Lowrance*"
        Stop-Process -Id $bt.Id -Force
        Stop-Process -Id $gps.Id -Force
    }
}
finally {
    foreach ($p in $started) { if (-not $p.HasExited) { Stop-Process -Id $p.Id -Force -ErrorAction SilentlyContinue } }
}

