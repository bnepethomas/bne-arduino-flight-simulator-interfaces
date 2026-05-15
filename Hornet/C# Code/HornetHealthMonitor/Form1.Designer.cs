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
            lblLeftConsole = new Label();
            tmrHealthCheck = new System.Windows.Forms.Timer(components);
            cmdToggleLogs = new Button();
            lblUIP = new Label();
            lblLIP = new Label();
            lblLowerInst = new Label();
            lblRightConsole = new Label();
            lblGauges = new Label();
            lblUDPToKeyboard = new Label();
            lblUDPToPixelLed = new Label();
            SuspendLayout();
            // 
            // listBoxLogs
            // 
            listBoxLogs.FormattingEnabled = true;
            listBoxLogs.Location = new Point(12, 99);
            listBoxLogs.Name = "listBoxLogs";
            listBoxLogs.Size = new Size(418, 199);
            listBoxLogs.TabIndex = 0;
            // 
            // lblLeftConsole
            // 
            lblLeftConsole.AutoSize = true;
            lblLeftConsole.BackColor = Color.Gray;
            lblLeftConsole.Font = new Font("Segoe UI", 12F, FontStyle.Bold, GraphicsUnit.Point, 0);
            lblLeftConsole.ForeColor = SystemColors.Control;
            lblLeftConsole.Location = new Point(12, 9);
            lblLeftConsole.Name = "lblLeftConsole";
            lblLeftConsole.Size = new Size(104, 21);
            lblLeftConsole.TabIndex = 7;
            lblLeftConsole.Text = "Left Console";
            // 
            // tmrHealthCheck
            // 
            tmrHealthCheck.Enabled = true;
            tmrHealthCheck.Interval = 1000;
            tmrHealthCheck.Tick += tmrHealthCheck_Tick;
            // 
            // cmdToggleLogs
            // 
            cmdToggleLogs.Location = new Point(12, 70);
            cmdToggleLogs.Name = "cmdToggleLogs";
            cmdToggleLogs.Size = new Size(75, 23);
            cmdToggleLogs.TabIndex = 2;
            cmdToggleLogs.Text = "Hide Logs";
            cmdToggleLogs.UseVisualStyleBackColor = true;
            cmdToggleLogs.Click += cmdToggleLogs_Click;
            // 
            // lblUIP
            // 
            lblUIP.AutoSize = true;
            lblUIP.BackColor = Color.Gray;
            lblUIP.Font = new Font("Segoe UI", 12F, FontStyle.Bold, GraphicsUnit.Point, 0);
            lblUIP.ForeColor = SystemColors.Control;
            lblUIP.Location = new Point(161, 9);
            lblUIP.Name = "lblUIP";
            lblUIP.Size = new Size(37, 21);
            lblUIP.TabIndex = 3;
            lblUIP.Text = "UIP";
            // 
            // lblLIP
            // 
            lblLIP.AutoSize = true;
            lblLIP.BackColor = Color.Gray;
            lblLIP.Font = new Font("Segoe UI", 12F, FontStyle.Bold, GraphicsUnit.Point, 0);
            lblLIP.ForeColor = SystemColors.Control;
            lblLIP.Location = new Point(122, 9);
            lblLIP.Name = "lblLIP";
            lblLIP.Size = new Size(33, 21);
            lblLIP.TabIndex = 4;
            lblLIP.Text = "LIP";
            // 
            // lblLowerInst
            // 
            lblLowerInst.AutoSize = true;
            lblLowerInst.BackColor = Color.Gray;
            lblLowerInst.Font = new Font("Segoe UI", 12F, FontStyle.Bold, GraphicsUnit.Point, 0);
            lblLowerInst.ForeColor = SystemColors.Control;
            lblLowerInst.Location = new Point(204, 9);
            lblLowerInst.Name = "lblLowerInst";
            lblLowerInst.Size = new Size(88, 21);
            lblLowerInst.TabIndex = 5;
            lblLowerInst.Text = "Lower Inst";
            // 
            // lblRightConsole
            // 
            lblRightConsole.AutoSize = true;
            lblRightConsole.BackColor = Color.Gray;
            lblRightConsole.Font = new Font("Segoe UI", 12F, FontStyle.Bold, GraphicsUnit.Point, 0);
            lblRightConsole.ForeColor = SystemColors.Control;
            lblRightConsole.Location = new Point(298, 9);
            lblRightConsole.Name = "lblRightConsole";
            lblRightConsole.Size = new Size(116, 21);
            lblRightConsole.TabIndex = 6;
            lblRightConsole.Text = "Right Console";
            // 
            // lblGauges
            // 
            lblGauges.AutoSize = true;
            lblGauges.BackColor = Color.Gray;
            lblGauges.Font = new Font("Segoe UI", 12F, FontStyle.Bold, GraphicsUnit.Point, 0);
            lblGauges.ForeColor = SystemColors.Control;
            lblGauges.Location = new Point(12, 41);
            lblGauges.Name = "lblGauges";
            lblGauges.Size = new Size(66, 21);
            lblGauges.TabIndex = 8;
            lblGauges.Text = "Gauges";
           // 
            // lblUDPToKeyboard
            // 
            lblUDPToKeyboard.AutoSize = true;
            lblUDPToKeyboard.BackColor = Color.Gray;
            lblUDPToKeyboard.Font = new Font("Segoe UI", 12F, FontStyle.Bold, GraphicsUnit.Point, 0);
            lblUDPToKeyboard.ForeColor = SystemColors.Control;
            lblUDPToKeyboard.Location = new Point(84, 41);
            lblUDPToKeyboard.Name = "lblUDPToKeyboard";
            lblUDPToKeyboard.Size = new Size(141, 21);
            lblUDPToKeyboard.TabIndex = 9;
            lblUDPToKeyboard.Text = "UDP to Keyboard";
            // 
            // lblUDPToPixelLed
            // 
            lblUDPToPixelLed.AutoSize = true;
            lblUDPToPixelLed.BackColor = Color.Gray;
            lblUDPToPixelLed.Font = new Font("Segoe UI", 12F, FontStyle.Bold, GraphicsUnit.Point, 0);
            lblUDPToPixelLed.ForeColor = SystemColors.Control;
            lblUDPToPixelLed.Location = new Point(231, 41);
            lblUDPToPixelLed.Name = "lblUDPToPixelLed";
            lblUDPToPixelLed.Size = new Size(79, 21);
            lblUDPToPixelLed.TabIndex = 10;
            lblUDPToPixelLed.Text = "Pixel Led";

            // 
            // frmMain
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(452, 310);
            Controls.Add(lblUDPToPixelLed);
            Controls.Add(lblUDPToKeyboard);
            Controls.Add(lblGauges);
            Controls.Add(lblRightConsole);
            Controls.Add(lblLowerInst);
            Controls.Add(lblLIP);
            Controls.Add(lblUIP);
            Controls.Add(cmdToggleLogs);
            Controls.Add(lblLeftConsole);
            Controls.Add(listBoxLogs);
            Name = "frmMain";
            Text = "Hornet Health Monitor";
            Load += Form1_Load;
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private ListBox listBoxLogs;
        private Label lblLeftConsole;
        private System.Windows.Forms.Timer tmrHealthCheck;
        private Button cmdToggleLogs;
        private Label lblUIP;
        private Label lblLIP;
        private Label lblLowerInst;
        private Label lblRightConsole;
        private Label lblGauges;
        private Label lblUDPToKeyboard;
        private Label lblUDPToPixelLed;
    }
}
