# Setting up a new PC

This chapter takes a PC from a fresh Windows install to a PC that can fly the cockpit: the tools, the network address, the Jet Ranger programs, and the simulators with their add-ons. It covers the development tools (**Visual Studio Community**, GitHub Desktop, Notepad++), the simulators (**MSFS**, **Prepar3D v5 and v6**, **DCS World** and **DCS-BIOS**), **FSUIPC for Prepar3D**, **Orbx Central**, the documentation tools and the utilities. X-Plane is covered under Running the X-Plane bridge, earlier in this guide.

Do the steps in order. The simulators are the slow part (large downloads), so start them early.

## What the PC needs

| Item | Needed for | Where it comes from |
|---|---|---|
| Windows 10 or 11, 64-bit | Everything | |
| A fixed address, 172.16.1.10 | The boards send to this address | Network adapter settings (see Network below) |
| .NET 10 SDK | Building and running the Jet Ranger programs | `dotnet.microsoft.com/download` |
| Visual Studio Community | Editing, debugging and building the programs (optional: `dotnet build` is enough to build) | `visualstudio.microsoft.com/vs/community` |
| Git, or GitHub Desktop | Getting the repository | `git-scm.com`, `desktop.github.com` |
| Notepad++ | Editing text files such as `FSUIPC.ini` | `notepad-plus-plus.org/downloads` |
| Arduino CLI | Flashing the boards (only on the PC you flash from) | `arduino.github.io/arduino-cli` |
| Python 3 | The UDP debug reflector (any program that listens on port 27000 will do) | `python.org` |
| MSFS 2020 or 2024 | The MSFS bridge | Microsoft Store (Xbox app) or Steam |
| Prepar3D v5 or v6 | The FSUIPC bridge | Lockheed Martin, `prepar3d.com` (needs a licence) |
| FSUIPC6 | The FSUIPC bridge, and the DodoSim events | `fsuipc.com` |
| DCS World and DCS-BIOS | The DCS bridge | Eagle Dynamics (or Steam); `github.com/DCS-Skunkworks/dcs-bios` |
| Orbx Central | Scenery for the simulators | `orbxdirect.com` |
| LibreOffice, draw.io | Building the documents and drawing diagrams | `libreoffice.org`, `get.diagrams.net` |
| NirSoft tools, USB Tree View, Wireshark, AutoLogon | Utilities (see Install the utilities) | listed below |

## Network

The boards, the PC and the USB HID board are on one switched network, 172.16.1.0/24, with fixed addresses and no DHCP. The boards send their debug text, heartbeats and events to **172.16.1.10**, so the PC must hold that address.

