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
            ((System.ComponentModel.ISupportInitialize)trkVsi).BeginInit();
            ((System.ComponentModel.ISupportInitialize)trkAlt).BeginInit();
            ((System.ComponentModel.ISupportInitialize)trkRadarAlt).BeginInit();
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
            lblNewGaugeHeader.Text = "New Gauges (raw steps, uncalibrated)";
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
            cboNewGauge.Items.AddRange(new object[] { "EOT", "XOT", "XOP", "EGT", "TS", "RS", "FA", "ET", "GP", "EOP" });
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
            // frmMain
            //
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(424, 716);
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
    }
}
