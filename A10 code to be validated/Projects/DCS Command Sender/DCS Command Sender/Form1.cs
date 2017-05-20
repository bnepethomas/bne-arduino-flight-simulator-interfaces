using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Net.Sockets;
using System.IO.Ports;
using System.Net;
using System.Threading;
using System.Windows.Forms;
using SlimDX;
using SlimDX.DirectInput;


namespace DCS_Command_Sender
{
    public partial class Form1 : Form
    {


        // List of all Joysticks connected to the system
        List<SlimDX.DirectInput.Joystick> Sticks = new List<SlimDX.DirectInput.Joystick>();
        // Holds button state of attached Joyasticks
        List<bool[]> lButtonState = new List<bool[]>();

        bool bConfigMode = false;                                      //

        static Socket newsock;
        static EndPoint Remote;
        static EndPoint CDURemote;

        static string CurrentJoyStickGUID;

        const int SizeOfArray = 300;
        const int iMaxNoOfButtons = 6;
        bool[] bSwitchState = new bool[SizeOfArray];



        string[] Stick_Names = new string[iMaxNoOfButtons];
        string[] Stick_GUIDs = new string[iMaxNoOfButtons];
        string[] Button_Names = new string[iMaxNoOfButtons];
        string[] Button_Devices = new string[iMaxNoOfButtons];
        string[] Button_Number_String = new string[iMaxNoOfButtons];
        int[] Button_Number = new int[iMaxNoOfButtons];
        string[] Button_Down_Command_String = new string[iMaxNoOfButtons];
        int[] Button_Down_Command = new int[iMaxNoOfButtons];
        string[] Button_Up_Command_String = new string[iMaxNoOfButtons];
        int[] Button_Up_Command = new int[iMaxNoOfButtons];
        bool[] Button_Hold = new bool[iMaxNoOfButtons];


        static string Changed_GUID = "";
        static string Changed_Description = "";
        static int Changed_Button = 0;
        static string Change_Press_Release = "";


        static bool State_SAS_YAW_L = false ;
        int ButtonNo_SAS_YAW_L = 15;

        Joystick joystick;

        JoystickState state = new JoystickState();


        public Form1()
        {
            InitializeComponent();


        }

