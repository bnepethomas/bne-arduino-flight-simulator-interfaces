using FSUIPC;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace FSUIPCTest
{
    public partial class Form1 : Form
    {

        private UdpClient udpServer;
        private const int listenPort = 27001;


        // =====================================
        // DECLARE OFFSETS YOU WANT TO USE HERE
        // =====================================
        private Offset<uint> airspeed = new Offset<uint>(0x02BC);
        private Offset<uint> avionicsMaster = new Offset<uint>(0x2E80);
        private Offset<uint> turbineOut = new Offset<uint>(0x08F0);
        

        private Offset<UInt16> engOilPress = new Offset<UInt16>(0x08BA);
        private Offset<UInt16> engOilTemp = new Offset<UInt16>(0x08B8);

        private Offset<int> transOilPress = new Offset<int>(0x0900);
        private Offset<uint> transOilTemp = new Offset<uint>(0x0904);

        private Offset<uint> fuelPercentQty = new Offset<uint>(0x0B74);
        private Offset<uint> fuelCapacity = new Offset<uint>(0x0B78);

        private Offset<double> battLoadAmps = new Offset<double>(0x282C);
        private Offset<uint> fuelPressure = new Offset<uint>(0x08F8);

        private Offset<uint> torquePercent = new Offset<uint>(0x08F4);
        private Offset<uint> turbineOutTemp = new Offset<uint>(0x08F0);

        private Offset<uint> gasProducer = new Offset<uint>(0x0898);

        private Offset<UInt16> rotorRPM = new Offset<UInt16>(0x0908);
        private Offset<UInt16> engN2= new Offset<UInt16>(0x0896);

        private Offset<Int16> altitude = new Offset<Int16>(0x3324);
        private Offset<int> vSI = new Offset<int>(0x02C8);
        private Offset<uint> radioAltimeter = new Offset<uint>(0x31E4);

        private Offset<double> attitudePitch = new Offset<double>(0x2F70);
        private Offset<double> attitudeBank = new Offset<double>(0x2F78);

        private readonly Offset<long> _annunciators = new Offset<long>(0x2F28);

        private Offset<double> mainBusVoltage = new Offset<double>(0x2840);
        private Offset<double> avionicsBusVoltage = new Offset<double>(0x2850);

        private Offset<UInt16> comm1Frequency = new Offset<UInt16>(0x034E);

        // Pulled Straight from the FSUIPC SDK documentation, these are the offsets for the radio stack.
        private Offset<ushort> com1 = new Offset<ushort>("RadioStack", 0x034E);
        private Offset<ushort> com1Standby = new Offset<ushort>("RadioStack", 0x311A);
        private Offset<ushort> com2 = new Offset<ushort>("RadioStack", 0x3118);
        private Offset<ushort> com2Standby = new Offset<ushort>("RadioStack", 0x311C);
        private Offset<ushort> nav1 = new Offset<ushort>("RadioStack", 0x0350);
        private Offset<ushort> nav1Standby = new Offset<ushort>("RadioStack", 0x311E);
        private Offset<ushort> nav2 = new Offset<ushort>("RadioStack", 0x0352);
        private Offset<ushort> nav2Standby = new Offset<ushort>("RadioStack", 0x3120);
        private Offset<ushort> transponder = new Offset<ushort>("RadioStack", 0x0354);
        // ADF frequencies are split over 2 offsets, the 'main' and 'extended'.
        // Make sure you declare both.
        private Offset<ushort> adf1Main = new Offset<ushort>("RadioStack", 0x034C);
        private Offset<ushort> adf1Extended = new Offset<ushort>("RadioStack", 0x0356);




        // Bit index -> annunciator name (as documented for offset 0x2F28)
        private static readonly (int Bit, string Name, string ShortCode, bool CurrentState)[] AnnunciatorBits =
        {
            (0,  "ENGINE_OUT", "EOUT", false),                          // Validated     ENG_OUT
            (1,  "TRANSMISSION_PRESSURE_FAIL", "TOPW", false),    // Validated
            (2,  "BATTERY_HOT", "BHOT", false),                          // Validated     
            (3,  "ENGINE_CHIP", "EC", false),             // Validated
            (4,  "BAGGAGE_DOOR", "BD", false),                  // Validated
            (5,  "SPARE_4", "SCF", false),
            (6,  "GENERATOR_FAIL", "GENF", false),                // Validated
            (7,  "SIMCONNECT_FAIL", "SCF", false),
            (8,  "TRANSMISSION_TEMPERATURE_FAIL", "TOTW", false),  //TRANSMISSION_TEMPERATURE_FAIL   
            (9,  "LOW_INLET_PRESSURE", "SCF", false),             // Validated
            (10, "ROTOR_LOW", "RLOW", false),                      // Validated    ROTOR_LOW
            (11, "FUEL_FILTER_FAIL", "FFLTR", false),
            (12, "BATTERY_WARM", "BTMP", false),                           // Validated    
            (13, "TRANSMISSION_CHIP", "TC", false),              // Validated    
            (14, "TAIL_ROTOR_CHIP", "TRC", false),                // Validated
            (15, "FUEL_PUMP_FAIL", "FPMP", false),                 // Validated
            (16, "SPARE_2", "SCF", false),
            (17, "SPARE_3", "SCF", false),
            (18, "FUEL_LOW", "LOWF", false),                       // Validated
            (19, "SPARE_5", "SCF", false),
            (20, "TURBINE_OVER_TEMP_LIGHT", "TOT", false),        // Validated
        };




        // A ListView or set of labels to show the light states
        private ListView _lightList;


        enum Servos
        {
            AirSpeed,
            VerticalSpeed,
            AttitudeIndicatorBankDegrees,
            AttitudeIndicatorPitchDegrees,
            RotorRpmPct1,
            GeneralEngPctMaxRpm1,
            EngTorquePercent1,
            ElectricalTotalLoadAmps,
            TurbEngItt1,
            EngOilTemperature1,
            FuelTotalQuantity,
            TurbEngCorrectedN11,
            EngOilPressure1,
            EngTransmissionPressure1,
            EngTransmissionTemperature1,
            PlaneAltAboveGround,
            FuelLoad,
            ElectricalLoad,
            Number_Of_Servos,
        }

        enum ServoShortCodes
        {
            IAS,
            VSI,
            BANK,
            PITCH,
            RPMR,
            RPME,
            TQ,
            AMPS,
            ITT,
            OILT,
            FUEL,
            N1,
            OILP,
            XMSNP,
            XMSNT,
            AGL,
            FLOAD,
            ELOAD,
        }
        enum GROUP_ID
        {
            GROUP0,
        }

        string UDP_Playload;
        string Output_Payload;
        UdpClient udpClient = new UdpClient();
        UdpClient frontPanelClient = new UdpClient();
        UdpClient OutputClient = new UdpClient();
        DateTime RadioTimeLastPacketSent;
        DateTime FrontPanelTimeLastPacketSent = DateTime.Now;
        TimeSpan span;
        int mS;


        // Will calaculate the position of the servo based on the value received from simconnect and the min and max positions defined for each servo.
        // instead of asking the Mega to do that.  This is because the servos have different ranges and it is easier to manage that in code than on the Mega.
        // The ServoZeroPosition array is to deal with gauges such as VSI and Pitch and Bank which have a zero position in the middle of the range.
        // This allows the code to calculate the position of the servo based on the value received from simconnect and the min and max positions defined
        // for each servo, as well as the zero position for those servos that have a zero position in the middle of the range.


        //                                    ASP  VSI  BNK  PCH  RPMR RPME TQ   AMPS ITT  OILT FUEL N1  OILP  XMNP XMNT  AGL FLOAD ELOAD

        //                                    ASP  VSI  BNK  PCH  RPMR RPME TQ   AMPS ITT  OILT FUEL N1  OILP XMNP XMNT AGL FLOAD ELOAD
        long[] ServMinPosition = new long[]  { 173,178,   5, 166, 177, 137, 176, 527, 159, 124, 159, 170, 82, 178, 134, 222, 131, 89 };
        long[] ServMaxPosition = new long[]  { 10,  14, 179,  70,  23,   6,  37, 740,  44, 175,  51,  30, 34, 128, 180, 222, 167, 46 };
        long[] ServZeroPosition = new long[] { 173, 93,  91, 113, 177, 137, 176, 527, 159, 124, 159, 170, 82, 178, 134, 222, 131, 89 };


        long[] ServoPosition = new long[] { 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000 };

        StructFrontPanel sFrontPanel = new StructFrontPanel();

        // Variables to hold front panel data
        // Using StructFrontPanel
        bool frontPanelDataChanged = false;
        // Used to determine if we need tp update certain elements are power has become available.
        bool CIRCUIT_NAVCOM1_CHANGED_ON = false;

        private String ALTITUDE = "";                           // ALT
        private String AIRSPEED = "";                           // IAS
        private String VERTICAL_SPEED = "";                     // VSI
        private String PLANE_ALT_ABOVE_GROUND = "";             // AGL
        private String ATTITUDE_INDICATOR_BANK_DEGREES = "";   // BANK
        private String ATTITUDE_INDICATOR_PITCH_DEGREES = "";  // PITCH
        private String ROTOR_RPM_PCT_1 = "";                    // RPMR
        private String GENERAL_ENG_PCT_MAX_RPM_1 = "";          // RPME
        private String ENG_TORQUE_PERCENT_1 = "";               // TQ
        private String ELECTRICAL_TOTAL_LOAD_AMPS = "";         // AMPS
        private String TURB_ENG_ITT_1 = "";                     // ITT
        private String ENG_OIL_TEMPERATURE_1 = "";              // OILT
        private String FUEL_TOTAL_QUANTITY = "";                // FUEL
        private String TURB_ENG_CORRECTED_N1_1 = "";            // N1
        private String ENG_OIL_PRESSURE_1 = "";                 // OILP 
        private String ENG_TRANSMISSION_PRESSURE_1 = "";        // XMSNP
        private String ENG_TRANSMISSION_TEMPERATURE_1 = "";     // XMSNT
        private String FUEL_LOAD = "";                          // FLOAD
        private String ELECTRICAL_LOAD = "";                    // ELOAD
        private String ELECTRICAL_MASTER_BATTERY = "";          // BATSW
        private String AIRSPEED_2 = "";

        private bool Rotor_RPM_Low = false;         //RLOW
        private bool Engine_Out = false;            //EOUT
        private bool Trans_Oil_Pressure = false;    //TOPW
        private bool Trans_Oil_Temp = false;        //TOTW
        private bool Battery_Temp = false;          //BTMP
        private bool Battery_Hot = false;           //BHOT
        private bool Trans_Chip = false;            //TC
        private bool Baggage_Door = false;          //BD
        private bool Engine_Chip = false;           //EC
        private bool TR_Chip = false;               //TRC
        private bool Fuel_Pump = false;             //FPMP
        private bool AFT_Fuel_Filter = false;       //FFLTR
        private bool Gen_Fail = false;              //GENF
        private bool Low_Fuel = false;              //LOWF
        private bool SC_Fail = false;               //SCF

        public Form1()
        {
            InitializeComponent();
            configureForm();
            BuildLightList();
            // Start the connection timer to look for a flight sim
            this.timerConnection.Start();
        }

        // This method is called every 1 second by the connection timer.
        private void timerConnection_Tick(object sender, EventArgs e)
        {
            // Try to open the connection
            try
            {
                FSUIPCConnection.Open();
                // If there was no problem, stop this timer and start the main timer
                this.timerConnection.Stop();
                this.timerMain.Start();
                // Update the connection status
                configureForm();
            }
            catch
            {
                // No connection found. Don't need to do anything, just keep trying
            }
        }

        // This method runs 20 times per second (every 50ms). This is set on the timerMain properties.
        private void timerMain_Tick(object sender, EventArgs e)
        {
            // Call process() to read/write data to/from FSUIPC
            // We do this in a Try/Catch block incase something goes wrong
            try
            {
                FSUIPCConnection.Process();

                // Update the information on the form
                // (See the Examples Application for more information on using Offsets).

                // 1. Airspeed
                double airspeedKnots = (double)this.airspeed.Value / 128d;
                sFrontPanel.AIRSPEED = airspeedKnots;
                sFrontPanel.AIRSPEED_2 = airspeedKnots;  

                // 2. Master Avionics
                this.chkAvionicsMaster.Checked = avionicsMaster.Value > 0;

                // Turbine Out
                double turbineOutPercent = (double)this.turbineOut.Value / 16384;
                sFrontPanel.TURB_ENG_ITT_1 = turbineOutPercent;

                // Gas Producer
                double gasProducerPercent = (double)this.gasProducer.Value / 16384 * 100;
                sFrontPanel.TURB_ENG_CORRECTED_N1_1 = gasProducerPercent;

                // Engine Oil Pressure
                double engOilPressValue = (double)this.engOilPress.Value / 16384 * 55;
                sFrontPanel.ENG_OIL_PRESSURE_1 = engOilPressValue;
                // Engine Oil Temperature
                double engOilTempValue = (double)this.engOilTemp.Value / 16384 * 140 ;
                sFrontPanel.ENG_OIL_TEMPERATURE_1 = engOilTempValue;


                // Transmission Oil Pressure
                double transOilPressValue = (double)this.transOilPress.Value / 16384;
                sFrontPanel.ENG_TRANSMISSION_PRESSURE_1 = transOilPressValue;
                // Transmission Oil Temperature
                double transOilTempValue = (double)this.transOilTemp.Value / 16384;
                sFrontPanel.ENG_TRANSMISSION_TEMPERATURE_1 = transOilTempValue;

                // Fuel Percent Quantity
                double fuelPercentQtyPercent = (double)this.fuelPercentQty.Value / (65536 * 128);

                // Fuel Capacity
                double fuelCapacityValue = (double)this.fuelCapacity.Value;
                double fuelCapacityGallons = fuelPercentQtyPercent * fuelCapacityValue;
                sFrontPanel.FUEL_TOTAL_QUANTITY = fuelCapacityGallons;

                // Battery Load Amps   
                double battLoadAmpsValue = (double)this.battLoadAmps.Value;
                sFrontPanel.ELECTRICAL_TOTAL_LOAD_AMPS = battLoadAmpsValue;
                // Fuel Pressure
                double fuelPressureValue = (double)this.fuelPressure.Value / 144;
                sFrontPanel.FUEL_PRESSURE = fuelPressureValue;

                // Engine Torque
                double torquePercentValue = (double)this.torquePercent.Value / 16384 * 100;
                sFrontPanel.ENG_TORQUE_PERCENT_1 = torquePercentValue;

                // Turbine Out Temperature
                double turbineOutTempValue = (double)this.turbineOutTemp.Value / 16384;
                sFrontPanel.TURB_ENG_ITT_1 = turbineOutTempValue;

                // Rotor RPM
                double rotorRPMValue = (double)this.rotorRPM.Value / 16384 * 100;
                sFrontPanel.ROTOR_RPM_PCT_1 = rotorRPMValue;

                // Engine N2
                double engN2Value = (double)this.engN2.Value / 16384 * 100;
                sFrontPanel.GENERAL_ENG_PCT_MAX_RPM_1 = engN2Value;

                // Altitude
                double altitudeValue = (double)this.altitude.Value;
                sFrontPanel.ALTITUDE = altitudeValue;
                // Vertical Speed Indicator
                double vSIValue = (double)this.vSI.Value * 60 * 3.28084 / 256;
                sFrontPanel.VERTICAL_SPEED = vSIValue;
                // Radio Altimeter 
                double radioAltimeterValue = (double)this.radioAltimeter.Value / 65536 * 3.28084;
                // 65536 * 3.28084
                sFrontPanel.PLANE_ALT_ABOVE_GROUND = radioAltimeterValue;

                // Attitude Pitch
                double attitudePitchValue = (double)this.attitudePitch.Value;
                sFrontPanel.ATTITUDE_INDICATOR_PITCH_DEGREES = attitudePitchValue;
                // Attitude Bank
                double attitudeBankValue = (double)this.attitudeBank.Value;
                sFrontPanel.ATTITUDE_INDICATOR_BANK_DEGREES = attitudeBankValue;

                // Main Bus Voltage - Drives all Instruments
                double mainBusVoltageValue = (double)this.mainBusVoltage.Value;
                sFrontPanel.ELECTRICAL_MASTER_BATTERY = mainBusVoltageValue;
                // Avionics Bus Voltage - Drives Radio Stack and Avionics
                double avionicsBusVoltageValue = (double)this.avionicsBusVoltage.Value;
                sFrontPanel.CIRCUIT_NAVCOM1_ON = avionicsBusVoltageValue;




                FSUIPCConnection.Process("RadioStack");
                FsFrequencyCOM com1Helper = new FsFrequencyCOM(this.com1.Value);
                FsFrequencyCOM com1StandbyHelper = new FsFrequencyCOM(this.com1Standby.Value);
                FsFrequencyCOM com2Helper = new FsFrequencyCOM(this.com2.Value);
                FsFrequencyCOM com2StandbyHelper = new FsFrequencyCOM(this.com2Standby.Value);
                FsFrequencyNAV nav1Helper = new FsFrequencyNAV(this.nav1.Value);
                FsFrequencyNAV nav1StandbyHelper = new FsFrequencyNAV(this.nav1Standby.Value);
                FsFrequencyNAV nav2Helper = new FsFrequencyNAV(this.nav2.Value);
                FsFrequencyNAV nav2StandbyHelper = new FsFrequencyNAV(this.nav2Standby.Value);
                FsFrequencyADF adf1Helper = new FsFrequencyADF(this.adf1Main.Value, this.adf1Extended.Value);
                FsTransponderCode txHelper = new FsTransponderCode(this.transponder.Value);


                string outstring = "Turbine Out: " + turbineOutPercent.ToString("F0") + " C" + Environment.NewLine +
                                   "Gas Producer: " + gasProducerPercent.ToString("F1") + "%" + Environment.NewLine +
                                   "Engine Oil Pressure: " + engOilPressValue.ToString("F0") + "PSI " + Environment.NewLine +
                                   "Engine Oil Temperature: " + engOilTempValue.ToString("F0") + " C" + Environment.NewLine +
                                   "Transmission Oil Pressure: " + transOilPressValue.ToString("F1") + " PSI" + Environment.NewLine +
                                   "Transmission Oil Temperature: " + transOilTempValue.ToString("F0") + " C" + Environment.NewLine +
                                   "Fuel Capacity: " + fuelCapacityGallons.ToString("F0") + " Gallons" + Environment.NewLine +
                                   "Battery Load Amps: " + battLoadAmpsValue.ToString("F1") + " A" + Environment.NewLine +
                                   "Fuel Pressure: " + fuelPressureValue.ToString("F1") + " PSI" + Environment.NewLine +
                                   "Torque Percent: " + torquePercentValue.ToString("F1") + " %" + Environment.NewLine +
                                   "Turbine Out Temperature: " + turbineOutTempValue.ToString("F1") + " C" + Environment.NewLine +
                                   "Gas Producer: " + gasProducerPercent.ToString("F1") + "%" + Environment.NewLine +
                                   "Air Speed: " + airspeedKnots.ToString("F0") + " Knots" + Environment.NewLine +
                                   "Rotor RPM: " + rotorRPMValue.ToString("F1") + " %" + Environment.NewLine +
                                   "Engine N2: " + engN2Value.ToString("F1") + " %" + Environment.NewLine +
                                   "Altitude: " + altitudeValue.ToString("F0") + " Feet" + Environment.NewLine +
                                   "Vertical Speed Indicator: " + vSIValue.ToString("F0") + " Feet/Minute" + Environment.NewLine +
                                   "Radio Altimeter: " + radioAltimeterValue.ToString("F2") + " Feet" + Environment.NewLine +
                                   "Attitude Pitch: " + attitudePitchValue.ToString("F2") + " Degrees" + Environment.NewLine +
                                   "Attitude Bank: " + attitudeBankValue.ToString("F2") + " Degrees" + Environment.NewLine +
                                   "Main Bus Voltage: " + mainBusVoltageValue.ToString("F1") + " V" + Environment.NewLine +
                                   "Avionics Bus Voltage: " + avionicsBusVoltageValue.ToString("F1") + " V" + Environment.NewLine +
                                   "Comm 1: " + com1Helper.ToString() + Environment.NewLine +
                                   "Comm 1 Standby: " + com1StandbyHelper.ToString() + Environment.NewLine +
                                   "Comm 2: " + com2Helper.ToString() + Environment.NewLine +
                                   "Comm 2 Standby: " + com2StandbyHelper.ToString() + Environment.NewLine +
                                   "NAV 1: " + nav1Helper.ToString() + Environment.NewLine +
                                   "NAV 1 Standby: " + nav1StandbyHelper.ToString() + Environment.NewLine +
                                   "NAV 2: " + nav2Helper.ToString() + Environment.NewLine +
                                   "NAV 2 Standby: " + nav2StandbyHelper.ToString() + Environment.NewLine +
                                   "ADF 1 : " + adf1Helper.ToString() + Environment.NewLine +
                                   "Transponder: " + txHelper.ToString();
                


                this.textBox1.Text = outstring;


                // --- Annunciator bit extraction ---
                long bits = _annunciators.Value;
                UpdateLights(bits);


                UDP_Playload = "D";
                CIRCUIT_NAVCOM1_CHANGED_ON = false;
                // Flag Power is available for guages through Master Electrical Bus 
                if (mainBusVoltageValue >= 20)
                {
                    sFrontPanel.MASTER_ELECTRICAL_BUS_POWERED = true;               // MASTER BUS
                }
                else
                {
                   sFrontPanel.MASTER_ELECTRICAL_BUS_POWERED = false;
                }

                
                // Data is updated, now send it to the front panel via UDP
                if (ALTITUDE != sFrontPanel.ALTITUDE.ToString("F0")) ;
                {
                    frontPanelDataChanged = true;
                    ALTITUDE = sFrontPanel.ALTITUDE.ToString("F0");
                    UDP_Playload = "D,ALT:" + ALTITUDE;
                };

                
                if (AIRSPEED != sFrontPanel.AIRSPEED.ToString("F0")) ;
                {
                    frontPanelDataChanged = true;
                    AIRSPEED = sFrontPanel.AIRSPEED.ToString("F0");
                    int a = (int)(sFrontPanel.AIRSPEED);
                    UDP_Playload = UDP_Playload + ",IAS:" + IAS_Process(a).ToString();
                };


                if (ROTOR_RPM_PCT_1 != sFrontPanel.ROTOR_RPM_PCT_1.ToString()
                    || sFrontPanel.MASTER_ELECTRICAL_BUS_POWERED == true) ;
                {
                    frontPanelDataChanged = true;
                    ROTOR_RPM_PCT_1 = sFrontPanel.ROTOR_RPM_PCT_1.ToString();
                    int a = (int)(sFrontPanel.ROTOR_RPM_PCT_1);
                    UDP_Playload = UDP_Playload + ",RPMR:" + RPMR_Process(a).ToString();
                }
                    ; // End Rotor RPM 


                if (GENERAL_ENG_PCT_MAX_RPM_1 != sFrontPanel.GENERAL_ENG_PCT_MAX_RPM_1.ToString()
                    || sFrontPanel.MASTER_ELECTRICAL_BUS_POWERED == true) ;
                {
                    frontPanelDataChanged = true;
                    GENERAL_ENG_PCT_MAX_RPM_1 = sFrontPanel.GENERAL_ENG_PCT_MAX_RPM_1.ToString();
                    int a = (int)(sFrontPanel.GENERAL_ENG_PCT_MAX_RPM_1);
                    UDP_Playload = UDP_Playload + ",RPME:" + RPME_Process(a).ToString();
                }
                    ;  // End Engine RPM

                
                if (FUEL_TOTAL_QUANTITY != sFrontPanel.FUEL_TOTAL_QUANTITY.ToString()
                    || sFrontPanel. MASTER_ELECTRICAL_BUS_POWERED == true) ;
                {
                    frontPanelDataChanged = true;
                    FUEL_TOTAL_QUANTITY = sFrontPanel.FUEL_TOTAL_QUANTITY.ToString();
                    int a = (int)(sFrontPanel.FUEL_TOTAL_QUANTITY);
                    UDP_Playload = UDP_Playload + ",FUEL:" + FUEL_Process(a).ToString();
                }
                    ;
                
                if (VERTICAL_SPEED != sFrontPanel.VERTICAL_SPEED.ToString("F0")) ;
                {
                    frontPanelDataChanged = true;
                    VERTICAL_SPEED = sFrontPanel.VERTICAL_SPEED.ToString("F0");
                    int a = (int)(sFrontPanel.VERTICAL_SPEED);
                    UDP_Playload = UDP_Playload + ",VSI:" + VSI_Process(a).ToString();
                }
                    ;

                
                if (PLANE_ALT_ABOVE_GROUND != sFrontPanel.PLANE_ALT_ABOVE_GROUND.ToString("F0")
                    || sFrontPanel.MASTER_ELECTRICAL_BUS_POWERED == true);
                // Radar Altitude is affected by NAVCOM1 power state as the radar altimeter is powered by NAVCOM1. If NAVCOM1 was turned on then we need to update the radar altitude value even if it did not change
                // as it would not have been updated while NAVCOM1 was off
                {
                    frontPanelDataChanged = true;
                    if (sFrontPanel.MASTER_ELECTRICAL_BUS_POWERED == true)
                    { 
                        PLANE_ALT_ABOVE_GROUND = sFrontPanel.PLANE_ALT_ABOVE_GROUND.ToString("F0");
                    }
                    else
                    {
                        PLANE_ALT_ABOVE_GROUND = "0";
                    }
                    ;
                    UDP_Playload = UDP_Playload + ",AGL:" + PLANE_ALT_ABOVE_GROUND;
                }
                    ;

            
                if (ATTITUDE_INDICATOR_BANK_DEGREES != sFrontPanel.ATTITUDE_INDICATOR_BANK_DEGREES.ToString("F3")) ;
                {
                    frontPanelDataChanged = true;
                    ATTITUDE_INDICATOR_BANK_DEGREES = sFrontPanel.ATTITUDE_INDICATOR_BANK_DEGREES.ToString("F3");
                    int a = (int)(sFrontPanel.ATTITUDE_INDICATOR_BANK_DEGREES);
                    UDP_Playload = UDP_Playload + ",BANK:" + ROLL_Process(a).ToString();
                }
                    ;
                if (ATTITUDE_INDICATOR_PITCH_DEGREES != sFrontPanel.ATTITUDE_INDICATOR_PITCH_DEGREES.ToString("F3")) ;
                {
                    frontPanelDataChanged = true;
                    ATTITUDE_INDICATOR_PITCH_DEGREES = sFrontPanel.ATTITUDE_INDICATOR_PITCH_DEGREES.ToString("F3");
                    int a = (int)(sFrontPanel.ATTITUDE_INDICATOR_PITCH_DEGREES);
                    UDP_Playload = UDP_Playload + ",PITCH:" + PITCH_Process(a).ToString();
                }
                    ;

            
                if (ENG_TORQUE_PERCENT_1 != (sFrontPanel.ENG_TORQUE_PERCENT_1 * 4 / 9).ToString()) ;
                {
                    frontPanelDataChanged = true;
                    ENG_TORQUE_PERCENT_1 = (sFrontPanel.ENG_TORQUE_PERCENT_1 * 4 / 9).ToString();
                    int a = (int)(sFrontPanel.ENG_TORQUE_PERCENT_1);

                    UDP_Playload = UDP_Playload + ",TQ:" + TQ_Process(a);
                    ;
                }
                    ;

                if (ELECTRICAL_TOTAL_LOAD_AMPS != (sFrontPanel.ELECTRICAL_TOTAL_LOAD_AMPS * 40 / 56).ToString()) ;
                {
                    frontPanelDataChanged = true;
                    ELECTRICAL_TOTAL_LOAD_AMPS = (sFrontPanel.ELECTRICAL_TOTAL_LOAD_AMPS * 40 / 56).ToString();
                    //UDP_Playload = UDP_Playload + ",AMPS:" + ELECTRICAL_TOTAL_LOAD_AMPS;
                    int a = (int)(sFrontPanel.ELECTRICAL_TOTAL_LOAD_AMPS);
                    UDP_Playload = UDP_Playload + ",ELOAD:" + ELOAD_Process(a).ToString();
                }
                    ;

                if (ELECTRICAL_TOTAL_LOAD_AMPS != (sFrontPanel.ELECTRICAL_TOTAL_LOAD_AMPS * 40 / 56).ToString()) ;
                {
                    frontPanelDataChanged = true;
                    ELECTRICAL_TOTAL_LOAD_AMPS = (sFrontPanel.ELECTRICAL_TOTAL_LOAD_AMPS * 40 / 56).ToString();
                    UDP_Playload = UDP_Playload + ",AMPS:" + ELECTRICAL_TOTAL_LOAD_AMPS;
                }
                    ;
                if ( FUEL_LOAD != sFrontPanel.FUEL_PRESSURE.ToString()) ;
                {
                    frontPanelDataChanged = true;
                    FUEL_LOAD = sFrontPanel.FUEL_PRESSURE.ToString();
                    int a = (int)(sFrontPanel.FUEL_PRESSURE);
                    UDP_Playload = UDP_Playload + ",FLOAD:" + FLOAD_Process(a).ToString();
                }
                    ;


                if (TURB_ENG_ITT_1 != sFrontPanel.TURB_ENG_ITT_1.ToString()) ;
                {
                    frontPanelDataChanged = true;
                    TURB_ENG_ITT_1 = sFrontPanel.TURB_ENG_ITT_1.ToString();
                    int a = (int)(sFrontPanel.TURB_ENG_ITT_1);
                    UDP_Playload = UDP_Playload + ",ITT:" + ITT_Process(a).ToString();
                }
                    ;

                if (ENG_OIL_TEMPERATURE_1 != sFrontPanel.ENG_OIL_TEMPERATURE_1.ToString()) ;
                {
                    frontPanelDataChanged = true;
                    ENG_OIL_TEMPERATURE_1 = sFrontPanel.ENG_OIL_TEMPERATURE_1.ToString();
                    int a = (int)(sFrontPanel.ENG_OIL_TEMPERATURE_1);
                    UDP_Playload = UDP_Playload + ",OILT:" + OILT_Process(a).ToString();
                }
                    ;



                if (TURB_ENG_CORRECTED_N1_1 != sFrontPanel.TURB_ENG_CORRECTED_N1_1.ToString()) ;
                {
                    frontPanelDataChanged = true;
                    TURB_ENG_CORRECTED_N1_1 = sFrontPanel.TURB_ENG_CORRECTED_N1_1.ToString();
                    int a = (int)(sFrontPanel.TURB_ENG_CORRECTED_N1_1);
                    UDP_Playload = UDP_Playload + ",N1:" + N1_Process(a).ToString();

                }
                    ;

                if (ENG_OIL_PRESSURE_1 != sFrontPanel.ENG_OIL_PRESSURE_1.ToString()) ;
                {
                    frontPanelDataChanged = true;
                    ENG_OIL_PRESSURE_1 = sFrontPanel.ENG_OIL_PRESSURE_1.ToString();
                    int a = (int)(sFrontPanel.ENG_OIL_PRESSURE_1);
                    UDP_Playload = UDP_Playload + ",OILP:" + OILP_Process(a).ToString();
                }
                    ;

                if (ENG_TRANSMISSION_PRESSURE_1 != sFrontPanel.ENG_TRANSMISSION_PRESSURE_1.ToString()) ;
                {
                    frontPanelDataChanged = true;
                    ENG_TRANSMISSION_PRESSURE_1 = sFrontPanel.ENG_TRANSMISSION_PRESSURE_1.ToString();
                    int a = (int)(sFrontPanel.ENG_TRANSMISSION_PRESSURE_1);
                    UDP_Playload = UDP_Playload + ",XMSNP:" + XMSNP_Process(a).ToString();
                }
                    ;

                if (ENG_TRANSMISSION_TEMPERATURE_1 != sFrontPanel.ENG_TRANSMISSION_TEMPERATURE_1.ToString()) ;
                {
                    frontPanelDataChanged = true;
                    ENG_TRANSMISSION_TEMPERATURE_1 = sFrontPanel.ENG_TRANSMISSION_TEMPERATURE_1.ToString();
                    int a = (int)(sFrontPanel.ENG_TRANSMISSION_TEMPERATURE_1);
                    UDP_Playload = UDP_Playload + ",XMSNT:" + XMSNT_Process(a).ToString();
                }; 

                if (ELECTRICAL_MASTER_BATTERY != sFrontPanel.ELECTRICAL_MASTER_BATTERY.ToString()) ;
                {
                    frontPanelDataChanged = true;
                    ELECTRICAL_MASTER_BATTERY = sFrontPanel.ELECTRICAL_MASTER_BATTERY.ToString();
                    UDP_Playload = UDP_Playload + ",BATSW:" + ELECTRICAL_MASTER_BATTERY;
                };

                
                for (int lightptr = 0; lightptr < _lightList.Items.Count; lightptr++)
                {
                    if (_lightList.Items[lightptr].SubItems[1].Text == "ON")
                    {
                        frontPanelDataChanged = true;
                        UDP_Playload = UDP_Playload + "," + AnnunciatorBits[lightptr].ShortCode.ToString() + ":1";
                    }
                    else
                    {
                        frontPanelDataChanged = true;
                        UDP_Playload = UDP_Playload + "," + AnnunciatorBits[lightptr].ShortCode.ToString() + ":0";
                    }
                }


                span = DateTime.Now - FrontPanelTimeLastPacketSent;
                mS = (int)span.TotalMilliseconds;
                // displayText("Its been this many mS since sending last packet: " + mS.ToString());

                if (mS >= 200 && frontPanelDataChanged == true)
                {
                    frontPanelDataChanged = false;



                    //UDP_Playload += ",TOPW:" + Trans_Oil_Pressure.ToString();
                    //UDP_Playload += ",TOWT:" + Trans_Oil_Temp.ToString();
                    //UDP_Playload += ",BTMP:" + Battery_Temp.ToString();
                    //UDP_Playload += ",BHOT:" + Battery_Hot.ToString();
                    //UDP_Playload += ",TC:" + Trans_Chip.ToString();
                    //UDP_Playload += ",BD:" + Baggage_Door.ToString();
                    //UDP_Playload += ",EC:" + Engine_Chip.ToString();
                    //UDP_Playload += ",TRC:" + TR_Chip.ToString();
                    //UDP_Playload += ",FPMP:" + Fuel_Pump.ToString();
                    //UDP_Playload += ",FFLTR:" + AFT_Fuel_Filter.ToString();
                    //UDP_Playload += ",GENF:" + Gen_Fail.ToString();
                    //UDP_Playload += ",SCF:" + SC_Fail.ToString();


                    if (UDP_Playload != "D")
                    {
                        // Something has changed so send it
                        Byte[] senddata = Encoding.ASCII.GetBytes(UDP_Playload);
                        frontPanelClient.Send(senddata, senddata.Length);
                        FrontPanelTimeLastPacketSent = DateTime.Now;
                    }

                }


            }
            catch (Exception ex)
            {
                // An error occured. Tell the user and stop this timer.
                this.timerMain.Stop();
                MessageBox.Show("Communication with FSUIPC Failed\n\n" + ex.Message, "FSUIPC", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                // Update the connection status
                configureForm();
                // start the connection timer
                this.timerConnection.Start();
            }
        }

        // This runs when the master avionics tick has been changed
        private void chkAvionicsMaster_CheckedChanged(object sender, EventArgs e)
        {
            // Update the FSUIPC offset with the new value (1 = Checked/On, 0 = Unchecked/Off)
            this.avionicsMaster.Value = (uint)(this.chkAvionicsMaster.Checked ? 1 : 0);
        }

        private void BuildLightList()
        {
            _lightList = new ListView
            {
                View = View.Details,
                Location = new System.Drawing.Point(320, 20),
                Size = new System.Drawing.Size(300, 460),
                FullRowSelect = true,
                GridLines = true,
                HeaderStyle = ColumnHeaderStyle.Nonclickable
            };
            _lightList.Columns.Add("Annunciator", 220);
            _lightList.Columns.Add("State", 70);

            // Pre-create one row per bit
            foreach (var (bit, name, abbreviation, ShortCode) in AnnunciatorBits)
            {
                var item = new ListViewItem(name);
                item.SubItems.Add("OFF");
                item.Tag = bit;
                _lightList.Items.Add(item);
            }

            this.Controls.Add(_lightList);
            this.ClientSize = new System.Drawing.Size(330, 600);
        }

        private void UpdateLights(long bits)
        {
            _lightList.BeginUpdate();
            foreach (ListViewItem item in _lightList.Items)
            {
                int bit = (int)item.Tag;
                bool isOn = IsBitSet(bits, bit);

                item.SubItems[1].Text = isOn ? "ON" : "OFF";
                item.BackColor = isOn
                    ? System.Drawing.Color.FromArgb(255, 90, 90)   // red-ish when active
                    : System.Drawing.Color.White;
            }

            _lightList.EndUpdate();
        }

        /// <summary>Returns true if the given bit index (0-based) is set.</summary>
        private static bool IsBitSet(long value, int bitIndex)
        {
            return (value & (1L << bitIndex)) != 0;
        }


        struct StructFrontPanel
        {
            public String title;
            public double ALTITUDE;                                     // ALT
            public double AIRSPEED;                                     // IAS
            public double VERTICAL_SPEED;                               // VSI
            public double PLANE_ALT_ABOVE_GROUND;                       // AGL
            public double ATTITUDE_INDICATOR_BANK_DEGREES;              // BANK
            public double ATTITUDE_INDICATOR_PITCH_DEGREES;             // PITCH
            public double ROTOR_RPM_PCT_1;                              // RPMR
            public double GENERAL_ENG_PCT_MAX_RPM_1;                    // RPME
            public double ENG_TORQUE_PERCENT_1;                         // TQ
            public double ELECTRICAL_TOTAL_LOAD_AMPS;                   // AMPS
            public double TURB_ENG_ITT_1;                               // ITT
            public double ENG_OIL_TEMPERATURE_1;                        // OILT
            public double FUEL_TOTAL_QUANTITY;                          // FUEL
            public double TURB_ENG_CORRECTED_N1_1;                      // N1
            public double ENG_OIL_PRESSURE_1;                           // OILP 
            public double ENG_TRANSMISSION_PRESSURE_1;                  // XMSNP
            public double ENG_TRANSMISSION_TEMPERATURE_1;               // XMSNT
            public double ELECTRICAL_MASTER_BATTERY;                    // BATSW
            public double CIRCUIT_NAVCOM1_ON;                           // NAVCOM1
            public double FUEL_PRESSURE;                                // FUELP?
            public bool MASTER_ELECTRICAL_BUS_POWERED;                  // MASTER BUS
            public double AIRSPEED_2;                                   // IAS
        };


        private long Mapper(long value, long fromLow, long fromHigh, long toLow, long toHigh)
        {
            /// <summary>
            /// Re-maps a number from one range to another, similar to the Arduino map() function.
            /// </summary>
            /// <param name="value">The input value to map.</param>
            /// <param name="fromLow">The lower bound of the value's current range.</param>
            /// <param name="fromHigh">The upper bound of the value's current range.</param>
            /// <param name="toLow">The lower bound of the value's target range.</param>
            /// <param name="toHigh">The upper bound of the value's target range.</param>
            /// <returns>The mapped value in the target range.</returns>


            // The formula for the Arduino map() function
            // Need to check how this handles the from being higher than the to range
            // and whether it handles values outside the from range.
            // It should handle both of those cases but need to test that.
            return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;

        }

        private int TQ_Process(int newValue)
        {
            // If the short code is TQ then we need to convert the value from a percentage to the corresponding value for the front panel
            // The front panel expects a value between 37 and 176 for the torque servo, which corresponds to 0% and 100% respectively.
            // So we need to map the input value (0-100) to the range of 37-176.
            int mappedvalue = (int)Mapper(newValue, 0, 120,
                ServMinPosition[(int)Servos.EngTorquePercent1], ServMaxPosition[(int)Servos.EngTorquePercent1]);
            return mappedvalue;
        }

        private int IAS_Process(long newValue)
        {
            // 0  - 173
            // 20 - 165
            // 40 - 142
            // 60 - 115
            // 80 - 84
            // 90 - 74
            //100 - 55
            //110 - 50
            //120 - 40
            //130 - 29
            //140 - 22
            //150 - 10
            switch (newValue)
            {
                case <= 20:
                    return (int)Mapper(newValue, 0, 20, 173, 165);
                case <= 40:
                    return (int)Mapper(newValue, 20, 40, 165, 142);
                case <= 60:
                    return (int)Mapper(newValue, 40, 60, 142, 115);
                case <= 80:
                    return (int)Mapper(newValue, 60, 80, 115, 84);
                case <= 90:
                    return (int)Mapper(newValue, 80, 90, 84, 74);
                case <= 100:
                    return (int)Mapper(newValue, 90, 100, 74, 55);
                case <= 110:
                    return (int)Mapper(newValue, 100, 110, 55, 50);
                case <= 120:
                    return (int)Mapper(newValue, 110, 120, 50, 40);
                case <= 130:
                    return (int)Mapper(newValue, 120, 130, 40, 29);
                case <= 140:
                    return (int)Mapper(newValue, 130, 140, 29, 22);
                case <= 150:
                    return (int)Mapper(newValue, 140, 150, 22, 10);
                case > 150:
                    return 10; ;

            }
        }


        private int RPMR_Process(long newValue)
        {
            // If the short code is TQ then we need to convert the value from a percentage to the corresponding value for the front panel
            // The front panel expects a value between 37 and 176 for the torque servo, which corresponds to 0% and 100% respectively.
            // So we need to map the input value (0-100) to the range of 37-176.
            int mappedvalue = (int)Mapper(newValue, 0, 120,
                ServMinPosition[(uint)(Servos.RotorRpmPct1)], ServMaxPosition[(uint)(Servos.RotorRpmPct1)]);
            return mappedvalue;
        }

        private int RPME_Process(long newValue)
        {
            // If the short code is TQ then we need to convert the value from a percentage to the corresponding value for the front panel
            // The front panel expects a value between 37 and 176 for the torque servo, which corresponds to 0% and 100% respectively.
            // So we need to map the input value (0-100) to the range of 37-176.
            int mappedvalue = (int)Mapper(newValue, 0, 120,
                ServMinPosition[(uint)(Servos.GeneralEngPctMaxRpm1)], ServMaxPosition[(uint)(Servos.GeneralEngPctMaxRpm1)]);
            return mappedvalue;
        }

        private int FUEL_Process(long newValue)
        {
            int mappedvalue = (int)Mapper(newValue, 0, 75,
                ServMinPosition[(uint)(Servos.FuelTotalQuantity)], ServMaxPosition[(uint)(Servos.FuelTotalQuantity)]);
            return mappedvalue;
        }

        private int VSI_Process(long newValue)
        {
            //-6000 - 178
            //-4000 - 161
            //-2000 - 134
            //-1000 - 115
            // 0    - 93
            // 1000 - 70
            // 2000 - 52
            // 4000 - 28
            // 6000 - 15
            switch (newValue)
            {
                case <= -6000:
                    return 178;
                case <= -4000:
                    return (int)Mapper(newValue, -6000, -4000, 178, 161);
                case <= -2000:
                    return (int)Mapper(newValue, -4000, -2000, 161, 134);
                case <= -1000:
                    return (int)Mapper(newValue, -2000, -1000, 134, 115);
                case <= 0:
                    return (int)Mapper(newValue, -1000, 0, 115, 93);
                case <= 1000:
                    return (int)Mapper(newValue, 0, 1000, 93, 70);
                case <= 2000:
                    return (int)Mapper(newValue, 1000, 2000, 70, 52);
                case <= 4000:
                    return (int)Mapper(newValue, 2000, 4000, 52, 28);
                case <= 6000:
                    return (int)Mapper(newValue, 4000, 6000, 28, 15);
                case > 6000:
                    return 15;

                default:
                    return 93;
            }

        }

        private int OILP_Process(long newValue)
        {
            int mappedvalue = (int)Mapper(newValue, 0, 150,
                ServMinPosition[(uint)(Servos.EngOilPressure1)], ServMaxPosition[(uint)(Servos.EngOilPressure1)]);
            return mappedvalue;
        }
        private int OILT_Process(long newValue)
        {
            int mappedvalue = (int)Mapper(newValue, 0, 150,
                ServMinPosition[(uint)(Servos.EngOilTemperature1)], ServMaxPosition[(uint)(Servos.EngOilTemperature1)]);
            return mappedvalue;
        }

        private int XMSNP_Process(long newValue)
        {

            int mappedvalue = (int)Mapper(newValue, 0, 110,
                ServMinPosition[(uint)(Servos.EngTransmissionPressure1)], ServMaxPosition[(uint)(Servos.EngTransmissionPressure1)]);
            return mappedvalue;
        }
        private int XMSNT_Process(long newValue)
        {
            int mappedvalue = (int)Mapper(newValue, 0, 110,
                ServMinPosition[(uint)(Servos.EngTransmissionTemperature1)], ServMaxPosition[(uint)(Servos.EngTransmissionTemperature1)]);
            return mappedvalue;
        }

        private int ITT_Process(long newValue)
        {
            // 0 - 159
            // 1 - 159
            // 600 - 134
            // 700 - 101
            // 800 - 65
            // 900 - 44

            /*
            float myvalue = (float)(newValue);
            myvalue -= 491;
            myvalue = (float)(myvalue * (3.8 / 9.0));
            myvalue = (int)(myvalue);
            newValue = (long)myvalue;
            */
            //myvalue = (myvalue − 491) *(5 / 9));
            switch (newValue)
            {
                case <= 100:
                    return (int)(159);
                case <= 600:
                    return (int)(Mapper(newValue, 100, 600, 159, 134));
                case <= 700:
                    return (int)(Mapper(newValue, 600, 700, 134, 101));
                case <= 800:
                    return (int)(Mapper(newValue, 700, 800, 101, 65));
                case <= 900:
                    return (int)(Mapper(newValue, 800, 900, 65, 44));
                default:
                    return (int)(44);
            }

        }

        private int PITCH_Process(long newValue)
        {
            newValue = newValue * -1;
            int mappedvalue = (int)Mapper(newValue, -90, +90,
                ServMinPosition[(uint)(Servos.AttitudeIndicatorPitchDegrees)], ServMaxPosition[(uint)(Servos.AttitudeIndicatorPitchDegrees)]);
            return mappedvalue;
        }

        private int ROLL_Process(long newValue)
        {
            int mappedvalue = (int)Mapper(newValue, -90, 90,
                ServMinPosition[(uint)(Servos.AttitudeIndicatorBankDegrees)], ServMaxPosition[(uint)(Servos.AttitudeIndicatorBankDegrees)]);
            return mappedvalue;
        }

        private int N1_Process(long newValue)
        {
            int mappedvalue = (int)Mapper(newValue, 0, 100,
                ServMinPosition[(uint)(Servos.TurbEngCorrectedN11)], ServMaxPosition[(uint)(Servos.TurbEngCorrectedN11)]);
            return mappedvalue;
        }

        private int FLOAD_Process(long newValue)
        {
            int mappedvalue = (int)Mapper(newValue, 0, 30,
                ServMinPosition[(uint)(Servos.FuelLoad)], ServMaxPosition[(uint)(Servos.FuelLoad)]);
            return mappedvalue;
        }

        private int ELOAD_Process(long newValue)
        {
             int mappedvalue = (int)Mapper(newValue, 0, 100,
                ServMinPosition[(uint)(Servos.ElectricalLoad)], ServMaxPosition[(uint)(Servos.ElectricalLoad)]);
            return mappedvalue;
        }



        // Configures the status label depending on if we're connected or not 
        private void configureForm()
        {
            if (FSUIPCConnection.IsOpen)
            {
                this.lblConnectionStatus.Text = "Connected to " + FSUIPCConnection.FlightSimVersionConnected.ToString();
                this.lblConnectionStatus.ForeColor = Color.Green;
            }
            else
            {
                this.lblConnectionStatus.Text = "Disconnected. Looking for Flight Simulator...";
                this.lblConnectionStatus.ForeColor = Color.Red;
            }
        }

        // Form is closing so stop all the timers and close FSUIPC Connection
        private void frmMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            this.timerConnection.Stop();
            this.timerMain.Stop();
            FSUIPCConnection.Close();
        }


    }
}
