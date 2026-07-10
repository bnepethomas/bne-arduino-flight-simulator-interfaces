using FSUIPC;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace FSUIPCTest
{
    public partial class Form1 : Form
    {
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

        private Offset<UInt16> altitude = new Offset<UInt16>(0x3324);
        private Offset<int> vSI = new Offset<int>(0x02C8);
        private Offset<uint> radioAltimeter = new Offset<uint>(0x31E4);

        private Offset<double> attitudePitch = new Offset<double>(0x2F70);
        private Offset<double> attitudeBank = new Offset<double>(0x2F78);

        private readonly Offset<long> _annunciators = new Offset<long>(0x2F28);

        private Offset<double> mainBusVoltage = new Offset<double>(0x2840);
        private Offset<double> avionicsBusVoltage = new Offset<double>(0x2850);

        private Offset<UInt16> comm1Frequency = new Offset<UInt16>(0x034E);


        // Bit index -> annunciator name (as documented for offset 0x2F28)
        private static readonly (int Bit, string Name)[] AnnunciatorBits =
        {
            (0,  "ENG_OUT"),                       // Validated
            (1,  "TRANSMISSION_PRESSURE_FAIL"),    // Validated
            (2,  "BATTERY_HOT"),                   // Validated
            (3,  "ENGINE_CHIP"),                   // Validated
            (4,  "BAGGAGE_DOOR"),                  // Validated
            (5,  "SPARE_4"),
            (6,  "GENERATOR_FAIL"),                // Validated
            (7,  "SIMCONNECT_FAIL"),
            (8,  "TRANSMISSION_TEMPERATURE_FAIL"),
            (9,  "LOW_INLET_PRESSURE"),             // Validated
            (10, "ROTOR_LOW"),                      // Validated
            (11, "FUEL_FILTER_FAIL"),
            (12, "BATTERY_WARM"),                   // Validated
            (13, "TRANSMISSION_CHIP"),              // Validated
            (14, "TAIL_ROTOR_CHIP"),                // Validated
            (15, "FUEL_PUMP_FAIL"),                 // Validated
            (16, "SPARE_2"),
            (17, "SPARE_3"),
            (18, "FUEL_LOW"),                       // Validated
            (19, "SPARE_5"),
            (20, "TURBINE_OVER_TEMP_LIGHT"),        // Validated
        };

        // A ListView or set of labels to show the light states
        private ListView _lightList;

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

                // 2. Master Avionics
                this.chkAvionicsMaster.Checked = avionicsMaster.Value > 0;

                // Turbine Out
                double turbineOutPercent = (double)this.turbineOut.Value / 16384;

                // Gas Producer
                double gasProducerPercent = (double)this.gasProducer.Value / 16384 * 100;

                // Engine Oil Pressure
                double engOilPressValue = (double)this.engOilPress.Value / 16384 * 55;
                // Engine Oil Temperature
                double engOilTempValue = (double)this.engOilTemp.Value / 16384 * 140 ;


                // Transmission Oil Pressure
                double transOilPressValue = (double)this.transOilPress.Value / 16384;
                // Transmission Oil Temperature
                double transOilTempValue = (double)this.transOilTemp.Value / 16384;

                // Fuel Percent Quantity
                double fuelPercentQtyPercent = (double)this.fuelPercentQty.Value / (65536 * 128);

                // Fuel Capacity
                double fuelCapacityValue = (double)this.fuelCapacity.Value;
                double fuelCapacityGallons = fuelPercentQtyPercent * fuelCapacityValue;

                // Battery Load Amps   
                double battLoadAmpsValue = (double)this.battLoadAmps.Value;
                // Fuel Pressure
                double fuelPressureValue = (double)this.fuelPressure.Value / 144;

                // Engine Torque
                double torquePercentValue = (double)this.torquePercent.Value / 16384 * 100;

                // Turbine Out Temperature
                double turbineOutTempValue = (double)this.turbineOutTemp.Value / 16384;

                // Rotor RPM
                double rotorRPMValue = (double)this.rotorRPM.Value / 16384 * 100;

                // Engine N2
                double engN2Value = (double)this.engN2.Value / 16384 * 100;

                // Altitude
                double altitudeValue = (double)this.altitude.Value;
                // Vertical Speed Indicator
                double vSIValue = (double)this.vSI.Value * 60 * 3.28084 / 256;
                // Radio Altimeter 
                double radioAltimeterValue = (double)this.radioAltimeter.Value / 65536 * 3.28084;
                // 65536 * 3.28084

                // Attitude Pitch
                double attitudePitchValue = (double)this.attitudePitch.Value;
                // Attitude Bank
                double attitudeBankValue = (double)this.attitudeBank.Value;

                // Main Bus Voltage - Drives all Instruments
                double mainBusVoltageValue = (double)this.mainBusVoltage.Value;
                // Avionics Bus Voltage - Drives Radio Stack and Avionics
                double avionicsBusVoltageValue = (double)this.avionicsBusVoltage.Value;

                // Comm1 Frequency
                int comm1FrequencyValue = (int)this.comm1Frequency.Value;





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
                                   "Comm1 Frequency: 1" + comm1FrequencyValue.ToString("X") + " MHz" + Environment.NewLine;


                this.textBox1.Text = outstring;

                // --- Annunciator bit extraction ---
                long bits = _annunciators.Value;
                UpdateLights(bits);
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
            foreach (var (bit, name) in AnnunciatorBits)
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

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