        private void Form1_Load(object sender1, EventArgs e)
        {

            tSStatuslbl2.Text = "Listing Joysticks";


            tSStatuslbl2.Text = "Opening Configuration File";
            if (Properties.Settings.Default.Button_Description == "")
            {
                MessageBox.Show("New Configuration - Please Select a Joystick and Save Settings", "No Saved Settings", MessageBoxButtons.OK);
                DrawButtons();
            }
            else
            {
                // Get settings from configuration file
                CurrentJoyStickGUID = Properties.Settings.Default.JoystickGUID;

                // Populate Arrays from Settings files

                try
                {

                    Stick_Names = Properties.Settings.Default.Stick_Names.Split(new Char[] { ',' }, 100, StringSplitOptions.RemoveEmptyEntries);
                    //int t = 0;

                    if (Stick_Names.Length == 0)
                    {

                        Stick_Names = new string[iMaxNoOfButtons];
                        Stick_Names[0] = "First Stick Name";
                    }

                    Stick_GUIDs = Properties.Settings.Default.Stick_GUIDs.Split(new Char[] { ',' }, 100, StringSplitOptions.RemoveEmptyEntries);


                    if (Stick_GUIDs.Length == 0)
                    {

                        Stick_GUIDs = new string[iMaxNoOfButtons];
                        Stick_GUIDs[0] = "First Stick GUID";
                    }

                    Button_Names = Properties.Settings.Default.Button_Description.Split(new Char[] { ',' }, 100, StringSplitOptions.RemoveEmptyEntries);


                    if (Button_Names.Length == 0)
                    {

                        Button_Names = new string[iMaxNoOfButtons];
                        Button_Names[0] = "First Control";
                    }


                    Button_Devices = Properties.Settings.Default.Button_Devices.Split(new Char[] { ',' }, 100, StringSplitOptions.RemoveEmptyEntries);
                    if (Button_Devices.Length == 0)
                    {

                        Button_Devices = new string[iMaxNoOfButtons];
                        Button_Devices[0] = "C38";
                    }


                    Button_Number_String = Properties.Settings.Default.Button_Number.Split(new Char[] { ',' }, 100, StringSplitOptions.RemoveEmptyEntries);
                    for (int b = 0; b < Button_Number_String.Length; b++)
                    {
                        Button_Number[b] = Int32.Parse(Button_Number_String[b]);
                    }


                    Button_Down_Command_String = Properties.Settings.Default.Button_Down_Command.Split(new Char[] { ',' }, 100, StringSplitOptions.RemoveEmptyEntries);
                    for (int b = 0; b < Button_Down_Command_String.Length; b++)
                    {
                        Button_Down_Command[b] = Int32.Parse(Button_Down_Command_String[b]);
                    }

                    Button_Up_Command_String = Properties.Settings.Default.Button_Up_Command.Split(new Char[] { ',' }, 100, StringSplitOptions.RemoveEmptyEntries);
                    for (int b = 0; b < Button_Up_Command_String.Length; b++)
                    {
                        Button_Up_Command[b] = Int32.Parse(Button_Up_Command_String[b]);
                    }


                    DrawButtons();



                    ButtonNo_SAS_YAW_L = Properties.Settings.Default.ButtonNo_SAS_YAW_L;
                    if (ButtonNo_SAS_YAW_L == 0)
                    {
                        this.btn_L_Yaw_State.BackColor = Color.Yellow;
                        this.btn_L_Yaw_State.Text = "Click to Set L Yaw";
                    }
                    else
                    {
                        this.lblYawStatusLeft.Text = ButtonNo_SAS_YAW_L.ToString();
                    }
                }
                catch(Exception darn)
                {
                    MessageBox.Show("Error loadng config file " + darn.ToString());
                }

            }
            
            tSStatuslbl2.Text = "Creating Socket";
            newsock = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
            
            IPEndPoint CDUSender = new IPEndPoint(IPAddress.Loopback, 7780);
            CDURemote = (EndPoint)(CDUSender);

            tSStatuslbl2.Text = "Creating Joystick Device";

            CreateJoystickDevice();


            tSStatuslbl2.Text = "Initialised";
            timer.Interval = 1000 / 12;
            timer.Start();
        }




        public static void cdusendToLUA(String luaDevice, String luaButton, string luaButtonValue)
        {
            int recv;
            byte[] data = new byte[1024];
            byte[] data2 = new byte[1024];

            // Send the button press command to Export.lua
            data = Encoding.ASCII.GetBytes(luaDevice + "," + luaButton + "," + luaButtonValue);
            newsock.SendTo(data, data.Length, SocketFlags.None, CDURemote);

            Console.WriteLine("Sent to LUA - " + luaDevice + "," + luaButton + "," + luaButtonValue);

        }

        private void button1_Click(object sender, EventArgs e)
        {
            cdusendToLUA("C38", "3004", "1.0");
        }

        private void button2_Click(object sender, EventArgs e)
        {
            cdusendToLUA("C38", "3003", "0.0");
        }




        void CreateJoystickDevice()
        {
            // make sure that DirectInput has been initialized
            DirectInput dinput = new DirectInput();

            // 20140416

            foreach (DeviceInstance device in dinput.GetDevices(DeviceClass.GameController, DeviceEnumerationFlags.AttachedOnly))
            {
                // create the device
                try
                {
                    var stick = new SlimDX.DirectInput.Joystick(dinput, device.InstanceGuid);
                    var localbool = new bool[SizeOfArray];
                    stick.SetCooperativeLevel(this, CooperativeLevel.Nonexclusive | CooperativeLevel.Background);
                    lstAttachedSticks.Items.Add(stick.Information.InstanceName + " " + stick.Information.InstanceGuid);
                    stick.Acquire();

                    Sticks.Add(stick);
                    lButtonState.Add(localbool);

                    Console.WriteLine(stick.Information.InstanceName);
                    // MessageBox.Show(stick.Information.InstanceName);
                }
                catch (DirectInputException)
                {
                }
            }


            // Initialise all button states to false
            foreach (bool[] localbol in lButtonState)
            {
                for (int i = 0; i < SizeOfArray; i++)
                {
                    localbol[i] = false;
                }
            }

            if (Sticks.Count == 0)
            {
                MessageBox.Show("There are no joysticks attached to the system.");
                return;
            }



        }


