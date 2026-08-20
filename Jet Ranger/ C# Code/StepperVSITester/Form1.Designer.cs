namespace StepperVSITester
{
    partial class frmMain
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
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
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            lblTarget = new Label();
            lblVsiHeader = new Label();
            trkVsi = new TrackBar();
            lblValue = new Label();
            lblMin = new Label();
            lblMax = new Label();
            txtRawInput = new TextBox();
            butSendRaw = new Button();
            butZero = new Button();
            lblAltHeader = new Label();
            trkAlt = new TrackBar();
            lblAltValue = new Label();
            lblAltMin = new Label();
            lblAltMax = new Label();
            txtAltInput = new TextBox();
            butSendAlt = new Button();
            butAltZero = new Button();
            lblRadarAltHeader = new Label();
            trkRadarAlt = new TrackBar();
            lblRadarAltValue = new Label();
            lblRadarAltMin = new Label();
            lblRadarAltMax = new Label();
            txtRadarAltInput = new TextBox();
            butSendRadarAlt = new Button();
            butRadarAltZero = new Button();
            lblJogHeader = new Label();
            lblJogSteps = new Label();
            txtJogSteps = new TextBox();
            lblJogInterval = new Label();
            txtJogInterval = new TextBox();
            butJogSend = new Button();
            lblNewGaugeHeader = new Label();
            lblNewGaugeGauge = new Label();
            cboNewGauge = new ComboBox();
            lblNewGaugeSteps = new Label();
            txtNewGaugeSteps = new TextBox();
            butNewGaugeSend = new Button();
            butNewGaugeZero = new Button();
            butNewGaugeStepBack = new Button();
            butNewGaugeStepFwd = new Button();
            lblEgtHeader = new Label();
            trkEgt = new TrackBar();
            lblEgtValue = new Label();
            lblEgtMin = new Label();
            lblEgtMax = new Label();
            txtEgtInput = new TextBox();
            butSendEgt = new Button();
            butEgtZero = new Button();
            lblRealGaugesHeader = new Label();
            lblEotRow = new Label();
            txtEot = new TextBox();
            butSendEot = new Button();
            lblEopRow = new Label();
            txtEop = new TextBox();
            butSendEop = new Button();
            lblXotRow = new Label();
            txtXot = new TextBox();
            butSendXot = new Button();
            lblXopRow = new Label();
            txtXop = new TextBox();
            butSendXop = new Button();
            lblGpRow = new Label();
            txtGp = new TextBox();
            butSendGp = new Button();
            lblFaRow = new Label();
            txtFa = new TextBox();
            butSendFa = new Button();
            lblIasHeader = new Label();
            trkIas = new TrackBar();
            lblIasValue = new Label();
            lblIasMin = new Label();
            lblIasMax = new Label();
            txtIasInput = new TextBox();
            butSendIas = new Button();
            butIasZero = new Button();
            lblRpmeHeader = new Label();
            trkRpme = new TrackBar();
            lblRpmeValue = new Label();
            lblRpmeMin = new Label();
            lblRpmeMax = new Label();
            txtRpmeInput = new TextBox();
            butSendRpme = new Button();
            butRpmeZero = new Button();
            lblRpmrHeader = new Label();
            trkRpmr = new TrackBar();
            lblRpmrValue = new Label();
            lblRpmrMin = new Label();
            lblRpmrMax = new Label();
            txtRpmrInput = new TextBox();
            butSendRpmr = new Button();
            butRpmrZero = new Button();
            lblDualStepperHeader = new Label();
            lblFuelLoadRow = new Label();
            txtFuelLoad = new TextBox();
            butSendFuelLoad = new Button();
            lblElectricalLoadRow = new Label();
            txtElectricalLoad = new TextBox();
            butSendElectricalLoad = new Button();
            lblClockRow = new Label();
            txtClockHour = new TextBox();
            lblClockColon = new Label();
            txtClockMinute = new TextBox();
            butSendClock = new Button();
            ((System.ComponentModel.ISupportInitialize)trkVsi).BeginInit();
            ((System.ComponentModel.ISupportInitialize)trkAlt).BeginInit();
            ((System.ComponentModel.ISupportInitialize)trkRadarAlt).BeginInit();
            ((System.ComponentModel.ISupportInitialize)trkEgt).BeginInit();
            ((System.ComponentModel.ISupportInitialize)trkIas).BeginInit();
            ((System.ComponentModel.ISupportInitialize)trkRpme).BeginInit();
            ((System.ComponentModel.ISupportInitialize)trkRpmr).BeginInit();
            SuspendLayout();
            //
            // lblTarget
            //
            lblTarget.AutoSize = true;
            lblTarget.Location = new Point(12, 9);
            lblTarget.Name = "lblTarget";
            lblTarget.Size = new Size(260, 15);
            lblTarget.TabIndex = 0;
            lblTarget.Text = "Target: 172.16.1.105:13136 (Stepper Controller)";
            //
            // lblVsiHeader
            //
            lblVsiHeader.AutoSize = true;
            lblVsiHeader.Font = new Font("Segoe UI", 9F, FontStyle.Bold);
            lblVsiHeader.Location = new Point(12, 34);
            lblVsiHeader.Name = "lblVsiHeader";
            lblVsiHeader.Size = new Size(60, 15);
            lblVsiHeader.TabIndex = 1;
            lblVsiHeader.Text = "VSI (fpm)";
            //
            // trkVsi
            //
            trkVsi.LargeChange = 250;
            trkVsi.Location = new Point(12, 80);
            trkVsi.Maximum = 1750;
            trkVsi.Minimum = -1750;
            trkVsi.Name = "trkVsi";
            trkVsi.Size = new Size(400, 45);
            trkVsi.SmallChange = 50;
            trkVsi.TabIndex = 3;
            trkVsi.TickFrequency = 250;
            trkVsi.Scroll += trkVsi_Scroll;
            //
            // lblValue
            //
            lblValue.AutoSize = true;
            lblValue.Location = new Point(12, 55);
            lblValue.Name = "lblValue";
            lblValue.Size = new Size(68, 15);
            lblValue.TabIndex = 2;
            lblValue.Text = "Value: 0 fpm";
            //
            // lblMin
            //
            lblMin.AutoSize = true;
            lblMin.Location = new Point(12, 128);
            lblMin.Name = "lblMin";
            lblMin.Size = new Size(48, 15);
            lblMin.TabIndex = 4;
            lblMin.Text = "-1750";
            //
            // lblMax
            //
            lblMax.AutoSize = true;
            lblMax.Location = new Point(357, 128);
            lblMax.Name = "lblMax";
            lblMax.Size = new Size(42, 15);
            lblMax.TabIndex = 5;
            lblMax.Text = "1750";
            //
            // txtRawInput
            //
            txtRawInput.Location = new Point(12, 160);
            txtRawInput.Name = "txtRawInput";
            txtRawInput.Size = new Size(100, 23);
            txtRawInput.TabIndex = 6;
            txtRawInput.Text = "0";
            txtRawInput.KeyDown += txtRawInput_KeyDown;
            //
            // butSendRaw
            //
            butSendRaw.Location = new Point(118, 159);
            butSendRaw.Name = "butSendRaw";
            butSendRaw.Size = new Size(92, 25);
            butSendRaw.TabIndex = 7;
            butSendRaw.Text = "Send";
            butSendRaw.UseVisualStyleBackColor = true;
            butSendRaw.Click += butSendRaw_Click;
            //
            // butZero
            //
            butZero.Location = new Point(216, 159);
            butZero.Name = "butZero";
            butZero.Size = new Size(92, 25);
            butZero.TabIndex = 8;
            butZero.Text = "Zero";
            butZero.UseVisualStyleBackColor = true;
            butZero.Click += butZero_Click;
            //
            // lblAltHeader
            //
            lblAltHeader.AutoSize = true;
            lblAltHeader.Font = new Font("Segoe UI", 9F, FontStyle.Bold);
            lblAltHeader.Location = new Point(12, 200);
            lblAltHeader.Name = "lblAltHeader";
            lblAltHeader.Size = new Size(55, 15);
            lblAltHeader.TabIndex = 9;
            lblAltHeader.Text = "ALT (ft)";
            //
            // trkAlt
            //
            trkAlt.LargeChange = 2000;
            trkAlt.Location = new Point(12, 246);
            trkAlt.Maximum = 20000;
            trkAlt.Minimum = 0;
            trkAlt.Name = "trkAlt";
            trkAlt.Size = new Size(400, 45);
            trkAlt.SmallChange = 500;
            trkAlt.TabIndex = 11;
            trkAlt.TickFrequency = 2000;
            trkAlt.Scroll += trkAlt_Scroll;
            //
            // lblAltValue
            //
            lblAltValue.AutoSize = true;
            lblAltValue.Location = new Point(12, 221);
            lblAltValue.Name = "lblAltValue";
            lblAltValue.Size = new Size(59, 15);
            lblAltValue.TabIndex = 10;
            lblAltValue.Text = "Value: 0 ft";
            //
            // lblAltMin
            //
            lblAltMin.AutoSize = true;
            lblAltMin.Location = new Point(12, 294);
            lblAltMin.Name = "lblAltMin";
            lblAltMin.Size = new Size(13, 15);
            lblAltMin.TabIndex = 12;
            lblAltMin.Text = "0";
            //
            // lblAltMax
            //
            lblAltMax.AutoSize = true;
            lblAltMax.Location = new Point(357, 294);
            lblAltMax.Name = "lblAltMax";
            lblAltMax.Size = new Size(48, 15);
            lblAltMax.TabIndex = 13;
            lblAltMax.Text = "20000";
            //
            // txtAltInput
            //
            txtAltInput.Location = new Point(12, 326);
            txtAltInput.Name = "txtAltInput";
            txtAltInput.Size = new Size(100, 23);
            txtAltInput.TabIndex = 14;
            txtAltInput.Text = "0";
            txtAltInput.KeyDown += txtAltInput_KeyDown;
            //
            // butSendAlt
            //
            butSendAlt.Location = new Point(118, 325);
            butSendAlt.Name = "butSendAlt";
            butSendAlt.Size = new Size(92, 25);
            butSendAlt.TabIndex = 15;
            butSendAlt.Text = "Send";
            butSendAlt.UseVisualStyleBackColor = true;
            butSendAlt.Click += butSendAlt_Click;
            //
            // butAltZero
            //
            butAltZero.Location = new Point(216, 325);
            butAltZero.Name = "butAltZero";
            butAltZero.Size = new Size(92, 25);
            butAltZero.TabIndex = 16;
            butAltZero.Text = "Zero";
            butAltZero.UseVisualStyleBackColor = true;
            butAltZero.Click += butAltZero_Click;
            //
            // lblRadarAltHeader
            //
            lblRadarAltHeader.AutoSize = true;
            lblRadarAltHeader.Font = new Font("Segoe UI", 9F, FontStyle.Bold);
            lblRadarAltHeader.Location = new Point(12, 366);
            lblRadarAltHeader.Name = "lblRadarAltHeader";
            lblRadarAltHeader.Size = new Size(90, 15);
            lblRadarAltHeader.TabIndex = 17;
            lblRadarAltHeader.Text = "Radar ALT (ft)";
            //
            // trkRadarAlt
            //
            trkRadarAlt.LargeChange = 250;
            trkRadarAlt.Location = new Point(12, 412);
            trkRadarAlt.Maximum = 2500;
            trkRadarAlt.Minimum = 0;
            trkRadarAlt.Name = "trkRadarAlt";
            trkRadarAlt.Size = new Size(400, 45);
            trkRadarAlt.SmallChange = 50;
            trkRadarAlt.TabIndex = 19;
            trkRadarAlt.TickFrequency = 250;
            trkRadarAlt.Scroll += trkRadarAlt_Scroll;
            //
            // lblRadarAltValue
            //
            lblRadarAltValue.AutoSize = true;
            lblRadarAltValue.Location = new Point(12, 387);
            lblRadarAltValue.Name = "lblRadarAltValue";
            lblRadarAltValue.Size = new Size(59, 15);
            lblRadarAltValue.TabIndex = 18;
            lblRadarAltValue.Text = "Value: 0 ft";
            //
            // lblRadarAltMin
            //
            lblRadarAltMin.AutoSize = true;
            lblRadarAltMin.Location = new Point(12, 460);
            lblRadarAltMin.Name = "lblRadarAltMin";
            lblRadarAltMin.Size = new Size(13, 15);
            lblRadarAltMin.TabIndex = 20;
            lblRadarAltMin.Text = "0";
            //
            // lblRadarAltMax
            //
            lblRadarAltMax.AutoSize = true;
            lblRadarAltMax.Location = new Point(357, 460);
            lblRadarAltMax.Name = "lblRadarAltMax";
            lblRadarAltMax.Size = new Size(30, 15);
            lblRadarAltMax.TabIndex = 21;
            lblRadarAltMax.Text = "2500";
            //
            // txtRadarAltInput
            //
            txtRadarAltInput.Location = new Point(12, 492);
            txtRadarAltInput.Name = "txtRadarAltInput";
            txtRadarAltInput.Size = new Size(100, 23);
            txtRadarAltInput.TabIndex = 22;
            txtRadarAltInput.Text = "0";
            txtRadarAltInput.KeyDown += txtRadarAltInput_KeyDown;
            //
            // butSendRadarAlt
            //
            butSendRadarAlt.Location = new Point(118, 491);
            butSendRadarAlt.Name = "butSendRadarAlt";
            butSendRadarAlt.Size = new Size(92, 25);
            butSendRadarAlt.TabIndex = 23;
            butSendRadarAlt.Text = "Send";
            butSendRadarAlt.UseVisualStyleBackColor = true;
            butSendRadarAlt.Click += butSendRadarAlt_Click;
            //
            // butRadarAltZero
            //
            butRadarAltZero.Location = new Point(216, 491);
            butRadarAltZero.Name = "butRadarAltZero";
            butRadarAltZero.Size = new Size(92, 25);
            butRadarAltZero.TabIndex = 24;
            butRadarAltZero.Text = "Zero";
            butRadarAltZero.UseVisualStyleBackColor = true;
            butRadarAltZero.Click += butRadarAltZero_Click;
            //
            // lblJogHeader
            //
            lblJogHeader.AutoSize = true;
            lblJogHeader.Font = new Font("Segoe UI", 9F, FontStyle.Bold);
            lblJogHeader.Location = new Point(12, 532);
            lblJogHeader.Name = "lblJogHeader";
            lblJogHeader.Size = new Size(300, 15);
            lblJogHeader.TabIndex = 25;
            lblJogHeader.Text = "ALT Direct Step Jog (raw, bypasses acceleration)";
            //
            // lblJogSteps
            //
            lblJogSteps.AutoSize = true;
            lblJogSteps.Location = new Point(12, 557);
            lblJogSteps.Name = "lblJogSteps";
            lblJogSteps.Size = new Size(70, 15);
            lblJogSteps.TabIndex = 26;
            lblJogSteps.Text = "Steps (+/-):";
            //
            // txtJogSteps
            //
            txtJogSteps.Location = new Point(12, 575);
            txtJogSteps.Name = "txtJogSteps";
            txtJogSteps.Size = new Size(100, 23);
            txtJogSteps.TabIndex = 27;
            txtJogSteps.Text = "0";
            txtJogSteps.KeyDown += txtJogSteps_KeyDown;
            //
            // lblJogInterval
            //
            lblJogInterval.AutoSize = true;
            lblJogInterval.Location = new Point(118, 557);
            lblJogInterval.Name = "lblJogInterval";
            lblJogInterval.Size = new Size(80, 15);
            lblJogInterval.TabIndex = 28;
            lblJogInterval.Text = "Interval (ms):";
            //
            // txtJogInterval
            //
            txtJogInterval.Location = new Point(118, 575);
            txtJogInterval.Name = "txtJogInterval";
            txtJogInterval.Size = new Size(100, 23);
            txtJogInterval.TabIndex = 29;
            txtJogInterval.Text = "5";
            txtJogInterval.KeyDown += txtJogInterval_KeyDown;
            //
            // butJogSend
            //
            butJogSend.Location = new Point(224, 574);
            butJogSend.Name = "butJogSend";
            butJogSend.Size = new Size(92, 25);
            butJogSend.TabIndex = 30;
            butJogSend.Text = "Jog";
            butJogSend.UseVisualStyleBackColor = true;
            butJogSend.Click += butJogSend_Click;
            //
            // lblNewGaugeHeader
            //
            lblNewGaugeHeader.AutoSize = true;
            lblNewGaugeHeader.Font = new Font("Segoe UI", 9F, FontStyle.Bold);
            lblNewGaugeHeader.Location = new Point(12, 632);
            lblNewGaugeHeader.Name = "lblNewGaugeHeader";
            lblNewGaugeHeader.Size = new Size(220, 15);
            lblNewGaugeHeader.TabIndex = 31;
            lblNewGaugeHeader.Text = "Raw Step Test (uncalibrated / bypass conversion)";
            //
            // lblNewGaugeGauge
            //
            lblNewGaugeGauge.AutoSize = true;
            lblNewGaugeGauge.Location = new Point(12, 657);
            lblNewGaugeGauge.Name = "lblNewGaugeGauge";
            lblNewGaugeGauge.Size = new Size(45, 15);
            lblNewGaugeGauge.TabIndex = 32;
            lblNewGaugeGauge.Text = "Gauge:";
            //
            // cboNewGauge
            //
            cboNewGauge.DropDownStyle = ComboBoxStyle.DropDownList;
            cboNewGauge.Items.AddRange(new object[] { "AGLRAW", "TQ", "FLAPS", "AOA", "GFORCE", "SPDMAX", "IASRAW", "ALTRAW", "VSIRAW", "OILTRAW", "OILPRAW", "XMSNTRAW", "XMSNPRAW", "ITTRAW", "RPMERAW", "RPMRRAW", "N1RAW", "FUELRAW", "FUELLOADRAW", "ELECTRICALLOADRAW" });
            cboNewGauge.Location = new Point(12, 675);
            cboNewGauge.Name = "cboNewGauge";
            cboNewGauge.Size = new Size(100, 23);
            cboNewGauge.TabIndex = 33;
            //
            // lblNewGaugeSteps
            //
            lblNewGaugeSteps.AutoSize = true;
            lblNewGaugeSteps.Location = new Point(118, 657);
            lblNewGaugeSteps.Name = "lblNewGaugeSteps";
            lblNewGaugeSteps.Size = new Size(70, 15);
            lblNewGaugeSteps.TabIndex = 34;
            lblNewGaugeSteps.Text = "Steps (+/-):";
            //
            // txtNewGaugeSteps
            //
            txtNewGaugeSteps.Location = new Point(118, 675);
            txtNewGaugeSteps.Name = "txtNewGaugeSteps";
            txtNewGaugeSteps.Size = new Size(100, 23);
            txtNewGaugeSteps.TabIndex = 35;
            txtNewGaugeSteps.Text = "0";
            txtNewGaugeSteps.KeyDown += txtNewGaugeSteps_KeyDown;
            //
            // butNewGaugeSend
            //
            butNewGaugeSend.Location = new Point(224, 674);
            butNewGaugeSend.Name = "butNewGaugeSend";
            butNewGaugeSend.Size = new Size(92, 25);
            butNewGaugeSend.TabIndex = 36;
            butNewGaugeSend.Text = "Send";
            butNewGaugeSend.UseVisualStyleBackColor = true;
            butNewGaugeSend.Click += butNewGaugeSend_Click;
            //
            // butNewGaugeZero
            //
            butNewGaugeZero.Location = new Point(322, 674);
            butNewGaugeZero.Name = "butNewGaugeZero";
            butNewGaugeZero.Size = new Size(92, 25);
            butNewGaugeZero.TabIndex = 37;
            butNewGaugeZero.Text = "Zero";
            butNewGaugeZero.UseVisualStyleBackColor = true;
            butNewGaugeZero.Click += butNewGaugeZero_Click;
            //
            // butNewGaugeStepBack
            //
            butNewGaugeStepBack.Location = new Point(420, 674);
            butNewGaugeStepBack.Name = "butNewGaugeStepBack";
            butNewGaugeStepBack.Size = new Size(85, 25);
            butNewGaugeStepBack.TabIndex = 79;
            butNewGaugeStepBack.Text = "-1 Step";
            butNewGaugeStepBack.UseVisualStyleBackColor = true;
            butNewGaugeStepBack.Click += butNewGaugeStepBack_Click;
            //
            // butNewGaugeStepFwd
            //
            butNewGaugeStepFwd.Location = new Point(511, 674);
            butNewGaugeStepFwd.Name = "butNewGaugeStepFwd";
            butNewGaugeStepFwd.Size = new Size(85, 25);
            butNewGaugeStepFwd.TabIndex = 80;
            butNewGaugeStepFwd.Text = "+1 Step";
            butNewGaugeStepFwd.UseVisualStyleBackColor = true;
            butNewGaugeStepFwd.Click += butNewGaugeStepFwd_Click;
            //
            // lblEgtHeader
            //
            lblEgtHeader.AutoSize = true;
            lblEgtHeader.Font = new Font("Segoe UI", 9F, FontStyle.Bold);
            lblEgtHeader.Location = new Point(12, 732);
            lblEgtHeader.Name = "lblEgtHeader";
            lblEgtHeader.Size = new Size(100, 15);
            lblEgtHeader.TabIndex = 38;
            lblEgtHeader.Text = "ITT / EGT (C)";
            //
            // trkEgt
            //
            trkEgt.LargeChange = 100;
            trkEgt.Location = new Point(12, 778);
            trkEgt.Maximum = 900;
            trkEgt.Minimum = 0;
            trkEgt.Name = "trkEgt";
            trkEgt.Size = new Size(400, 45);
            trkEgt.SmallChange = 25;
            trkEgt.TabIndex = 40;
            trkEgt.TickFrequency = 100;
            trkEgt.Scroll += trkEgt_Scroll;
            //
            // lblEgtValue
            //
            lblEgtValue.AutoSize = true;
            lblEgtValue.Location = new Point(12, 753);
            lblEgtValue.Name = "lblEgtValue";
            lblEgtValue.Size = new Size(60, 15);
            lblEgtValue.TabIndex = 39;
            lblEgtValue.Text = "Value: 0 C";
            //
            // lblEgtMin
            //
            lblEgtMin.AutoSize = true;
            lblEgtMin.Location = new Point(12, 826);
            lblEgtMin.Name = "lblEgtMin";
            lblEgtMin.Size = new Size(13, 15);
            lblEgtMin.TabIndex = 41;
            lblEgtMin.Text = "0";
            //
            // lblEgtMax
            //
            lblEgtMax.AutoSize = true;
            lblEgtMax.Location = new Point(357, 826);
            lblEgtMax.Name = "lblEgtMax";
            lblEgtMax.Size = new Size(24, 15);
            lblEgtMax.TabIndex = 42;
            lblEgtMax.Text = "900";
            //
            // txtEgtInput
            //
            txtEgtInput.Location = new Point(12, 858);
            txtEgtInput.Name = "txtEgtInput";
            txtEgtInput.Size = new Size(100, 23);
            txtEgtInput.TabIndex = 43;
            txtEgtInput.Text = "0";
            txtEgtInput.KeyDown += txtEgtInput_KeyDown;
            //
            // butSendEgt
            //
            butSendEgt.Location = new Point(118, 857);
            butSendEgt.Name = "butSendEgt";
            butSendEgt.Size = new Size(92, 25);
            butSendEgt.TabIndex = 44;
            butSendEgt.Text = "Send";
            butSendEgt.UseVisualStyleBackColor = true;
            butSendEgt.Click += butSendEgt_Click;
            //
            // butEgtZero
            //
            butEgtZero.Location = new Point(216, 857);
            butEgtZero.Name = "butEgtZero";
            butEgtZero.Size = new Size(92, 25);
            butEgtZero.TabIndex = 45;
            butEgtZero.Text = "Zero";
            butEgtZero.UseVisualStyleBackColor = true;
            butEgtZero.Click += butEgtZero_Click;
            //
            // lblRealGaugesHeader
            //
            lblRealGaugesHeader.AutoSize = true;
            lblRealGaugesHeader.Font = new Font("Segoe UI", 9F, FontStyle.Bold);
            lblRealGaugesHeader.Location = new Point(12, 920);
            lblRealGaugesHeader.Name = "lblRealGaugesHeader";
            lblRealGaugesHeader.Size = new Size(300, 15);
            lblRealGaugesHeader.TabIndex = 46;
            lblRealGaugesHeader.Text = "Real-Value Gauges (uncalibrated linear scale)";
            //
            // lblEotRow
            //
            lblEotRow.AutoSize = true;
            lblEotRow.Location = new Point(12, 952);
            lblEotRow.Name = "lblEotRow";
            lblEotRow.Size = new Size(200, 15);
            lblEotRow.TabIndex = 47;
            lblEotRow.Text = "OILT - Engine Oil Temp (C, 0-150):";
            //
            // txtEot
            //
            txtEot.Location = new Point(220, 948);
            txtEot.Name = "txtEot";
            txtEot.Size = new Size(80, 23);
            txtEot.TabIndex = 48;
            txtEot.Text = "0";
            txtEot.KeyDown += txtEot_KeyDown;
            //
            // butSendEot
            //
            butSendEot.Location = new Point(306, 947);
            butSendEot.Name = "butSendEot";
            butSendEot.Size = new Size(70, 25);
            butSendEot.TabIndex = 49;
            butSendEot.Text = "Send";
            butSendEot.UseVisualStyleBackColor = true;
            butSendEot.Click += butSendEot_Click;
            //
            // lblEopRow
            //
            lblEopRow.AutoSize = true;
            lblEopRow.Location = new Point(12, 986);
            lblEopRow.Name = "lblEopRow";
            lblEopRow.Size = new Size(200, 15);
            lblEopRow.TabIndex = 50;
            lblEopRow.Text = "OILP - Engine Oil Pressure (PSI, 0-150):";
            //
            // txtEop
            //
            txtEop.Location = new Point(220, 982);
            txtEop.Name = "txtEop";
            txtEop.Size = new Size(80, 23);
            txtEop.TabIndex = 51;
            txtEop.Text = "0";
            txtEop.KeyDown += txtEop_KeyDown;
            //
            // butSendEop
            //
            butSendEop.Location = new Point(306, 981);
            butSendEop.Name = "butSendEop";
            butSendEop.Size = new Size(70, 25);
            butSendEop.TabIndex = 52;
            butSendEop.Text = "Send";
            butSendEop.UseVisualStyleBackColor = true;
            butSendEop.Click += butSendEop_Click;
            //
            // lblXotRow
            //
            lblXotRow.AutoSize = true;
            lblXotRow.Location = new Point(12, 1020);
            lblXotRow.Name = "lblXotRow";
            lblXotRow.Size = new Size(200, 15);
            lblXotRow.TabIndex = 53;
            lblXotRow.Text = "XMSNT - Transmission Oil Temp (C, 0-150):";
            //
            // txtXot
            //
            txtXot.Location = new Point(220, 1016);
            txtXot.Name = "txtXot";
            txtXot.Size = new Size(80, 23);
            txtXot.TabIndex = 54;
            txtXot.Text = "0";
            txtXot.KeyDown += txtXot_KeyDown;
            //
            // butSendXot
            //
            butSendXot.Location = new Point(306, 1015);
            butSendXot.Name = "butSendXot";
            butSendXot.Size = new Size(70, 25);
            butSendXot.TabIndex = 55;
            butSendXot.Text = "Send";
            butSendXot.UseVisualStyleBackColor = true;
            butSendXot.Click += butSendXot_Click;
            //
            // lblXopRow
            //
            lblXopRow.AutoSize = true;
            lblXopRow.Location = new Point(12, 1054);
            lblXopRow.Name = "lblXopRow";
            lblXopRow.Size = new Size(200, 15);
            lblXopRow.TabIndex = 56;
            lblXopRow.Text = "XMSNP - Transmission Oil Pressure (PSI, 0-150):";
            //
            // txtXop
            //
            txtXop.Location = new Point(220, 1050);
            txtXop.Name = "txtXop";
            txtXop.Size = new Size(80, 23);
            txtXop.TabIndex = 57;
            txtXop.Text = "0";
            txtXop.KeyDown += txtXop_KeyDown;
            //
            // butSendXop
            //
            butSendXop.Location = new Point(306, 1049);
            butSendXop.Name = "butSendXop";
            butSendXop.Size = new Size(70, 25);
            butSendXop.TabIndex = 58;
            butSendXop.Text = "Send";
            butSendXop.UseVisualStyleBackColor = true;
            butSendXop.Click += butSendXop_Click;
            //
            // lblGpRow
            //
            lblGpRow.AutoSize = true;
            lblGpRow.Location = new Point(12, 1088);
            lblGpRow.Name = "lblGpRow";
            lblGpRow.Size = new Size(200, 15);
            lblGpRow.TabIndex = 65;
            lblGpRow.Text = "N1 - Gas Producer (%, 0-105):";
            //
            // txtGp
            //
            txtGp.Location = new Point(220, 1084);
            txtGp.Name = "txtGp";
            txtGp.Size = new Size(80, 23);
            txtGp.TabIndex = 66;
            txtGp.Text = "0";
            txtGp.KeyDown += txtGp_KeyDown;
            //
            // butSendGp
            //
            butSendGp.Location = new Point(306, 1083);
            butSendGp.Name = "butSendGp";
            butSendGp.Size = new Size(70, 25);
            butSendGp.TabIndex = 67;
            butSendGp.Text = "Send";
            butSendGp.UseVisualStyleBackColor = true;
            butSendGp.Click += butSendGp_Click;
            //
            // lblFaRow
            //
            lblFaRow.AutoSize = true;
            lblFaRow.Location = new Point(12, 1122);
            lblFaRow.Name = "lblFaRow";
            lblFaRow.Size = new Size(200, 15);
            lblFaRow.TabIndex = 68;
            lblFaRow.Text = "FUEL - Fuel Available (gal, 0-75):";
            //
            // txtFa
            //
            txtFa.Location = new Point(220, 1118);
            txtFa.Name = "txtFa";
            txtFa.Size = new Size(80, 23);
            txtFa.TabIndex = 69;
            txtFa.Text = "0";
            txtFa.KeyDown += txtFa_KeyDown;
            //
            // butSendFa
            //
            butSendFa.Location = new Point(306, 1117);
            butSendFa.Name = "butSendFa";
            butSendFa.Size = new Size(70, 25);
            butSendFa.TabIndex = 70;
            butSendFa.Text = "Send";
            butSendFa.UseVisualStyleBackColor = true;
            butSendFa.Click += butSendFa_Click;
            //
            // lblIasHeader
            //
            lblIasHeader.AutoSize = true;
            lblIasHeader.Font = new Font("Segoe UI", 9F, FontStyle.Bold);
            lblIasHeader.Location = new Point(12, 1179);
            lblIasHeader.Name = "lblIasHeader";
            lblIasHeader.Size = new Size(120, 15);
            lblIasHeader.TabIndex = 71;
            lblIasHeader.Text = "IAS (knots)";
            //
            // trkIas
            //
            trkIas.LargeChange = 20;
            trkIas.Location = new Point(12, 1225);
            trkIas.Maximum = 140;
            trkIas.Minimum = 0;
            trkIas.Name = "trkIas";
            trkIas.Size = new Size(400, 45);
            trkIas.SmallChange = 5;
            trkIas.TabIndex = 73;
            trkIas.TickFrequency = 20;
            trkIas.Scroll += trkIas_Scroll;
            //
            // lblIasValue
            //
            lblIasValue.AutoSize = true;
            lblIasValue.Location = new Point(12, 1200);
            lblIasValue.Name = "lblIasValue";
            lblIasValue.Size = new Size(80, 15);
            lblIasValue.TabIndex = 72;
            lblIasValue.Text = "Value: 0 kt";
            //
            // lblIasMin
            //
            lblIasMin.AutoSize = true;
            lblIasMin.Location = new Point(12, 1273);
            lblIasMin.Name = "lblIasMin";
            lblIasMin.Size = new Size(13, 15);
            lblIasMin.TabIndex = 74;
            lblIasMin.Text = "0";
            //
            // lblIasMax
            //
            lblIasMax.AutoSize = true;
            lblIasMax.Location = new Point(357, 1273);
            lblIasMax.Name = "lblIasMax";
            lblIasMax.Size = new Size(24, 15);
            lblIasMax.TabIndex = 75;
            lblIasMax.Text = "140";
            //
            // txtIasInput
            //
            txtIasInput.Location = new Point(12, 1305);
            txtIasInput.Name = "txtIasInput";
            txtIasInput.Size = new Size(100, 23);
            txtIasInput.TabIndex = 76;
            txtIasInput.Text = "0";
            txtIasInput.KeyDown += txtIasInput_KeyDown;
            //
            // butSendIas
            //
            butSendIas.Location = new Point(118, 1304);
            butSendIas.Name = "butSendIas";
            butSendIas.Size = new Size(92, 25);
            butSendIas.TabIndex = 77;
            butSendIas.Text = "Send";
            butSendIas.UseVisualStyleBackColor = true;
            butSendIas.Click += butSendIas_Click;
            //
            // butIasZero
            //
            butIasZero.Location = new Point(216, 1304);
            butIasZero.Name = "butIasZero";
            butIasZero.Size = new Size(92, 25);
            butIasZero.TabIndex = 78;
            butIasZero.Text = "Zero";
            butIasZero.UseVisualStyleBackColor = true;
            butIasZero.Click += butIasZero_Click;
            //
            // lblRpmeHeader
            //
            lblRpmeHeader.AutoSize = true;
            lblRpmeHeader.Font = new Font("Segoe UI", 9F, FontStyle.Bold);
            lblRpmeHeader.Location = new Point(12, 1345);
            lblRpmeHeader.Name = "lblRpmeHeader";
            lblRpmeHeader.Size = new Size(230, 15);
            lblRpmeHeader.TabIndex = 79;
            lblRpmeHeader.Text = "RPME - Turbine/Engine Speed (%)";
            //
            // trkRpme
            //
            trkRpme.LargeChange = 20;
            trkRpme.Location = new Point(12, 1391);
            trkRpme.Maximum = 117;
            trkRpme.Minimum = 0;
            trkRpme.Name = "trkRpme";
            trkRpme.Size = new Size(400, 45);
            trkRpme.SmallChange = 5;
            trkRpme.TabIndex = 81;
            trkRpme.TickFrequency = 20;
            trkRpme.Scroll += trkRpme_Scroll;
            //
            // lblRpmeValue
            //
            lblRpmeValue.AutoSize = true;
            lblRpmeValue.Location = new Point(12, 1366);
            lblRpmeValue.Name = "lblRpmeValue";
            lblRpmeValue.Size = new Size(60, 15);
            lblRpmeValue.TabIndex = 80;
            lblRpmeValue.Text = "Value: 0 %";
            //
            // lblRpmeMin
            //
            lblRpmeMin.AutoSize = true;
            lblRpmeMin.Location = new Point(12, 1439);
            lblRpmeMin.Name = "lblRpmeMin";
            lblRpmeMin.Size = new Size(13, 15);
            lblRpmeMin.TabIndex = 82;
            lblRpmeMin.Text = "0";
            //
            // lblRpmeMax
            //
            lblRpmeMax.AutoSize = true;
            lblRpmeMax.Location = new Point(357, 1439);
            lblRpmeMax.Name = "lblRpmeMax";
            lblRpmeMax.Size = new Size(24, 15);
            lblRpmeMax.TabIndex = 83;
            lblRpmeMax.Text = "117";
            //
            // txtRpmeInput
            //
            txtRpmeInput.Location = new Point(12, 1471);
            txtRpmeInput.Name = "txtRpmeInput";
            txtRpmeInput.Size = new Size(100, 23);
            txtRpmeInput.TabIndex = 84;
            txtRpmeInput.Text = "0";
            txtRpmeInput.KeyDown += txtRpmeInput_KeyDown;
            //
            // butSendRpme
            //
            butSendRpme.Location = new Point(118, 1470);
            butSendRpme.Name = "butSendRpme";
            butSendRpme.Size = new Size(92, 25);
            butSendRpme.TabIndex = 85;
            butSendRpme.Text = "Send";
            butSendRpme.UseVisualStyleBackColor = true;
            butSendRpme.Click += butSendRpme_Click;
            //
            // butRpmeZero
            //
            butRpmeZero.Location = new Point(216, 1470);
            butRpmeZero.Name = "butRpmeZero";
            butRpmeZero.Size = new Size(92, 25);
            butRpmeZero.TabIndex = 86;
            butRpmeZero.Text = "Zero";
            butRpmeZero.UseVisualStyleBackColor = true;
            butRpmeZero.Click += butRpmeZero_Click;
            //
            // lblRpmrHeader
            //
            lblRpmrHeader.AutoSize = true;
            lblRpmrHeader.Font = new Font("Segoe UI", 9F, FontStyle.Bold);
            lblRpmrHeader.Location = new Point(12, 1511);
            lblRpmrHeader.Name = "lblRpmrHeader";
            lblRpmrHeader.Size = new Size(150, 15);
            lblRpmrHeader.TabIndex = 87;
            lblRpmrHeader.Text = "RPMR - Rotor Speed (%)";
            //
            // trkRpmr
            //
            trkRpmr.LargeChange = 20;
            trkRpmr.Location = new Point(12, 1557);
            trkRpmr.Maximum = 117;
            trkRpmr.Minimum = 0;
            trkRpmr.Name = "trkRpmr";
            trkRpmr.Size = new Size(400, 45);
            trkRpmr.SmallChange = 5;
            trkRpmr.TabIndex = 89;
            trkRpmr.TickFrequency = 20;
            trkRpmr.Scroll += trkRpmr_Scroll;
            //
            // lblRpmrValue
            //
            lblRpmrValue.AutoSize = true;
            lblRpmrValue.Location = new Point(12, 1532);
            lblRpmrValue.Name = "lblRpmrValue";
            lblRpmrValue.Size = new Size(60, 15);
            lblRpmrValue.TabIndex = 88;
            lblRpmrValue.Text = "Value: 0 %";
            //
            // lblRpmrMin
            //
            lblRpmrMin.AutoSize = true;
            lblRpmrMin.Location = new Point(12, 1605);
            lblRpmrMin.Name = "lblRpmrMin";
            lblRpmrMin.Size = new Size(13, 15);
            lblRpmrMin.TabIndex = 90;
            lblRpmrMin.Text = "0";
            //
            // lblRpmrMax
            //
            lblRpmrMax.AutoSize = true;
            lblRpmrMax.Location = new Point(357, 1605);
            lblRpmrMax.Name = "lblRpmrMax";
            lblRpmrMax.Size = new Size(24, 15);
            lblRpmrMax.TabIndex = 91;
            lblRpmrMax.Text = "117";
            //
            // txtRpmrInput
            //
            txtRpmrInput.Location = new Point(12, 1637);
            txtRpmrInput.Name = "txtRpmrInput";
            txtRpmrInput.Size = new Size(100, 23);
            txtRpmrInput.TabIndex = 92;
            txtRpmrInput.Text = "0";
            txtRpmrInput.KeyDown += txtRpmrInput_KeyDown;
            //
            // butSendRpmr
            //
            butSendRpmr.Location = new Point(118, 1636);
            butSendRpmr.Name = "butSendRpmr";
            butSendRpmr.Size = new Size(92, 25);
            butSendRpmr.TabIndex = 93;
            butSendRpmr.Text = "Send";
            butSendRpmr.UseVisualStyleBackColor = true;
            butSendRpmr.Click += butSendRpmr_Click;
            //
            // butRpmrZero
            //
            butRpmrZero.Location = new Point(216, 1636);
            butRpmrZero.Name = "butRpmrZero";
            butRpmrZero.Size = new Size(92, 25);
            butRpmrZero.TabIndex = 94;
            butRpmrZero.Text = "Zero";
            butRpmrZero.UseVisualStyleBackColor = true;
            butRpmrZero.Click += butRpmrZero_Click;
            //
            // lblDualStepperHeader
            //
            lblDualStepperHeader.AutoSize = true;
            lblDualStepperHeader.Font = new Font("Segoe UI", 9F, FontStyle.Bold);
            lblDualStepperHeader.Location = new Point(12, 1699);
            lblDualStepperHeader.Name = "lblDualStepperHeader";
            lblDualStepperHeader.Size = new Size(300, 15);
            lblDualStepperHeader.TabIndex = 95;
            lblDualStepperHeader.Text = "Dual Stepper (172.16.1.106) Test";
            //
            // lblFuelLoadRow
            //
            lblFuelLoadRow.AutoSize = true;
            lblFuelLoadRow.Location = new Point(12, 1731);
            lblFuelLoadRow.Name = "lblFuelLoadRow";
            lblFuelLoadRow.Size = new Size(200, 15);
            lblFuelLoadRow.TabIndex = 96;
            lblFuelLoadRow.Text = "FUELLOAD - Fuel Load (PSI, 0-30):";
            //
            // txtFuelLoad
            //
            txtFuelLoad.Location = new Point(220, 1727);
            txtFuelLoad.Name = "txtFuelLoad";
            txtFuelLoad.Size = new Size(80, 23);
            txtFuelLoad.TabIndex = 97;
            txtFuelLoad.Text = "0";
            txtFuelLoad.KeyDown += txtFuelLoad_KeyDown;
            //
            // butSendFuelLoad
            //
            butSendFuelLoad.Location = new Point(306, 1726);
            butSendFuelLoad.Name = "butSendFuelLoad";
            butSendFuelLoad.Size = new Size(70, 25);
            butSendFuelLoad.TabIndex = 98;
            butSendFuelLoad.Text = "Send";
            butSendFuelLoad.UseVisualStyleBackColor = true;
            butSendFuelLoad.Click += butSendFuelLoad_Click;
            //
            // lblElectricalLoadRow
            //
            lblElectricalLoadRow.AutoSize = true;
            lblElectricalLoadRow.Location = new Point(12, 1765);
            lblElectricalLoadRow.Name = "lblElectricalLoadRow";
            lblElectricalLoadRow.Size = new Size(200, 15);
            lblElectricalLoadRow.TabIndex = 99;
            lblElectricalLoadRow.Text = "ELECTRICALLOAD - Electrical Load (%, 0-100):";
            //
            // txtElectricalLoad
            //
            txtElectricalLoad.Location = new Point(220, 1761);
            txtElectricalLoad.Name = "txtElectricalLoad";
            txtElectricalLoad.Size = new Size(80, 23);
            txtElectricalLoad.TabIndex = 100;
            txtElectricalLoad.Text = "0";
            txtElectricalLoad.KeyDown += txtElectricalLoad_KeyDown;
            //
            // butSendElectricalLoad
            //
            butSendElectricalLoad.Location = new Point(306, 1760);
            butSendElectricalLoad.Name = "butSendElectricalLoad";
            butSendElectricalLoad.Size = new Size(70, 25);
            butSendElectricalLoad.TabIndex = 101;
            butSendElectricalLoad.Text = "Send";
            butSendElectricalLoad.UseVisualStyleBackColor = true;
            butSendElectricalLoad.Click += butSendElectricalLoad_Click;
            //
            // lblClockRow
            //
            lblClockRow.AutoSize = true;
            lblClockRow.Location = new Point(12, 1799);
            lblClockRow.Name = "lblClockRow";
            lblClockRow.Size = new Size(200, 15);
            lblClockRow.TabIndex = 102;
            lblClockRow.Text = "ZULU - Clock OLED (HH:MM):";
            //
            // txtClockHour
            //
            txtClockHour.Location = new Point(220, 1795);
            txtClockHour.Name = "txtClockHour";
            txtClockHour.Size = new Size(40, 23);
            txtClockHour.TabIndex = 103;
            txtClockHour.Text = "0";
            txtClockHour.TextAlign = HorizontalAlignment.Center;
            txtClockHour.KeyDown += txtClock_KeyDown;
            //
            // lblClockColon
            //
            lblClockColon.AutoSize = true;
            lblClockColon.Location = new Point(263, 1798);
            lblClockColon.Name = "lblClockColon";
            lblClockColon.Size = new Size(12, 15);
            lblClockColon.TabIndex = 104;
            lblClockColon.Text = ":";
            //
            // txtClockMinute
            //
            txtClockMinute.Location = new Point(278, 1795);
            txtClockMinute.Name = "txtClockMinute";
            txtClockMinute.Size = new Size(40, 23);
            txtClockMinute.TabIndex = 105;
            txtClockMinute.Text = "0";
            txtClockMinute.TextAlign = HorizontalAlignment.Center;
            txtClockMinute.KeyDown += txtClock_KeyDown;
            //
            // butSendClock
            //
            butSendClock.Location = new Point(326, 1794);
            butSendClock.Name = "butSendClock";
            butSendClock.Size = new Size(70, 25);
            butSendClock.TabIndex = 106;
            butSendClock.Text = "Send";
            butSendClock.UseVisualStyleBackColor = true;
            butSendClock.Click += butSendClock_Click;
            //
            // frmMain
            //
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            AutoScroll = true;
            AutoScrollMinSize = new Size(610, 1865);
            ClientSize = new Size(630, 760);
            Controls.Add(butNewGaugeStepFwd);
            Controls.Add(butNewGaugeStepBack);
            Controls.Add(butSendClock);
            Controls.Add(txtClockMinute);
            Controls.Add(lblClockColon);
            Controls.Add(txtClockHour);
            Controls.Add(lblClockRow);
            Controls.Add(butSendElectricalLoad);
            Controls.Add(txtElectricalLoad);
            Controls.Add(lblElectricalLoadRow);
            Controls.Add(butSendFuelLoad);
            Controls.Add(txtFuelLoad);
            Controls.Add(lblFuelLoadRow);
            Controls.Add(lblDualStepperHeader);
            Controls.Add(butRpmrZero);
            Controls.Add(butSendRpmr);
            Controls.Add(txtRpmrInput);
            Controls.Add(lblRpmrMax);
            Controls.Add(lblRpmrMin);
            Controls.Add(trkRpmr);
            Controls.Add(lblRpmrValue);
            Controls.Add(lblRpmrHeader);
            Controls.Add(butRpmeZero);
            Controls.Add(butSendRpme);
            Controls.Add(txtRpmeInput);
            Controls.Add(lblRpmeMax);
            Controls.Add(lblRpmeMin);
            Controls.Add(trkRpme);
            Controls.Add(lblRpmeValue);
            Controls.Add(lblRpmeHeader);
            Controls.Add(butIasZero);
            Controls.Add(butSendIas);
            Controls.Add(txtIasInput);
            Controls.Add(lblIasMax);
            Controls.Add(lblIasMin);
            Controls.Add(trkIas);
            Controls.Add(lblIasValue);
            Controls.Add(lblIasHeader);
            Controls.Add(butSendFa);
            Controls.Add(txtFa);
            Controls.Add(lblFaRow);
            Controls.Add(butSendGp);
            Controls.Add(txtGp);
            Controls.Add(lblGpRow);
            Controls.Add(butSendXop);
            Controls.Add(txtXop);
            Controls.Add(lblXopRow);
            Controls.Add(butSendXot);
            Controls.Add(txtXot);
            Controls.Add(lblXotRow);
            Controls.Add(butSendEop);
            Controls.Add(txtEop);
            Controls.Add(lblEopRow);
            Controls.Add(butSendEot);
            Controls.Add(txtEot);
            Controls.Add(lblEotRow);
            Controls.Add(lblRealGaugesHeader);
            Controls.Add(butEgtZero);
            Controls.Add(butSendEgt);
            Controls.Add(txtEgtInput);
            Controls.Add(lblEgtMax);
            Controls.Add(lblEgtMin);
            Controls.Add(trkEgt);
            Controls.Add(lblEgtValue);
            Controls.Add(lblEgtHeader);
            Controls.Add(butNewGaugeZero);
            Controls.Add(butNewGaugeSend);
            Controls.Add(txtNewGaugeSteps);
            Controls.Add(lblNewGaugeSteps);
            Controls.Add(cboNewGauge);
            Controls.Add(lblNewGaugeGauge);
            Controls.Add(lblNewGaugeHeader);
            Controls.Add(butJogSend);
            Controls.Add(txtJogInterval);
            Controls.Add(lblJogInterval);
            Controls.Add(txtJogSteps);
            Controls.Add(lblJogSteps);
            Controls.Add(lblJogHeader);
            Controls.Add(butRadarAltZero);
            Controls.Add(butSendRadarAlt);
            Controls.Add(txtRadarAltInput);
            Controls.Add(lblRadarAltMax);
            Controls.Add(lblRadarAltMin);
            Controls.Add(trkRadarAlt);
            Controls.Add(lblRadarAltValue);
            Controls.Add(lblRadarAltHeader);
            Controls.Add(butAltZero);
            Controls.Add(butSendAlt);
            Controls.Add(txtAltInput);
            Controls.Add(lblAltMax);
            Controls.Add(lblAltMin);
            Controls.Add(trkAlt);
            Controls.Add(lblAltValue);
            Controls.Add(lblAltHeader);
            Controls.Add(butZero);
            Controls.Add(butSendRaw);
            Controls.Add(txtRawInput);
            Controls.Add(lblMax);
            Controls.Add(lblMin);
            Controls.Add(lblValue);
            Controls.Add(trkVsi);
            Controls.Add(lblVsiHeader);
            Controls.Add(lblTarget);
            Name = "frmMain";
            Text = "Stepper VSI/ALT Tester";
            ((System.ComponentModel.ISupportInitialize)trkVsi).EndInit();
            ((System.ComponentModel.ISupportInitialize)trkAlt).EndInit();
            ((System.ComponentModel.ISupportInitialize)trkRadarAlt).EndInit();
            ((System.ComponentModel.ISupportInitialize)trkEgt).EndInit();
            ((System.ComponentModel.ISupportInitialize)trkIas).EndInit();
            ((System.ComponentModel.ISupportInitialize)trkRpme).EndInit();
            ((System.ComponentModel.ISupportInitialize)trkRpmr).EndInit();
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private Label lblTarget;
        private Label lblVsiHeader;
        private TrackBar trkVsi;
        private Label lblValue;
        private Label lblMin;
        private Label lblMax;
        private TextBox txtRawInput;
        private Button butSendRaw;
        private Button butZero;
        private Label lblAltHeader;
        private TrackBar trkAlt;
        private Label lblAltValue;
        private Label lblAltMin;
        private Label lblAltMax;
        private TextBox txtAltInput;
        private Button butSendAlt;
        private Button butAltZero;
        private Label lblRadarAltHeader;
        private TrackBar trkRadarAlt;
        private Label lblRadarAltValue;
        private Label lblRadarAltMin;
        private Label lblRadarAltMax;
        private TextBox txtRadarAltInput;
        private Button butSendRadarAlt;
        private Button butRadarAltZero;
        private Label lblJogHeader;
        private Label lblJogSteps;
        private TextBox txtJogSteps;
        private Label lblJogInterval;
        private TextBox txtJogInterval;
        private Button butJogSend;
        private Label lblNewGaugeHeader;
        private Label lblNewGaugeGauge;
        private ComboBox cboNewGauge;
        private Label lblNewGaugeSteps;
        private TextBox txtNewGaugeSteps;
        private Button butNewGaugeSend;
        private Button butNewGaugeZero;
        private Button butNewGaugeStepBack;
        private Button butNewGaugeStepFwd;
        private Label lblEgtHeader;
        private TrackBar trkEgt;
        private Label lblEgtValue;
        private Label lblEgtMin;
        private Label lblEgtMax;
        private TextBox txtEgtInput;
        private Button butSendEgt;
        private Button butEgtZero;
        private Label lblRealGaugesHeader;
        private Label lblEotRow;
        private TextBox txtEot;
        private Button butSendEot;
        private Label lblEopRow;
        private TextBox txtEop;
        private Button butSendEop;
        private Label lblXotRow;
        private TextBox txtXot;
        private Button butSendXot;
        private Label lblXopRow;
        private TextBox txtXop;
        private Button butSendXop;
        private Label lblGpRow;
        private TextBox txtGp;
        private Button butSendGp;
        private Label lblFaRow;
        private TextBox txtFa;
        private Button butSendFa;
        private Label lblIasHeader;
        private TrackBar trkIas;
        private Label lblIasValue;
        private Label lblIasMin;
        private Label lblIasMax;
        private TextBox txtIasInput;
        private Button butSendIas;
        private Button butIasZero;
        private Label lblRpmeHeader;
        private TrackBar trkRpme;
        private Label lblRpmeValue;
        private Label lblRpmeMin;
        private Label lblRpmeMax;
        private TextBox txtRpmeInput;
        private Button butSendRpme;
        private Button butRpmeZero;
        private Label lblRpmrHeader;
        private TrackBar trkRpmr;
        private Label lblRpmrValue;
        private Label lblRpmrMin;
        private Label lblRpmrMax;
        private TextBox txtRpmrInput;
        private Button butSendRpmr;
        private Button butRpmrZero;
        private Label lblDualStepperHeader;
        private Label lblFuelLoadRow;
        private TextBox txtFuelLoad;
        private Button butSendFuelLoad;
        private Label lblElectricalLoadRow;
        private TextBox txtElectricalLoad;
        private Button butSendElectricalLoad;
        private Label lblClockRow;
        private TextBox txtClockHour;
        private Label lblClockColon;
        private TextBox txtClockMinute;
        private Button butSendClock;
    }
}
