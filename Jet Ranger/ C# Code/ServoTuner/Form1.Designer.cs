namespace ServoTuner
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
            vScrollBar1 = new VScrollBar();
            label1 = new Label();
            lstServos = new ListBox();
            lblShortCode = new Label();
            label2 = new Label();
            label3 = new Label();
            label4 = new Label();
            label5 = new Label();
            lblMinValue = new Label();
            lblMaxValue = new Label();
            txtRawInput = new TextBox();
            butSendRaw = new Button();
            txtConvertedInput = new TextBox();
            butSendConv = new Button();
            lblConvertedValue = new Label();
            SuspendLayout();
            // 
            // vScrollBar1
            // 
            vScrollBar1.Location = new Point(488, 31);
            vScrollBar1.Maximum = 1000;
            vScrollBar1.Name = "vScrollBar1";
            vScrollBar1.Size = new Size(17, 336);
            vScrollBar1.TabIndex = 1;
            vScrollBar1.Scroll += vScrollBar1_Scroll;
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new Point(385, 28);
            label1.Name = "label1";
            label1.Size = new Size(68, 15);
            label1.TabIndex = 2;
            label1.Text = "Unassigned";
            // 
            // lstServos
            // 
            lstServos.FormattingEnabled = true;
            lstServos.Location = new Point(58, 84);
            lstServos.Name = "lstServos";
            lstServos.Size = new Size(395, 244);
            lstServos.TabIndex = 3;
            lstServos.SelectedIndexChanged += lstServos_SelectedIndexChanged;
            // 
            // lblShortCode
            // 
            lblShortCode.AutoSize = true;
            lblShortCode.Location = new Point(157, 28);
            lblShortCode.Name = "lblShortCode";
            lblShortCode.Size = new Size(68, 15);
            lblShortCode.TabIndex = 4;
            lblShortCode.Text = "Unassigned";
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new Point(85, 28);
            label2.Name = "label2";
            label2.Size = new Size(69, 15);
            label2.TabIndex = 5;
            label2.Text = "Short Code:";
            // 
            // label3
            // 
            label3.AutoSize = true;
            label3.Location = new Point(299, 28);
            label3.Name = "label3";
            label3.Size = new Size(80, 15);
            label3.TabIndex = 6;
            label3.Text = "Value to send:";
            label3.TextAlign = ContentAlignment.TopRight;
            // 
            // label4
            // 
            label4.AutoSize = true;
            label4.Location = new Point(227, 54);
            label4.Name = "label4";
            label4.Size = new Size(64, 15);
            label4.TabIndex = 7;
            label4.Text = "Max Value:";
            label4.TextAlign = ContentAlignment.TopRight;
            // 
            // label5
            // 
            label5.AutoSize = true;
            label5.Location = new Point(93, 54);
            label5.Name = "label5";
            label5.Size = new Size(62, 15);
            label5.TabIndex = 8;
            label5.Text = "Min Value:";
            label5.TextAlign = ContentAlignment.TopRight;
            // 
            // lblMinValue
            // 
            lblMinValue.AutoSize = true;
            lblMinValue.Location = new Point(157, 54);
            lblMinValue.Name = "lblMinValue";
            lblMinValue.Size = new Size(68, 15);
            lblMinValue.TabIndex = 9;
            lblMinValue.Text = "Unassigned";
            // 
            // lblMaxValue
            // 
            lblMaxValue.AutoSize = true;
            lblMaxValue.Location = new Point(297, 54);
            lblMaxValue.Name = "lblMaxValue";
            lblMaxValue.Size = new Size(68, 15);
            lblMaxValue.TabIndex = 10;
            lblMaxValue.Text = "Unassigned";
            // 
            // txtRawInput
            // 
            txtRawInput.Location = new Point(236, 368);
            txtRawInput.Name = "txtRawInput";
            txtRawInput.Size = new Size(100, 23);
            txtRawInput.TabIndex = 11;
            txtRawInput.Text = "0";
            // 
            // butSendRaw
            // 
            butSendRaw.Location = new Point(126, 366);
            butSendRaw.Name = "butSendRaw";
            butSendRaw.Size = new Size(92, 25);
            butSendRaw.TabIndex = 12;
            butSendRaw.Text = "Send Raw";
            butSendRaw.UseVisualStyleBackColor = true;
            butSendRaw.Click += button1_Click;
            // 
            // txtConvertedInput
            // 
            txtConvertedInput.CausesValidation = false;
            txtConvertedInput.Location = new Point(236, 397);
            txtConvertedInput.Name = "txtConvertedInput";
            txtConvertedInput.Size = new Size(100, 23);
            txtConvertedInput.TabIndex = 13;
            txtConvertedInput.TextChanged += txtConvertedInput_TextChanged;
            txtConvertedInput.KeyDown += txtConvertedInput_KeyDown;
            // 
            // butSendConv
            // 
            butSendConv.Location = new Point(126, 395);
            butSendConv.Name = "butSendConv";
            butSendConv.Size = new Size(92, 25);
            butSendConv.TabIndex = 14;
            butSendConv.Text = "Send Conv";
            butSendConv.UseVisualStyleBackColor = true;
            butSendConv.Click += button2_Click;
            // 
            // lblConvertedValue
            // 
            lblConvertedValue.AutoSize = true;
            lblConvertedValue.Location = new Point(348, 402);
            lblConvertedValue.Name = "lblConvertedValue";
            lblConvertedValue.Size = new Size(68, 15);
            lblConvertedValue.TabIndex = 15;
            lblConvertedValue.Text = "Unassigned";
            // 
            // frmMain
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(566, 450);
            Controls.Add(lblConvertedValue);
            Controls.Add(butSendConv);
            Controls.Add(txtConvertedInput);
            Controls.Add(butSendRaw);
            Controls.Add(txtRawInput);
            Controls.Add(lblMaxValue);
            Controls.Add(lblMinValue);
            Controls.Add(label5);
            Controls.Add(label4);
            Controls.Add(label3);
            Controls.Add(label2);
            Controls.Add(lblShortCode);
            Controls.Add(lstServos);
            Controls.Add(label1);
            Controls.Add(vScrollBar1);
            Name = "frmMain";
            Text = "Servo Tuner";
            Load += Form1_Load;
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion
        private VScrollBar vScrollBar1;
        private Label label1;
        private ListBox lstServos;
        private Label lblShortCode;
        private Label label2;
        private Label label3;
        private Label label4;
        private Label label5;
        private Label lblMinValue;
        private Label lblMaxValue;
        private TextBox txtRawInput;
        private Button butSendRaw;
        private TextBox txtConvertedInput;
        private Button butSendConv;
        private Label lblConvertedValue;
    }
}