        void ReadImmediateData()
        {
            for (int i = 0; i < Sticks.Count; i++)
            {
                try
                {
                    if (Sticks[i] != null)
                    {
                        if (Sticks[i].Acquire().IsFailure)
                            return;

                        if (Sticks[i].Poll().IsFailure)
                            return;

                        UpdateUI(Sticks[i], i);
                    }
                   
                }
                catch
                {
                }

            }
        }


        void UpdateUI(Joystick localstick, int ListPointer)
        {

            tSStatuslbl1.Text  = DateTime.Now.ToString("HH:mm:ss tt");

            bool[] buttons = state.GetButtons();



            for (int i = 0; i < Sticks.Count; i++)
            {
                try
                {
                    if (Sticks[i] != null)
                    {

                        if (Sticks[i].Acquire().IsFailure)
                            return;

                        if (Sticks[i].Poll().IsFailure)
                            return;

                        // Update the Button array for every attached stick
                        if (HasButtonChanged(Sticks[i], i) == true)
                        {
                            tSStatuslbl2.Text = "Button " + Changed_Description + " "  + Changed_Button.ToString() + " " +  Change_Press_Release + " "  + Changed_GUID;
                            UpdateAllStickButtons();

                            if (!bConfigMode)
                            {
                                for (int j = 0; j < iMaxNoOfButtons; j++)
                                {

                                    if ((Stick_GUIDs[j] == Changed_GUID) && (Button_Number[j] == Changed_Button))
                                    {
                                        // MessageBox.Show("Match found in button search");
                                        if (Change_Press_Release == "Press")
                                        {
                                            lButtonState[ListPointer][j] = buttons[Button_Number[j]];
                                            tSStatuslbl3.Text = Button_Number[j].ToString() + " State changed to on";
                                            UpdateButtonColour("btn_" + j.ToString(), Color.Green);
                                            if ((Button_Down_Command[j] != 0) && (Button_Devices[j] != ""))
                                                cdusendToLUA(Button_Devices[j], Button_Down_Command[j].ToString(), "1.0");

                                        }
                                        else
                                        {
                                            lButtonState[ListPointer][j] = buttons[Button_Number[j]];
                                            tSStatuslbl3.Text = Button_Number[j].ToString() + " State changed to off";
                                            UpdateButtonColour("btn_" + j.ToString(), Color.Red);
                                            if ((Button_Up_Command[j] != 0) && (Button_Devices[j] != ""))
                                                cdusendToLUA(Button_Devices[j], Button_Up_Command[j].ToString(), "0.0");

                                        }

                                    }
                                           
                                    /*
                                    if ((buttons[Button_Number[i]] == true) && (lButtonState[ListPointer][i] == false))
                                    {
                                        lButtonState[ListPointer][i] = buttons[Button_Number[i]];
                                        tSStatuslbl3.Text = Button_Number[i].ToString() + " State changed to on";
                                        UpdateButtonColour("btn_" + i.ToString(), Color.Green);
                                        if ((Button_Down_Command[i] != 0) && (Button_Devices[i] != ""))
                                            cdusendToLUA(Button_Devices[i], Button_Down_Command[i].ToString(), "1.0");

                                    }
                                    else if ((buttons[Button_Number[i]] == false) && (lButtonState[ListPointer][i] == true))
                                    {
                                        lButtonState[ListPointer][i] = buttons[Button_Number[i]];
                                        tSStatuslbl3.Text = Button_Number[i].ToString() + " State changed to off";
                                        UpdateButtonColour("btn_" + i.ToString(), Color.Red);
                                        if ((Button_Up_Command[i] != 0) && (Button_Devices[i] != ""))
                                            cdusendToLUA(Button_Devices[i], Button_Up_Command[i].ToString(), "0.0");
                                    }
                                     * */

                                }
                            }
                            break;
                        }

                    }
                }

                catch
                {
                }

            }





             

     

        }


 



