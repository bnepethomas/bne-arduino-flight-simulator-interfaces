namespace JetRangerRadioControllerEmulator
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
            pnlContentHost = new Panel();
            pnlContent = new FlowLayoutPanel();
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
            pnlTop.Size = new Size(960, 48);
            pnlTop.TabIndex = 0;
            //
            // lblStatus
            //
            lblStatus.AutoSize = true;
            lblStatus.Location = new Point(330, 17);
            lblStatus.Name = "lblStatus";
            lblStatus.Size = new Size(38, 15);
            lblStatus.TabIndex = 4;
            lblStatus.Text = "Ready";
            //
            // txtPort
            //
            txtPort.Location = new Point(240, 13);
            txtPort.Name = "txtPort";
            txtPort.Size = new Size(60, 23);
            txtPort.TabIndex = 3;
            txtPort.Text = "27001";
            //
            // lblPort
            //
            lblPort.AutoSize = true;
            lblPort.Location = new Point(205, 17);
            lblPort.Name = "lblPort";
            lblPort.Size = new Size(32, 15);
            lblPort.TabIndex = 2;
            lblPort.Text = "Port:";
            //
            // txtHost
            //
            txtHost.Location = new Point(100, 13);
            txtHost.Name = "txtHost";
            txtHost.Size = new Size(100, 23);
            txtHost.TabIndex = 1;
            txtHost.Text = "172.16.1.10";
            //
            // lblHost
            //
            lblHost.AutoSize = true;
            lblHost.Location = new Point(12, 17);
            lblHost.Name = "lblHost";
            lblHost.Size = new Size(84, 15);
            lblHost.TabIndex = 0;
            lblHost.Text = "Reflector IP:";
            //
            // pnlContentHost
            //
            pnlContentHost.AutoScroll = true;
            pnlContentHost.Controls.Add(pnlContent);
            pnlContentHost.Dock = DockStyle.Fill;
            pnlContentHost.Location = new Point(0, 48);
            pnlContentHost.Name = "pnlContentHost";
            pnlContentHost.Size = new Size(960, 632);
            pnlContentHost.TabIndex = 1;
            //
            // pnlContent
            //
            pnlContent.AutoSize = true;
            pnlContent.FlowDirection = FlowDirection.TopDown;
            pnlContent.Location = new Point(0, 0);
            pnlContent.Name = "pnlContent";
            pnlContent.Padding = new Padding(10);
            pnlContent.Size = new Size(0, 0);
            pnlContent.TabIndex = 0;
            pnlContent.WrapContents = false;
            //
            // frmMain
            //
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(960, 680);
            Controls.Add(pnlContentHost);
            Controls.Add(pnlTop);
            MinimumSize = new Size(760, 500);
            Name = "frmMain";
            Text = "Jet Ranger Radio Controller Emulator";
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
        private Panel pnlContentHost;
        private FlowLayoutPanel pnlContent;
    }
}
