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
            // frmMain
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(465, 282);
            Controls.Add(cmdToggleLogs);
            Controls.Add(lblCommNav);
            Controls.Add(listBoxLogs);
            Name = "frmMain";
            Text = "Mega Health Monitor";
            Load += Form1_Load;
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private ListBox listBoxLogs;
        private Label lblCommNav;
        private System.Windows.Forms.Timer tmrHealthCheck;
        private Button cmdToggleLogs;
    }
}