1. Open the network adapter's properties, select *Internet Protocol Version 4* and set a fixed address: **172.16.1.10**, subnet mask **255.255.255.0**. A gateway is not needed for the cockpit network. If the PC also needs the internet (to download the software below), use a second adapter, or set the address back to automatic until the downloads are done.
2. Set the cockpit network to **Private** in Windows (Settings, Network, the adapter's properties). Windows firewall rules for private networks are the easy ones to allow.
3. The first time each Jet Ranger program runs, Windows asks to allow it through the firewall. Allow it on **private networks**. If a program hears nothing from the boards, check the firewall first. The programs need to receive UDP on these ports: 27000 (debug), 27001 (radio commands to the bridge), 27002 (GPS, on this PC), 27003 and 27004 (ADF events and switch states), 13137 (heartbeats). The programs send UDP to the boards on 13136 and to the USB HID board on 4210.
4. Check it: with the boards powered, run `ping 172.16.1.101` (Radio), `.102` (Servo), `.105` (Stepper), `.106` (Dual Stepper), `.107` (ADF and Upper) and `ping 172.16.1.11` (USB HID board). Each should answer.

## Install the tools

1. **.NET 10 SDK.** Download the Windows x64 SDK installer from `dotnet.microsoft.com/download` and run it. Check with `dotnet --version` in a new command window. The programs are Windows Forms programs, so they need Windows (they do not run on Linux or macOS).
2. **Visual Studio Community.** Download the installer from `visualstudio.microsoft.com/vs/community` and run it. Choose a version that supports **.NET 10** (Visual Studio 2026) and, in the installer, tick the **.NET desktop development** workload. Visual Studio is for editing, debugging and building the programs (open the `.slnx` file in a program's folder). It is not needed just to run them, and `dotnet build` (below) builds without it. Community Edition is free for individuals and small teams under Microsoft's licence terms, so check that your use is covered.
3. **Git, and GitHub Desktop.** Install Git for Windows with the default options if you want the `git` command. GitHub Desktop (`desktop.github.com`) is the easier way to clone, pull and push: after installing it, sign in and add the primary repository (see Get the code, below).
4. **Notepad++** (`notepad-plus-plus.org/downloads`), for editing text files such as `FSUIPC.ini` and the start-up script's JSON.
5. **Python 3** (for the debug reflector). Install it, and tick *Add python.exe to PATH*.
6. **Arduino CLI**, on the PC you flash the boards from. Download it, put `arduino-cli.exe` in a folder on the `PATH`, then install the board support and the libraries:

```
arduino-cli core update-index
arduino-cli core install arduino:avr
```

The libraries (Ethernet, AccelStepper, Servo, Ewma, U8g2, DCS-BIOS and the others) and their versions are listed in Appendix B of the master document; install each with `arduino-cli lib install "<name>@<version>"`. The Mega controllers use the board option `arduino:avr:mega` and the USB HID board `arduino:avr:leonardo`. See COM ports and flashing for the ports and the commands.

## Get the code and build the programs

1. Clone the repository somewhere with a short path, for example `C:\Jet Ranger`, with the first command below. With GitHub Desktop instead: *File, Clone repository, URL*, enter `https://github.com/bnepethomas/bne-arduino-flight-simulator-interfaces`, choose the folder and click *Clone*.
2. The programs are in the `Jet Ranger` folder, in a folder called **` C# Code`**. Its name **starts with a space**, so put paths that contain it in quotes in a command window.
3. Build each program in Release with the second command below. The first build downloads the NuGet packages the programs use (for example the FSUIPC client for the FSUIPC bridge), so the PC needs the internet for it. The MSFS bridge carries its SimConnect files with it, so the MSFS SDK is not needed to build.
4. Each program is then in its own `bin\Release` folder. The start-up script (Starting each simulator) starts the right ones and reports any that are not built.
5. The UDP debug reflector is not part of the repository's programs: use your own small listener on UDP port 27000 (the start-up script only checks that something is listening, and can start one for you if you set its command in `Start Jet Ranger.json`).

Clone:

```
git clone https://github.com/bnepethomas/bne-arduino-flight-simulator-interfaces.git "C:\Jet Ranger"
```

Build every program:

```
cd "C:\Jet Ranger\Jet Ranger\ C# Code"
foreach ($p in "BenchTester","JetRangerHealthMonitor","LowranceAirMapGps","FSUIPCWinformsAutoCS","MSFSWinformsAutoCS","DCSBiosBridge","XPlaneBridge","JetRangerPanelEmulator") { dotnet build "$p" -c Release }
```

## Install MSFS

1. Install **Microsoft Flight Simulator 2020 or 2024** from the Microsoft Store (through the Xbox app, or Game Pass) or from Steam. The game and its content run to many tens of gigabytes, so check the free disk space and Microsoft's current system requirements before you start, and expect a long first download.
2. Start it once and let it finish its updates and content downloads, then load a flight and quit. The MSFS bridge cannot connect until MSFS is in a flight.
3. Install the Jet Ranger you fly. The bridge has estimated dials for the **FlyInside B206** and the **CowaSim Jet Ranger** (see Aircraft with estimated dials); install whichever you use from its own installer or the in-sim marketplace.
4. **No FSUIPC is needed for MSFS.** The MSFS bridge reads the simulator through SimConnect, which comes with MSFS.
5. Bind the panel's joystick buttons in the MSFS controls menu (the DS206 events are not available in MSFS).
6. Check it: load a Jet Ranger, run `MSFSWinformsAutoCS.exe`. The status bar turns green and shows the aircraft name.

## Install Prepar3D v5 and v6

Lockheed Martin Prepar3D needs a licence and is downloaded from the Prepar3D website with your account. Prepar3D v5 and v6 can be installed on the same PC, each in its own folder, and run one at a time.

1. Download the installers for the version you want from `prepar3d.com`. You need the **Client** (the simulator itself) and the **Content** and **Scenery** installers it depends on. The SDK is optional.
2. Close other programs, then run each installer **as administrator**, in the order the Prepar3D installer pages give, and accept the default folders unless you have a reason to move them.
3. Start Prepar3D once and let it create its configuration and its folder in *Documents* (`Prepar3D v5` or `Prepar3D v6` with its `Add-ons` folder). Choose a windowed or full-screen mode that suits the cockpit's displays, then quit.
4. Install the **DodoSim 206** for Prepar3D by following the DodoSim 206 V2 Installation and Configuration PDF in the `Jet Ranger Documentation` folder.
5. Do the same again for the other version if you want both.

## Install FSUIPC for Prepar3D

The FSUIPC bridge reads Prepar3D through **FSUIPC6**. FSUIPC6 is the version for Prepar3D (64-bit) v4 (from 4.3), **v5 and v6**. FSUIPC7 is a separate program for MSFS only, and is not used for Prepar3D. For FSX the older FSUIPC4 is used.

1. Download **FSUIPC6** from the downloads page at `fsuipc.com`. Always take the current release: older builds are not supported by the author and may not support the newest Prepar3D.
2. Close Prepar3D, then run the FSUIPC6 installer **as administrator**.
3. The installer looks for Prepar3D installations. If it finds both v5 and v6, install into each one you use (run it once per version if it asks). If it offers to remove an older FSUIPC from a version you are upgrading, accept.
4. The installer puts `FSUIPC6.dll` and its files in the simulator's `Modules` folder and registers it with the simulator through an add-on entry in *Documents*, `Prepar3D v5 Add-ons` or `Prepar3D v6 Add-ons`. Start Prepar3D: **FSUIPC** should now appear in its **Add-ons** menu. If it does not, run the installer again and check that it picked the right version.
5. **Register FSUIPC.** The button and axis assignments that this cockpit uses (the joystick buttons bound to the DS206 events) need a registered, paid copy of FSUIPC. Buy a licence from the FSUIPC site and enter your name, email address and key on FSUIPC's **Registration** tab. A key for an earlier FSUIPC generation does not necessarily work for FSUIPC6, so check the licence you hold covers it.
6. Install the DodoSim event file and add its `[EventFiles]` line (see Installing the DodoSim event file (FSUIPC)). The file goes in each Prepar3D version's `Modules` folder.
7. Set up the joystick buttons: either follow Setting FSUIPC events from the joystick buttons, or copy the settings from another computer (see Copying the FSUIPC joystick settings to another computer).
8. Check it: load the DodoSim 206 in Prepar3D, run `FSUIPCWinformsAutoCS.exe` and look for the connected status. Flip a switch on the panel and check the cockpit follows it.

## Install DCS World and DCS-BIOS

1. Install **DCS World** from Eagle Dynamics (the standalone installer from the DCS website) or from Steam. Start it once, let it finish its updates and make the first-run settings, then quit. Install the modules you fly (the UH-1H Huey is the first one the DCS bridge has a profile for).
2. Install **DCS-BIOS** once per PC by following Installing DCS-BIOS (DCS World), earlier in this guide. DCS must be closed while you do it.
3. Start the DCS bridge (see Running the DCS bridge) and load a mission to check it connects.

## Install Orbx Central

Orbx Central is Orbx's installer and manager for scenery. It works with MSFS, Prepar3D and X-Plane.

1. Download and install **Orbx Central** from the Orbx site, and sign in with your Orbx account.
2. Tell it which simulators you have installed (it finds most of them itself), so that scenery goes to the right one.
3. Download the **scenery of interest** for each simulator from the Orbx Central library. Large scenery packages are big downloads, so choose what you will fly over.

## Install the documentation tools

These tools were chosen because they are cross-platform and light.

1. **LibreOffice** (`libreoffice.org`). It opens and edits the `.odt` documents. The documents are also built with LibreOffice's Python (see `DocBuild`).
2. **draw.io** (diagrams.net). Use the offline installer from `get.diagrams.net`, for drawing diagrams without the internet. The main site is `diagrams.net`.

## Install the utilities

| Utility | What it is for | Where |
|---|---|---|
| NirSoft **Multi Monitor Tool** | Shows and saves the layout of the PC's monitors, for the cockpit displays | `nirsoft.net/utils/multi_monitor_tool.html` |
| NirSoft **Device Manager View** (Device Viewer) | Lists the devices and drivers Windows knows about | `nirsoft.net/utils/device_manager_view.html` |
| **USB Tree View** | Shows every USB device and the hub it hangs off, for finding the boards and the USB HID joysticks | `uwe-sieber.de/usbtreeview_e.html` |
| **Wireshark** | Captures the network traffic, to see the UDP packets between the boards and the PC | `wireshark.org/download.html` |
| Sysinternals **AutoLogon** | Makes the PC log in by itself after a restart | `learn.microsoft.com/en-us/sysinternals/downloads/autologon` |

Run AutoLogon once so that logging in happens automatically when the PC starts. You may want to create a local account for it (rather than using your Microsoft account). AutoLogon stores the password on the PC, so anyone who can switch the PC on can use the account.

## Finally

1. X-Plane, if you fly it, is set up as described in Running the X-Plane bridge.
2. Run `"Start Jet Ranger.bat" -Simulator <MSFS, P3D, DCS or XPlane> -DryRun` to see what the start-up script would do, then run it without `-DryRun` (see The start-up script). If it names a program as not built, build that program as above.
3. Open the Health Monitor and check that the six lights turn green within about a minute of the boards being powered.

Download sizes, installer screens and licence terms for the simulators and FSUIPC belong to their makers and change from time to time. Where this chapter and the maker's current instructions differ, follow the maker's.
