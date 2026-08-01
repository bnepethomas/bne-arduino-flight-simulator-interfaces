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
            trkVsi = new TrackBar();
            lblValue = new Label();
            lblMin = new Label();
            lblMax = new Label();
            txtRawInput = new TextBox();
            butSendRaw = new Button();
            butZero = new Button();
            ((System.ComponentModel.ISupportInitialize)trkVsi).BeginInit();
            SuspendLayout();
            //
            // lblTarget
            //
            lblTarget.AutoSize = true;
            lblTarget.Location = new Point(12, 9);
            lblTarget.Name = "lblTarget";
            lblTarget.Size = new Size(220, 15);
            lblTarget.TabIndex = 0;
            lblTarget.Text = "Sending \"D,VSI:<fpm>\" to 172.16.1.105:13136";
            //
            // trkVsi
            //
            trkVsi.LargeChange = 250;
            trkVsi.Location = new Point(12, 60);
            trkVsi.Maximum = 1750;
            trkVsi.Minimum = -1750;
            trkVsi.Name = "trkVsi";
            trkVsi.Size = new Size(400, 45);
            trkVsi.SmallChange = 50;
            trkVsi.TabIndex = 1;
            trkVsi.TickFrequency = 250;
            trkVsi.Scroll += trkVsi_Scroll;
            //
            // lblValue
            //
            lblValue.AutoSize = true;
            lblValue.Location = new Point(12, 34);
            lblValue.Name = "lblValue";
            lblValue.Size = new Size(68, 15);
            lblValue.TabIndex = 2;
            lblValue.Text = "Value: 0 fpm";
            //
            // lblMin
            //
            lblMin.AutoSize = true;
            lblMin.Location = new Point(12, 108);
            lblMin.Name = "lblMin";
            lblMin.Size = new Size(48, 15);
            lblMin.TabIndex = 3;
            lblMin.Text = "-1750";
            //
            // lblMax
            //
            lblMax.AutoSize = true;
            lblMax.Location = new Point(357, 108);
            lblMax.Name = "lblMax";
            lblMax.Size = new Size(42, 15);
            lblMax.TabIndex = 4;
            lblMax.Text = "1750";
            //
            // txtRawInput
            //
            txtRawInput.Location = new Point(12, 140);
            txtRawInput.Name = "txtRawInput";
            txtRawInput.Size = new Size(100, 23);
            txtRawInput.TabIndex = 5;
            txtRawInput.Text = "0";
            txtRawInput.KeyDown += txtRawInput_KeyDown;
            //
            // butSendRaw
            //
            butSendRaw.Location = new Point(118, 139);
            butSendRaw.Name = "butSendRaw";
            butSendRaw.Size = new Size(92, 25);
            butSendRaw.TabIndex = 6;
            butSendRaw.Text = "Send";
            butSendRaw.UseVisualStyleBackColor = true;
            butSendRaw.Click += butSendRaw_Click;
            //
            // butZero
            //
            butZero.Location = new Point(216, 139);
            butZero.Name = "butZero";
            butZero.Size = new Size(92, 25);
            butZero.TabIndex = 7;
            butZero.Text = "Zero";
            butZero.UseVisualStyleBackColor = true;
            butZero.Click += butZero_Click;
            //
            // frmMain
            //
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(424, 181);
            Controls.Add(butZero);
            Controls.Add(butSendRaw);
            Controls.Add(txtRawInput);
            Controls.Add(lblMax);
            Controls.Add(lblMin);
            Controls.Add(lblValue);
            Controls.Add(trkVsi);
            Controls.Add(lblTarget);
            Name = "frmMain";
            Text = "Stepper VSI Tester";
            ((System.ComponentModel.ISupportInitialize)trkVsi).EndInit();
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private Label lblTarget;
        private TrackBar trkVsi;
        private Label lblValue;
        private Label lblMin;
        private Label lblMax;
        private TextBox txtRawInput;
        private Button butSendRaw;
        private Button butZero;
    }
}
