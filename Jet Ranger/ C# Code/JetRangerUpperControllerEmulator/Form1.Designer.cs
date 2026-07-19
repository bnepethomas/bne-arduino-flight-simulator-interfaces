namespace JetRangerUpperControllerEmulator
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
            pnlTop = new Panel();
            lblStatus = new Label();
            txtPort = new TextBox();
            lblPort = new Label();
            txtHost = new TextBox();
            lblHost = new Label();
            pnlSwitchesHost = new Panel();
            pnlSwitches = new FlowLayoutPanel();
            pnlTop.SuspendLayout();
            SuspendLayout();
            //
            // pnlTop
            //
            pnlTop.Controls.Add(lblStatus);
            pnlTop.Controls.Add(txtPort);
            pnlTop.Controls.Add(lblPort);
            pnlTop.Controls.Add(txtHost);
            pnlTop.Controls.Add(lblHost);
            pnlTop.Dock = DockStyle.Top;
            pnlTop.Location = new Point(0, 0);
            pnlTop.Name = "pnlTop";
            pnlTop.Size = new Size(900, 48);
            pnlTop.TabIndex = 0;
            //
            // lblStatus
            //
            lblStatus.AutoSize = true;
            lblStatus.Location = new Point(320, 17);
            lblStatus.Name = "lblStatus";
            lblStatus.Size = new Size(38, 15);
            lblStatus.TabIndex = 4;
            lblStatus.Text = "Ready";
            //
            // txtPort
            //
            txtPort.Location = new Point(230, 13);
            txtPort.Name = "txtPort";
            txtPort.Size = new Size(60, 23);
            txtPort.TabIndex = 3;
            txtPort.Text = "4210";
            //
            // lblPort
            //
            lblPort.AutoSize = true;
            lblPort.Location = new Point(195, 17);
            lblPort.Name = "lblPort";
            lblPort.Size = new Size(32, 15);
            lblPort.TabIndex = 2;
            lblPort.Text = "Port:";
            //
            // txtHost
            //
            txtHost.Location = new Point(90, 13);
            txtHost.Name = "txtHost";
            txtHost.Size = new Size(100, 23);
            txtHost.TabIndex = 1;
            txtHost.Text = "172.16.1.11";
            //
            // lblHost
            //
            lblHost.AutoSize = true;
            lblHost.Location = new Point(12, 17);
            lblHost.Name = "lblHost";
            lblHost.Size = new Size(74, 15);
            lblHost.TabIndex = 0;
            lblHost.Text = "UDP_HID IP:";
            //
            // pnlSwitchesHost
            //
            pnlSwitchesHost.AutoScroll = true;
            pnlSwitchesHost.Controls.Add(pnlSwitches);
            pnlSwitchesHost.Dock = DockStyle.Fill;
            pnlSwitchesHost.Location = new Point(0, 48);
            pnlSwitchesHost.Name = "pnlSwitchesHost";
            pnlSwitchesHost.Size = new Size(900, 552);
            pnlSwitchesHost.TabIndex = 1;
            //
            // pnlSwitches
            //
            pnlSwitches.AutoSize = true;
            pnlSwitches.FlowDirection = FlowDirection.LeftToRight;
            pnlSwitches.Location = new Point(0, 0);
            pnlSwitches.Name = "pnlSwitches";
            pnlSwitches.Size = new Size(0, 0);
            pnlSwitches.TabIndex = 0;
            pnlSwitches.WrapContents = true;
            //
            // frmMain
            //
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(900, 600);
            Controls.Add(pnlSwitchesHost);
            Controls.Add(pnlTop);
            MinimumSize = new Size(700, 400);
            Name = "frmMain";
            Text = "Jet Ranger Upper Controller Emulator";
            pnlTop.ResumeLayout(false);
            pnlTop.PerformLayout();
            ResumeLayout(false);
        }

        #endregion

        private Panel pnlTop;
        private Label lblHost;
        private TextBox txtHost;
        private Label lblPort;
        private TextBox txtPort;
        private Label lblStatus;
        private Panel pnlSwitchesHost;
        private FlowLayoutPanel pnlSwitches;
    }
}
