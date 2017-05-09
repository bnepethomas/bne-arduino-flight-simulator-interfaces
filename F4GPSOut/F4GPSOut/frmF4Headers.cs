using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Configuration;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.IO.MemoryMappedFiles;

namespace F4GPSOut.Headers
{

    public class F4GPSout
    {


        public enum LightBits : int
        {
            MasterCaution = 0x1,  // Left eyebrow

            // Brow Lights
            TF = 0x2,   // Left eyebrow
            OBS = 0x4,   // Not used
            OXY_BROW = 0x4, // repurposed for eyebrow OXY LOW (was OBS, unused)
            ALT = 0x8,   // Caution light; not used
            EQUIP_HOT = 0x8, // Caution light; repurposed for cooling fault (was: not used)
            WOW = 0x10,  // True if weight is on wheels: this is not a lamp bit!
            ENG_FIRE = 0x20,  // Right eyebrow; upper half of split face lamp
            CONFIG = 0x40,  // Stores config, caution panel
            HYD = 0x80,  // Right eyebrow; see also OIL (this lamp is not split face)
            OIL = 0x100, // Right eyebrow; see also HYD (this lamp is not split face)
            Flcs_ABCD = 0x100, // TEST panel FLCS channel lamps; repurposed, was OIL (see HYD; that lamp is not split face)
            DUAL = 0x200, // Right eyebrow; block 25, 30/32 and older 40/42
            FLCS = 0x200,// Right eyebrow; was called DUAL which matches block 25, 30/32 and older 40/42
            CAN = 0x400, // Right eyebrow
            T_L_CFG = 0x800, // Right eyebrow

            // AOA Indexers
            AOAAbove = 0x1000,
            AOAOn = 0x2000,
            AOABelow = 0x4000,

            // Refuel/NWS
            RefuelRDY = 0x8000,
            RefuelAR = 0x10000,
            RefuelDSC = 0x20000,

            // Caution Lights
            FltControlSys = 0x40000,
            LEFlaps = 0x80000,
            EngineFault = 0x100000,
            Overheat = 0x200000,
            FuelLow = 0x400000,
            Avionics = 0x800000,
            RadarAlt = 0x1000000,
            IFF = 0x2000000,
            ECM = 0x4000000,
            Hook = 0x8000000,
            NWSFail = 0x10000000,
            CabinPress = 0x20000000,

            AutoPilotOn = 0x40000000,  // TRUE if is AP on.  NB: This is not a lamp bit!
            //TFR_STBY = 0x80000000,  // MISC panel; lower half of split face TFR lamp
            TFR_STBY = -2147483648,  // MISC panel; lower half of split face TFR lamp

            // Used with the MAL/IND light code to light up "everything"
            // please update this is you add/change bits!
            AllLampBitsOn = -1073741841 //0xBFFFFFEF
        };

        [ComVisible(true)]
        [Flags]
        [Serializable]
        public enum LightBits2 : int
        {
            // Threat Warning Prime
            HandOff = 0x1,
            Launch = 0x2,
            PriMode = 0x4,
            Naval = 0x8,
            Unk = 0x10,
            TgtSep = 0x20,

            // EWS
            Go = 0x40,        // On and operating normally
            NoGo = 0x80,     // On but malfunction present
            Degr = 0x100,    // Status message: AUTO DEGR
            Rdy = 0x200,    // Status message: DISPENSE RDY
            ChaffLo = 0x400,    // Bingo chaff quantity reached
            FlareLo = 0x800,    // Bingo flare quantity reached

            // Aux Threat Warning
            AuxSrch = 0x1000,
            AuxAct = 0x2000,
            AuxLow = 0x4000,
            AuxPwr = 0x8000,

            // ECM
            EcmPwr = 0x10000,
            EcmFail = 0x20000,

            // Caution Lights
            FwdFuelLow = 0x40000,
            AftFuelLow = 0x80000,

            EPUOn = 0x100000,  // EPU panel; run light
            JFSOn = 0x200000,  // Eng Jet Start panel; run light

            // Caution panel
            SEC = 0x400000,
            OXY_LOW = 0x800000,
            PROBEHEAT = 0x1000000,
            SEAT_ARM = 0x2000000,
            BUC = 0x4000000,
            FUEL_OIL_HOT = 0x8000000,
            ANTI_SKID = 0x10000000,

            TFR_ENGAGED = 0x20000000,  // MISC panel; upper half of split face TFR lamp
            GEARHANDLE = 0x40000000,  // Lamp in gear handle lights on fault or gear in motion
            ENGINE = -2147483648, //0x80000000,  // Lower half of right eyebrow ENG FIRE/ENGINE lamp

            // Used with the MAL/IND light code to light up "everything"
            // please update this is you add/change bits!
            AllLampBits2On = -4033//0xFFFFF03F  //ATARIBABY EWS CMDS bits excluded from test lamps
        };

        [ComVisible(true)]
        [Flags]
        [Serializable]
        public enum LightBits3 : int
        {
            // Elec panel
            FlcsPmg = 0x1,
            MainGen = 0x2,
            StbyGen = 0x4,
            EpuGen = 0x8,
            EpuPmg = 0x10,
            ToFlcs = 0x20,
            FlcsRly = 0x40,
            BatFail = 0x80,

            // EPU panel
            Hydrazine = 0x100,
            Air = 0x200,

            // Caution panel
            Elec_Fault = 0x400,
            Lef_Fault = 0x800,

            Power_Off = 0x1000,   // Set if there is no electrical power.  NB: not a lamp bit
            Eng2_Fire = 0x2000,   // Multi-engine
            Lock = 0x4000,   // Lock light Cue; non-F-16
            Shoot = 0x8000,   // Shoot light cue; non-F16
            NoseGearDown = 0x10000,  // Landing gear panel; on means down and locked
            LeftGearDown = 0x20000,  // Landing gear panel; on means down and locked
            RightGearDown = 0x40000,  // Landing gear panel; on means down and locked

            // Used with the MAL/IND light code to light up "everything"
            // please update this is you add/change bits!
            AllLampBits3On = 0x0007EFFF
        };

        [ComVisible(true)]
        [Flags]
        [Serializable]
        public enum Bms4LightBits3 : int
        {
            // Elec panel
            FlcsPmg = 0x1,
            MainGen = 0x2,
            StbyGen = 0x4,
            EpuGen = 0x8,
            EpuPmg = 0x10,
            ToFlcs = 0x20,
            FlcsRly = 0x40,
            BatFail = 0x80,