        private void timer_Tick(object sender, EventArgs e)
        {
            ReadImmediateData();
        }

        private void lstAttachedSticks_SelectedIndexChanged(object sender, EventArgs e)
        {
        }



        private void defaultConfigurationToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DialogResult result;
            result = MessageBox.Show("Do you wish to erase all configuration data", "WARNING", MessageBoxButtons.YesNo);
            if (result == System.Windows.Forms.DialogResult.Yes)
            {
                Properties.Settings.Default.Reset();
            }
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string wrkstring;



            MessageBox.Show("Saving Settings","Saving", MessageBoxButtons.OK);


            // Save Stick Names Array
            wrkstring = Stick_Names[0];
            for (int b = 1; b < Stick_Names.Length; b++)
            {
                if (Stick_Names[b] != "")
                {
                    wrkstring = wrkstring + "," + Stick_Names[b];
                }
                else
                    break;

            }
            Properties.Settings.Default.Stick_Names = wrkstring;

            // Save Stick GUIDs Array
            wrkstring = Stick_GUIDs[0];
            for (int b = 1; b < Stick_GUIDs.Length; b++)
            {
                if (Stick_GUIDs[b] != "")
                {
                    wrkstring = wrkstring + "," + Stick_GUIDs[b];
                }
                else
                    break;

            }
            Properties.Settings.Default.Stick_GUIDs = wrkstring;

            // Save Button Names Array
            wrkstring = Button_Names[0];
            for (int b = 1; b < Button_Names.Length; b++)
            {
                if (Button_Names[b] != "")
                {
                    wrkstring = wrkstring + "," + Button_Names[b];
                }
                else
                    break;

            }
            Properties.Settings.Default.Button_Description = wrkstring;


            // Save Button Devices Array
            wrkstring = Button_Devices[0];
            for (int b = 1; b < Button_Devices.Length; b++)
            {
                if (Button_Devices[b] != "")
                {
                    wrkstring = wrkstring + "," + Button_Devices[b];
                }
                else
                    break;

            }
            Properties.Settings.Default.Button_Devices = wrkstring;


            // Save Button Number Array
            wrkstring = Button_Number[0].ToString();
            for (int b = 1; b < Button_Number.Length; b++)
            {
                wrkstring = wrkstring + "," + Button_Number[b].ToString();
            }
            Properties.Settings.Default.Button_Number = wrkstring;

            // Save Button Command Down Array
            wrkstring = Button_Down_Command[0].ToString();
            for (int b = 1; b < Button_Down_Command.Length; b++)
            {
                wrkstring = wrkstring + "," + Button_Down_Command[b].ToString();
            }
            Properties.Settings.Default.Button_Down_Command = wrkstring;


            // Save Button Command Up Array
            wrkstring = Button_Up_Command[0].ToString();
            for (int b = 1; b < Button_Up_Command.Length; b++)
            {
                wrkstring = wrkstring + "," + Button_Up_Command[b].ToString();
            }
            Properties.Settings.Default.Button_Up_Command = wrkstring;


            Properties.Settings.Default.Save();

        }

        private void btn_L_Yaw_State_Click(object sender, EventArgs e)
        {

            tSStatuslbl2.Text = "Please Press a Button";
            this.Refresh();

            bool FoundChange = false;
            int LoopCounter = 0;


            //Grab Current State of Joystick
            bool[] buttons = state.GetButtons();
            for (int c = 0; c < buttons.Length; c++)
            {
                bSwitchState[c] = buttons[c];
            }

            
            // Now loop looking for changes until one is found
            while( !FoundChange & (LoopCounter < 100))
            {
            
                tSStatuslbl1.Text = DateTime.Now.ToString("HH:mm:ss tt");
                tSStatuslbl2.Text = "Please Press a Button " + LoopCounter.ToString();
                this.Refresh();

                LoopCounter++;
                Thread.Sleep(100);


                state = joystick.GetCurrentState();
                buttons = state.GetButtons();

                for (int c = 0; c < buttons.Length; c++)
                {


                    if (buttons[c] != bSwitchState[c])
                    {
                        FoundChange = true;
                        tSStatuslbl2.Text = "Button " + c.ToString() + " pressed";
                        Properties.Settings.Default.ButtonNo_SAS_YAW_L = c;
                        MessageBox.Show("Found a change " + c.ToString());
                    }

                }
            
            }

            if (LoopCounter > 99)
                tSStatuslbl2.Text = "Timeout Selecting a button";
            else
            {
                // Save Button value to temporary value
            }

        }

