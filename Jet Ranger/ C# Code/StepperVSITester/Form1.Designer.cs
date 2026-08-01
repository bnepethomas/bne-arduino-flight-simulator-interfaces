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
            ((System.ComponentModel.ISupportInitialize)trkVsi).BeginInit();
            ((System.ComponentModel.ISupportInitialize)trkAlt).BeginInit();
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
            // frmMain
            //
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(424, 366);
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
    }
}
