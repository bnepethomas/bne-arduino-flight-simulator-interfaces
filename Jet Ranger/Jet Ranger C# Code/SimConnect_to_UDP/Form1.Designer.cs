namespace SimConnect_to_UDP
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
            richResponse = new RichTextBox();
            buttonConnect = new Button();
            buttonRequestData = new Button();
            buttonDisconnect = new Button();
            timer1 = new System.Windows.Forms.Timer(components);
            SuspendLayout();
            // 
            // listBoxLogs
            // 
            listBoxLogs.FormattingEnabled = true;
            listBoxLogs.Location = new Point(29, 406);
            listBoxLogs.Name = "listBoxLogs";
            listBoxLogs.Size = new Size(379, 109);
            listBoxLogs.TabIndex = 0;
            // 
            // richResponse
            // 
            richResponse.Location = new Point(29, 77);
            richResponse.Name = "richResponse";
            richResponse.Size = new Size(379, 296);
            richResponse.TabIndex = 1;
            richResponse.Text = "";
            // 
            // buttonConnect
            // 
            buttonConnect.Location = new Point(29, 12);
            buttonConnect.Name = "buttonConnect";
            buttonConnect.Size = new Size(75, 23);
            buttonConnect.TabIndex = 2;
            buttonConnect.Text = "Connect";
            buttonConnect.UseVisualStyleBackColor = true;
            buttonConnect.Click += button1_Click;
            // 
            // buttonRequestData
            // 
            buttonRequestData.Location = new Point(131, 12);
            buttonRequestData.Name = "buttonRequestData";
            buttonRequestData.Size = new Size(102, 23);
            buttonRequestData.TabIndex = 3;
            buttonRequestData.Text = "Request Data";
            buttonRequestData.UseVisualStyleBackColor = true;
            buttonRequestData.Click += buttonRequestData_Click;
            // 
            // buttonDisconnect
            // 
            buttonDisconnect.Location = new Point(251, 12);
            buttonDisconnect.Name = "buttonDisconnect";
            buttonDisconnect.Size = new Size(75, 23);
            buttonDisconnect.TabIndex = 4;
            buttonDisconnect.Text = "Disconnect";
            buttonDisconnect.UseVisualStyleBackColor = true;
            buttonDisconnect.Click += button1_Click_1;
            // 
            // timer1
            // 
            timer1.Enabled = true;
            timer1.Interval = 2000;
            timer1.Tick += timer1_Tick;
            // 
            // frmMain
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(568, 537);
            Controls.Add(buttonDisconnect);
            Controls.Add(buttonRequestData);
            Controls.Add(buttonConnect);
            Controls.Add(richResponse);
            Controls.Add(listBoxLogs);
            Name = "frmMain";
            Text = "SimConnect to UDP 2026";
            FormClosed += frmMain_FormClosed;
            Load += frmMain_Load;
            ResumeLayout(false);
        }

        #endregion

        private ListBox listBoxLogs;
        private RichTextBox richResponse;
        private Button buttonConnect;
        private Button buttonRequestData;
        private Button buttonDisconnect;
        private System.Windows.Forms.Timer timer1;
    }
}
