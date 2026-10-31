<#
.SYNOPSIS
  Starts the Jet Ranger PC programs for one simulator, in order: UDP reflector check, Health Monitor, the Lowrance GPS interface, the simulator (optional), and the bridge.

.DESCRIPTION
  Run only one bridge at a time: any other bridge that is running is stopped first (only the four Jet Ranger bridge programs are ever stopped).
  Programs that are already running are left alone. Nothing is started that is already there, so the script can be run again safely.

  Examples
    .\Start Jet Ranger.ps1                          menu: pick the simulator
    .\Start Jet Ranger.ps1 -Simulator DCS -DcsTimeZone 4
    .\Start Jet Ranger.ps1 -Simulator XPlane -XPlaneHost 192.168.1.20
    .\Start Jet Ranger.ps1 -Simulator MSFS -StartSimulator
    .\Start Jet Ranger.ps1 -Simulator P3D -NoGps
    .\Start Jet Ranger.ps1 -Simulator DCS -DryRun   shows what would be done without starting anything
    .\Start Jet Ranger.ps1 -Stop                    stops the bridges, the GPS interface and the Health Monitor (not the reflector or the simulator)

  Optional settings file "Start Jet Ranger.json" next to this script (see "Start Jet Ranger.sample.json"):
    Reflector       command that starts your UDP debug reflector on port 27000 if it is not running, for example  python "C:\tools\reflector.py"
    Simulators      the program (or a steam:// address) to start for -StartSimulator, per simulator (MSFS, P3D, FSX, DCS, XPlane)
    DcsTimeZone     default Mission time zone for the DCS bridge (hours from UTC)
    XPlaneHost      default X-Plane host (otherwise the bridge finds X-Plane itself)
#>
param(
    [ValidateSet("MSFS", "P3D", "FSX", "DCS", "XPlane")][string]$Simulator,
    [double]$DcsTimeZone = [double]::NaN,
    [string]$XPlaneHost,
    [switch]$StartSimulator,
    [switch]$NoGps,
    [switch]$NoHealthMonitor,
    [switch]$NoReflector,
    [switch]$Stop,
    [switch]$DryRun
)

$ErrorActionPreference = "Stop"
$here = Split-Path -Parent $MyInvocation.MyCommand.Path
$code = Join-Path $here " C# Code"        # the folder name starts with a space

# ------------------------------------------------------------------------------------------------------------------------------------------
$cfg = $null
$cfgFile = Join-Path $here "Start Jet Ranger.json"
if (Test-Path -LiteralPath $cfgFile) {
    try { $cfg = Get-Content -LiteralPath $cfgFile -Raw | ConvertFrom-Json } catch { Write-Warning "Could not read '$cfgFile': $($_.Exception.Message)" }
}
function CfgValue($name) { if ($cfg -and $cfg.PSObject.Properties[$name]) { return $cfg.$name } return $null }

$Programs = @{
    FSUIPC  = @{ Project = "FSUIPCWinformsAutoCS"; Exe = "FSUIPCWinformsAutoCS" }
    MSFS    = @{ Project = "MSFSWinformsAutoCS";   Exe = "MSFSWinformsAutoCS" }
    DCS     = @{ Project = "DCSBiosBridge";        Exe = "DCSBiosBridge" }
    XPlane  = @{ Project = "XPlaneBridge";         Exe = "XPlaneBridge" }
    Health  = @{ Project = "JetRangerHealthMonitor"; Exe = "JetRangerHealthMonitor" }
    Gps     = @{ Project = "LowranceAirMapGps";    Exe = "LowranceAirMapGps" }
}
$BridgeNames = @("FSUIPCWinformsAutoCS", "MSFSWinformsAutoCS", "DCSBiosBridge", "XPlaneBridge")

# The newest build of a program (Release or Debug).
function Find-Exe($key) {
    $p = $Programs[$key]
    $found = Get-ChildItem -LiteralPath (Join-Path (Join-Path $code $p.Project) "bin") -Recurse -Filter ($p.Exe + ".exe") -ErrorAction SilentlyContinue |
        Sort-Object LastWriteTime -Descending | Select-Object -First 1
    if ($found) { return $found.FullName }
    return $null
}

function Say($text, $color = "Gray") { Write-Host $text -ForegroundColor $color }
function Is-Running($name) { return [bool](Get-Process -Name $name -ErrorAction SilentlyContinue) }

function Start-Program($label, $exe, $arguments = @()) {
    $name = [IO.Path]::GetFileNameWithoutExtension($exe)
    if (Is-Running $name) { Say ("  {0,-22} already running" -f $label) "DarkGray"; return }
    if ($DryRun) { Say ("  {0,-22} would start {1} {2}" -f $label, $exe, ($arguments -join " ")) "Yellow"; return }
    if ($arguments.Count -gt 0) { Start-Process -FilePath $exe -ArgumentList $arguments -WorkingDirectory (Split-Path -Parent $exe) | Out-Null }
    else { Start-Process -FilePath $exe -WorkingDirectory (Split-Path -Parent $exe) | Out-Null }
    Say ("  {0,-22} started" -f $label) "Green"
}

function Stop-Program($name, $why) {
    $procs = Get-Process -Name $name -ErrorAction SilentlyContinue
    if (-not $procs) { return }
    if ($DryRun) { Say ("  would stop {0} ({1})" -f $name, $why) "Yellow"; return }
    $procs | ForEach-Object { try { $null = $_.CloseMainWindow() } catch {} }
    Start-Sleep -Milliseconds 800
    Get-Process -Name $name -ErrorAction SilentlyContinue | Stop-Process -Force -ErrorAction SilentlyContinue
    Say ("  stopped {0} ({1})" -f $name, $why) "Green"
}

# ------------------------------------------------------------------------------------------------------------------------------------------
if ($Stop) {
    Say "Stopping the Jet Ranger programs" "Cyan"
    foreach ($n in $BridgeNames) { Stop-Program $n "bridge" }
    Stop-Program "LowranceAirMapGps" "GPS interface"
    Stop-Program "JetRangerHealthMonitor" "Health Monitor"
    Say "The UDP reflector and the simulator are left running." "DarkGray"
    return
}

if (-not $Simulator) {
    Say "Which simulator?" "Cyan"
    $options = @("MSFS", "P3D", "FSX", "DCS", "XPlane")
    for ($i = 0; $i -lt $options.Count; $i++) { Say ("  {0}  {1}" -f ($i + 1), $options[$i]) }
    $choice = Read-Host "Number"
    if ($choice -notmatch '^[1-5]$') { Say "No simulator chosen." "Red"; return }
    $Simulator = $options[[int]$choice - 1]
}

$bridgeKey = switch ($Simulator) { "MSFS" { "MSFS" } "DCS" { "DCS" } "XPlane" { "XPlane" } default { "FSUIPC" } }
$bridgeName = $Programs[$bridgeKey].Exe

Say "Jet Ranger start-up for $Simulator$(if ($DryRun) { ' (dry run: nothing is started)' })" "Cyan"

# 1. Find the programs; stop here if one has not been built.
$exe = @{}
$missing = @()
$wanted = @($bridgeKey)
if (-not $NoHealthMonitor) { $wanted += "Health" }
if (-not $NoGps) { $wanted += "Gps" }
foreach ($k in $wanted) {
    $e = Find-Exe $k
    if ($e) { $exe[$k] = $e } else { $missing += $Programs[$k].Project }
}
if ($missing.Count -gt 0) {
    Say "Not built yet: $($missing -join ', '). Build each with: dotnet build -c Release  (in its folder under ' C# Code')." "Red"
    return
}

# 2. UDP debug reflector (UDP 27000): this script never starts a second listener on that port.
Say "1. UDP debug reflector (port 27000)"
if (-not $NoReflector) {
    $listening = Get-NetUDPEndpoint -LocalPort 27000 -ErrorAction SilentlyContinue
    if ($listening) { Say "  reflector already running" "DarkGray" }
    else {
        $cmd = CfgValue "Reflector"
        if ($cmd -and -not $DryRun) { Start-Process -FilePath "cmd.exe" -ArgumentList "/c", $cmd -WindowStyle Minimized | Out-Null; Say "  reflector started ($cmd)" "Green" }
        elseif ($cmd) { Say "  would start the reflector: $cmd" "Yellow" }
        else { Say "  nothing is listening on UDP 27000: start your debug reflector (or set 'Reflector' in Start Jet Ranger.json). Boards still work without it." "DarkYellow" }
    }
} else { Say "  skipped" "DarkGray" }

# 3. Only one bridge at a time.
Say "2. Other bridges"
$others = $BridgeNames | Where-Object { $_ -ne $bridgeName -and (Is-Running $_) }
if ($others) { foreach ($o in $others) { Stop-Program $o "only one bridge at a time" } } else { Say "  none running" "DarkGray" }

# 4. Health Monitor and the GPS interface.
Say "3. Health Monitor"
if ($NoHealthMonitor) { Say "  skipped" "DarkGray" } else { Start-Program "Health Monitor" $exe["Health"] }
Say "4. Lowrance GPS interface (UDP 27002)"
if ($NoGps) { Say "  skipped" "DarkGray" } else { Start-Program "GPS interface" $exe["Gps"] }

# 5. The simulator itself, only when asked and only from a configured path.
Say "5. Simulator"
$simExe = $null
$sims = CfgValue "Simulators"
if ($sims -and $sims.PSObject.Properties[$Simulator]) { $simExe = $sims.$Simulator }
if ($StartSimulator) {
    $isUri = $simExe -and ($simExe -match '^[A-Za-z][A-Za-z0-9+.-]+://')     # for example steam://rungameid/... for a Steam game
    if ($simExe -and ($isUri -or (Test-Path -LiteralPath $simExe))) {
        if ($DryRun) { Say "  would start $simExe" "Yellow" }
        elseif ($isUri) { Start-Process -FilePath $simExe | Out-Null; Say "  started $simExe" "Green" }
        else { Start-Process -FilePath $simExe -WorkingDirectory (Split-Path -Parent $simExe) | Out-Null; Say "  started $simExe" "Green" }
    } else { Say "  no program set for $Simulator in Start Jet Ranger.json (Simulators): start $Simulator yourself." "DarkYellow" }
} else { Say "  start $Simulator yourself (or use -StartSimulator with a path in Start Jet Ranger.json)" "DarkGray" }

# 6. The bridge.
Say "6. Bridge"
$bridgeArgs = @()
if ($Simulator -eq "DCS") {
    $zone = $DcsTimeZone
    if ([double]::IsNaN($zone)) { $z = CfgValue "DcsTimeZone"; if ($null -ne $z) { $zone = [double]$z } }
    if (-not [double]::IsNaN($zone)) { $bridgeArgs += @("--utc-offset", ($zone.ToString([Globalization.CultureInfo]::InvariantCulture))) }
    else { Say "  DCS time zone not given: the GPS clock will be the mission's local time. Set it in the bridge window, or use -DcsTimeZone (4 Caucasus and Persian Gulf, 3 Syria, -8 Nevada)." "DarkYellow" }
}
if ($Simulator -eq "XPlane") {
    $xh = $XPlaneHost
    if (-not $xh) { $xh = CfgValue "XPlaneHost" }
    if ($xh) { $bridgeArgs += @("--xplane-host", $xh) }
}
Start-Program "$Simulator bridge" $exe[$bridgeKey] $bridgeArgs

# 7. Check what is running and who owns the GPS port.
if (-not $DryRun) {
    Start-Sleep -Seconds 5
    Say "Check" "Cyan"
    $expected = @($bridgeName)
    if (-not $NoHealthMonitor) { $expected += "JetRangerHealthMonitor" }
    if (-not $NoGps) { $expected += "LowranceAirMapGps" }
    foreach ($n in $expected) {
        if (Is-Running $n) { Say ("  {0,-26} running" -f $n) "Green" } else { Say ("  {0,-26} NOT running (it may have shown an error window)" -f $n) "Red" }
    }
    if (-not $NoGps) {
        $ep = Get-NetUDPEndpoint -LocalPort 27002 -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($ep) { $owner = (Get-Process -Id $ep.OwningProcess -ErrorAction SilentlyContinue).ProcessName; Say ("  UDP 27002 (GPS data) owned by {0}" -f $owner) $(if ($owner -eq "LowranceAirMapGps") { "Green" } else { "Red" }) }
        else { Say "  nothing is listening on UDP 27002 yet (the GPS interface may be waiting for you to pick its serial port)" "DarkYellow" }
    }
}

Say ""
Say "Next:" "Cyan"
switch ($Simulator) {
    "MSFS"   { Say "  Load a Jet Ranger in MSFS; the MSFS bridge's status bar turns green when it connects." }
    "P3D"    { Say "  Start Prepar3D with FSUIPC and the DodoSim 206; the bridge's status bar turns green when it connects." }
    "FSX"    { Say "  Start FSX with FSUIPC and the DodoSim 206; the bridge's status bar turns green when it connects." }
    "DCS"    { Say "  Start DCS with DCS-BIOS installed and load an aircraft; the bridge shows 'receiving' and the aircraft name." }
    "XPlane" { Say "  Start X-Plane with 'accept incoming connections' on (Settings, Network); the bridge shows 'receiving'." }
}
Say "  Open the Health Monitor: every board light should be green within about 10 seconds."
Say "  The GPS window shows 'simulator time' once the bridge is sending (the first run asks for the GPS serial port)."
