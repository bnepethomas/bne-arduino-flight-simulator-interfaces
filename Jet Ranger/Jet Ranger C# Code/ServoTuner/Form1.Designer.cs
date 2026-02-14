namespace ServoTuner
{
    partial class Form1
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
            label2.Click += label2_Click;
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
            // Form1
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(566, 450);
            Controls.Add(label3);
            Controls.Add(label2);
            Controls.Add(lblShortCode);
            Controls.Add(lstServos);
            Controls.Add(label1);
            Controls.Add(vScrollBar1);
            Name = "Form1";
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
    }
}
