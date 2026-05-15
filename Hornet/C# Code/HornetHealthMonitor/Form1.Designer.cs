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
            // lblLeftConsole
            // 
            lblLeftConsole.AutoSize = true;
            lblLeftConsole.BackColor = Color.White;
            lblLeftConsole.ForeColor = SystemColors.ControlText;
            lblLeftConsole.Location = new Point(12, 9);
            lblLeftConsole.Name = "lblLeftConsole";
            lblLeftConsole.Size = new Size(73, 15);
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
            cmdToggleLogs.Location = new Point(12, 38);
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
            lblUIP.BackColor = Color.White;
            lblUIP.ForeColor = SystemColors.ControlText;
            lblUIP.Location = new Point(86, 9);
            lblUIP.Name = "lblUIP";
            lblUIP.Size = new Size(25, 15);
            lblUIP.TabIndex = 3;
            lblUIP.Text = "UIP";
            // 
            // lblLIP
            // 
            lblLIP.AutoSize = true;
            lblLIP.BackColor = Color.White;
            lblLIP.ForeColor = SystemColors.ControlText;
            lblLIP.Location = new Point(117, 9);
            lblLIP.Name = "lblLIP";
            lblLIP.Size = new Size(23, 15);
            lblLIP.TabIndex = 4;
            lblLIP.Text = "LIP";
            // 
            // lblLowerInst
            // 
            lblLowerInst.AutoSize = true;
            lblLowerInst.BackColor = Color.White;
            lblLowerInst.ForeColor = SystemColors.ControlText;
            lblLowerInst.Location = new Point(146, 9);
            lblLowerInst.Name = "lblLowerInst";
            lblLowerInst.Size = new Size(61, 15);
            lblLowerInst.TabIndex = 5;
            lblLowerInst.Text = "Lower Inst";
            // 
            // lblRightConsole
            // 
            lblRightConsole.AutoSize = true;
            lblRightConsole.BackColor = Color.White;
            lblRightConsole.ForeColor = SystemColors.ControlText;
            lblRightConsole.Location = new Point(213, 9);
            lblRightConsole.Name = "lblRightConsole";
            lblRightConsole.Size = new Size(81, 15);
            lblRightConsole.TabIndex = 6;
            lblRightConsole.Text = "Right Console";
            // 
            // lblGauges
            // 
            lblGauges.AutoSize = true;
            lblGauges.BackColor = Color.White;
            lblGauges.ForeColor = SystemColors.ControlText;
            lblGauges.Location = new Point(300, 9);
            lblGauges.Name = "lblGauges";
            lblGauges.Size = new Size(46, 15);
            lblGauges.TabIndex = 8;
            lblGauges.Text = "Gauges";
            lblGauges.Click += label1_Click;
            // 
            // frmMain
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(465, 282);
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
    }
}