            // EPU panel
            Hydrazine = 0x100,
            Air = 0x200,

            // Caution panel
            Elec_Fault = 0x400,
            Lef_Fault = 0x800,

            OnGround = 0x1000,   // weight-on-wheels
            FlcsBitRun = 0x2000,   // FLT CONTROL panel RUN light (used to be Multi-engine fire light)
            FlcsBitFail = 0x4000,   // FLT CONTROL panel FAIL light (used to be Lock light Cue; non-F-16)
            DbuWarn = 0x8000,   // Right eyebrow DBU ON cell; was Shoot light cue; non-F16
            NoseGearDown = 0x10000,  // Landing gear panel; on means down and locked
            LeftGearDown = 0x20000,  // Landing gear panel; on means down and locked
            RightGearDown = 0x40000,  // Landing gear panel; on means down and locked
            ParkBrakeOn = 0x100000, // Parking brake engaged; NOTE: not a lamp bit
            Power_Off = 0x200000, // Set if there is no electrical power.  NB: not a lamp bit

            // Caution panel
            cadc = 0x400000,

            // Left Aux console
            SpeedBrake = 0x800000,  // True if speed brake is in anything other than stowed position

            // Threat Warning Prime - additional bits
            SysTest = 0x1000000,

            // Master Caution WILL come up (actual lightBit has 3sec delay like in RL),
            // usable for cockpit builders with RL equipment which has a delay on its own.
            // Will be set to false again as soon as the MasterCaution bit is set.
            MCAnnounced = 0x2000000,

            // Free bits in LightBits3
            //0x4000000,
            //0x8000000,
            //0x10000000,
            //0x20000000,
            //0x40000000,
            //0x80000000,

            // Used with the MAL/IND light code to light up "everything"
            // please update this if you add/change bits!
            AllLampBits3On = 0x0147EFFF

            // Used with the MAL/IND light code to light up "everything"
            // please update this if you add/change bits!
            //AllLampBits3On = 0x0047EFFF
        };

        [ComVisible(true)]
        [Flags]
        [Serializable]
        public enum HsiBits : int
        {
            ToTrue = 0x01,    // HSI_FLAG_TO_TRUE
            IlsWarning = 0x02,    // HSI_FLAG_ILS_WARN
            CourseWarning = 0x04,    // HSI_FLAG_CRS_WARN
            Init = 0x08,    // HSI_FLAG_INIT
            TotalFlags = 0x10,    // HSI_FLAG_TOTAL_FLAGS; never set
            ADI_OFF = 0x20,    // ADI OFF Flag
            ADI_AUX = 0x40,    // ADI AUX Flag
            ADI_GS = 0x80,    // ADI GS FLAG
            ADI_LOC = 0x100,   // ADI LOC FLAG
            HSI_OFF = 0x200,   // HSI OFF Flag
            BUP_ADI_OFF = 0x400,   // Backup ADI Off Flag
            VVI = 0x800,   // VVI OFF Flag
            AOA = 0x1000,  // AOA OFF Flag
            AVTR = 0x2000,  // AVTR Light

            OuterMarker = 0x4000,  // MARKER beacon light for outer marker
            MiddleMarker = 0x8000,  // MARKER beacon light for middle marker
            FromTrue = 0x10000, // HSI_FLAG_TO_TRUE == 2, FROM

            Flying = -2147483648,  //0x80000000, // true if player is attached to an aircraft (i.e. not in UI state).  NOTE: Not a lamp bit
            // Used with the MAL/IND light code to light up "everything"
            // please update this is you add/change bits!
            AllLampHsiBitsOn = 0xE000
        };

        [ComVisible(true)]
        [Flags]
        [Serializable]
        public enum AltBits : int
        {
            CalType = 0x01,    // true if calibration in inches of Mercury (Hg), false if in hectoPascal (hPa)
            PneuFlag = 0x02,    // true if PNEU flag is visible
        };

