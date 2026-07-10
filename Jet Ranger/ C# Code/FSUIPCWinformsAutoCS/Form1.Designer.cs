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
            txtAirspeed = new TextBox();
            chkAvionicsMaster = new CheckBox();
            label1 = new Label();
            timerConnection = new System.Windows.Forms.Timer(components);
            label2 = new Label();
            txtGasProducer = new TextBox();
            txtTurbineOut = new TextBox();
            label3 = new Label();
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
            statusStrip.Size = new Size(502, 22);
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
            // txtAirspeed
            // 
            txtAirspeed.Location = new Point(187, 14);
            txtAirspeed.Margin = new Padding(4, 3, 4, 3);
            txtAirspeed.Name = "txtAirspeed";
            txtAirspeed.ReadOnly = true;
            txtAirspeed.Size = new Size(116, 23);
            txtAirspeed.TabIndex = 9;
            // 
            // chkAvionicsMaster
            // 
            chkAvionicsMaster.CheckAlign = ContentAlignment.MiddleRight;
            chkAvionicsMaster.Location = new Point(13, 46);
            chkAvionicsMaster.Margin = new Padding(4, 3, 4, 3);
            chkAvionicsMaster.Name = "chkAvionicsMaster";
            chkAvionicsMaster.Size = new Size(191, 28);
            chkAvionicsMaster.TabIndex = 8;
            chkAvionicsMaster.Text = "Avionics Master";
            chkAvionicsMaster.UseVisualStyleBackColor = true;
            chkAvionicsMaster.CheckedChanged += chkAvionicsMaster_CheckedChanged;
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new Point(13, 17);
            label1.Margin = new Padding(4, 0, 4, 0);
            label1.Name = "label1";
            label1.Size = new Size(141, 15);
            label1.TabIndex = 7;
            label1.Text = "Indicated Airpeed (Knots)";
            // 
            // timerConnection
            // 
            timerConnection.Interval = 1000;
            timerConnection.Tick += timerConnection_Tick;
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new Point(13, 120);
            label2.Margin = new Padding(4, 0, 4, 0);
            label2.Name = "label2";
            label2.Size = new Size(90, 15);
            label2.TabIndex = 11;
            label2.Text = "Gas Producer %";
            // 
            // txtGasProducer
            // 
            txtGasProducer.Location = new Point(187, 112);
            txtGasProducer.Margin = new Padding(4, 3, 4, 3);
            txtGasProducer.Name = "txtGasProducer";
            txtGasProducer.ReadOnly = true;
            txtGasProducer.Size = new Size(116, 23);
            txtGasProducer.TabIndex = 12;
            // 
            // txtTurbineOut
            // 
            txtTurbineOut.Location = new Point(187, 82);
            txtTurbineOut.Margin = new Padding(4, 3, 4, 3);
            txtTurbineOut.Name = "txtTurbineOut";
            txtTurbineOut.ReadOnly = true;
            txtTurbineOut.Size = new Size(116, 23);
            txtTurbineOut.TabIndex = 14;
            txtTurbineOut.TextChanged += textBox1_TextChanged;
            // 
            // label3
            // 
            label3.AutoSize = true;
            label3.Location = new Point(13, 90);
            label3.Margin = new Padding(4, 0, 4, 0);
            label3.Name = "label3";
            label3.Size = new Size(70, 15);
            label3.TabIndex = 13;
            label3.Text = "Turbine Out";
            // 
            // textBox1
            // 
            textBox1.Location = new Point(22, 145);
            textBox1.Multiline = true;
            textBox1.Name = "textBox1";
            textBox1.Size = new Size(281, 366);
            textBox1.TabIndex = 15;
            // 
            // Form1
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(502, 586);
            Controls.Add(textBox1);
            Controls.Add(txtTurbineOut);
            Controls.Add(label3);
            Controls.Add(txtGasProducer);
            Controls.Add(label2);
            Controls.Add(statusStrip);
            Controls.Add(txtAirspeed);
            Controls.Add(chkAvionicsMaster);
            Controls.Add(label1);
            Margin = new Padding(4, 3, 4, 3);
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
        private System.Windows.Forms.TextBox txtAirspeed;
        private System.Windows.Forms.CheckBox chkAvionicsMaster;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Timer timerConnection;
        private Label label2;
        private TextBox txtGasProducer;
        private TextBox txtTurbineOut;
        private Label label3;
        private TextBox textBox1;
    }
}

