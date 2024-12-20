// Copyright (c) 2010-2018 Lockheed Martin Corporation. All rights reserved.
// Use of this file is bound by the PREPAR3D® SOFTWARE DEVELOPER KIT END USER LICENSE AGREEMENT

namespace Managed_Data_Request
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.buttonConnect = new System.Windows.Forms.Button();
            this.buttonDisconnect = new System.Windows.Forms.Button();
            this.buttonRequestData = new System.Windows.Forms.Button();
            this.richResponse = new System.Windows.Forms.RichTextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.button2 = new System.Windows.Forms.Button();
            this.button3 = new System.Windows.Forms.Button();
            this.btn_Gear_Up = new System.Windows.Forms.Button();
            this.btn_Gear_Down = new System.Windows.Forms.Button();
            this.btn_UnPause = new System.Windows.Forms.Button();
            this.btn_Toggle_Gear = new System.Windows.Forms.Button();
            this.btnSocketTest = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // buttonConnect
            // 
            this.buttonConnect.Location = new System.Drawing.Point(12, 38);
            this.buttonConnect.Name = "buttonConnect";
            this.buttonConnect.Size = new System.Drawing.Size(131, 23);
            this.buttonConnect.TabIndex = 0;
            this.buttonConnect.Text = "Connect to P3D";
            this.buttonConnect.UseVisualStyleBackColor = true;
            this.buttonConnect.Click += new System.EventHandler(this.buttonConnect_Click);
            // 
            // buttonDisconnect
            // 
            this.buttonDisconnect.Location = new System.Drawing.Point(12, 168);
            this.buttonDisconnect.Name = "buttonDisconnect";
            this.buttonDisconnect.Size = new System.Drawing.Size(131, 23);
            this.buttonDisconnect.TabIndex = 1;
            this.buttonDisconnect.Text = "Disconnect from P3D";
            this.buttonDisconnect.UseVisualStyleBackColor = true;
            this.buttonDisconnect.Click += new System.EventHandler(this.buttonDisconnect_Click);
            // 
            // buttonRequestData
            // 
            this.buttonRequestData.Location = new System.Drawing.Point(12, 96);
            this.buttonRequestData.Name = "buttonRequestData";
            this.buttonRequestData.Size = new System.Drawing.Size(131, 41);
            this.buttonRequestData.TabIndex = 2;
            this.buttonRequestData.Text = "Request User Aircraft Data";
            this.buttonRequestData.UseVisualStyleBackColor = true;
            this.buttonRequestData.Click += new System.EventHandler(this.buttonRequestData_Click);
            // 
            // richResponse
            // 
            this.richResponse.Location = new System.Drawing.Point(149, 40);
            this.richResponse.Name = "richResponse";
            this.richResponse.ReadOnly = true;
            this.richResponse.Size = new System.Drawing.Size(221, 151);
            this.richResponse.TabIndex = 3;
            this.richResponse.Text = "";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(149, 21);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(60, 13);
            this.label1.TabIndex = 4;
            this.label1.Text = "Responses";
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(27, 210);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(94, 45);
            this.button2.TabIndex = 6;
            this.button2.Text = "Throttle 50%";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(27, 271);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(94, 42);
            this.button3.TabIndex = 7;
            this.button3.Text = "AP On Alt 5000 ft";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // btn_Gear_Up
            // 
            this.btn_Gear_Up.Location = new System.Drawing.Point(167, 210);
            this.btn_Gear_Up.Name = "btn_Gear_Up";
            this.btn_Gear_Up.Size = new System.Drawing.Size(105, 40);
            this.btn_Gear_Up.TabIndex = 8;
            this.btn_Gear_Up.Text = "Gear UP";
            this.btn_Gear_Up.UseVisualStyleBackColor = true;
            this.btn_Gear_Up.Click += new System.EventHandler(this.btn_Gear_Up_Click);
            // 
            // btn_Gear_Down
            // 
            this.btn_Gear_Down.Location = new System.Drawing.Point(167, 271);
            this.btn_Gear_Down.Name = "btn_Gear_Down";
            this.btn_Gear_Down.Size = new System.Drawing.Size(105, 45);
            this.btn_Gear_Down.TabIndex = 9;
            this.btn_Gear_Down.Text = "Gear Down";
            this.btn_Gear_Down.UseVisualStyleBackColor = true;
            this.btn_Gear_Down.Click += new System.EventHandler(this.btn_Gear_Down_Click);
            // 
            // btn_UnPause
            // 
            this.btn_UnPause.Location = new System.Drawing.Point(27, 331);
            this.btn_UnPause.Name = "btn_UnPause";
            this.btn_UnPause.Size = new System.Drawing.Size(94, 36);
            this.btn_UnPause.TabIndex = 10;
            this.btn_UnPause.Text = "UnPause";
            this.btn_UnPause.UseVisualStyleBackColor = true;
            this.btn_UnPause.Click += new System.EventHandler(this.btn_UnPause_Click);
            // 
            // btn_Toggle_Gear
            // 
            this.btn_Toggle_Gear.Location = new System.Drawing.Point(167, 331);
            this.btn_Toggle_Gear.Name = "btn_Toggle_Gear";
            this.btn_Toggle_Gear.Size = new System.Drawing.Size(105, 42);
            this.btn_Toggle_Gear.TabIndex = 11;
            this.btn_Toggle_Gear.Text = "Toggle Gear";
            this.btn_Toggle_Gear.UseVisualStyleBackColor = true;
            this.btn_Toggle_Gear.Click += new System.EventHandler(this.btn_Toggle_Gear_Click);
            // 
            // btnSocketTest
            // 
            this.btnSocketTest.Location = new System.Drawing.Point(335, 252);
            this.btnSocketTest.Name = "btnSocketTest";
            this.btnSocketTest.Size = new System.Drawing.Size(96, 47);
            this.btnSocketTest.TabIndex = 12;
            this.btnSocketTest.Text = "SocketTest";
            this.btnSocketTest.UseVisualStyleBackColor = true;
            this.btnSocketTest.Click += new System.EventHandler(this.button1_Click_1);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(479, 450);
            this.Controls.Add(this.btnSocketTest);
            this.Controls.Add(this.btn_Toggle_Gear);
            this.Controls.Add(this.btn_UnPause);
            this.Controls.Add(this.btn_Gear_Down);
            this.Controls.Add(this.btn_Gear_Up);
            this.Controls.Add(this.button3);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.richResponse);
            this.Controls.Add(this.buttonRequestData);
            this.Controls.Add(this.buttonDisconnect);
            this.Controls.Add(this.buttonConnect);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "Form1";
            this.Text = "  SimConnect Managed Data Request";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form1_FormClosed);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button buttonConnect;
        private System.Windows.Forms.Button buttonDisconnect;
        private System.Windows.Forms.Button buttonRequestData;
        private System.Windows.Forms.RichTextBox richResponse;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button btn_Gear_Up;
        private System.Windows.Forms.Button btn_Gear_Down;
        private System.Windows.Forms.Button btn_UnPause;
        private System.Windows.Forms.Button btn_Toggle_Gear;
        private System.Windows.Forms.Button btnSocketTest;
    }
}

