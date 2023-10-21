namespace DCSExtractGui
{
    partial class Form1
    {
        /// <summary>
        /// Variable del diseñador requerida.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Limpiar los recursos que se estén utilizando.
        /// </summary>
        /// <param name="disposing">true si los recursos administrados se deben eliminar; false en caso contrario, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Código generado por el Diseñador de Windows Forms

        /// <summary>
        /// Método necesario para admitir el Diseñador. No se puede modificar
        /// el contenido del método con el editor de código.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.main = new System.Windows.Forms.Timer(this.components);
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.uv26 = new System.Windows.Forms.Label();
            this.ekran_line1 = new System.Windows.Forms.Label();
            this.ekran_line2 = new System.Windows.Forms.Label();
            this.ekran_line3 = new System.Windows.Forms.Label();
            this.ekran_line4 = new System.Windows.Forms.Label();
            this.pvi_sign_up = new System.Windows.Forms.Label();
            this.pvi_up = new System.Windows.Forms.Label();
            this.pvi_up_right = new System.Windows.Forms.Label();
            this.pvi_sign_down = new System.Windows.Forms.Label();
            this.pvi_down = new System.Windows.Forms.Label();
            this.pvi_down_right = new System.Windows.Forms.Label();
            this.pui_left = new System.Windows.Forms.Label();
            this.pui_middle = new System.Windows.Forms.Label();
            this.pui_right = new System.Windows.Forms.Label();
            this.r828_on = new System.Windows.Forms.Label();
            this.r828_channel = new System.Windows.Forms.Label();
            this.spu9_on = new System.Windows.Forms.Label();
            this.spu9_channel = new System.Windows.Forms.Label();
            this.r800_on = new System.Windows.Forms.Label();
            this.r800_am = new System.Windows.Forms.Label();
            this.r800_frequency = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.label13 = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.cmsc_chaff = new System.Windows.Forms.Label();
            this.cmsc_flare = new System.Windows.Forms.Label();
            this.cmsc_jammer = new System.Windows.Forms.Label();
            this.cmsc_mws = new System.Windows.Forms.Label();
            this.cmsc_uwthreats = new System.Windows.Forms.Label();
            this.cmsc_uwsymbols = new System.Windows.Forms.Label();
            this.cmsp_up4 = new System.Windows.Forms.Label();
            this.cmsp_up3 = new System.Windows.Forms.Label();
            this.cmsp_up2 = new System.Windows.Forms.Label();
            this.cmsp_up1 = new System.Windows.Forms.Label();
            this.cmsp_down4 = new System.Windows.Forms.Label();
            this.cmsp_down3 = new System.Windows.Forms.Label();
            this.cmsp_down2 = new System.Windows.Forms.Label();
            this.cmsp_down1 = new System.Windows.Forms.Label();
            this.tacan_modedial = new System.Windows.Forms.Label();
            this.tacan_frequency = new System.Windows.Forms.Label();
            this.ils_on = new System.Windows.Forms.Label();
            this.ils_frequency = new System.Windows.Forms.Label();
            this.uhf_functiondial = new System.Windows.Forms.Label();
            this.uhf_modefrequency = new System.Windows.Forms.Label();
            this.uhf_frequency = new System.Windows.Forms.Label();
            this.uhf_channel = new System.Windows.Forms.Label();
            this.vhf_am_on = new System.Windows.Forms.Label();
            this.vhf_am_modefrequency = new System.Windows.Forms.Label();
            this.vhf_am_selectfrequency = new System.Windows.Forms.Label();
            this.vhf_am_frequency = new System.Windows.Forms.Label();
            this.vhf_am_channel = new System.Windows.Forms.Label();
            this.vhf_fm_on = new System.Windows.Forms.Label();
            this.vhf_fm_modefrequency = new System.Windows.Forms.Label();
            this.vhf_fm_selectfrequency = new System.Windows.Forms.Label();
            this.vhf_fm_frequency = new System.Windows.Forms.Label();
            this.vhf_fm_channel = new System.Windows.Forms.Label();
            this.ekran_failure = new System.Windows.Forms.Label();
            this.ekran_memory = new System.Windows.Forms.Label();
            this.ekran_queue = new System.Windows.Forms.Label();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.SuspendLayout();
            // 
            // main
            // 
            this.main.Tick += new System.EventHandler(this.OnTick);
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Location = new System.Drawing.Point(-1, -1);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(429, 386);
            this.tabControl1.TabIndex = 0;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.ekran_queue);
            this.tabPage1.Controls.Add(this.ekran_memory);
            this.tabPage1.Controls.Add(this.ekran_failure);
            this.tabPage1.Controls.Add(this.r800_frequency);
            this.tabPage1.Controls.Add(this.r800_am);
            this.tabPage1.Controls.Add(this.r800_on);
            this.tabPage1.Controls.Add(this.spu9_channel);
            this.tabPage1.Controls.Add(this.spu9_on);
            this.tabPage1.Controls.Add(this.r828_channel);
            this.tabPage1.Controls.Add(this.r828_on);
            this.tabPage1.Controls.Add(this.pui_right);
            this.tabPage1.Controls.Add(this.pui_middle);
            this.tabPage1.Controls.Add(this.pui_left);
            this.tabPage1.Controls.Add(this.pvi_down_right);
            this.tabPage1.Controls.Add(this.pvi_down);
            this.tabPage1.Controls.Add(this.pvi_sign_down);
            this.tabPage1.Controls.Add(this.pvi_up_right);
            this.tabPage1.Controls.Add(this.pvi_up);
            this.tabPage1.Controls.Add(this.pvi_sign_up);
            this.tabPage1.Controls.Add(this.ekran_line4);
            this.tabPage1.Controls.Add(this.ekran_line3);
            this.tabPage1.Controls.Add(this.ekran_line2);
            this.tabPage1.Controls.Add(this.ekran_line1);
            this.tabPage1.Controls.Add(this.uv26);
            this.tabPage1.Controls.Add(this.label7);
            this.tabPage1.Controls.Add(this.label6);
            this.tabPage1.Controls.Add(this.label5);
            this.tabPage1.Controls.Add(this.label4);
            this.tabPage1.Controls.Add(this.label3);
            this.tabPage1.Controls.Add(this.label2);
            this.tabPage1.Controls.Add(this.label1);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(421, 360);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "KA-50";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.vhf_fm_channel);
            this.tabPage2.Controls.Add(this.vhf_fm_frequency);
            this.tabPage2.Controls.Add(this.vhf_fm_selectfrequency);
            this.tabPage2.Controls.Add(this.vhf_fm_modefrequency);
            this.tabPage2.Controls.Add(this.vhf_fm_on);
            this.tabPage2.Controls.Add(this.vhf_am_channel);
            this.tabPage2.Controls.Add(this.vhf_am_frequency);
            this.tabPage2.Controls.Add(this.vhf_am_selectfrequency);
            this.tabPage2.Controls.Add(this.vhf_am_modefrequency);
            this.tabPage2.Controls.Add(this.vhf_am_on);
            this.tabPage2.Controls.Add(this.uhf_channel);
            this.tabPage2.Controls.Add(this.uhf_frequency);
            this.tabPage2.Controls.Add(this.uhf_modefrequency);
            this.tabPage2.Controls.Add(this.uhf_functiondial);
            this.tabPage2.Controls.Add(this.ils_frequency);
            this.tabPage2.Controls.Add(this.ils_on);
            this.tabPage2.Controls.Add(this.tacan_frequency);
            this.tabPage2.Controls.Add(this.tacan_modedial);
            this.tabPage2.Controls.Add(this.cmsp_down1);
            this.tabPage2.Controls.Add(this.cmsp_down2);
            this.tabPage2.Controls.Add(this.cmsp_down3);
            this.tabPage2.Controls.Add(this.cmsp_down4);
            this.tabPage2.Controls.Add(this.cmsp_up1);
            this.tabPage2.Controls.Add(this.cmsp_up2);
            this.tabPage2.Controls.Add(this.cmsp_up3);
            this.tabPage2.Controls.Add(this.cmsp_up4);
            this.tabPage2.Controls.Add(this.cmsc_uwsymbols);
            this.tabPage2.Controls.Add(this.cmsc_uwthreats);
            this.tabPage2.Controls.Add(this.cmsc_mws);
            this.tabPage2.Controls.Add(this.cmsc_jammer);
            this.tabPage2.Controls.Add(this.cmsc_flare);
            this.tabPage2.Controls.Add(this.cmsc_chaff);
            this.tabPage2.Controls.Add(this.label14);
            this.tabPage2.Controls.Add(this.label13);
            this.tabPage2.Controls.Add(this.label12);
            this.tabPage2.Controls.Add(this.label11);
            this.tabPage2.Controls.Add(this.label10);
            this.tabPage2.Controls.Add(this.label9);
            this.tabPage2.Controls.Add(this.label8);
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(421, 360);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "A-10C";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(348, 3);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(54, 20);
            this.label1.TabIndex = 0;
            this.label1.Text = "UV26";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(176, 126);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(39, 20);
            this.label2.TabIndex = 1;
            this.label2.Text = "PUI";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(338, 176);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(38, 20);
            this.label3.TabIndex = 2;
            this.label3.Text = "PVI";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.Location = new System.Drawing.Point(331, 71);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(69, 20);
            this.label4.TabIndex = 3;
            this.label4.Text = "EKRAN";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label5.Location = new System.Drawing.Point(9, 162);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(52, 20);
            this.label5.TabIndex = 4;
            this.label5.Text = "R800";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label6.Location = new System.Drawing.Point(9, 3);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(55, 20);
            this.label6.TabIndex = 5;
            this.label6.Text = "SPU9";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label7.Location = new System.Drawing.Point(348, 287);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(52, 20);
            this.label7.TabIndex = 6;
            this.label7.Text = "R828";
            // 
            // uv26
            // 
            this.uv26.AutoSize = true;
            this.uv26.Location = new System.Drawing.Point(362, 23);
            this.uv26.Name = "uv26";
            this.uv26.Size = new System.Drawing.Size(35, 13);
            this.uv26.TabIndex = 7;
            this.uv26.Text = "label8";
            // 
            // ekran_line1
            // 
            this.ekran_line1.AutoSize = true;
            this.ekran_line1.Location = new System.Drawing.Point(349, 113);
            this.ekran_line1.Name = "ekran_line1";
            this.ekran_line1.Size = new System.Drawing.Size(35, 13);
            this.ekran_line1.TabIndex = 8;
            this.ekran_line1.Text = "label8";
            // 
            // ekran_line2
            // 
            this.ekran_line2.AutoSize = true;
            this.ekran_line2.Location = new System.Drawing.Point(349, 126);
            this.ekran_line2.Name = "ekran_line2";
            this.ekran_line2.Size = new System.Drawing.Size(35, 13);
            this.ekran_line2.TabIndex = 9;
            this.ekran_line2.Text = "label9";
            // 
            // ekran_line3
            // 
            this.ekran_line3.AutoSize = true;
            this.ekran_line3.Location = new System.Drawing.Point(349, 139);
            this.ekran_line3.Name = "ekran_line3";
            this.ekran_line3.Size = new System.Drawing.Size(41, 13);
            this.ekran_line3.TabIndex = 10;
            this.ekran_line3.Text = "label10";
            // 
            // ekran_line4
            // 
            this.ekran_line4.AutoSize = true;
            this.ekran_line4.Location = new System.Drawing.Point(349, 155);
            this.ekran_line4.Name = "ekran_line4";
            this.ekran_line4.Size = new System.Drawing.Size(41, 13);
            this.ekran_line4.TabIndex = 11;
            this.ekran_line4.Text = "label11";
            // 
            // pvi_sign_up
            // 
            this.pvi_sign_up.AutoSize = true;
            this.pvi_sign_up.Location = new System.Drawing.Point(298, 208);
            this.pvi_sign_up.Name = "pvi_sign_up";
            this.pvi_sign_up.Size = new System.Drawing.Size(35, 13);
            this.pvi_sign_up.TabIndex = 12;
            this.pvi_sign_up.Text = "label8";
            // 
            // pvi_up
            // 
            this.pvi_up.AutoSize = true;
            this.pvi_up.Location = new System.Drawing.Point(339, 208);
            this.pvi_up.Name = "pvi_up";
            this.pvi_up.Size = new System.Drawing.Size(35, 13);
            this.pvi_up.TabIndex = 13;
            this.pvi_up.Text = "label8";
            // 
            // pvi_up_right
            // 
            this.pvi_up_right.AutoSize = true;
            this.pvi_up_right.Location = new System.Drawing.Point(380, 208);
            this.pvi_up_right.Name = "pvi_up_right";
            this.pvi_up_right.Size = new System.Drawing.Size(35, 13);
            this.pvi_up_right.TabIndex = 14;
            this.pvi_up_right.Text = "label8";
            // 
            // pvi_sign_down
            // 
            this.pvi_sign_down.AutoSize = true;
            this.pvi_sign_down.Location = new System.Drawing.Point(298, 231);
            this.pvi_sign_down.Name = "pvi_sign_down";
            this.pvi_sign_down.Size = new System.Drawing.Size(35, 13);
            this.pvi_sign_down.TabIndex = 15;
            this.pvi_sign_down.Text = "label8";
            // 
            // pvi_down
            // 
            this.pvi_down.AutoSize = true;
            this.pvi_down.Location = new System.Drawing.Point(339, 231);
            this.pvi_down.Name = "pvi_down";
            this.pvi_down.Size = new System.Drawing.Size(35, 13);
            this.pvi_down.TabIndex = 16;
            this.pvi_down.Text = "label8";
            // 
            // pvi_down_right
            // 
            this.pvi_down_right.AutoSize = true;
            this.pvi_down_right.Location = new System.Drawing.Point(380, 231);
            this.pvi_down_right.Name = "pvi_down_right";
            this.pvi_down_right.Size = new System.Drawing.Size(35, 13);
            this.pvi_down_right.TabIndex = 17;
            this.pvi_down_right.Text = "label8";
            // 
            // pui_left
            // 
            this.pui_left.AutoSize = true;
            this.pui_left.Location = new System.Drawing.Point(130, 162);
            this.pui_left.Name = "pui_left";
            this.pui_left.Size = new System.Drawing.Size(35, 13);
            this.pui_left.TabIndex = 18;
            this.pui_left.Text = "label8";
            // 
            // pui_middle
            // 
            this.pui_middle.AutoSize = true;
            this.pui_middle.Location = new System.Drawing.Point(172, 162);
            this.pui_middle.Name = "pui_middle";
            this.pui_middle.Size = new System.Drawing.Size(35, 13);
            this.pui_middle.TabIndex = 19;
            this.pui_middle.Text = "label8";
            // 
            // pui_right
            // 
            this.pui_right.AutoSize = true;
            this.pui_right.Location = new System.Drawing.Point(214, 162);
            this.pui_right.Name = "pui_right";
            this.pui_right.Size = new System.Drawing.Size(35, 13);
            this.pui_right.TabIndex = 20;
            this.pui_right.Text = "label8";
            // 
            // r828_on
            // 
            this.r828_on.AutoSize = true;
            this.r828_on.Location = new System.Drawing.Point(365, 307);
            this.r828_on.Name = "r828_on";
            this.r828_on.Size = new System.Drawing.Size(35, 13);
            this.r828_on.TabIndex = 21;
            this.r828_on.Text = "label8";
            // 
            // r828_channel
            // 
            this.r828_channel.AutoSize = true;
            this.r828_channel.Location = new System.Drawing.Point(365, 324);
            this.r828_channel.Name = "r828_channel";
            this.r828_channel.Size = new System.Drawing.Size(35, 13);
            this.r828_channel.TabIndex = 22;
            this.r828_channel.Text = "label8";
            // 
            // spu9_on
            // 
            this.spu9_on.AutoSize = true;
            this.spu9_on.Location = new System.Drawing.Point(13, 27);
            this.spu9_on.Name = "spu9_on";
            this.spu9_on.Size = new System.Drawing.Size(35, 13);
            this.spu9_on.TabIndex = 23;
            this.spu9_on.Text = "label8";
            // 
            // spu9_channel
            // 
            this.spu9_channel.AutoSize = true;
            this.spu9_channel.Location = new System.Drawing.Point(13, 44);
            this.spu9_channel.Name = "spu9_channel";
            this.spu9_channel.Size = new System.Drawing.Size(35, 13);
            this.spu9_channel.TabIndex = 24;
            this.spu9_channel.Text = "label8";
            // 
            // r800_on
            // 
            this.r800_on.AutoSize = true;
            this.r800_on.Location = new System.Drawing.Point(13, 186);
            this.r800_on.Name = "r800_on";
            this.r800_on.Size = new System.Drawing.Size(35, 13);
            this.r800_on.TabIndex = 25;
            this.r800_on.Text = "label8";
            // 
            // r800_am
            // 
            this.r800_am.AutoSize = true;
            this.r800_am.Location = new System.Drawing.Point(13, 203);
            this.r800_am.Name = "r800_am";
            this.r800_am.Size = new System.Drawing.Size(35, 13);
            this.r800_am.TabIndex = 26;
            this.r800_am.Text = "label8";
            // 
            // r800_frequency
            // 
            this.r800_frequency.AutoSize = true;
            this.r800_frequency.Location = new System.Drawing.Point(13, 220);
            this.r800_frequency.Name = "r800_frequency";
            this.r800_frequency.Size = new System.Drawing.Size(35, 13);
            this.r800_frequency.TabIndex = 27;
            this.r800_frequency.Text = "label8";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label8.Location = new System.Drawing.Point(329, 3);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(59, 20);
            this.label8.TabIndex = 6;
            this.label8.Text = "CMSC";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label9.Location = new System.Drawing.Point(296, 105);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(58, 20);
            this.label9.TabIndex = 7;
            this.label9.Text = "CMSP";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label10.Location = new System.Drawing.Point(348, 185);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(67, 20);
            this.label10.TabIndex = 8;
            this.label10.Text = "TACAN";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label11.Location = new System.Drawing.Point(378, 261);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(37, 20);
            this.label11.TabIndex = 9;
            this.label11.Text = "ILS";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label12.Location = new System.Drawing.Point(2, 3);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(46, 20);
            this.label12.TabIndex = 10;
            this.label12.Text = "UHF";
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label13.Location = new System.Drawing.Point(2, 105);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(76, 20);
            this.label13.TabIndex = 11;
            this.label13.Text = "VHF AM";
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label14.Location = new System.Drawing.Point(3, 225);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(75, 20);
            this.label14.TabIndex = 12;
            this.label14.Text = "VHF FM";
            // 
            // cmsc_chaff
            // 
            this.cmsc_chaff.AutoSize = true;
            this.cmsc_chaff.Location = new System.Drawing.Point(330, 23);
            this.cmsc_chaff.Name = "cmsc_chaff";
            this.cmsc_chaff.Size = new System.Drawing.Size(41, 13);
            this.cmsc_chaff.TabIndex = 13;
            this.cmsc_chaff.Text = "label15";
            // 
            // cmsc_flare
            // 
            this.cmsc_flare.AutoSize = true;
            this.cmsc_flare.Location = new System.Drawing.Point(377, 23);
            this.cmsc_flare.Name = "cmsc_flare";
            this.cmsc_flare.Size = new System.Drawing.Size(41, 13);
            this.cmsc_flare.TabIndex = 14;
            this.cmsc_flare.Text = "label15";
            // 
            // cmsc_jammer
            // 
            this.cmsc_jammer.AutoSize = true;
            this.cmsc_jammer.Location = new System.Drawing.Point(283, 23);
            this.cmsc_jammer.Name = "cmsc_jammer";
            this.cmsc_jammer.Size = new System.Drawing.Size(41, 13);
            this.cmsc_jammer.TabIndex = 15;
            this.cmsc_jammer.Text = "label15";
            // 
            // cmsc_mws
            // 
            this.cmsc_mws.AutoSize = true;
            this.cmsc_mws.Location = new System.Drawing.Point(283, 46);
            this.cmsc_mws.Name = "cmsc_mws";
            this.cmsc_mws.Size = new System.Drawing.Size(41, 13);
            this.cmsc_mws.TabIndex = 16;
            this.cmsc_mws.Text = "label15";
            // 
            // cmsc_uwthreats
            // 
            this.cmsc_uwthreats.AutoSize = true;
            this.cmsc_uwthreats.Location = new System.Drawing.Point(330, 46);
            this.cmsc_uwthreats.Name = "cmsc_uwthreats";
            this.cmsc_uwthreats.Size = new System.Drawing.Size(41, 13);
            this.cmsc_uwthreats.TabIndex = 17;
            this.cmsc_uwthreats.Text = "label15";
            // 
            // cmsc_uwsymbols
            // 
            this.cmsc_uwsymbols.AutoSize = true;
            this.cmsc_uwsymbols.Location = new System.Drawing.Point(377, 46);
            this.cmsc_uwsymbols.Name = "cmsc_uwsymbols";
            this.cmsc_uwsymbols.Size = new System.Drawing.Size(41, 13);
            this.cmsc_uwsymbols.TabIndex = 18;
            this.cmsc_uwsymbols.Text = "label15";
            // 
            // cmsp_up4
            // 
            this.cmsp_up4.AutoSize = true;
            this.cmsp_up4.Location = new System.Drawing.Point(373, 129);
            this.cmsp_up4.Name = "cmsp_up4";
            this.cmsp_up4.Size = new System.Drawing.Size(41, 13);
            this.cmsp_up4.TabIndex = 19;
            this.cmsp_up4.Text = "label15";
            // 
            // cmsp_up3
            // 
            this.cmsp_up3.AutoSize = true;
            this.cmsp_up3.Location = new System.Drawing.Point(326, 129);
            this.cmsp_up3.Name = "cmsp_up3";
            this.cmsp_up3.Size = new System.Drawing.Size(41, 13);
            this.cmsp_up3.TabIndex = 20;
            this.cmsp_up3.Text = "label15";
            // 
            // cmsp_up2
            // 
            this.cmsp_up2.AutoSize = true;
            this.cmsp_up2.Location = new System.Drawing.Point(279, 129);
            this.cmsp_up2.Name = "cmsp_up2";
            this.cmsp_up2.Size = new System.Drawing.Size(41, 13);
            this.cmsp_up2.TabIndex = 21;
            this.cmsp_up2.Text = "label15";
            // 
            // cmsp_up1
            // 
            this.cmsp_up1.AutoSize = true;
            this.cmsp_up1.Location = new System.Drawing.Point(232, 129);
            this.cmsp_up1.Name = "cmsp_up1";
            this.cmsp_up1.Size = new System.Drawing.Size(41, 13);
            this.cmsp_up1.TabIndex = 22;
            this.cmsp_up1.Text = "label15";
            // 
            // cmsp_down4
            // 
            this.cmsp_down4.AutoSize = true;
            this.cmsp_down4.Location = new System.Drawing.Point(373, 152);
            this.cmsp_down4.Name = "cmsp_down4";
            this.cmsp_down4.Size = new System.Drawing.Size(41, 13);
            this.cmsp_down4.TabIndex = 23;
            this.cmsp_down4.Text = "label15";
            // 
            // cmsp_down3
            // 
            this.cmsp_down3.AutoSize = true;
            this.cmsp_down3.Location = new System.Drawing.Point(326, 152);
            this.cmsp_down3.Name = "cmsp_down3";
            this.cmsp_down3.Size = new System.Drawing.Size(41, 13);
            this.cmsp_down3.TabIndex = 24;
            this.cmsp_down3.Text = "label15";
            // 
            // cmsp_down2
            // 
            this.cmsp_down2.AutoSize = true;
            this.cmsp_down2.Location = new System.Drawing.Point(279, 152);
            this.cmsp_down2.Name = "cmsp_down2";
            this.cmsp_down2.Size = new System.Drawing.Size(41, 13);
            this.cmsp_down2.TabIndex = 25;
            this.cmsp_down2.Text = "label15";
            // 
            // cmsp_down1
            // 
            this.cmsp_down1.AutoSize = true;
            this.cmsp_down1.Location = new System.Drawing.Point(232, 152);
            this.cmsp_down1.Name = "cmsp_down1";
            this.cmsp_down1.Size = new System.Drawing.Size(41, 13);
            this.cmsp_down1.TabIndex = 26;
            this.cmsp_down1.Text = "label15";
            // 
            // tacan_modedial
            // 
            this.tacan_modedial.AutoSize = true;
            this.tacan_modedial.Location = new System.Drawing.Point(349, 208);
            this.tacan_modedial.Name = "tacan_modedial";
            this.tacan_modedial.Size = new System.Drawing.Size(41, 13);
            this.tacan_modedial.TabIndex = 27;
            this.tacan_modedial.Text = "label15";
            // 
            // tacan_frequency
            // 
            this.tacan_frequency.AutoSize = true;
            this.tacan_frequency.Location = new System.Drawing.Point(349, 232);
            this.tacan_frequency.Name = "tacan_frequency";
            this.tacan_frequency.Size = new System.Drawing.Size(41, 13);
            this.tacan_frequency.TabIndex = 28;
            this.tacan_frequency.Text = "label15";
            // 
            // ils_on
            // 
            this.ils_on.AutoSize = true;
            this.ils_on.Location = new System.Drawing.Point(361, 285);
            this.ils_on.Name = "ils_on";
            this.ils_on.Size = new System.Drawing.Size(41, 13);
            this.ils_on.TabIndex = 29;
            this.ils_on.Text = "label15";
            // 
            // ils_frequency
            // 
            this.ils_frequency.AutoSize = true;
            this.ils_frequency.Location = new System.Drawing.Point(361, 311);
            this.ils_frequency.Name = "ils_frequency";
            this.ils_frequency.Size = new System.Drawing.Size(41, 13);
            this.ils_frequency.TabIndex = 30;
            this.ils_frequency.Text = "label15";
            // 
            // uhf_functiondial
            // 
            this.uhf_functiondial.AutoSize = true;
            this.uhf_functiondial.Location = new System.Drawing.Point(4, 27);
            this.uhf_functiondial.Name = "uhf_functiondial";
            this.uhf_functiondial.Size = new System.Drawing.Size(41, 13);
            this.uhf_functiondial.TabIndex = 31;
            this.uhf_functiondial.Text = "label15";
            // 
            // uhf_modefrequency
            // 
            this.uhf_modefrequency.AutoSize = true;
            this.uhf_modefrequency.Location = new System.Drawing.Point(4, 44);
            this.uhf_modefrequency.Name = "uhf_modefrequency";
            this.uhf_modefrequency.Size = new System.Drawing.Size(41, 13);
            this.uhf_modefrequency.TabIndex = 32;
            this.uhf_modefrequency.Text = "label15";
            // 
            // uhf_frequency
            // 
            this.uhf_frequency.AutoSize = true;
            this.uhf_frequency.Location = new System.Drawing.Point(4, 61);
            this.uhf_frequency.Name = "uhf_frequency";
            this.uhf_frequency.Size = new System.Drawing.Size(41, 13);
            this.uhf_frequency.TabIndex = 33;
            this.uhf_frequency.Text = "label15";
            // 
            // uhf_channel
            // 
            this.uhf_channel.AutoSize = true;
            this.uhf_channel.Location = new System.Drawing.Point(4, 78);
            this.uhf_channel.Name = "uhf_channel";
            this.uhf_channel.Size = new System.Drawing.Size(41, 13);
            this.uhf_channel.TabIndex = 34;
            this.uhf_channel.Text = "label15";
            // 
            // vhf_am_on
            // 
            this.vhf_am_on.AutoSize = true;
            this.vhf_am_on.Location = new System.Drawing.Point(4, 129);
            this.vhf_am_on.Name = "vhf_am_on";
            this.vhf_am_on.Size = new System.Drawing.Size(41, 13);
            this.vhf_am_on.TabIndex = 35;
            this.vhf_am_on.Text = "label15";
            // 
            // vhf_am_modefrequency
            // 
            this.vhf_am_modefrequency.AutoSize = true;
            this.vhf_am_modefrequency.Location = new System.Drawing.Point(4, 146);
            this.vhf_am_modefrequency.Name = "vhf_am_modefrequency";
            this.vhf_am_modefrequency.Size = new System.Drawing.Size(41, 13);
            this.vhf_am_modefrequency.TabIndex = 36;
            this.vhf_am_modefrequency.Text = "label15";
            // 
            // vhf_am_selectfrequency
            // 
            this.vhf_am_selectfrequency.AutoSize = true;
            this.vhf_am_selectfrequency.Location = new System.Drawing.Point(4, 163);
            this.vhf_am_selectfrequency.Name = "vhf_am_selectfrequency";
            this.vhf_am_selectfrequency.Size = new System.Drawing.Size(41, 13);
            this.vhf_am_selectfrequency.TabIndex = 37;
            this.vhf_am_selectfrequency.Text = "label15";
            // 
            // vhf_am_frequency
            // 
            this.vhf_am_frequency.AutoSize = true;
            this.vhf_am_frequency.Location = new System.Drawing.Point(4, 180);
            this.vhf_am_frequency.Name = "vhf_am_frequency";
            this.vhf_am_frequency.Size = new System.Drawing.Size(41, 13);
            this.vhf_am_frequency.TabIndex = 38;
            this.vhf_am_frequency.Text = "label15";
            // 
            // vhf_am_channel
            // 
            this.vhf_am_channel.AutoSize = true;
            this.vhf_am_channel.Location = new System.Drawing.Point(4, 197);
            this.vhf_am_channel.Name = "vhf_am_channel";
            this.vhf_am_channel.Size = new System.Drawing.Size(41, 13);
            this.vhf_am_channel.TabIndex = 39;
            this.vhf_am_channel.Text = "label15";
            // 
            // vhf_fm_on
            // 
            this.vhf_fm_on.AutoSize = true;
            this.vhf_fm_on.Location = new System.Drawing.Point(4, 249);
            this.vhf_fm_on.Name = "vhf_fm_on";
            this.vhf_fm_on.Size = new System.Drawing.Size(41, 13);
            this.vhf_fm_on.TabIndex = 40;
            this.vhf_fm_on.Text = "label15";
            // 
            // vhf_fm_modefrequency
            // 
            this.vhf_fm_modefrequency.AutoSize = true;
            this.vhf_fm_modefrequency.Location = new System.Drawing.Point(4, 266);
            this.vhf_fm_modefrequency.Name = "vhf_fm_modefrequency";
            this.vhf_fm_modefrequency.Size = new System.Drawing.Size(41, 13);
            this.vhf_fm_modefrequency.TabIndex = 41;
            this.vhf_fm_modefrequency.Text = "label15";
            // 
            // vhf_fm_selectfrequency
            // 
            this.vhf_fm_selectfrequency.AutoSize = true;
            this.vhf_fm_selectfrequency.Location = new System.Drawing.Point(4, 283);
            this.vhf_fm_selectfrequency.Name = "vhf_fm_selectfrequency";
            this.vhf_fm_selectfrequency.Size = new System.Drawing.Size(41, 13);
            this.vhf_fm_selectfrequency.TabIndex = 42;
            this.vhf_fm_selectfrequency.Text = "label15";
            // 
            // vhf_fm_frequency
            // 
            this.vhf_fm_frequency.AutoSize = true;
            this.vhf_fm_frequency.Location = new System.Drawing.Point(4, 300);
            this.vhf_fm_frequency.Name = "vhf_fm_frequency";
            this.vhf_fm_frequency.Size = new System.Drawing.Size(41, 13);
            this.vhf_fm_frequency.TabIndex = 43;
            this.vhf_fm_frequency.Text = "label15";
            // 
            // vhf_fm_channel
            // 
            this.vhf_fm_channel.AutoSize = true;
            this.vhf_fm_channel.Location = new System.Drawing.Point(4, 317);
            this.vhf_fm_channel.Name = "vhf_fm_channel";
            this.vhf_fm_channel.Size = new System.Drawing.Size(41, 13);
            this.vhf_fm_channel.TabIndex = 44;
            this.vhf_fm_channel.Text = "label15";
            // 
            // ekran_failure
            // 
            this.ekran_failure.AutoSize = true;
            this.ekran_failure.Location = new System.Drawing.Point(374, 91);
            this.ekran_failure.Name = "ekran_failure";
            this.ekran_failure.Size = new System.Drawing.Size(41, 13);
            this.ekran_failure.TabIndex = 28;
            this.ekran_failure.Text = "label15";
            // 
            // ekran_memory
            // 
            this.ekran_memory.AutoSize = true;
            this.ekran_memory.Location = new System.Drawing.Point(327, 91);
            this.ekran_memory.Name = "ekran_memory";
            this.ekran_memory.Size = new System.Drawing.Size(41, 13);
            this.ekran_memory.TabIndex = 29;
            this.ekran_memory.Text = "label15";
            // 
            // ekran_queue
            // 
            this.ekran_queue.AutoSize = true;
            this.ekran_queue.Location = new System.Drawing.Point(280, 91);
            this.ekran_queue.Name = "ekran_queue";
            this.ekran_queue.Size = new System.Drawing.Size(41, 13);
            this.ekran_queue.TabIndex = 30;
            this.ekran_queue.Text = "label15";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(430, 383);
            this.Controls.Add(this.tabControl1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "Form1";
            this.Text = "DCSExtract";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.OnClosing);
            this.Load += new System.EventHandler(this.OnLoad);
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage1.PerformLayout();
            this.tabPage2.ResumeLayout(false);
            this.tabPage2.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Timer main;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label pvi_down_right;
        private System.Windows.Forms.Label pvi_down;
        private System.Windows.Forms.Label pvi_sign_down;
        private System.Windows.Forms.Label pvi_up_right;
        private System.Windows.Forms.Label pvi_up;
        private System.Windows.Forms.Label pvi_sign_up;
        private System.Windows.Forms.Label ekran_line4;
        private System.Windows.Forms.Label ekran_line3;
        private System.Windows.Forms.Label ekran_line2;
        private System.Windows.Forms.Label ekran_line1;
        private System.Windows.Forms.Label uv26;
        private System.Windows.Forms.Label pui_right;
        private System.Windows.Forms.Label pui_middle;
        private System.Windows.Forms.Label pui_left;
        private System.Windows.Forms.Label r828_channel;
        private System.Windows.Forms.Label r828_on;
        private System.Windows.Forms.Label spu9_channel;
        private System.Windows.Forms.Label spu9_on;
        private System.Windows.Forms.Label r800_frequency;
        private System.Windows.Forms.Label r800_am;
        private System.Windows.Forms.Label r800_on;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label cmsc_uwsymbols;
        private System.Windows.Forms.Label cmsc_uwthreats;
        private System.Windows.Forms.Label cmsc_mws;
        private System.Windows.Forms.Label cmsc_jammer;
        private System.Windows.Forms.Label cmsc_flare;
        private System.Windows.Forms.Label cmsc_chaff;
        private System.Windows.Forms.Label cmsp_down1;
        private System.Windows.Forms.Label cmsp_down2;
        private System.Windows.Forms.Label cmsp_down3;
        private System.Windows.Forms.Label cmsp_down4;
        private System.Windows.Forms.Label cmsp_up1;
        private System.Windows.Forms.Label cmsp_up2;
        private System.Windows.Forms.Label cmsp_up3;
        private System.Windows.Forms.Label cmsp_up4;
        private System.Windows.Forms.Label ils_on;
        private System.Windows.Forms.Label tacan_frequency;
        private System.Windows.Forms.Label tacan_modedial;
        private System.Windows.Forms.Label ils_frequency;
        private System.Windows.Forms.Label uhf_functiondial;
        private System.Windows.Forms.Label uhf_channel;
        private System.Windows.Forms.Label uhf_frequency;
        private System.Windows.Forms.Label uhf_modefrequency;
        private System.Windows.Forms.Label vhf_am_selectfrequency;
        private System.Windows.Forms.Label vhf_am_modefrequency;
        private System.Windows.Forms.Label vhf_am_on;
        private System.Windows.Forms.Label vhf_am_channel;
        private System.Windows.Forms.Label vhf_am_frequency;
        private System.Windows.Forms.Label vhf_fm_channel;
        private System.Windows.Forms.Label vhf_fm_frequency;
        private System.Windows.Forms.Label vhf_fm_selectfrequency;
        private System.Windows.Forms.Label vhf_fm_modefrequency;
        private System.Windows.Forms.Label vhf_fm_on;
        private System.Windows.Forms.Label ekran_queue;
        private System.Windows.Forms.Label ekran_memory;
        private System.Windows.Forms.Label ekran_failure;
    }
}

