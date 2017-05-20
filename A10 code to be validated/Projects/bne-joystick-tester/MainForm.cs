/*
* Copyright (c) 2007-2009 SlimDX Group
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
using System;
using System.Globalization;
using System.Windows.Forms;
using SlimDX;
using SlimDX.DirectInput;
using System.Collections.Generic;
using System.Speech.Synthesis;
using System.Threading;

namespace JoystickTest
{
    public partial class MainForm : Form
    {
        Joystick joystick;

        JoystickState state = new JoystickState();

        // List of all Joysticks connected to the system
        List<SlimDX.DirectInput.Joystick> Sticks = new List<SlimDX.DirectInput.Joystick>();
        // Holds button state of attached Joyasticks
        List<bool[]> lButtonState = new List<bool[]>();

        int numPOVs;
        int SliderCount;
        int iCurrentButtonPointer;
        const int SizeOfArray = 300;
        bool[] bSwitchState = new bool[SizeOfArray];
        bool bFirstTime = true;

       


        SpeechSynthesizer synth = new SpeechSynthesizer();
        PromptBuilder builder = new PromptBuilder();

        void CreateDevice()
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


            // set the timer to go off 12 times a second to read input
            // NOTE: Normally applications would read this much faster.
            // This rate is for demonstration purposes only.
            timer.Interval = 1000 /12;
            bFirstTime = true;
            tmrNoSpeak.Start();
            timer.Start();
        }

        void SetDevice(string GuidToMonitor)
        {
            string wrkstr;
            GuidToMonitor = GuidToMonitor.Trim();
            timer.Stop();
            // make sure that DirectInput has been initialized
            DirectInput dinput = new DirectInput();

            // search for devices
            foreach (DeviceInstance device in dinput.GetDevices(DeviceClass.GameController, DeviceEnumerationFlags.AttachedOnly))
            {
                // create the device
                try
                {
                    joystick = new Joystick(dinput, device.InstanceGuid);
                    joystick.SetCooperativeLevel(this, CooperativeLevel.Exclusive | CooperativeLevel.Foreground);
                    wrkstr = joystick.Information.InstanceGuid.ToString();
                    if (wrkstr  == GuidToMonitor )
                        break;
                }
                catch (DirectInputException)
                {
                }
            }

            if (joystick == null)
            {
                MessageBox.Show("There are no joysticks attached to the system.");
                return;
            }

            foreach (DeviceObjectInstance deviceObject in joystick.GetObjects())
            {
                if ((deviceObject.ObjectType & ObjectDeviceType.Axis) != 0)
                    joystick.GetObjectPropertiesById((int)deviceObject.ObjectType).SetRange(-1000, 1000);

                UpdateControl(deviceObject);
            }

            // acquire the device
            joystick.Acquire();


            // set the timer to go off 12 times a second to read input
            // NOTE: Normally applications would read this much faster.
            // This rate is for demonstration purposes only.
            timer.Interval = 1000 / 12;
            bFirstTime = true;
            tmrNoSpeak.Start();
            timer.Start();
        }

        void ReadImmediateData()
        {


            // Clear last button state box
            this.label_ButtonList.Text = "";

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


                        // MessageBox.Show("Index of Joystick is " + i.ToString() + " " + Sticks[i].Information.InstanceName.ToString());

                        UpdateUI(Sticks[i], i);

                        // delay while troublshooting
                        //Thread.Sleep(1000);

                    }
                    //localstick = null;
                }
  
                catch
                {
                }
 
            }
            

        }

        void ReleaseDevice()
        {
            
            timer.Stop();

            foreach (Joystick localstick in Sticks)
            {
                try
                {
                    if (localstick != null)
                    {
                        localstick.Unacquire();
                        localstick.Dispose();
                    }
                    //localstick = null;
                }
                catch
                {
                }
            }
        }

        #region Boilerplate

        public MainForm()
        {
            InitializeComponent();
            //UpdateUI();
        }

        private void timer_Tick(object sender, EventArgs e)
        {
            ReadImmediateData();
        }

        private void exitButton_Click(object sender, EventArgs e)
        {
            ReleaseDevice();
            Close();
        }

        void UpdateUI( Joystick localstick, int ListPointer)
        {

            
            // Get the state of the Joystick
            state = localstick.GetCurrentState();
            label_ButtonList.Text = label_ButtonList.Text + " " + localstick.Information.InstanceName.ToString() + "-";


            if (Result.Last.IsFailure)
                return;
            
            if (joystick == null)
                createDeviceButton.Text = "Create Device";
            else
                createDeviceButton.Text = "Release Device";

            string strText = null;



            int[] pov = state.GetPointOfViewControllers();

            label_P0.Text = pov[0].ToString(CultureInfo.CurrentCulture);
            label_P1.Text = pov[1].ToString(CultureInfo.CurrentCulture);
            label_P2.Text = pov[2].ToString(CultureInfo.CurrentCulture);
            label_P3.Text = pov[3].ToString(CultureInfo.CurrentCulture);

            bool[] buttons = state.GetButtons();

            for (int b = 0; b < buttons.Length; b++)
            {
                if (buttons[b])
                {
                    strText = (b + 1).ToString("00 ", CultureInfo.CurrentCulture);
                    label_ButtonList.Text = label_ButtonList.Text + strText;
                }


                if (buttons[b] && (lButtonState[ListPointer][b] == false))
                {
                    strText = "new ";
                    label_ButtonList.Text = label_ButtonList.Text + " " + strText;
                    // DCS uses a 1 array base whereas here we use a 0
                    iCurrentButtonPointer = b + 1;
                    if (!bFirstTime) 
                    {
                        builder.ClearContent();
                        builder.AppendText(iCurrentButtonPointer.ToString("00", CultureInfo.CurrentCulture));

                        synth.Speak(builder);
                    }

                }


                lButtonState[ListPointer][b] = buttons[b];
            }
            
            
        }

        void UpdateControl(DeviceObjectInstance d)
        {
            if (ObjectGuid.XAxis == d.ObjectTypeGuid)
            {
                label_XAxis.Enabled = true;
                label_X.Enabled = true;
            }
            if (ObjectGuid.YAxis == d.ObjectTypeGuid)
            {
                label_YAxis.Enabled = true;
                label_Y.Enabled = true;
            }
            if (ObjectGuid.ZAxis == d.ObjectTypeGuid)
            {
                label_ZAxis.Enabled = true;
                label_Z.Enabled = true;
            }
            if (ObjectGuid.RotationalXAxis == d.ObjectTypeGuid)
            {
                label_XRotation.Enabled = true;
                label_XRot.Enabled = true;
            }
            if (ObjectGuid.RotationalYAxis == d.ObjectTypeGuid)
            {
                label_YRotation.Enabled = true;
                label_YRot.Enabled = true;
            }
            if (ObjectGuid.RotationalZAxis == d.ObjectTypeGuid)
            {
                label_ZRotation.Enabled = true;
                label_ZRot.Enabled = true;
            }

            if (ObjectGuid.Slider == d.ObjectTypeGuid)
            {
                switch (SliderCount++)
                {
                    case 0:
                        label_Slider0.Enabled = true;
                        label_S0.Enabled = true;
                        break;

                    case 1:
                        label_Slider1.Enabled = true;
                        label_S1.Enabled = true;
                        break;
                }
            }

            if (ObjectGuid.PovController == d.ObjectTypeGuid)
            {
                switch (numPOVs++)
                {
                    case 0:
                        label_POV0.Enabled = true;
                        label_P0.Enabled = true;
                        break;

                    case 1:
                        label_POV1.Enabled = true;
                        label_P1.Enabled = true;
                        break;

                    case 2:
                        label_POV2.Enabled = true;
                        label_P2.Enabled = true;
                        break;

                    case 3:
                        label_POV3.Enabled = true;
                        label_P3.Enabled = true;
                        break;
                }
            }
        }

        private void createDeviceButton_Click(object sender, EventArgs e)
        {
            if (joystick == null)
                CreateDevice();
            else
                ReleaseDevice();
            //UpdateUI();
        }

        private void MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            ReleaseDevice();
        }

        #endregion

        private void MainForm_Load(object sender, EventArgs e)
        {
            // Create and Initialise two arrays better still draw a square
            // For each cycle compare array with current status
            // If new button is pressed speak it
          
        }

        private void lstAttachedSticks_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void lstAttachedSticks_Click(object sender, EventArgs e)
        {
            string wrkstring;
            wrkstring = lstAttachedSticks.SelectedItem.ToString();
            wrkstring = wrkstring.Substring(wrkstring.LastIndexOf(" "), wrkstring.Length - wrkstring.LastIndexOf(" "));
            SetDevice(wrkstring);
        }

        private void tmrNoSpeak_Tick(object sender, EventArgs e)
        {
            bFirstTime = false;
            tmrNoSpeak.Stop();
        }

        private void groupBox_JoystickState_Enter(object sender, EventArgs e)
        {

        }


    }
}