        [ComVisible(false)]
        [Serializable]
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct DED_PFL_LineOfText
        {
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 26)]
            public sbyte[] chars;
        }

        // MFD On Screen Button Labels(OF)
        [ComVisible(false)]
        [Serializable]
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct OSBLabel
        {
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
            public sbyte[] Line1;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
            public sbyte[] Line2;
            [MarshalAs(UnmanagedType.I1)]
            public bool Inverted;
        }

        // BMS3/Open Falcon Flight Data Model
        [ComVisible(false)]
        [Serializable]
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct BMS3FlightData
        {
            // These are outputs from the sim
            public float x;            // Ownship North (Ft)
            public float y;            // Ownship East (Ft)
            public float z;            // Ownship Down (Ft)
            public float xDot;         // Ownship North Rate (ft/sec)
            public float yDot;         // Ownship East Rate (ft/sec)
            public float zDot;         // Ownship Down Rate (ft/sec)
            public float alpha;        // Ownship AOA (Degrees)
            public float beta;         // Ownship Beta (Degrees)
            public float gamma;        // Ownship Gamma (Radians)
            public float pitch;        // Ownship Pitch (Radians)
            public float roll;         // Ownship Roll (Radians)
            public float yaw;          // Ownship Yaw (Radians)
            public float mach;         // Ownship Mach number
            public float kias;         // Ownship Indicated Airspeed (Knots)
            public float vt;           // Ownship True Airspeed (Ft/Sec)
            public float gs;           // Ownship Normal Gs
            public float windOffset;   // Wind delta to FPM (Radians)
            public float nozzlePos;    // Ownship engine nozzle percent open (0-100)
            //        public float nozzlePos2;   // Ownship engine nozzle2 percent open (0-100)
            public float internalFuel; // Ownship internal fuel (Lbs)
            public float externalFuel; // Ownship external fuel (Lbs)
            public float fuelFlow;     // Ownship fuel flow (Lbs/Hour)
            public float rpm;          // Ownship engine rpm (Percent 0-103)
            //        public float rpm2;         // Ownship engine rpm2 (Percent 0-103)
            public float ftit;         // Ownship Forward Turbine Inlet Temp (Degrees C)
            //        public float ftit2;        // Ownship Forward Turbine Inlet Temp2 (Degrees C)
            public float gearPos;      // Ownship Gear position 0 = up, 1 = down;
            public float speedBrake;   // Ownship speed brake position 0 = closed, 1 = 60 Degrees open
            public float epuFuel;      // Ownship EPU fuel (Percent 0-100)
            public float oilPressure;  // Ownship Oil Pressure (Percent 0-100)
            public uint lightBits;      // Cockpit Indicator Lights, one bit per bulb. See enum

            // These are inputs. Use them carefully
            // NB: these do not work when TrackIR device is enabled
            public float headPitch;    // Head pitch offset from design eye (radians)
            public float headRoll;     // Head roll offset from design eye (radians)
            public float headYaw;      // Head yaw offset from design eye (radians)

            // new lights
            public uint lightBits2;   // Cockpit Indicator Lights, one bit per bulb. See enum
            public uint lightBits3;   // Cockpit Indicator Lights, one bit per bulb. See enum

            // chaff/flare
            public float ChaffCount;   // Number of Chaff left
            public float FlareCount;   // Number of Flare left

            // landing gear
            public float NoseGearPos;  // Position of the nose landinggear; caution: full down values defined in dat files
            public float LeftGearPos;  // Position of the left landinggear; caution: full down values defined in dat files
            public float RightGearPos; // Position of the right landinggear; caution: full down values defined in dat files

            // ADI values
            public float AdiIlsHorPos; // Position of horizontal ILS bar
            public float AdiIlsVerPos; // Position of vertical ILS bar

            // HSI states
            public int courseState;    // HSI_STA_CRS_STATE
            public int headingState;   // HSI_STA_HDG_STATE
            public int totalStates;    // HSI_STA_TOTAL_STATES; never set

            // HSI values
            public float courseDeviation;  // HSI_VAL_CRS_DEVIATION
            public float desiredCourse;    // HSI_VAL_DESIRED_CRS
            public float distanceToBeacon;    // HSI_VAL_DISTANCE_TO_BEACON
            public float bearingToBeacon;  // HSI_VAL_BEARING_TO_BEACON
            public float currentHeading;      // HSI_VAL_CURRENT_HEADING
            public float desiredHeading;   // HSI_VAL_DESIRED_HEADING
            public float deviationLimit;      // HSI_VAL_DEV_LIMIT
            public float halfDeviationLimit;  // HSI_VAL_HALF_DEV_LIMIT
            public float localizerCourse;     // HSI_VAL_LOCALIZER_CRS
            public float airbaseX;            // HSI_VAL_AIRBASE_X
            public float airbaseY;            // HSI_VAL_AIRBASE_Y
            public float totalValues;         // HSI_VAL_TOTAL_VALUES; never set

            public float TrimPitch;  // Value of trim in pitch axis, -0.5 to +0.5
            public float TrimRoll;   // Value of trim in roll axis, -0.5 to +0.5
            public float TrimYaw;    // Value of trim in yaw axis, -0.5 to +0.5

            // HSI flags
            public uint hsiBits;      // HSI flags

            //DED Lines
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
            public DED_PFL_LineOfText[] DEDLines;  //25 usable chars
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
            public DED_PFL_LineOfText[] Invert;    //25 usable chars

            //PFL Lines
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
            public DED_PFL_LineOfText[] PFLLines;  //25 usable chars
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
            public DED_PFL_LineOfText[] PFLInvert; //25 usable chars

            //TacanChannel
            public int UFCTChan;
            public int AUXTChan;
            //RWR
            public int RwrObjectCount;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
            public int[] RWRsymbol;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
            public float[] bearing;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
            public uint[] missileActivity;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
            public uint[] missileLaunch;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
            public uint[] selected;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
            public float[] lethality;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
            public uint[] newDetection;

            //fuel values
            public float fwd;
            public float aft;
            public float total;

            public int VersionNum;    //Version of Mem area

            //       public float headX;        // Head X offset from design eye (feet)
            //       public float headY;        // Head Y offset from design eye (feet)
            //       public float headZ;        // Head Z offset from design eye (feet)
            //       public int MainPower;

        }

        // BMS4 Flight Data Model
        [ComVisible(false)]
        [Serializable]
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct BMS4FlightData
        {
            // These are outputs from the sim
            public float x;            // Ownship North (Ft)
            public float y;            // Ownship East (Ft)
            public float z;            // Ownship Down (Ft)
            public float xDot;         // Ownship North Rate (ft/sec)
            public float yDot;         // Ownship East Rate (ft/sec)
            public float zDot;         // Ownship Down Rate (ft/sec)
            public float alpha;        // Ownship AOA (Degrees)
            public float beta;         // Ownship Beta (Degrees)
            public float gamma;        // Ownship Gamma (Radians)
            public float pitch;        // Ownship Pitch (Radians)
            public float roll;         // Ownship Roll (Radians)
            public float yaw;          // Ownship Yaw (Radians)
            public float mach;         // Ownship Mach number
            public float kias;         // Ownship Indicated Airspeed (Knots)
            public float vt;           // Ownship True Airspeed (Ft/Sec)
            public float gs;           // Ownship Normal Gs
            public float windOffset;   // Wind delta to FPM (Radians)
            public float nozzlePos;    // Ownship engine nozzle percent open (0-100)
            //        public float nozzlePos2;   // Ownship engine nozzle2 percent open (0-100)
            public float internalFuel; // Ownship internal fuel (Lbs)
            public float externalFuel; // Ownship external fuel (Lbs)
            public float fuelFlow;     // Ownship fuel flow (Lbs/Hour)
            public float rpm;          // Ownship engine rpm (Percent 0-103)
            //        public float rpm2;         // Ownship engine rpm2 (Percent 0-103)
            public float ftit;         // Ownship Forward Turbine Inlet Temp (Degrees C)
            //        public float ftit2;        // Ownship Forward Turbine Inlet Temp2 (Degrees C)
            public float gearPos;      // Ownship Gear position 0 = up, 1 = down;
            public float speedBrake;   // Ownship speed brake position 0 = closed, 1 = 60 Degrees open
            public float epuFuel;      // Ownship EPU fuel (Percent 0-100)
            public float oilPressure;  // Ownship Oil Pressure (Percent 0-100)
            public uint lightBits;      // Cockpit Indicator Lights, one bit per bulb. See enum

            // These are inputs. Use them carefully
            // NB: these do not work when TrackIR device is enabled
            public float headPitch;    // Head pitch offset from design eye (radians)
            public float headRoll;     // Head roll offset from design eye (radians)
            public float headYaw;      // Head yaw offset from design eye (radians)

            // new lights
            public uint lightBits2;   // Cockpit Indicator Lights, one bit per bulb. See enum
            public uint lightBits3;   // Cockpit Indicator Lights, one bit per bulb. See enum

            // chaff/flare
            public float ChaffCount;   // Number of Chaff left
            public float FlareCount;   // Number of Flare left

            // landing gear
            public float NoseGearPos;  // Position of the nose landinggear; caution: full down values defined in dat files
            public float LeftGearPos;  // Position of the left landinggear; caution: full down values defined in dat files
            public float RightGearPos; // Position of the right landinggear; caution: full down values defined in dat files

            // ADI values
            public float AdiIlsHorPos; // Position of horizontal ILS bar
            public float AdiIlsVerPos; // Position of vertical ILS bar

            // HSI states
            public int courseState;    // HSI_STA_CRS_STATE
            public int headingState;   // HSI_STA_HDG_STATE
            public int totalStates;    // HSI_STA_TOTAL_STATES; never set

            // HSI values
            public float courseDeviation;  // HSI_VAL_CRS_DEVIATION
            public float desiredCourse;    // HSI_VAL_DESIRED_CRS
            public float distanceToBeacon;    // HSI_VAL_DISTANCE_TO_BEACON
            public float bearingToBeacon;  // HSI_VAL_BEARING_TO_BEACON
            public float currentHeading;      // HSI_VAL_CURRENT_HEADING
            public float desiredHeading;   // HSI_VAL_DESIRED_HEADING
            public float deviationLimit;      // HSI_VAL_DEV_LIMIT
            public float halfDeviationLimit;  // HSI_VAL_HALF_DEV_LIMIT
            public float localizerCourse;     // HSI_VAL_LOCALIZER_CRS
            public float airbaseX;            // HSI_VAL_AIRBASE_X
            public float airbaseY;            // HSI_VAL_AIRBASE_Y
            public float totalValues;         // HSI_VAL_TOTAL_VALUES; never set

            public float TrimPitch;  // Value of trim in pitch axis, -0.5 to +0.5
            public float TrimRoll;   // Value of trim in roll axis, -0.5 to +0.5
            public float TrimYaw;    // Value of trim in yaw axis, -0.5 to +0.5

            // HSI flags
            public uint hsiBits;      // HSI flags

            //DED Lines
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
            public DED_PFL_LineOfText[] DEDLines;  //25 usable chars
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
            public DED_PFL_LineOfText[] Invert;    //25 usable chars

            //PFL Lines
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
            public DED_PFL_LineOfText[] PFLLines;  //25 usable chars
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
            public DED_PFL_LineOfText[] PFLInvert; //25 usable chars

            //TacanChannel
            public int UFCTChan;
            public int AUXTChan;
            //RWR
            public int RwrObjectCount;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
            public int[] RWRsymbol;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
            public float[] bearing;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
            public uint[] missileActivity;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
            public uint[] missileLaunch;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
            public uint[] selected;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
            public float[] lethality;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
            public uint[] newDetection;

            //fuel values
            public float fwd;
            public float aft;
            public float total;

            public int VersionNum;    //Version of Mem area

            public float headX;        // Head X offset from design eye (feet)
            public float headY;        // Head Y offset from design eye (feet)
            public float headZ;        // Head Z offset from design eye (feet)
            public int MainPower;


        }
        [ComVisible(false)]
        [Serializable]
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct OSBData
        {
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 20)]
            public OSBLabel[] leftMFD;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 20)]
            public OSBLabel[] rightMFD;
        }
        [ComVisible(false)]
        [Serializable]
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct FlightData2
        {
            int VersionNum;        // Version of FlightData2 mem area
            float nozzlePos2;   // Ownship engine nozzle2 percent open (0-100)
            float rpm2;         // Ownship engine rpm2 (Percent 0-103)
            float ftit2;        // Ownship Forward Turbine Inlet Temp2 (Degrees C)
            float oilPressure2; // Ownship Oil Pressure2 (Percent 0-100)
            float hydPressureA;  // Ownship Hydraulic Pressure A
            float hydPressureB;  // Ownship Hydraulic Pressure B

            byte navMode;  // current mode selected for HSI/eHSI, see NavModes enum for details

            float AAUZ;         // Ownship barometric altitude given by AAU (depends on calibration)
            int AltCalReading;    // barometric altitude calibration (depends on CalType)
            int altBits;        // various altimeter bits, see AltBits enum for details
            float cabinAlt;        // Ownship cabin altitude

            int BupUhfPreset;    // BUP UHF channel preset
            int BupUhfFreq;        // BUP UHF channel frequency

            int powerBits;        // Ownship power bus / generator states, see PowerBits enum for details
            int blinkBits;        // Cockpit indicator lights blink status, see BlinkBits enum for details
            // NOTE: these bits indicate only *if* a lamp is blinking, in addition to the
            // existing on/off bits. It's up to the external program to implement the
            // *actual* blinking.
            int cmdsMode;        // Ownship CMDS mode state, see CmdsModes enum for details
            int currentTime;    // Current time in seconds (max 60 * 60 * 24)

            short vehicleACD;    // Ownship ACD index number, i.e. which aircraft type are we flying.

            byte[] tacanInfo; //TACAN info (new in BMS4)
        }

        [ComVisible(true)]
        [Serializable]
        public enum TacanSources : int
        {
            UFC = 0,
            AUX,
            NUMBER_OF_SOURCES,
        };

        [ComVisible(true)]
        [Serializable]
        public enum TacanBits : byte
        {
            band = 0x01,   // true in this bit position if band is X
            mode = 0x02,   // true in this bit position if domain is air to air
        };


        [ComVisible(false)]
        [Serializable]
        [StructLayout(LayoutKind.Sequential)]
        public struct BMS4FlightData2
        {
            float nozzlePos2;   // Ownship engine nozzle2 percent open (0-100)
            float rpm2;         // Ownship engine rpm2 (Percent 0-103)
            float ftit2;        // Ownship Forward Turbine Inlet Temp2 (Degrees C)
            float oilPressure2; // Ownship Oil Pressure2 (Percent 0-100)
            byte navMode;  // current mode selected for HSI/eHSI (added in BMS4)
            float aauz; // Ownship barometric altitude given by AAU (depends on calibration)
            int AltCalReading;    // barometric altitude calibration (depends on CalType)
            int altBits;        // various altimeter bits, see AltBits enum for details
            int BupUhfPreset;    // BUP UHF channel preset

            int powerBits;        // Ownship power bus / generator states, see PowerBits enum for details
            int blinkBits;        // Cockpit indicator lights blink status, see BlinkBits enum for details
            // NOTE: these bits indicate only *if* a lamp is blinking, in addition to the
            // existing on/off bits. It's up to the external program to implement the
            // *actual* blinking.
            int cmdsMode;        // Ownship CMDS mode state, see CmdsModes enum for details
            int currentTime;    // Current time in seconds (max 60 * 60 * 24)
            char[] vehicleType;    // Ownship vehicle type name: vehicleType[VEHICLE_TYPE_STRING_LENGTH] (VEHICLE_TYPE_STRING_LENGTH = 15)

            [MarshalAs(UnmanagedType.ByValArray, SizeConst = (int)TacanSources.NUMBER_OF_SOURCES)]
            byte[] tacanInfo;      // Tacan band/mode settings for UFC and AUX COMM

        }


        [ComVisible(false)]
        [Serializable]
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct BMS2FlightData
        {
            // These are outputs from the sim
            public float x;            // Ownship North (Ft)
            public float y;            // Ownship East (Ft)
            public float z;            // Ownship Down (Ft)
            public float xDot;         // Ownship North Rate (ft/sec)
            public float yDot;         // Ownship East Rate (ft/sec)
            public float zDot;         // Ownship Down Rate (ft/sec)
            public float alpha;        // Ownship AOA (Degrees)
            public float beta;         // Ownship Beta (Degrees)
            public float gamma;        // Ownship Gamma (Radians)
            public float pitch;        // Ownship Pitch (Radians)
            public float roll;         // Ownship Roll (Radians)
            public float yaw;          // Ownship Yaw (Radians)
            public float mach;         // Ownship Mach number
            public float kias;         // Ownship Indicated Airspeed (Knots)
            public float vt;           // Ownship True Airspeed (Ft/Sec)
            public float gs;           // Ownship Normal Gs
            public float windOffset;   // Wind delta to FPM (Radians)
            public float nozzlePos;    // Ownship engine nozzle percent open (0-100)
            public float nozzlePos2;   // Ownship engine nozzle2 percent open (0-100)
            public float internalFuel; // Ownship internal fuel (Lbs)
            public float externalFuel; // Ownship external fuel (Lbs)
            public float fuelFlow;     // Ownship fuel flow (Lbs/Hour)
            public float rpm;          // Ownship engine rpm (Percent 0-103)
            public float rpm2;         // Ownship engine rpm2 (Percent 0-103)
            public float ftit;         // Ownship Forward Turbine Inlet Temp (Degrees C)
            public float ftit2;        // Ownship Forward Turbine Inlet Temp2 (Degrees C)
            public float gearPos;      // Ownship Gear position 0 = up, 1 = down;
            public float speedBrake;   // Ownship speed brake position 0 = closed, 1 = 60 Degrees open
            public float epuFuel;      // Ownship EPU fuel (Percent 0-100)
            public float oilPressure;  // Ownship Oil Pressure (Percent 0-100)
            public float oilPressure2; // Ownship Oil Pressure2 (Percent 0-100)
            public uint lightBits;    // Cockpit Indicator Lights, one bit per bulb. See enum

            // These are inputs. Use them carefully
            // NB: these do not work when TrackIR device is enabled
            public float headPitch;    // Head pitch offset from design eye (radians)
            public float headRoll;     // Head roll offset from design eye (radians)
            public float headYaw;      // Head yaw offset from design eye (radians)

            // new lights
            public uint lightBits2;   // Cockpit Indicator Lights, one bit per bulb. See enum
            public uint lightBits3;   // Cockpit Indicator Lights, one bit per bulb. See enum

            // chaff/flare
            public float ChaffCount;   // Number of Chaff left
            public float FlareCount;   // Number of Flare left

            // landing gear
            public float NoseGearPos;  // Position of the nose landinggear; caution: full down values defined in dat files
            public float LeftGearPos;  // Position of the left landinggear; caution: full down values defined in dat files
            public float RightGearPos; // Position of the right landinggear; caution: full down values defined in dat files

            // ADI values
            public float AdiIlsHorPos; // Position of horizontal ILS bar
            public float AdiIlsVerPos; // Position of vertical ILS bar

            // HSI states
            public int courseState;    // HSI_STA_CRS_STATE
            public int headingState;   // HSI_STA_HDG_STATE
            public int totalStates;    // HSI_STA_TOTAL_STATES; never set

            // HSI values
            public float courseDeviation;  // HSI_VAL_CRS_DEVIATION
            public float desiredCourse;    // HSI_VAL_DESIRED_CRS
            public float distanceToBeacon;    // HSI_VAL_DISTANCE_TO_BEACON
            public float bearingToBeacon;  // HSI_VAL_BEARING_TO_BEACON
            public float currentHeading;      // HSI_VAL_CURRENT_HEADING
            public float desiredHeading;   // HSI_VAL_DESIRED_HEADING
            public float deviationLimit;      // HSI_VAL_DEV_LIMIT
            public float halfDeviationLimit;  // HSI_VAL_HALF_DEV_LIMIT
            public float localizerCourse;     // HSI_VAL_LOCALIZER_CRS
            public float airbaseX;            // HSI_VAL_AIRBASE_X
            public float airbaseY;            // HSI_VAL_AIRBASE_Y
            public float totalValues;         // HSI_VAL_TOTAL_VALUES; never set

            public float TrimPitch;  // Value of trim in pitch axis, -0.5 to +0.5
            public float TrimRoll;   // Value of trim in roll axis, -0.5 to +0.5
            public float TrimYaw;    // Value of trim in yaw axis, -0.5 to +0.5

            // HSI flags
            public uint hsiBits;      // HSI flags

            //DED Lines
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
            public DED_PFL_LineOfText[] DEDLines;  //25 usable chars
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
            public DED_PFL_LineOfText[] Invert;    //25 usable chars

            //PFL Lines
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
            public DED_PFL_LineOfText[] PFLLines;  //25 usable chars
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
            public DED_PFL_LineOfText[] PFLInvert; //25 usable chars

            //TacanChannel
            public int UFCTChan;
            public int AUXTChan;

            //RWR
            public int RwrObjectCount;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 20)]
            public int[] RWRsymbol;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 20)]
            public float[] bearing;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 20)]
            public uint[] missileActivity;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 20)]
            public uint[] missileLaunch;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 20)]
            public uint[] selected;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 20)]
            public float[] lethality;
            //        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 20)]
            //        public uint[] newDetection;

            //fuel values
            public float fwd;
            public float aft;
            public float total;

            public int VersionNum;    //Version of Mem area
            //       public float headX;        // Head X offset from design eye (feet)
            //       public float headY;        // Head Y offset from design eye (feet)
            //       public float headZ;        // Head Z offset from design eye (feet)
            //       public int MainPower;

        }


        [ComVisible(false)]
        [Serializable]
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct FreeFalcon5FlightData
        {
            // These are outputs from the sim
            public float x;            // Ownship North (Ft)
            public float y;            // Ownship East (Ft)
            public float z;            // Ownship Down (Ft)
            public float xDot;         // Ownship North Rate (ft/sec)
            public float yDot;         // Ownship East Rate (ft/sec)
            public float zDot;         // Ownship Down Rate (ft/sec)
            public float alpha;        // Ownship AOA (Degrees)
            public float beta;         // Ownship Beta (Degrees)
            public float gamma;        // Ownship Gamma (Radians)
            public float pitch;        // Ownship Pitch (Radians)
            public float roll;         // Ownship Roll (Radians)
            public float yaw;          // Ownship Yaw (Radians)
            public float mach;         // Ownship Mach number
            public float kias;         // Ownship Indicated Airspeed (Knots)
            public float vt;           // Ownship True Airspeed (Ft/Sec)
            public float gs;           // Ownship Normal Gs
            public float windOffset;   // Wind delta to FPM (Radians)
            public float nozzlePos;    // Ownship engine nozzle percent open (0-100)
            public float nozzlePos2;   // Ownship engine nozzle2 percent open (0-100)
            public float internalFuel; // Ownship internal fuel (Lbs)
            public float externalFuel; // Ownship external fuel (Lbs)
            public float fuelFlow;     // Ownship fuel flow (Lbs/Hour)
            public float rpm;          // Ownship engine rpm (Percent 0-103)
            public float rpm2;         // Ownship engine rpm2 (Percent 0-103)
            public float ftit;         // Ownship Forward Turbine Inlet Temp (Degrees C)
            public float ftit2;        // Ownship Forward Turbine Inlet Temp2 (Degrees C)
            public float gearPos;      // Ownship Gear position 0 = up, 1 = down;
            public float speedBrake;   // Ownship speed brake position 0 = closed, 1 = 60 Degrees open
            public float epuFuel;      // Ownship EPU fuel (Percent 0-100)
            public float oilPressure;  // Ownship Oil Pressure (Percent 0-100)
            public float oilPressure2; // Ownship Oil Pressure2 (Percent 0-100)
            public uint lightBits;    // Cockpit Indicator Lights, one bit per bulb. See enum

            // These are inputs. Use them carefully
            // NB: these do not work when TrackIR device is enabled
            public float headPitch;    // Head pitch offset from design eye (radians)
            public float headRoll;     // Head roll offset from design eye (radians)
            public float headYaw;      // Head yaw offset from design eye (radians)

            // new lights
            public uint lightBits2;   // Cockpit Indicator Lights, one bit per bulb. See enum
            public uint lightBits3;   // Cockpit Indicator Lights, one bit per bulb. See enum

            // chaff/flare
            public float ChaffCount;   // Number of Chaff left
            public float FlareCount;   // Number of Flare left

            // landing gear
            public float NoseGearPos;  // Position of the nose landinggear; caution: full down values defined in dat files
            public float LeftGearPos;  // Position of the left landinggear; caution: full down values defined in dat files
            public float RightGearPos; // Position of the right landinggear; caution: full down values defined in dat files

            // ADI values
            public float AdiIlsHorPos; // Position of horizontal ILS bar
            public float AdiIlsVerPos; // Position of vertical ILS bar

            // HSI states
            public int courseState;    // HSI_STA_CRS_STATE
            public int headingState;   // HSI_STA_HDG_STATE
            public int totalStates;    // HSI_STA_TOTAL_STATES; never set

            // HSI values
            public float courseDeviation;  // HSI_VAL_CRS_DEVIATION
            public float desiredCourse;    // HSI_VAL_DESIRED_CRS
            public float distanceToBeacon;    // HSI_VAL_DISTANCE_TO_BEACON
            public float bearingToBeacon;  // HSI_VAL_BEARING_TO_BEACON
            public float currentHeading;      // HSI_VAL_CURRENT_HEADING
            public float desiredHeading;   // HSI_VAL_DESIRED_HEADING
            public float deviationLimit;      // HSI_VAL_DEV_LIMIT
            public float halfDeviationLimit;  // HSI_VAL_HALF_DEV_LIMIT
            public float localizerCourse;     // HSI_VAL_LOCALIZER_CRS
            public float airbaseX;            // HSI_VAL_AIRBASE_X
            public float airbaseY;            // HSI_VAL_AIRBASE_Y
            public float totalValues;         // HSI_VAL_TOTAL_VALUES; never set

            public float TrimPitch;  // Value of trim in pitch axis, -0.5 to +0.5
            public float TrimRoll;   // Value of trim in roll axis, -0.5 to +0.5
            public float TrimYaw;    // Value of trim in yaw axis, -0.5 to +0.5

            // HSI flags
            public uint hsiBits;      // HSI flags

            //DED Lines
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
            public DED_PFL_LineOfText[] DEDLines;  //25 usable chars
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
            public DED_PFL_LineOfText[] Invert;    //25 usable chars

            //PFL Lines
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
            public DED_PFL_LineOfText[] PFLLines;  //25 usable chars
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
            public DED_PFL_LineOfText[] PFLInvert; //25 usable chars

            //TacanChannel
            public int UFCTChan;
            public int AUXTChan;

            //RWR
            public int RwrObjectCount;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 20)]
            public int[] RWRsymbol;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 20)]
            public float[] bearing;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 20)]
            public uint[] missileActivity;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 20)]
            public uint[] missileLaunch;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 20)]
            public uint[] selected;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 20)]
            public float[] lethality;
            //[MarshalAs(UnmanagedType.ByValArray, SizeConst = 20)]
            //public uint[] newDetection;

            //fuel values
            public float fwd;
            public float aft;
            public float total;

            public int VersionNum;    //Version of Mem area
            //public float headX;        // Head X offset from design eye (feet)
            //public float headY;        // Head Y offset from design eye (feet)
            //public float headZ;        // Head Z offset from design eye (feet)
            //public int MainPower;
        }


        [ComVisible(false)]
        [Serializable]
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct AFFlightData
        {
            // These are outputs from the sim
            public float x;            // Ownship North (Ft)
            public float y;            // Ownship East (Ft)
            public float z;            // Ownship Down (Ft)
            public float xDot;         // Ownship North Rate (ft/sec)
            public float yDot;         // Ownship East Rate (ft/sec)
            public float zDot;         // Ownship Down Rate (ft/sec)
            public float alpha;        // Ownship AOA (Degrees)
            public float beta;         // Ownship Beta (Degrees)
            public float gamma;        // Ownship Gamma (Radians)
            public float pitch;        // Ownship Pitch (Radians)
            public float roll;         // Ownship Roll (Radians)
            public float yaw;          // Ownship Yaw (Radians)
            public float mach;         // Ownship Mach number
            public float kias;         // Ownship Indicated Airspeed (Knots)
            public float vt;           // Ownship True Airspeed (Ft/Sec)
            public float gs;           // Ownship Normal Gs
            public float windOffset;   // Wind delta to FPM (Radians)
            public float nozzlePos;    // Ownship engine nozzle percent open (0-100)
            //        public float nozzlePos2;   // Ownship engine nozzle2 percent open (0-100)
            public float internalFuel; // Ownship internal fuel (Lbs)
            public float externalFuel; // Ownship external fuel (Lbs)
            public float fuelFlow;     // Ownship fuel flow (Lbs/Hour)
            public float rpm;          // Ownship engine rpm (Percent 0-103)
            //        public float rpm2;         // Ownship engine rpm2 (Percent 0-103)
            public float ftit;         // Ownship Forward Turbine Inlet Temp (Degrees C)
            //        public float ftit2;        // Ownship Forward Turbine Inlet Temp2 (Degrees C)
            public float gearPos;      // Ownship Gear position 0 = up, 1 = down;
            public float speedBrake;   // Ownship speed brake position 0 = closed, 1 = 60 Degrees open
            public float epuFuel;      // Ownship EPU fuel (Percent 0-100)
            public float oilPressure;  // Ownship Oil Pressure (Percent 0-100)
            //        public float oilPressure2; // Ownship Oil Pressure2 (Percent 0-100)
            public uint lightBits;    // Cockpit Indicator Lights, one bit per bulb. See enum

            // These are inputs. Use them carefully
            // NB: these do not work when TrackIR device is enabled
            public float headPitch;    // Head pitch offset from design eye (radians)
            public float headRoll;     // Head roll offset from design eye (radians)
            public float headYaw;      // Head yaw offset from design eye (radians)

            // new lights
            public uint lightBits2;   // Cockpit Indicator Lights, one bit per bulb. See enum
            public uint lightBits3;   // Cockpit Indicator Lights, one bit per bulb. See enum

            // chaff/flare
            public float ChaffCount;   // Number of Chaff left
            public float FlareCount;   // Number of Flare left

            // landing gear
            public float NoseGearPos;  // Position of the nose landinggear; caution: full down values defined in dat files
            public float LeftGearPos;  // Position of the left landinggear; caution: full down values defined in dat files
            public float RightGearPos; // Position of the right landinggear; caution: full down values defined in dat files

            // ADI values
            public float AdiIlsHorPos; // Position of horizontal ILS bar
            public float AdiIlsVerPos; // Position of vertical ILS bar

            // HSI states
            public int courseState;    // HSI_STA_CRS_STATE
            public int headingState;   // HSI_STA_HDG_STATE
            public int totalStates;    // HSI_STA_TOTAL_STATES; never set

            // HSI values
            public float courseDeviation;  // HSI_VAL_CRS_DEVIATION
            public float desiredCourse;    // HSI_VAL_DESIRED_CRS
            public float distanceToBeacon;    // HSI_VAL_DISTANCE_TO_BEACON
            public float bearingToBeacon;  // HSI_VAL_BEARING_TO_BEACON
            public float currentHeading;      // HSI_VAL_CURRENT_HEADING
            public float desiredHeading;   // HSI_VAL_DESIRED_HEADING
            public float deviationLimit;      // HSI_VAL_DEV_LIMIT
            public float halfDeviationLimit;  // HSI_VAL_HALF_DEV_LIMIT
            public float localizerCourse;     // HSI_VAL_LOCALIZER_CRS
            public float airbaseX;            // HSI_VAL_AIRBASE_X
            public float airbaseY;            // HSI_VAL_AIRBASE_Y
            public float totalValues;         // HSI_VAL_TOTAL_VALUES; never set

            public float TrimPitch;  // Value of trim in pitch axis, -0.5 to +0.5
            public float TrimRoll;   // Value of trim in roll axis, -0.5 to +0.5
            public float TrimYaw;    // Value of trim in yaw axis, -0.5 to +0.5

            // HSI flags
            public uint hsiBits;      // HSI flags

            //DED Lines
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
            public DED_PFL_LineOfText[] DEDLines;  //25 usable chars
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
            public DED_PFL_LineOfText[] Invert;    //25 usable chars

            //PFL Lines
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
            public DED_PFL_LineOfText[] PFLLines;  //25 usable chars
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
            public DED_PFL_LineOfText[] PFLInvert; //25 usable chars

            //TacanChannel
            public int UFCTChan;
            public int AUXTChan;

            //RWR
            public int RwrObjectCount;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
            public int[] RWRsymbol;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
            public float[] bearing;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
            public uint[] missileActivity;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
            public uint[] missileLaunch;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
            public uint[] selected;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
            public float[] lethality;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
            public uint[] newDetection;

            //fuel values
            public float fwd;
            public float aft;
            public float total;

            public int VersionNum;    //Version of Mem area
            public float headX;        // Head X offset from design eye (feet)
            public float headY;        // Head Y offset from design eye (feet)
            public float headZ;        // Head Z offset from design eye (feet)
            public int MainPower;
        }

        public float x;            // Ownship North (Ft)
        public float y;            // Ownship East (Ft)
        public float z;            // Ownship Down (Ft)
        public float xDot;         // Ownship North Rate (ft/sec)
        public float yDot;         // Ownship East Rate (ft/sec)
        public float zDot;         // Ownship Down Rate (ft/sec)
        public float alpha;        // Ownship AOA (Degrees)
        public float beta;         // Ownship Beta (Degrees)
        public float gamma;        // Ownship Gamma (Radians)
        public float pitch;        // Ownship Pitch (Radians)
        public float roll;         // Ownship Roll (Radians)
        public float yaw;          // Ownship Yaw (Radians)
        public float mach;         // Ownship Mach number
        public float kias;         // Ownship Indicated Airspeed (Knots)
        public float vt;           // Ownship True Airspeed (Ft/Sec)
        public float gs;           // Ownship Normal Gs
        public float windOffset;   // Wind delta to FPM (Radians)
        public float nozzlePos;    // Ownship engine nozzle percent open (0-100)
        public float nozzlePos2;   // Ownship engine nozzle2 percent open (0-100)
        public float internalFuel; // Ownship internal fuel (Lbs)
        public float externalFuel; // Ownship external fuel (Lbs)
        public float fuelFlow;     // Ownship fuel flow (Lbs/Hour)
        public float rpm;          // Ownship engine rpm (Percent 0-103)
        public float rpm2;         // Ownship engine rpm2 (Percent 0-103)
        public float ftit;         // Ownship Forward Turbine Inlet Temp (Degrees C)
        public float ftit2;        // Ownship Forward Turbine Inlet Temp2 (Degrees C)
        public float gearPos;      // Ownship Gear position 0 = up, 1 = down;
        public float speedBrake;   // Ownship speed brake position 0 = closed, 1 = 60 Degrees open
        public float epuFuel;      // Ownship EPU fuel (Percent 0-100)
        public float oilPressure;  // Ownship Oil Pressure (Percent 0-100)
        public float oilPressure2; // Ownship Oil Pressure2 (Percent 0-100)
        public float hydPressureA;  // Ownship Hydraulic Pressure A
        public float hydPressureB;  // Ownship Hydraulic Pressure B
        public int lightBits;    // Cockpit Indicator Lights, one bit per bulb. See enum

        // These are inputs. Use them carefully
        // NB: these do not work when TrackIR device is enabled
        public float headPitch;    // Head pitch offset from design eye (radians)
        public float headRoll;     // Head roll offset from design eye (radians)
        public float headYaw;      // Head yaw offset from design eye (radians)

        // new lights
        public int lightBits2;   // Cockpit Indicator Lights, one bit per bulb. See enum
        public int lightBits3;   // Cockpit Indicator Lights, one bit per bulb. See enum

        // chaff/flare
        public float ChaffCount;   // Number of Chaff left
        public float FlareCount;   // Number of Flare left

        // landing gear
        public float NoseGearPos;  // Position of the nose landinggear; caution: full down values defined in dat files
        public float LeftGearPos;  // Position of the left landinggear; caution: full down values defined in dat files
        public float RightGearPos; // Position of the right landinggear; caution: full down values defined in dat files

        // ADI values
        public float AdiIlsHorPos; // Position of horizontal ILS bar
        public float AdiIlsVerPos; // Position of vertical ILS bar

        // HSI states
        public int courseState;    // HSI_STA_CRS_STATE
        public int headingState;   // HSI_STA_HDG_STATE
        public int totalStates;    // HSI_STA_TOTAL_STATES; never set

        // HSI values
        public float courseDeviation;  // HSI_VAL_CRS_DEVIATION
        public float desiredCourse;    // HSI_VAL_DESIRED_CRS
        public float distanceToBeacon;    // HSI_VAL_DISTANCE_TO_BEACON
        public float bearingToBeacon;  // HSI_VAL_BEARING_TO_BEACON
        public float currentHeading;      // HSI_VAL_CURRENT_HEADING
        public float desiredHeading;   // HSI_VAL_DESIRED_HEADING
        public float deviationLimit;      // HSI_VAL_DEV_LIMIT
        public float halfDeviationLimit;  // HSI_VAL_HALF_DEV_LIMIT
        public float localizerCourse;     // HSI_VAL_LOCALIZER_CRS
        public float airbaseX;            // HSI_VAL_AIRBASE_X
        public float airbaseY;            // HSI_VAL_AIRBASE_Y
        public float totalValues;         // HSI_VAL_TOTAL_VALUES; never set

        public float TrimPitch;  // Value of trim in pitch axis, -0.5 to +0.5
        public float TrimRoll;   // Value of trim in roll axis, -0.5 to +0.5
        public float TrimYaw;    // Value of trim in yaw axis, -0.5 to +0.5

        // HSI flags
        public int hsiBits;      // HSI flags

        //DED Lines
        public string[] DEDLines;  //25 usable chars
        public string[] Invert;    //25 usable chars

        //PFL Lines
        public string[] PFLLines;  //25 usable chars
        public string[] PFLInvert; //25 usable chars

        //TacanChannel
        public int UFCTChan;
        public int AUXTChan;

        //RWR
        public int RwrObjectCount;
        public int[] RWRsymbol;
        public float[] bearing;
        public int[] missileActivity;
        public int[] missileLaunch;
        public int[] selected;
        public float[] lethality;
        public int[] newDetection;

        //fuel values
        public float fwd;
        public float aft;
        public float total;

        public int VersionNum;    //Version of Mem area
        public float headX;        // Head X offset from design eye (feet)
        public float headY;        // Head Y offset from design eye (feet)
        public float headZ;        // Head Z offset from design eye (feet)
        public int MainPower;
        public byte navMode; //HSI nav mode (new in BMS4)
        public float aauz; //AAU altimeter indicated altitude (new in BMS4)
        public int AltCalReading;    // barometric altitude calibration (depends on CalType)
        public int altBits;        // various altimeter bits, see AltBits enum for details
        public float cabinAlt;        // Ownship cabin altitude

        public int BupUhfPreset;    // BUP UHF channel preset
        public int BupUhfFreq;        // BUP UHF channel frequency

        public int powerBits;        // Ownship power bus / generator states, see PowerBits enum for details
        public int blinkBits;        // Cockpit indicator lights blink status, see BlinkBits enum for details
        // NOTE: these bits indicate only *if* a lamp is blinking, in addition to the
        // existing on/off bits. It's up to the external program to implement the
        // *actual* blinking.
        public int cmdsMode;        // Ownship CMDS mode state, see CmdsModes enum for details
        public int currentTime;        // Current time in seconds (max 60 * 60 * 24)
        public short vehicleACD;    // Ownship ACD index number, i.e. which aircraft type are we flying.

        public byte[] tacanInfo;    //TACAN info (new in BMS4)

        /*
        public OptionSelectButtonLabel[] leftMFD;
        public OptionSelectButtonLabel[] rightMFD;
        public FalconDataFormats DataFormat;
        */

        // PTs variables

        
    }
}