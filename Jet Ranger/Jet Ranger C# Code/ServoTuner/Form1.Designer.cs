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
            button1 = new Button();
            vScrollBar1 = new VScrollBar();
            label1 = new Label();
            lstServos = new ListBox();
            SuspendLayout();
            // 
            // button1
            // 
            button1.Location = new Point(378, 43);
            button1.Name = "button1";
            button1.Size = new Size(75, 23);
            button1.TabIndex = 0;
            button1.Text = "button1";
            button1.UseVisualStyleBackColor = true;
            button1.Click += button1_Click_1;
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
            label1.Location = new Point(302, 51);
            label1.Name = "label1";
            label1.Size = new Size(38, 15);
            label1.TabIndex = 2;
            label1.Text = "label1";
            // 
            // lstServos
            // 
            lstServos.FormattingEnabled = true;
            lstServos.Location = new Point(58, 84);
            lstServos.Name = "lstServos";
            lstServos.Size = new Size(395, 244);
            lstServos.TabIndex = 3;
            // 
            // Form1
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(800, 450);
            Controls.Add(lstServos);
            Controls.Add(label1);
            Controls.Add(vScrollBar1);
            Controls.Add(button1);
            Name = "Form1";
            Text = "Servo Tuner";
            Load += Form1_Load;
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private Button button1;
        private VScrollBar vScrollBar1;
        private Label label1;
        private ListBox lstServos;
    }
}
