namespace FSUIPCClientExample_CSharp
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.label1 = new System.Windows.Forms.Label();
            this.txtIAS = new System.Windows.Forms.TextBox();
            this.txtFSDateTime = new System.Windows.Forms.TextBox();
            this.txtAircraftType = new System.Windows.Forms.TextBox();
            this.chkNavigation = new System.Windows.Forms.CheckBox();
            this.chkBeacon = new System.Windows.Forms.CheckBox();
            this.chkLanding = new System.Windows.Forms.CheckBox();
            this.chkTaxi = new System.Windows.Forms.CheckBox();
            this.chkStrobes = new System.Windows.Forms.CheckBox();
            this.chkInstuments = new System.Windows.Forms.CheckBox();
            this.chkRecognition = new System.Windows.Forms.CheckBox();
            this.chkWing = new System.Windows.Forms.CheckBox();
            this.chkLogo = new System.Windows.Forms.CheckBox();
            this.chkCabin = new System.Windows.Forms.CheckBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.chkAvionics = new System.Windows.Forms.CheckBox();
            this.txtCompass = new System.Windows.Forms.TextBox();
            this.btnDisconnect = new System.Windows.Forms.Button();
            this.btnDisconnectAfterNext = new System.Windows.Forms.Button();
            this.btnReconnect = new System.Windows.Forms.Button();
            this.btnReconnectOnce = new System.Windows.Forms.Button();
            this.chkPause = new System.Windows.Forms.CheckBox();
            this.btnStart = new System.Windows.Forms.Button();
            this.txtCOM2 = new System.Windows.Forms.TextBox();
            this.label11 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.label13 = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.tabControl2 = new System.Windows.Forms.TabControl();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.lblDebug = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.btnGetAircraftType = new System.Windows.Forms.Button();
            this.tabPage4 = new System.Windows.Forms.TabPage();
            this.label3 = new System.Windows.Forms.Label();
            this.btnMoveToEGLL = new System.Windows.Forms.Button();
            this.chkPlaneOnRunway = new System.Windows.Forms.CheckBox();
            this.txtBearing = new System.Windows.Forms.TextBox();
            this.cbxDistanceUnits = new System.Windows.Forms.ComboBox();
            this.txtDistance = new System.Windows.Forms.TextBox();
            this.label18 = new System.Windows.Forms.Label();
            this.txtLongitude = new System.Windows.Forms.TextBox();
            this.txtLatitude = new System.Windows.Forms.TextBox();
            this.label17 = new System.Windows.Forms.Label();
            this.tabPage5 = new System.Windows.Forms.TabPage();
            this.label2 = new System.Windows.Forms.Label();
            this.chkShowGroundAI = new System.Windows.Forms.CheckBox();
            this.chkShowAirborneAI = new System.Windows.Forms.CheckBox();
            this.cbxRadarRange = new System.Windows.Forms.ComboBox();
            this.chkEnableAIRadar = new System.Windows.Forms.CheckBox();
            this.pnlAIRadar = new FSUIPCClientExample_CSharp.DoubleBufferPanel();
            this.AIRadarTimer = new System.Windows.Forms.Timer(this.components);
            this.tabControl2.SuspendLayout();
            this.tabPage3.SuspendLayout();
            this.tabPage4.SuspendLayout();
            this.tabPage5.SuspendLayout();
            this.SuspendLayout();
            // 
            // timer1
            // 
            this.timer1.Interval = 50;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(6, 52);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(194, 37);
            this.label1.TabIndex = 0;
            this.label1.Text = "Indicated Air Speed.  (Basic Integer Read)";
            // 
            // txtIAS
            // 
            this.txtIAS.Location = new System.Drawing.Point(206, 49);
            this.txtIAS.Name = "txtIAS";
            this.txtIAS.ReadOnly = true;
            this.txtIAS.Size = new System.Drawing.Size(59, 20);
            this.txtIAS.TabIndex = 1;
            // 
            // txtFSDateTime
            // 
            this.txtFSDateTime.Location = new System.Drawing.Point(206, 165);
            this.txtFSDateTime.Name = "txtFSDateTime";
            this.txtFSDateTime.ReadOnly = true;
            this.txtFSDateTime.Size = new System.Drawing.Size(245, 20);
            this.txtFSDateTime.TabIndex = 1;
            // 
            // txtAircraftType
            // 
            this.txtAircraftType.Location = new System.Drawing.Point(206, 201);
            this.txtAircraftType.Name = "txtAircraftType";
            this.txtAircraftType.ReadOnly = true;
            this.txtAircraftType.Size = new System.Drawing.Size(245, 20);
            this.txtAircraftType.TabIndex = 1;
            // 
            // chkNavigation
            // 
            this.chkNavigation.AutoSize = true;
            this.chkNavigation.Location = new System.Drawing.Point(206, 261);
            this.chkNavigation.Name = "chkNavigation";
            this.chkNavigation.Size = new System.Drawing.Size(77, 17);
            this.chkNavigation.TabIndex = 3;
            this.chkNavigation.Text = "Navigation";
            this.chkNavigation.UseVisualStyleBackColor = true;
            this.chkNavigation.CheckedChanged += new System.EventHandler(this.chkNavigation_CheckedChanged);
            // 
            // chkBeacon
            // 
            this.chkBeacon.AutoSize = true;
            this.chkBeacon.Location = new System.Drawing.Point(206, 284);
            this.chkBeacon.Name = "chkBeacon";
            this.chkBeacon.Size = new System.Drawing.Size(63, 17);
            this.chkBeacon.TabIndex = 3;
            this.chkBeacon.Text = "Beacon";
            this.chkBeacon.UseVisualStyleBackColor = true;
            this.chkBeacon.CheckedChanged += new System.EventHandler(this.chkBeacon_CheckedChanged);
            // 
            // chkLanding
            // 
            this.chkLanding.AutoSize = true;
            this.chkLanding.Location = new System.Drawing.Point(206, 307);
            this.chkLanding.Name = "chkLanding";
            this.chkLanding.Size = new System.Drawing.Size(64, 17);
            this.chkLanding.TabIndex = 3;
            this.chkLanding.Text = "Landing";
            this.chkLanding.UseVisualStyleBackColor = true;
            this.chkLanding.CheckedChanged += new System.EventHandler(this.chkLanding_CheckedChanged);
            // 
            // chkTaxi
            // 
            this.chkTaxi.AutoSize = true;
            this.chkTaxi.Location = new System.Drawing.Point(206, 330);
            this.chkTaxi.Name = "chkTaxi";
            this.chkTaxi.Size = new System.Drawing.Size(46, 17);
            this.chkTaxi.TabIndex = 3;
            this.chkTaxi.Text = "Taxi";
            this.chkTaxi.UseVisualStyleBackColor = true;
            this.chkTaxi.CheckedChanged += new System.EventHandler(this.chkTaxi_CheckedChanged);
            // 
            // chkStrobes
            // 
            this.chkStrobes.AutoSize = true;
            this.chkStrobes.Location = new System.Drawing.Point(206, 354);
            this.chkStrobes.Name = "chkStrobes";
            this.chkStrobes.Size = new System.Drawing.Size(62, 17);
            this.chkStrobes.TabIndex = 3;
            this.chkStrobes.Text = "Strobes";
            this.chkStrobes.UseVisualStyleBackColor = true;
            this.chkStrobes.CheckedChanged += new System.EventHandler(this.chkStrobes_CheckedChanged);
            // 
            // chkInstuments
            // 
            this.chkInstuments.AutoSize = true;
            this.chkInstuments.Location = new System.Drawing.Point(339, 255);
            this.chkInstuments.Name = "chkInstuments";
            this.chkInstuments.Size = new System.Drawing.Size(80, 17);
            this.chkInstuments.TabIndex = 3;
            this.chkInstuments.Text = "Instruments";
            this.chkInstuments.UseVisualStyleBackColor = true;
            this.chkInstuments.CheckedChanged += new System.EventHandler(this.chkInstuments_CheckedChanged);
            // 
            // chkRecognition
            // 
            this.chkRecognition.AutoSize = true;
            this.chkRecognition.Location = new System.Drawing.Point(339, 278);
            this.chkRecognition.Name = "chkRecognition";
            this.chkRecognition.Size = new System.Drawing.Size(83, 17);
            this.chkRecognition.TabIndex = 3;
            this.chkRecognition.Text = "Recognition";
            this.chkRecognition.UseVisualStyleBackColor = true;
            this.chkRecognition.CheckedChanged += new System.EventHandler(this.chkRecognition_CheckedChanged);
            // 
            // chkWing
            // 
            this.chkWing.AutoSize = true;
            this.chkWing.Location = new System.Drawing.Point(339, 301);
            this.chkWing.Name = "chkWing";
            this.chkWing.Size = new System.Drawing.Size(51, 17);
            this.chkWing.TabIndex = 3;
            this.chkWing.Text = "Wing";
            this.chkWing.UseVisualStyleBackColor = true;
            this.chkWing.CheckedChanged += new System.EventHandler(this.chkWing_CheckedChanged);
            // 
            // chkLogo
            // 
            this.chkLogo.AutoSize = true;
            this.chkLogo.Location = new System.Drawing.Point(339, 324);
            this.chkLogo.Name = "chkLogo";
            this.chkLogo.Size = new System.Drawing.Size(50, 17);
            this.chkLogo.TabIndex = 3;
            this.chkLogo.Text = "Logo";
            this.chkLogo.UseVisualStyleBackColor = true;
            this.chkLogo.CheckedChanged += new System.EventHandler(this.chkLogo_CheckedChanged);
            // 
            // chkCabin
            // 
            this.chkCabin.AutoSize = true;
            this.chkCabin.Location = new System.Drawing.Point(339, 347);
            this.chkCabin.Name = "chkCabin";
            this.chkCabin.Size = new System.Drawing.Size(53, 17);
            this.chkCabin.TabIndex = 3;
            this.chkCabin.Text = "Cabin";
            this.chkCabin.UseVisualStyleBackColor = true;
            this.chkCabin.CheckedChanged += new System.EventHandler(this.chkCabin_CheckedChanged);
            // 
            // label5
            // 
            this.label5.Location = new System.Drawing.Point(6, 484);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(168, 39);
            this.label5.TabIndex = 4;
            this.label5.Text = "Pause control.  (Example of a write only Offset)";
            // 
            // label6
            // 
            this.label6.Location = new System.Drawing.Point(6, 391);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(137, 48);
            this.label6.TabIndex = 4;
            this.label6.Text = "Compass heading: (Example of disconnecting an offset an Offset)";
            // 
            // chkAvionics
            // 
            this.chkAvionics.AutoSize = true;
            this.chkAvionics.Location = new System.Drawing.Point(206, 94);
            this.chkAvionics.Name = "chkAvionics";
            this.chkAvionics.Size = new System.Drawing.Size(15, 14);
            this.chkAvionics.TabIndex = 3;
            this.chkAvionics.UseVisualStyleBackColor = true;
            this.chkAvionics.CheckedChanged += new System.EventHandler(this.chkAvionics_CheckedChanged);
            // 
            // txtCompass
            // 
            this.txtCompass.Location = new System.Drawing.Point(206, 391);
            this.txtCompass.Name = "txtCompass";
            this.txtCompass.ReadOnly = true;
            this.txtCompass.Size = new System.Drawing.Size(59, 20);
            this.txtCompass.TabIndex = 1;
            // 
            // btnDisconnect
            // 
            this.btnDisconnect.Location = new System.Drawing.Point(206, 417);
            this.btnDisconnect.Name = "btnDisconnect";
            this.btnDisconnect.Size = new System.Drawing.Size(77, 22);
            this.btnDisconnect.TabIndex = 2;
            this.btnDisconnect.Text = "Disconnect";
            this.btnDisconnect.UseVisualStyleBackColor = true;
            this.btnDisconnect.Click += new System.EventHandler(this.btnDisconnect_Click);
            // 
            // btnDisconnectAfterNext
            // 
            this.btnDisconnectAfterNext.Location = new System.Drawing.Point(289, 417);
            this.btnDisconnectAfterNext.Name = "btnDisconnectAfterNext";
            this.btnDisconnectAfterNext.Size = new System.Drawing.Size(165, 23);
            this.btnDisconnectAfterNext.TabIndex = 2;
            this.btnDisconnectAfterNext.Text = "Disconnect after next Process()";
            this.btnDisconnectAfterNext.UseVisualStyleBackColor = true;
            this.btnDisconnectAfterNext.Click += new System.EventHandler(this.btnDisconnectAfterNext_Click);
            // 
            // btnReconnect
            // 
            this.btnReconnect.Location = new System.Drawing.Point(206, 445);
            this.btnReconnect.Name = "btnReconnect";
            this.btnReconnect.Size = new System.Drawing.Size(77, 25);
            this.btnReconnect.TabIndex = 2;
            this.btnReconnect.Text = "Reconnect";
            this.btnReconnect.UseVisualStyleBackColor = true;
            this.btnReconnect.Click += new System.EventHandler(this.btnReconnect_Click);
            // 
            // btnReconnectOnce
            // 
            this.btnReconnectOnce.Location = new System.Drawing.Point(289, 445);
            this.btnReconnectOnce.Name = "btnReconnectOnce";
            this.btnReconnectOnce.Size = new System.Drawing.Size(165, 25);
            this.btnReconnectOnce.TabIndex = 2;
            this.btnReconnectOnce.Text = "Reconnect for one Process()";
            this.btnReconnectOnce.UseVisualStyleBackColor = true;
            this.btnReconnectOnce.Click += new System.EventHandler(this.btnReconnectOnce_Click);
            // 
            // chkPause
            // 
            this.chkPause.AutoSize = true;
            this.chkPause.Location = new System.Drawing.Point(210, 486);
            this.chkPause.Name = "chkPause";
            this.chkPause.Size = new System.Drawing.Size(56, 17);
            this.chkPause.TabIndex = 5;
            this.chkPause.Text = "Pause";
            this.chkPause.UseVisualStyleBackColor = true;
            this.chkPause.CheckedChanged += new System.EventHandler(this.chkPause_CheckedChanged);
            // 
            // btnStart
            // 
            this.btnStart.Location = new System.Drawing.Point(206, 11);
            this.btnStart.Name = "btnStart";
            this.btnStart.Size = new System.Drawing.Size(191, 24);
            this.btnStart.TabIndex = 6;
            this.btnStart.Text = "Connect to FSUIPC";
            this.btnStart.UseVisualStyleBackColor = true;
            this.btnStart.Click += new System.EventHandler(this.btnStart_Click);
            // 
            // txtCOM2
            // 
            this.txtCOM2.Location = new System.Drawing.Point(206, 129);
            this.txtCOM2.Name = "txtCOM2";
            this.txtCOM2.ReadOnly = true;
            this.txtCOM2.Size = new System.Drawing.Size(77, 20);
            this.txtCOM2.TabIndex = 1;
            // 
            // label11
            // 
            this.label11.Location = new System.Drawing.Point(6, 165);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(194, 20);
            this.label11.TabIndex = 0;
            this.label11.Text = "FS Date/Time (Array Read)";
            // 
            // label12
            // 
            this.label12.Location = new System.Drawing.Point(6, 126);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(194, 33);
            this.label12.TabIndex = 0;
            this.label12.Text = "COM2 (Example of decoding a BCD frequency)";
            // 
            // label13
            // 
            this.label13.Location = new System.Drawing.Point(6, 201);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(137, 51);
            this.label13.TabIndex = 0;
            this.label13.Text = "Aircraft Type: (Example of string read and different offset group)";
            // 
            // label14
            // 
            this.label14.Location = new System.Drawing.Point(6, 261);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(137, 51);
            this.label14.TabIndex = 0;
            this.label14.Text = "Lights: (Example of using a BitArray for a bitmap type offset)";
            // 
            // tabControl2
            // 
            this.tabControl2.Controls.Add(this.tabPage3);
            this.tabControl2.Controls.Add(this.tabPage4);
            this.tabControl2.Controls.Add(this.tabPage5);
            this.tabControl2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl2.Location = new System.Drawing.Point(0, 0);
            this.tabControl2.Name = "tabControl2";
            this.tabControl2.SelectedIndex = 0;
            this.tabControl2.Size = new System.Drawing.Size(1069, 562);
            this.tabControl2.TabIndex = 7;
            // 
            // tabPage3
            // 
            this.tabPage3.Controls.Add(this.lblDebug);
            this.tabPage3.Controls.Add(this.label1);
            this.tabPage3.Controls.Add(this.btnStart);
            this.tabPage3.Controls.Add(this.label10);
            this.tabPage3.Controls.Add(this.chkPause);
            this.tabPage3.Controls.Add(this.label11);
            this.tabPage3.Controls.Add(this.label6);
            this.tabPage3.Controls.Add(this.label12);
            this.tabPage3.Controls.Add(this.label5);
            this.tabPage3.Controls.Add(this.label13);
            this.tabPage3.Controls.Add(this.txtIAS);
            this.tabPage3.Controls.Add(this.chkCabin);
            this.tabPage3.Controls.Add(this.label14);
            this.tabPage3.Controls.Add(this.txtCOM2);
            this.tabPage3.Controls.Add(this.chkLogo);
            this.tabPage3.Controls.Add(this.txtCompass);
            this.tabPage3.Controls.Add(this.chkWing);
            this.tabPage3.Controls.Add(this.txtFSDateTime);
            this.tabPage3.Controls.Add(this.chkRecognition);
            this.tabPage3.Controls.Add(this.txtAircraftType);
            this.tabPage3.Controls.Add(this.chkTaxi);
            this.tabPage3.Controls.Add(this.btnGetAircraftType);
            this.tabPage3.Controls.Add(this.chkInstuments);
            this.tabPage3.Controls.Add(this.btnDisconnect);
            this.tabPage3.Controls.Add(this.chkLanding);
            this.tabPage3.Controls.Add(this.btnDisconnectAfterNext);
            this.tabPage3.Controls.Add(this.chkStrobes);
            this.tabPage3.Controls.Add(this.btnReconnect);
            this.tabPage3.Controls.Add(this.chkBeacon);
            this.tabPage3.Controls.Add(this.btnReconnectOnce);
            this.tabPage3.Controls.Add(this.chkAvionics);
            this.tabPage3.Controls.Add(this.chkNavigation);
            this.tabPage3.Location = new System.Drawing.Point(4, 22);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage3.Size = new System.Drawing.Size(1061, 536);
            this.tabPage3.TabIndex = 0;
            this.tabPage3.Text = "Basics";
            this.tabPage3.UseVisualStyleBackColor = true;
            this.tabPage3.Click += new System.EventHandler(this.tabPage3_Click);
            // 
            // lblDebug
            // 
            this.lblDebug.Location = new System.Drawing.Point(469, 29);
            this.lblDebug.Name = "lblDebug";
            this.lblDebug.Size = new System.Drawing.Size(576, 493);
            this.lblDebug.TabIndex = 7;
            this.lblDebug.Text = "label4";

            // 
            // label10
            // 
            this.label10.Location = new System.Drawing.Point(6, 89);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(194, 37);
            this.label10.TabIndex = 0;
            this.label10.Text = "Master Avionic Switch.  (Example of basic Integer read and write)";
            // 
            // btnGetAircraftType
            // 
            this.btnGetAircraftType.Location = new System.Drawing.Point(162, 201);
            this.btnGetAircraftType.Name = "btnGetAircraftType";
            this.btnGetAircraftType.Size = new System.Drawing.Size(38, 23);
            this.btnGetAircraftType.TabIndex = 2;
            this.btnGetAircraftType.Text = "Get";
            this.btnGetAircraftType.UseVisualStyleBackColor = true;
            this.btnGetAircraftType.Click += new System.EventHandler(this.btnGetAircraftType_Click);
            // 
            // tabPage4
            // 
            this.tabPage4.Controls.Add(this.label3);
            this.tabPage4.Controls.Add(this.btnMoveToEGLL);
            this.tabPage4.Controls.Add(this.chkPlaneOnRunway);
            this.tabPage4.Controls.Add(this.txtBearing);
            this.tabPage4.Controls.Add(this.cbxDistanceUnits);
            this.tabPage4.Controls.Add(this.txtDistance);
            this.tabPage4.Controls.Add(this.label18);
            this.tabPage4.Controls.Add(this.txtLongitude);
            this.tabPage4.Controls.Add(this.txtLatitude);
            this.tabPage4.Controls.Add(this.label17);
            this.tabPage4.Location = new System.Drawing.Point(4, 22);
            this.tabPage4.Name = "tabPage4";
            this.tabPage4.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage4.Size = new System.Drawing.Size(1061, 536);
            this.tabPage4.TabIndex = 1;
            this.tabPage4.Text = "Latitude/Longitude";
            this.tabPage4.UseVisualStyleBackColor = true;
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(137, 264);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(252, 68);
            this.label3.TabIndex = 9;
            this.label3.Text = "Move the plane to the start of Runway 27L at London Heathrow (EGLL).  \r\n(Example " +
                "of writing longitudes and latitudes and performing simple maths with them).";
            // 
            // btnMoveToEGLL
            // 
            this.btnMoveToEGLL.Location = new System.Drawing.Point(11, 264);
            this.btnMoveToEGLL.Name = "btnMoveToEGLL";
            this.btnMoveToEGLL.Size = new System.Drawing.Size(123, 23);
            this.btnMoveToEGLL.TabIndex = 8;
            this.btnMoveToEGLL.Text = "Move to EGLL 27L";
            this.btnMoveToEGLL.UseVisualStyleBackColor = true;
            this.btnMoveToEGLL.Click += new System.EventHandler(this.btnMoveToEGLL_Click);
            // 
            // chkPlaneOnRunway
            // 
            this.chkPlaneOnRunway.CheckAlign = System.Drawing.ContentAlignment.TopLeft;
            this.chkPlaneOnRunway.Location = new System.Drawing.Point(10, 185);
            this.chkPlaneOnRunway.Name = "chkPlaneOnRunway";
            this.chkPlaneOnRunway.Size = new System.Drawing.Size(382, 51);
            this.chkPlaneOnRunway.TabIndex = 7;
            this.chkPlaneOnRunway.Text = "Is plane on runway 27L (09R) at EGLL?   (Example of using the FsLatLonQuadrange c" +
                "lass to determine if the current position is within the bound of the quadrangle " +
                "- in this case the runway).";
            this.chkPlaneOnRunway.TextAlign = System.Drawing.ContentAlignment.TopLeft;
            this.chkPlaneOnRunway.UseVisualStyleBackColor = true;
            // 
            // txtBearing
            // 
            this.txtBearing.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.txtBearing.Location = new System.Drawing.Point(305, 131);
            this.txtBearing.Name = "txtBearing";
            this.txtBearing.Size = new System.Drawing.Size(84, 20);
            this.txtBearing.TabIndex = 6;
            // 
            // cbxDistanceUnits
            // 
            this.cbxDistanceUnits.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbxDistanceUnits.FormattingEnabled = true;
            this.cbxDistanceUnits.Items.AddRange(new object[] {
            "Nautical Miles",
            "Statute Miles",
            "Kilometres"});
            this.cbxDistanceUnits.Location = new System.Drawing.Point(140, 130);
            this.cbxDistanceUnits.Name = "cbxDistanceUnits";
            this.cbxDistanceUnits.Size = new System.Drawing.Size(146, 21);
            this.cbxDistanceUnits.TabIndex = 5;
            // 
            // txtDistance
            // 
            this.txtDistance.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.txtDistance.Location = new System.Drawing.Point(10, 131);
            this.txtDistance.Name = "txtDistance";
            this.txtDistance.Size = new System.Drawing.Size(124, 20);
            this.txtDistance.TabIndex = 4;
            // 
            // label18
            // 
            this.label18.Location = new System.Drawing.Point(7, 77);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(319, 51);
            this.label18.TabIndex = 3;
            this.label18.Text = "Distance and Magnetic Bearing to London Heathrow (EGLL).  (Example of using the f" +
                "sLatLonPoint class to calculate distance and bearing between two points).";
            // 
            // txtLongitude
            // 
            this.txtLongitude.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.txtLongitude.Location = new System.Drawing.Point(205, 33);
            this.txtLongitude.Name = "txtLongitude";
            this.txtLongitude.Size = new System.Drawing.Size(188, 20);
            this.txtLongitude.TabIndex = 2;
            // 
            // txtLatitude
            // 
            this.txtLatitude.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.txtLatitude.Location = new System.Drawing.Point(11, 33);
            this.txtLatitude.Name = "txtLatitude";
            this.txtLatitude.Size = new System.Drawing.Size(188, 20);
            this.txtLatitude.TabIndex = 1;
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Location = new System.Drawing.Point(8, 12);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(319, 13);
            this.label17.TabIndex = 0;
            this.label17.Text = "Current position.  (Example of FsLongitude and FsLatitude classes)";
            // 
            // tabPage5
            // 
            this.tabPage5.Controls.Add(this.label2);
            this.tabPage5.Controls.Add(this.chkShowGroundAI);
            this.tabPage5.Controls.Add(this.chkShowAirborneAI);
            this.tabPage5.Controls.Add(this.cbxRadarRange);
            this.tabPage5.Controls.Add(this.chkEnableAIRadar);
            this.tabPage5.Controls.Add(this.pnlAIRadar);
            this.tabPage5.Location = new System.Drawing.Point(4, 22);
            this.tabPage5.Name = "tabPage5";
            this.tabPage5.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage5.Size = new System.Drawing.Size(1061, 536);
            this.tabPage5.TabIndex = 2;
            this.tabPage5.Text = "AI Traffic Radar";
            this.tabPage5.UseVisualStyleBackColor = true;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(8, 53);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(114, 13);
            this.label2.TabIndex = 15;
            this.label2.Text = "Range (Nautical Miles)";
            // 
            // chkShowGroundAI
            // 
            this.chkShowGroundAI.AutoSize = true;
            this.chkShowGroundAI.Location = new System.Drawing.Point(346, 52);
            this.chkShowGroundAI.Name = "chkShowGroundAI";
            this.chkShowGroundAI.Size = new System.Drawing.Size(104, 17);
            this.chkShowGroundAI.TabIndex = 14;
            this.chkShowGroundAI.Text = "Show Ground AI";
            this.chkShowGroundAI.UseVisualStyleBackColor = true;
            // 
            // chkShowAirborneAI
            // 
            this.chkShowAirborneAI.AutoSize = true;
            this.chkShowAirborneAI.Checked = true;
            this.chkShowAirborneAI.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkShowAirborneAI.Location = new System.Drawing.Point(222, 52);
            this.chkShowAirborneAI.Name = "chkShowAirborneAI";
            this.chkShowAirborneAI.Size = new System.Drawing.Size(108, 17);
            this.chkShowAirborneAI.TabIndex = 13;
            this.chkShowAirborneAI.Text = "Show Airborne AI";
            this.chkShowAirborneAI.UseVisualStyleBackColor = true;
            // 
            // cbxRadarRange
            // 
            this.cbxRadarRange.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbxRadarRange.FormattingEnabled = true;
            this.cbxRadarRange.Items.AddRange(new object[] {
            "50",
            "40",
            "30",
            "20",
            "10",
            "5",
            "1"});
            this.cbxRadarRange.Location = new System.Drawing.Point(135, 50);
            this.cbxRadarRange.Name = "cbxRadarRange";
            this.cbxRadarRange.Size = new System.Drawing.Size(65, 21);
            this.cbxRadarRange.TabIndex = 12;
            // 
            // chkEnableAIRadar
            // 
            this.chkEnableAIRadar.CheckAlign = System.Drawing.ContentAlignment.TopLeft;
            this.chkEnableAIRadar.Enabled = false;
            this.chkEnableAIRadar.Location = new System.Drawing.Point(10, 20);
            this.chkEnableAIRadar.Name = "chkEnableAIRadar";
            this.chkEnableAIRadar.Size = new System.Drawing.Size(440, 24);
            this.chkEnableAIRadar.TabIndex = 11;
            this.chkEnableAIRadar.Text = "Turn on AI Radar.  (Example of using the AITrafficServices to render a simple AI " +
                "Radar).";
            this.chkEnableAIRadar.TextAlign = System.Drawing.ContentAlignment.TopLeft;
            this.chkEnableAIRadar.UseVisualStyleBackColor = true;
            this.chkEnableAIRadar.CheckedChanged += new System.EventHandler(this.chkEnableAIRadar_CheckedChanged);
            // 
            // pnlAIRadar
            // 
            this.pnlAIRadar.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.pnlAIRadar.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pnlAIRadar.Location = new System.Drawing.Point(10, 83);
            this.pnlAIRadar.Name = "pnlAIRadar";
            this.pnlAIRadar.Size = new System.Drawing.Size(440, 440);
            this.pnlAIRadar.TabIndex = 10;
            this.pnlAIRadar.Paint += new System.Windows.Forms.PaintEventHandler(this.pnlAIRadar_Paint);
            // 
            // AIRadarTimer
            // 
            this.AIRadarTimer.Interval = 1000;
            this.AIRadarTimer.Tick += new System.EventHandler(this.AIRadarTimer_Tick);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1069, 562);
            this.Controls.Add(this.tabControl2);
            this.DoubleBuffered = true;
            this.Name = "Form1";
            this.Text = "FSUIPCClientExample_CSharp";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form1_FormClosed);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.tabControl2.ResumeLayout(false);
            this.tabPage3.ResumeLayout(false);
            this.tabPage3.PerformLayout();
            this.tabPage4.ResumeLayout(false);
            this.tabPage4.PerformLayout();
            this.tabPage5.ResumeLayout(false);
            this.tabPage5.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txtIAS;
        private System.Windows.Forms.TextBox txtFSDateTime;
        private System.Windows.Forms.TextBox txtAircraftType;
        private System.Windows.Forms.CheckBox chkNavigation;
        private System.Windows.Forms.CheckBox chkBeacon;
        private System.Windows.Forms.CheckBox chkLanding;
        private System.Windows.Forms.CheckBox chkTaxi;
        private System.Windows.Forms.CheckBox chkStrobes;
        private System.Windows.Forms.CheckBox chkInstuments;
        private System.Windows.Forms.CheckBox chkRecognition;
        private System.Windows.Forms.CheckBox chkWing;
        private System.Windows.Forms.CheckBox chkLogo;
        private System.Windows.Forms.CheckBox chkCabin;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.CheckBox chkAvionics;
        private System.Windows.Forms.TextBox txtCompass;
        private System.Windows.Forms.Button btnDisconnect;
        private System.Windows.Forms.Button btnDisconnectAfterNext;
        private System.Windows.Forms.Button btnReconnect;
        private System.Windows.Forms.Button btnReconnectOnce;
        private System.Windows.Forms.CheckBox chkPause;
        private System.Windows.Forms.Button btnStart;
        private System.Windows.Forms.TextBox txtCOM2;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.TabControl tabControl2;
        private System.Windows.Forms.TabPage tabPage3;
        private System.Windows.Forms.TabPage tabPage4;
        private System.Windows.Forms.TextBox txtLongitude;
        private System.Windows.Forms.TextBox txtLatitude;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.ComboBox cbxDistanceUnits;
        private System.Windows.Forms.TextBox txtDistance;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.CheckBox chkPlaneOnRunway;
        private System.Windows.Forms.TextBox txtBearing;
        private System.Windows.Forms.Timer AIRadarTimer;
        private System.Windows.Forms.TabPage tabPage5;
        private System.Windows.Forms.CheckBox chkEnableAIRadar;
        private DoubleBufferPanel pnlAIRadar;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Button btnGetAircraftType;
        private System.Windows.Forms.ComboBox cbxRadarRange;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.CheckBox chkShowGroundAI;
        private System.Windows.Forms.CheckBox chkShowAirborneAI;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button btnMoveToEGLL;
        private System.Windows.Forms.Label lblDebug;
    }
}