        private void modifyConfigurationToolStripMenuItem_Click(object sender, EventArgs e)
        {
            
            // Toggles the global state to enable editing of values - by defaulty this is disabled
            // Then enables or disables all text boxes on the form.

            Type type = typeof(System.Windows.Forms.TextBox);


            bConfigMode = !bConfigMode;
            
            foreach (Control control in this.Controls)
                // Only enable/disable text boxes on the form
                if (control.GetType() == type)
                    control.Enabled = bConfigMode;
          

        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            System.Windows.Forms.Application.Exit();
        }

        private void restartToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MessageBox.Show("This isn't working", "Not working", MessageBoxButtons.OK);
            string arguments = string.Empty;
            string[] args = Environment.GetCommandLineArgs();
            for (int i = 1; i < args.Length; i++) // args[0] is always exe path/filename
                arguments += args[i] + " ";

            // Restart current application, with same arguments/parameters
            Application.Exit();
            System.Diagnostics.Process.Start(Application.ExecutablePath, arguments);
        }


        private void DrawButtons()
        {

            const int FirstColumnPos = 120;
            const int ColumnWidth = 120;
            
            for (int b = 0; b < Button_Names.Length; b++)
            {
                if (Button_Names[b] != "")
                {
                    var btn = new Button();
                    btn.Text = Button_Names[b];
                    btn.Tag = b;
                    btn.Location = new Point(ColumnWidth * (b) + FirstColumnPos, 25);
                    btn.Click += (sender, args) =>
                    {

                        int iCurrentButtonPtr = Int32.Parse(((Button)sender).Tag.ToString());
                        // sender is the instance of the button that was clicked
                        //MessageBox.Show(((Button)sender).Tag.ToString() + " was clicked really ");
                        //MessageBox.Show(((Button)sender).Name.ToString());

                        tSStatuslbl2.Text = "Please Press a Button, mapping Buttton " + iCurrentButtonPtr.ToString();
                        this.Refresh();

                        bool FoundChange = false;
                        int LoopCounter = 0;


                        //Grab Current State of Joystick
                        // Sync Button State for all Sticks

                        UpdateAllStickButtons();


                        bool[] buttons = state.GetButtons();


                        // Now loop looking for changes until one is found
                        while (!FoundChange & (LoopCounter < 100))
                        {

                            tSStatuslbl1.Text = DateTime.Now.ToString("HH:mm:ss tt");
                            tSStatuslbl2.Text = "Please Press a Button " + LoopCounter.ToString() + " - mapping Buttton " + iCurrentButtonPtr.ToString(); ;
                            this.Refresh();

                            LoopCounter++;
                            Thread.Sleep(100);

                            for (int i = 0; i < Sticks.Count; i++)
                            {
                                try
                                {
                                    if (Sticks[i] != null)
                                    {

                                        if (Sticks[i].Acquire().IsFailure)
                                            return;

                                        if (Sticks[i].Poll().IsFailure)
                                            return;

                                        // Update the Button array for every attached stick
                                        if (HasButtonChanged(Sticks[i], i) == true)
                                        {
                                            tSStatuslbl2.Text = "Button " + Changed_Button.ToString() + " pressed";

                                            Stick_Names[iCurrentButtonPtr] = Changed_Description;
                                            Stick_GUIDs[iCurrentButtonPtr] = Changed_GUID;
                                            Button_Number[iCurrentButtonPtr] = Changed_Button;
                                            UpdateButtonValue("lbl_Number_" + iCurrentButtonPtr, Changed_Button.ToString());


                                            Properties.Settings.Default.ButtonNo_SAS_YAW_L = Changed_Button;
                                            MessageBox.Show("Found a change " + Changed_Description + " " +  Changed_Button.ToString() + " " + Change_Press_Release + " " + Changed_GUID );
                                            FoundChange = true;
                                            break;
                                        }
                                            
                                    }
                                }

                                catch
                                {
                                    MessageBox.Show("Erro");
                                    
                                }

                            }

                        }

                        if (LoopCounter > 99)
                            tSStatuslbl2.Text = "Timeout Selecting a button";
                        else
                        {
                            // Save Button value to temporary value
                        }

                    };
                    btn.Name = "btn_" + b;
                    Controls.Add(btn);



                    var lbl_Name = new TextBox();
                    lbl_Name.Text = Button_Names[b];
                    lbl_Name.Tag = b;
                    lbl_Name.Location = new Point(ColumnWidth * (b) + FirstColumnPos, 57);
                    lbl_Name.Width = 70;
                    lbl_Name.Click += (sender, args) =>
                    {
                        // sender is the instance of the button that was clicked
                        //MessageBox.Show(((TextBox )sender).Tag.ToString() + " was clicked");
                    };
                    lbl_Name.LostFocus += (sender, args) =>
                    {
                        // Update Name for Button
                        string wrkstring = ((TextBox)sender).Text;

                        int iCurrentButtonPtr = Int32.Parse(((TextBox)sender).Tag.ToString());
                        Button_Names[iCurrentButtonPtr] = ((TextBox)sender).Text;
                        UpdateButtonValue("btn_" + iCurrentButtonPtr, wrkstring);
                    };
                    lbl_Name.Enabled = false;
                    lbl_Name.Name = "lbl_Name_" + b;
                    Controls.Add(lbl_Name);





                    var lbl_Number = new TextBox();
                    lbl_Number.Text = Button_Number[b].ToString();
                    lbl_Number.Tag = b;
                    lbl_Number.Location = new Point(ColumnWidth * (b) + FirstColumnPos, 77);
                    lbl_Number.Width = 70;
                    lbl_Number.Click += (sender, args) =>
                    {
                        // sender is the instance of the button that was clicked
                        //MessageBox.Show(((TextBox )sender).Tag.ToString() + " was clicked");
                    };
                    lbl_Number.LostFocus += (sender, args) =>
                    {
                        // Update Name for Button
                        string wrkstring = ((TextBox)sender).Tag.ToString();
                        Button_Number[Int32.Parse(wrkstring)] = Int32.Parse(((TextBox)sender).Text);
                    };
                    lbl_Number.Enabled = false;
                    lbl_Number.Name = "lbl_Number_" + b;
                    Controls.Add(lbl_Number);


                    var lbl_Device = new TextBox();
                    lbl_Device.Text = Button_Devices[b];
                    lbl_Device.Tag = b;
                    lbl_Device.Location = new Point(ColumnWidth * (b) + FirstColumnPos, 97);
                    lbl_Device.Width = 70;
                    lbl_Device.Click += (sender, args) =>
                    {
                        // sender is the instance of the button that was clicked
                        //MessageBox.Show(((TextBox )sender).Tag.ToString() + " was clicked");
                    };
                    lbl_Device.LostFocus += (sender, args) =>
                    {
                        // Update Device
                        string wrkstring = ((TextBox)sender).Text;

                        int iCurrentButtonPtr = Int32.Parse(((TextBox)sender).Tag.ToString());
                        Button_Devices[iCurrentButtonPtr] = ((TextBox)sender).Text;
                    };
                    lbl_Device.Enabled = false;
                    lbl_Device.Name = "lbl_Device_" + b;
                    Controls.Add(lbl_Device);

                    var lbl_Down_Command = new TextBox();
                    lbl_Down_Command.Text = Button_Down_Command[b].ToString();
                    lbl_Down_Command.Tag = b;
                    lbl_Down_Command.Location = new Point(ColumnWidth * (b) + FirstColumnPos, 117);
                    lbl_Down_Command.Width = 70;
                    lbl_Down_Command.Click += (sender, args) =>
                    {
                        // sender is the instance of the button that was clicked
                        //MessageBox.Show(((TextBox )sender).Tag.ToString() + " was clicked");
                    };
                    lbl_Down_Command.LostFocus += (sender, args) =>
                    {
                        // Update Name for Button
                        string wrkstring = ((TextBox)sender).Tag.ToString();
                        Button_Down_Command[Int32.Parse(wrkstring)] = Int32.Parse(((TextBox)sender).Text);
                    };
                    lbl_Down_Command.Enabled = false;
                    Controls.Add(lbl_Down_Command);

                    var lbl_Up_Command = new TextBox();
                    lbl_Up_Command.Text = Button_Up_Command[b].ToString();
                    lbl_Up_Command.Tag = b;
                    lbl_Up_Command.Location = new Point(ColumnWidth * (b) + FirstColumnPos, 137);
                    lbl_Up_Command.Width = 70;
                    lbl_Up_Command.Click += (sender, args) =>
                    {
                        // sender is the instance of the button that was clicked
                        //MessageBox.Show(((TextBox )sender).Tag.ToString() + " was clicked");
                    };
                    lbl_Up_Command.LostFocus += (sender, args) =>
                    {
                        // Update Name for Button
                        string wrkstring = ((TextBox)sender).Tag.ToString();
                        Button_Up_Command[Int32.Parse(wrkstring)] = Int32.Parse(((TextBox)sender).Text);
                    };
                    lbl_Up_Command.Enabled = false;
                    lbl_Up_Command.Name = "lbl_UpCommand_" + b;
                    Controls.Add(lbl_Up_Command);

                    this.Refresh();

                }
                else
                    break;

            }

         }


        private void UpdateButtonValue(string ControlToUpdate, string ValueToWrite)
        {
            foreach (Control control in this.Controls)
                // Only enable/disable text boxes on the form
                if (control.Name == ControlToUpdate)
                    control.Text = ValueToWrite;

        }

        private void UpdateButtonColour(string ControlToUpdate, Color ColorToWrite)
        {
            foreach (Control control in this.Controls)
                // Only enable/disable text boxes on the form
                if (control.Name == ControlToUpdate)
                    control.BackColor = ColorToWrite;

        }



        private void UpdateAllStickButtons()
        {
            for (int i = 0; i < Sticks.Count; i++)
            {
                try
                {
                    if (Sticks[i] != null)
                    {

                        if (Sticks[i].Acquire().IsFailure)
                            return;

                        if (Sticks[i].Poll().IsFailure)
                            return;

                        UpdateSticksButtons(Sticks[i], i);
                    }
                }

                catch
                {
                }

            }
        }


        private void UpdateSticksButtons(Joystick localstick, int ListPointer)
        {

            // Updates the button array for every stick
            // Get the state of the Joystick
            state = localstick.GetCurrentState();    

            if (Result.Last.IsFailure)
                return;


            bool[] buttons = state.GetButtons();

            for (int b = 0; b < buttons.Length; b++)
            {
                lButtonState[ListPointer][b] = buttons[b];
            }


        }

        private Boolean HasButtonChanged(Joystick localstick, int ListPointer)
        {

            // Updates the button array for every stick
            // Get the state of the Joystick
            state = localstick.GetCurrentState();

            
            Changed_GUID = "";
            Changed_Description = "";
            Changed_Button = 0;
            Change_Press_Release = "";

            bool[] buttons = state.GetButtons();

            for (int b = 0; b < buttons.Length; b++)
            {
                if (lButtonState[ListPointer][b] != buttons[b])
                {
                    Changed_GUID = localstick.Information.InstanceGuid.ToString();
                    Changed_Description = localstick.Information.InstanceName.ToString();
                    Changed_Button = b;
                    if (buttons[b] == true)
                        Change_Press_Release = "Press";
                    else
                        Change_Press_Release = "Release";

                    return true;
                }
            }
            return false;


        }
    }


}
