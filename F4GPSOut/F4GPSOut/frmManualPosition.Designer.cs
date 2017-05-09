namespace F4GPSOut
{
    partial class frmManualPosition
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
            this.components = new System.ComponentModel.Container();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.cancelToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.goToToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.brisbaneToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.queenstownToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.uRKAToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.rbNorth = new System.Windows.Forms.RadioButton();
            this.rbNorthSouthStatic = new System.Windows.Forms.RadioButton();
            this.rbSouth = new System.Windows.Forms.RadioButton();
            this.rbWest = new System.Windows.Forms.RadioButton();
            this.rbEastWestStatic = new System.Windows.Forms.RadioButton();
            this.rbEast = new System.Windows.Forms.RadioButton();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.rbSpeed10000 = new System.Windows.Forms.RadioButton();
            this.rbSpeed1000 = new System.Windows.Forms.RadioButton();
            this.rbSpeed100 = new System.Windows.Forms.RadioButton();
            this.rbSpeed10 = new System.Windows.Forms.RadioButton();
            this.rbSpeed1 = new System.Windows.Forms.RadioButton();
            this.rbSpeed0 = new System.Windows.Forms.RadioButton();
            this.txtInputLat = new System.Windows.Forms.TextBox();
            this.txtInputLong = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.lblCurrentLat = new System.Windows.Forms.Label();
            this.lblCurrentLong = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.rbHDGStarboard = new System.Windows.Forms.RadioButton();
            this.rbHDGStatic = new System.Windows.Forms.RadioButton();
            this.rbHDGPort = new System.Windows.Forms.RadioButton();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.rbAltDec = new System.Windows.Forms.RadioButton();
            this.rbAltStatic = new System.Windows.Forms.RadioButton();
            this.rbAltInc = new System.Windows.Forms.RadioButton();
            this.menuStrip1.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.cancelToolStripMenuItem,
            this.goToToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(517, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // cancelToolStripMenuItem
            // 
            this.cancelToolStripMenuItem.Name = "cancelToolStripMenuItem";
            this.cancelToolStripMenuItem.Size = new System.Drawing.Size(51, 20);
            this.cancelToolStripMenuItem.Text = "&Cancel";
            this.cancelToolStripMenuItem.Click += new System.EventHandler(this.cancelToolStripMenuItem_Click);
            // 
            // goToToolStripMenuItem
            // 
            this.goToToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.brisbaneToolStripMenuItem,
            this.queenstownToolStripMenuItem,
            this.uRKAToolStripMenuItem});
            this.goToToolStripMenuItem.Name = "goToToolStripMenuItem";
            this.goToToolStripMenuItem.Size = new System.Drawing.Size(45, 20);
            this.goToToolStripMenuItem.Text = "&Go to";
            // 
            // brisbaneToolStripMenuItem
            // 
            this.brisbaneToolStripMenuItem.Name = "brisbaneToolStripMenuItem";
            this.brisbaneToolStripMenuItem.Size = new System.Drawing.Size(135, 22);
            this.brisbaneToolStripMenuItem.Text = "Brisbane";
            this.brisbaneToolStripMenuItem.Click += new System.EventHandler(this.brisbaneToolStripMenuItem_Click);
            // 
            // queenstownToolStripMenuItem
            // 
            this.queenstownToolStripMenuItem.Name = "queenstownToolStripMenuItem";
            this.queenstownToolStripMenuItem.Size = new System.Drawing.Size(135, 22);
            this.queenstownToolStripMenuItem.Text = "Queenstown";
            this.queenstownToolStripMenuItem.Click += new System.EventHandler(this.queenstownToolStripMenuItem_Click);
            // 
            // uRKAToolStripMenuItem
            // 
            this.uRKAToolStripMenuItem.Name = "uRKAToolStripMenuItem";
            this.uRKAToolStripMenuItem.Size = new System.Drawing.Size(135, 22);
            this.uRKAToolStripMenuItem.Text = "URKA";
            this.uRKAToolStripMenuItem.Click += new System.EventHandler(this.uRKAToolStripMenuItem_Click);
            // 
            // rbNorth
            // 
            this.rbNorth.AutoSize = true;
            this.rbNorth.Location = new System.Drawing.Point(18, 19);
            this.rbNorth.Name = "rbNorth";
            this.rbNorth.Size = new System.Drawing.Size(51, 17);
            this.rbNorth.TabIndex = 1;
            this.rbNorth.Text = "North";
            this.rbNorth.UseVisualStyleBackColor = true;
            this.rbNorth.CheckedChanged += new System.EventHandler(this.rbNorth_CheckedChanged);
            // 
            // rbNorthSouthStatic
            // 
            this.rbNorthSouthStatic.AutoSize = true;
            this.rbNorthSouthStatic.Checked = true;
            this.rbNorthSouthStatic.Location = new System.Drawing.Point(18, 42);
            this.rbNorthSouthStatic.Name = "rbNorthSouthStatic";
            this.rbNorthSouthStatic.Size = new System.Drawing.Size(52, 17);
            this.rbNorthSouthStatic.TabIndex = 2;
            this.rbNorthSouthStatic.TabStop = true;
            this.rbNorthSouthStatic.Text = "Static";
            this.rbNorthSouthStatic.UseVisualStyleBackColor = true;
            this.rbNorthSouthStatic.CheckedChanged += new System.EventHandler(this.rbNorthSouthStatic_CheckedChanged);
            // 
            // rbSouth
            // 
            this.rbSouth.AutoSize = true;
            this.rbSouth.Location = new System.Drawing.Point(18, 65);
            this.rbSouth.Name = "rbSouth";
            this.rbSouth.Size = new System.Drawing.Size(53, 17);
            this.rbSouth.TabIndex = 3;
            this.rbSouth.Text = "South";
            this.rbSouth.UseVisualStyleBackColor = true;
            this.rbSouth.CheckedChanged += new System.EventHandler(this.rbSouth_CheckedChanged);
            // 
            // rbWest
            // 
            this.rbWest.AutoSize = true;
            this.rbWest.Location = new System.Drawing.Point(18, 32);
            this.rbWest.Name = "rbWest";
            this.rbWest.Size = new System.Drawing.Size(50, 17);
            this.rbWest.TabIndex = 4;
            this.rbWest.Text = "West";
            this.rbWest.UseVisualStyleBackColor = true;
            this.rbWest.CheckedChanged += new System.EventHandler(this.rbWest_CheckedChanged);
            // 
            // rbEastWestStatic
            // 
            this.rbEastWestStatic.AutoSize = true;
            this.rbEastWestStatic.Checked = true;
            this.rbEastWestStatic.Location = new System.Drawing.Point(77, 32);
            this.rbEastWestStatic.Name = "rbEastWestStatic";
            this.rbEastWestStatic.Size = new System.Drawing.Size(52, 17);
            this.rbEastWestStatic.TabIndex = 5;
            this.rbEastWestStatic.TabStop = true;
            this.rbEastWestStatic.Text = "Static";
            this.rbEastWestStatic.UseVisualStyleBackColor = true;
            this.rbEastWestStatic.CheckedChanged += new System.EventHandler(this.rbEastWestStatic_CheckedChanged);
            // 
            // rbEast
            // 
            this.rbEast.AutoSize = true;
            this.rbEast.Location = new System.Drawing.Point(135, 32);
            this.rbEast.Name = "rbEast";
            this.rbEast.Size = new System.Drawing.Size(46, 17);
            this.rbEast.TabIndex = 6;
            this.rbEast.Text = "East";
            this.rbEast.UseVisualStyleBackColor = true;
            this.rbEast.CheckedChanged += new System.EventHandler(this.rbEast_CheckedChanged);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.rbWest);
            this.groupBox1.Controls.Add(this.rbEast);
            this.groupBox1.Controls.Add(this.rbEastWestStatic);
            this.groupBox1.Location = new System.Drawing.Point(21, 153);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(200, 67);
            this.groupBox1.TabIndex = 7;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "East/West";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.rbNorth);
            this.groupBox2.Controls.Add(this.rbNorthSouthStatic);
            this.groupBox2.Controls.Add(this.rbSouth);
            this.groupBox2.Location = new System.Drawing.Point(21, 36);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(86, 100);
            this.groupBox2.TabIndex = 8;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "North/South";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.rbSpeed10000);
            this.groupBox3.Controls.Add(this.rbSpeed1000);
            this.groupBox3.Controls.Add(this.rbSpeed100);
            this.groupBox3.Controls.Add(this.rbSpeed10);
            this.groupBox3.Controls.Add(this.rbSpeed1);
            this.groupBox3.Controls.Add(this.rbSpeed0);
            this.groupBox3.Location = new System.Drawing.Point(21, 252);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(304, 64);
            this.groupBox3.TabIndex = 9;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Speed";
            // 
            // rbSpeed10000
            // 
            this.rbSpeed10000.AutoSize = true;
            this.rbSpeed10000.Location = new System.Drawing.Point(242, 32);
            this.rbSpeed10000.Name = "rbSpeed10000";
            this.rbSpeed10000.Size = new System.Drawing.Size(58, 17);
            this.rbSpeed10000.TabIndex = 5;
            this.rbSpeed10000.Text = "10,000";
            this.rbSpeed10000.UseVisualStyleBackColor = true;
            this.rbSpeed10000.CheckedChanged += new System.EventHandler(this.rbSpeed10000_CheckedChanged);
            // 
            // rbSpeed1000
            // 
            this.rbSpeed1000.AutoSize = true;
            this.rbSpeed1000.Location = new System.Drawing.Point(184, 32);
            this.rbSpeed1000.Name = "rbSpeed1000";
            this.rbSpeed1000.Size = new System.Drawing.Size(52, 17);
            this.rbSpeed1000.TabIndex = 4;
            this.rbSpeed1000.Text = "1,000";
            this.rbSpeed1000.UseVisualStyleBackColor = true;
            this.rbSpeed1000.CheckedChanged += new System.EventHandler(this.rbSpeed1000_CheckedChanged);
            // 
            // rbSpeed100
            // 
            this.rbSpeed100.AutoSize = true;
            this.rbSpeed100.Location = new System.Drawing.Point(135, 32);
            this.rbSpeed100.Name = "rbSpeed100";
            this.rbSpeed100.Size = new System.Drawing.Size(43, 17);
            this.rbSpeed100.TabIndex = 3;
            this.rbSpeed100.Text = "100";
            this.rbSpeed100.UseVisualStyleBackColor = true;
            this.rbSpeed100.CheckedChanged += new System.EventHandler(this.rbSpeed100_CheckedChanged);
            // 
            // rbSpeed10
            // 
            this.rbSpeed10.AutoSize = true;
            this.rbSpeed10.Location = new System.Drawing.Point(92, 32);
            this.rbSpeed10.Name = "rbSpeed10";
            this.rbSpeed10.Size = new System.Drawing.Size(37, 17);
            this.rbSpeed10.TabIndex = 2;
            this.rbSpeed10.Text = "10";
            this.rbSpeed10.UseVisualStyleBackColor = true;
            this.rbSpeed10.CheckedChanged += new System.EventHandler(this.rbSpeed10_CheckedChanged);
            // 
            // rbSpeed1
            // 
            this.rbSpeed1.AutoSize = true;
            this.rbSpeed1.Location = new System.Drawing.Point(55, 32);
            this.rbSpeed1.Name = "rbSpeed1";
            this.rbSpeed1.Size = new System.Drawing.Size(31, 17);
            this.rbSpeed1.TabIndex = 1;
            this.rbSpeed1.Text = "1";
            this.rbSpeed1.UseVisualStyleBackColor = true;
            this.rbSpeed1.CheckedChanged += new System.EventHandler(this.rbSpeed1_CheckedChanged);
            // 
            // rbSpeed0
            // 
            this.rbSpeed0.AutoSize = true;
            this.rbSpeed0.Checked = true;
            this.rbSpeed0.Location = new System.Drawing.Point(18, 32);
            this.rbSpeed0.Name = "rbSpeed0";
            this.rbSpeed0.Size = new System.Drawing.Size(31, 17);
            this.rbSpeed0.TabIndex = 0;
            this.rbSpeed0.TabStop = true;
            this.rbSpeed0.Text = "0";
            this.rbSpeed0.UseVisualStyleBackColor = true;
            this.rbSpeed0.CheckedChanged += new System.EventHandler(this.rbSpeed0_CheckedChanged);
            // 
            // txtInputLat
            // 
            this.txtInputLat.Location = new System.Drawing.Point(290, 49);
            this.txtInputLat.Name = "txtInputLat";
            this.txtInputLat.Size = new System.Drawing.Size(100, 20);
            this.txtInputLat.TabIndex = 10;
            // 
            // txtInputLong
            // 
            this.txtInputLong.Location = new System.Drawing.Point(290, 75);
            this.txtInputLong.Name = "txtInputLong";
            this.txtInputLong.Size = new System.Drawing.Size(100, 20);
            this.txtInputLong.TabIndex = 11;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(228, 49);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(22, 13);
            this.label1.TabIndex = 12;
            this.label1.Text = "Lat";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(228, 78);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(31, 13);
            this.label2.TabIndex = 13;
            this.label2.Text = "Long";
            // 
            // lblCurrentLat
            // 
            this.lblCurrentLat.AutoSize = true;
            this.lblCurrentLat.Location = new System.Drawing.Point(410, 55);
            this.lblCurrentLat.Name = "lblCurrentLat";
            this.lblCurrentLat.Size = new System.Drawing.Size(56, 13);
            this.lblCurrentLat.TabIndex = 14;
            this.lblCurrentLat.Text = "CurrentLat";
            // 
            // lblCurrentLong
            // 
            this.lblCurrentLong.AutoSize = true;
            this.lblCurrentLong.Location = new System.Drawing.Point(410, 77);
            this.lblCurrentLong.Name = "lblCurrentLong";
            this.lblCurrentLong.Size = new System.Drawing.Size(65, 13);
            this.lblCurrentLong.TabIndex = 15;
            this.lblCurrentLong.Text = "CurrentLong";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(307, 113);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 16;
            this.button1.Text = "&Update";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // timer1
            // 
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.rbHDGStarboard);
            this.groupBox4.Controls.Add(this.rbHDGStatic);
            this.groupBox4.Controls.Add(this.rbHDGPort);
            this.groupBox4.Location = new System.Drawing.Point(247, 153);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(219, 67);
            this.groupBox4.TabIndex = 17;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Heading";
            // 
            // rbHDGStarboard
            // 
            this.rbHDGStarboard.AutoSize = true;
            this.rbHDGStarboard.Location = new System.Drawing.Point(126, 29);
            this.rbHDGStarboard.Name = "rbHDGStarboard";
            this.rbHDGStarboard.Size = new System.Drawing.Size(72, 17);
            this.rbHDGStarboard.TabIndex = 2;
            this.rbHDGStarboard.Text = "StarBoard";
            this.rbHDGStarboard.UseVisualStyleBackColor = true;
            this.rbHDGStarboard.CheckedChanged += new System.EventHandler(this.rbHDGStarboard_CheckedChanged);
            // 
            // rbHDGStatic
            // 
            this.rbHDGStatic.AutoSize = true;
            this.rbHDGStatic.Checked = true;
            this.rbHDGStatic.Location = new System.Drawing.Point(68, 29);
            this.rbHDGStatic.Name = "rbHDGStatic";
            this.rbHDGStatic.Size = new System.Drawing.Size(52, 17);
            this.rbHDGStatic.TabIndex = 1;
            this.rbHDGStatic.TabStop = true;
            this.rbHDGStatic.Text = "Static";
            this.rbHDGStatic.UseVisualStyleBackColor = true;
            this.rbHDGStatic.CheckedChanged += new System.EventHandler(this.rbHDGStatic_CheckedChanged);
            // 
            // rbHDGPort
            // 
            this.rbHDGPort.AutoSize = true;
            this.rbHDGPort.Location = new System.Drawing.Point(18, 29);
            this.rbHDGPort.Name = "rbHDGPort";
            this.rbHDGPort.Size = new System.Drawing.Size(44, 17);
            this.rbHDGPort.TabIndex = 0;
            this.rbHDGPort.Text = "Port";
            this.rbHDGPort.UseVisualStyleBackColor = true;
            this.rbHDGPort.CheckedChanged += new System.EventHandler(this.rbHDGPort_CheckedChanged);
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.rbAltDec);
            this.groupBox5.Controls.Add(this.rbAltStatic);
            this.groupBox5.Controls.Add(this.rbAltInc);
            this.groupBox5.Location = new System.Drawing.Point(125, 36);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(85, 100);
            this.groupBox5.TabIndex = 18;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Altitude";
            // 
            // rbAltDec
            // 
            this.rbAltDec.AutoSize = true;
            this.rbAltDec.Location = new System.Drawing.Point(7, 65);
            this.rbAltDec.Name = "rbAltDec";
            this.rbAltDec.Size = new System.Drawing.Size(45, 17);
            this.rbAltDec.TabIndex = 2;
            this.rbAltDec.Text = "Dec";
            this.rbAltDec.UseVisualStyleBackColor = true;
            this.rbAltDec.CheckedChanged += new System.EventHandler(this.rbAltDec_CheckedChanged);
            // 
            // rbAltStatic
            // 
            this.rbAltStatic.AutoSize = true;
            this.rbAltStatic.Checked = true;
            this.rbAltStatic.Location = new System.Drawing.Point(7, 42);
            this.rbAltStatic.Name = "rbAltStatic";
            this.rbAltStatic.Size = new System.Drawing.Size(52, 17);
            this.rbAltStatic.TabIndex = 1;
            this.rbAltStatic.TabStop = true;
            this.rbAltStatic.Text = "Static";
            this.rbAltStatic.UseVisualStyleBackColor = true;
            this.rbAltStatic.CheckedChanged += new System.EventHandler(this.rbAltStatic_CheckedChanged);
            // 
            // rbAltInc
            // 
            this.rbAltInc.AutoSize = true;
            this.rbAltInc.Location = new System.Drawing.Point(7, 19);
            this.rbAltInc.Name = "rbAltInc";
            this.rbAltInc.Size = new System.Drawing.Size(40, 17);
            this.rbAltInc.TabIndex = 0;
            this.rbAltInc.Text = "Inc";
            this.rbAltInc.UseVisualStyleBackColor = true;
            this.rbAltInc.CheckedChanged += new System.EventHandler(this.rbAltInc_CheckedChanged);
            // 
            // frmManualPosition
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(517, 335);
            this.Controls.Add(this.groupBox5);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.lblCurrentLong);
            this.Controls.Add(this.lblCurrentLat);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.txtInputLong);
            this.Controls.Add(this.txtInputLat);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "frmManualPosition";
            this.Text = "Manual Positioning";
            this.Load += new System.EventHandler(this.frmManualPosition_Load);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem cancelToolStripMenuItem;
        private System.Windows.Forms.RadioButton rbNorth;
        private System.Windows.Forms.RadioButton rbNorthSouthStatic;
        private System.Windows.Forms.RadioButton rbSouth;
        private System.Windows.Forms.RadioButton rbWest;
        private System.Windows.Forms.RadioButton rbEastWestStatic;
        private System.Windows.Forms.RadioButton rbEast;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.RadioButton rbSpeed10;
        private System.Windows.Forms.RadioButton rbSpeed1;
        private System.Windows.Forms.RadioButton rbSpeed0;
        private System.Windows.Forms.TextBox txtInputLat;
        private System.Windows.Forms.TextBox txtInputLong;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label lblCurrentLat;
        private System.Windows.Forms.Label lblCurrentLong;
        private System.Windows.Forms.RadioButton rbSpeed10000;
        private System.Windows.Forms.RadioButton rbSpeed1000;
        private System.Windows.Forms.RadioButton rbSpeed100;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.RadioButton rbHDGStarboard;
        private System.Windows.Forms.RadioButton rbHDGStatic;
        private System.Windows.Forms.RadioButton rbHDGPort;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.RadioButton rbAltDec;
        private System.Windows.Forms.RadioButton rbAltStatic;
        private System.Windows.Forms.RadioButton rbAltInc;
        private System.Windows.Forms.ToolStripMenuItem goToToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem brisbaneToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem queenstownToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem uRKAToolStripMenuItem;
    }
}