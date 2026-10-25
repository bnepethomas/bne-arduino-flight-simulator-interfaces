namespace MegaHealthMonitor
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
            components = new System.ComponentModel.Container();
            listBoxLogs = new ListBox();
            lblCommNav = new Label();
            tmrHealthCheck = new System.Windows.Forms.Timer(components);
            cmdToggleLogs = new Button();
            lblServo = new Label();
            lblJoyStick = new Label();
            lblUpperInput = new Label();
            lblStepper = new Label();
            lblDualStepper = new Label();
            lblAdf = new Label();
            cmdToggleVoltages = new Button();
            lvVoltages = new ListView();
            colBoard = new ColumnHeader();
            col5V = new ColumnHeader();
            col12V = new ColumnHeader();
            colLast = new ColumnHeader();
            SuspendLayout();
            // 
            // listBoxLogs
            // 
            listBoxLogs.FormattingEnabled = true;
            listBoxLogs.Location = new Point(12, 67);
            listBoxLogs.Name = "listBoxLogs";
            listBoxLogs.Size = new Size(418, 199);
            listBoxLogs.TabIndex = 0;
            // 
            // lblCommNav
            // 
            lblCommNav.AutoSize = true;
            lblCommNav.BackColor = Color.White;
            lblCommNav.ForeColor = SystemColors.ControlText;
            lblCommNav.Location = new Point(12, 9);
            lblCommNav.Name = "lblCommNav";
            lblCommNav.Size = new Size(68, 15);
            lblCommNav.TabIndex = 1;
            lblCommNav.Text = "Comm Nav";
            lblCommNav.Click += label1_Click;
            // 
            // tmrHealthCheck
            // 
            tmrHealthCheck.Enabled = true;
            tmrHealthCheck.Interval = 1000;
            tmrHealthCheck.Tick += tmrHealthCheck_Tick;
            // 
            // cmdToggleLogs
            // 
            cmdToggleLogs.Location = new Point(12, 38);
            cmdToggleLogs.Name = "cmdToggleLogs";
            cmdToggleLogs.Size = new Size(75, 23);
            cmdToggleLogs.TabIndex = 2;
            cmdToggleLogs.Text = "Hide Logs";
            cmdToggleLogs.UseVisualStyleBackColor = true;
            cmdToggleLogs.Click += cmdToggleLogs_Click;
            // 
            // lblServo
            // 
            lblServo.AutoSize = true;
            lblServo.BackColor = Color.White;
            lblServo.ForeColor = SystemColors.ControlText;
            lblServo.Location = new Point(86, 9);
            lblServo.Name = "lblServo";
            lblServo.Size = new Size(36, 15);
            lblServo.TabIndex = 3;
            lblServo.Text = "Servo";
            // 
            // lblJoyStick
            // 
            lblJoyStick.AutoSize = true;
            lblJoyStick.BackColor = Color.White;
            lblJoyStick.ForeColor = SystemColors.ControlText;
            lblJoyStick.Location = new Point(128, 9);
            lblJoyStick.Name = "lblJoyStick";
            lblJoyStick.Size = new Size(48, 15);
            lblJoyStick.TabIndex = 4;
            lblJoyStick.Text = "Joystick";
            //
            // lblUpperInput
            //
            lblUpperInput.AutoSize = true;
            lblUpperInput.BackColor = Color.White;
            lblUpperInput.ForeColor = SystemColors.ControlText;
            lblUpperInput.Location = new Point(186, 9);
            lblUpperInput.Name = "lblUpperInput";
            lblUpperInput.Size = new Size(72, 15);
            lblUpperInput.TabIndex = 5;
            lblUpperInput.Text = "Upper Input";
            //
            // lblStepper
            //
            lblStepper.AutoSize = true;
            lblStepper.BackColor = Color.White;
            lblStepper.ForeColor = SystemColors.ControlText;
            lblStepper.Location = new Point(268, 9);
            lblStepper.Name = "lblStepper";
            lblStepper.Size = new Size(52, 15);
            lblStepper.TabIndex = 6;
            lblStepper.Text = "Stepper";
            //
            // lblDualStepper
            //
            lblDualStepper.AutoSize = true;
            lblDualStepper.BackColor = Color.White;
            lblDualStepper.ForeColor = SystemColors.ControlText;
            lblDualStepper.Location = new Point(330, 9);
            lblDualStepper.Name = "lblDualStepper";
            lblDualStepper.Size = new Size(80, 15);
            lblDualStepper.TabIndex = 7;
            lblDualStepper.Text = "Dual Stepper";
            //
            // lblAdf
            //
            lblAdf.AutoSize = true;
            lblAdf.BackColor = Color.White;
            lblAdf.ForeColor = SystemColors.ControlText;
            lblAdf.Location = new Point(416, 9);
            lblAdf.Name = "lblAdf";
            lblAdf.Size = new Size(28, 15);
            lblAdf.TabIndex = 8;
            lblAdf.Text = "ADF";
            //
            // cmdToggleVoltages
            //
            cmdToggleVoltages.Location = new Point(93, 38);
            cmdToggleVoltages.Name = "cmdToggleVoltages";
            cmdToggleVoltages.Size = new Size(110, 23);
            cmdToggleVoltages.TabIndex = 9;
            cmdToggleVoltages.Text = "Show Voltages";
            cmdToggleVoltages.UseVisualStyleBackColor = true;
            cmdToggleVoltages.Click += cmdToggleVoltages_Click;
            //
            // lvVoltages
            //
            lvVoltages.Columns.AddRange(new ColumnHeader[] { colBoard, col5V, col12V, colLast });
            lvVoltages.FullRowSelect = true;
            lvVoltages.GridLines = true;
            lvVoltages.Location = new Point(12, 274);
            lvVoltages.Name = "lvVoltages";
            lvVoltages.Size = new Size(441, 178);
            lvVoltages.TabIndex = 10;
            lvVoltages.UseCompatibleStateImageBehavior = false;
            lvVoltages.View = View.Details;
            lvVoltages.Visible = false;
            //
            // colBoard
            //
            colBoard.Text = "Board";
            colBoard.Width = 115;
            //
            // col5V
            //
            col5V.Text = "5 V rail";
            col5V.Width = 70;
            //
            // col12V
            //
            col12V.Text = "12 V rail";
            col12V.Width = 95;
            //
            // colLast
            //
            colLast.Text = "Last reading";
            colLast.Width = 155;
            //
            // frmMain
            //
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(465, 282);
            Controls.Add(lvVoltages);
            Controls.Add(cmdToggleVoltages);
            Controls.Add(lblAdf);
            Controls.Add(lblDualStepper);
            Controls.Add(lblStepper);
            Controls.Add(lblUpperInput);
            Controls.Add(lblJoyStick);
            Controls.Add(lblServo);
            Controls.Add(cmdToggleLogs);
            Controls.Add(lblCommNav);
            Controls.Add(listBoxLogs);
            Name = "frmMain";
            Text = "Jet Ranger Health Monitor";
            Load += Form1_Load;
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private ListBox listBoxLogs;
        private Label lblCommNav;
        private System.Windows.Forms.Timer tmrHealthCheck;
        private Button cmdToggleLogs;
        private Label lblServo;
        private Label lblJoyStick;
        private Label lblUpperInput;
        private Label lblStepper;
        private Label lblDualStepper;
        private Label lblAdf;
        private Button cmdToggleVoltages;
        private ListView lvVoltages;
        private ColumnHeader colBoard;
        private ColumnHeader col5V;
        private ColumnHeader col12V;
        private ColumnHeader colLast;
    }
}
