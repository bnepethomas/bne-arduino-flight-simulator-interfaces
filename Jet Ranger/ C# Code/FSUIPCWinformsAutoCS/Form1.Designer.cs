namespace FSUIPCTest
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
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
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            components = new System.ComponentModel.Container();
            statusStrip = new StatusStrip();
            lblConnectionStatus = new ToolStripStatusLabel();
            timerMain = new System.Windows.Forms.Timer(components);
            chkAvionicsMaster = new CheckBox();
            timerConnection = new System.Windows.Forms.Timer(components);
            textBox1 = new TextBox();
            statusStrip.SuspendLayout();
            SuspendLayout();
            // 
            // statusStrip
            // 
            statusStrip.Items.AddRange(new ToolStripItem[] { lblConnectionStatus });
            statusStrip.Location = new Point(0, 564);
            statusStrip.Name = "statusStrip";
            statusStrip.Padding = new Padding(1, 0, 16, 0);
            statusStrip.Size = new Size(767, 22);
            statusStrip.TabIndex = 6;
            statusStrip.Text = "statusStrip1";
            // 
            // lblConnectionStatus
            // 
            lblConnectionStatus.Name = "lblConnectionStatus";
            lblConnectionStatus.Size = new Size(104, 17);
            lblConnectionStatus.Text = "Connection Status";
            // 
            // timerMain
            // 
            timerMain.Tick += timerMain_Tick;
            // 
            // chkAvionicsMaster
            // 
            chkAvionicsMaster.CheckAlign = ContentAlignment.MiddleRight;
            chkAvionicsMaster.Location = new Point(13, 12);
            chkAvionicsMaster.Margin = new Padding(4, 3, 4, 3);
            chkAvionicsMaster.Name = "chkAvionicsMaster";
            chkAvionicsMaster.Size = new Size(191, 28);
            chkAvionicsMaster.TabIndex = 8;
            chkAvionicsMaster.Text = "Avionics Master";
            chkAvionicsMaster.UseVisualStyleBackColor = true;
            chkAvionicsMaster.CheckedChanged += chkAvionicsMaster_CheckedChanged;
            // 
            // timerConnection
            // 
            timerConnection.Interval = 1000;
            timerConnection.Tick += timerConnection_Tick;
            // 
            // textBox1
            // 
            textBox1.Location = new Point(22, 46);
            textBox1.Multiline = true;
            textBox1.Name = "textBox1";
            textBox1.Size = new Size(281, 515);
            textBox1.TabIndex = 15;
            // 
            // Form1
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(767, 586);
            Controls.Add(textBox1);
            Controls.Add(statusStrip);
            Controls.Add(chkAvionicsMaster);
            Margin = new Padding(4, 3, 4, 3);
            MinimumSize = new Size(783, 625);
            Name = "Form1";
            Text = "Main Form";
            FormClosing += frmMain_FormClosing;
            statusStrip.ResumeLayout(false);
            statusStrip.PerformLayout();
            ResumeLayout(false);
            PerformLayout();

        }

        #endregion

        private System.Windows.Forms.StatusStrip statusStrip;
        private System.Windows.Forms.ToolStripStatusLabel lblConnectionStatus;
        private System.Windows.Forms.Timer timerMain;
        private System.Windows.Forms.CheckBox chkAvionicsMaster;
        private System.Windows.Forms.Timer timerConnection;
        private TextBox textBox1;
    }
}

