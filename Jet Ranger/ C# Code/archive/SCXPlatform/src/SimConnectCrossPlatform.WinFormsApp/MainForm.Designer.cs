// File: src/SimConnectCrossPlatform.WinFormsApp/MainForm.Designer.cs

using System.Drawing;
using System.Windows.Forms;

namespace SimConnectCrossPlatform.WinFormsApp
{
    partial class MainForm
    {
        private System.ComponentModel.IContainer components = null;

        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();

            // =====================================================
            // Main Layout
            // =====================================================
            this.tableLayoutMain = new TableLayoutPanel();
            this.panelHeader = new Panel();
            this.panelFlightData = new Panel();
            this.panelControls = new Panel();
            this.panelLog = new Panel();

            // =====================================================
            // Header Panel
            // =====================================================
            this.lblTitle = new Label();
            this.lblConnectionStatus = new Label();
            this.lblPlatform = new Label();

            // =====================================================
            // Flight Data Controls
            // =====================================================
            this.grpFlightData = new GroupBox();
            this.tableFlightData = new TableLayoutPanel();

            // Altitude
            this.lblAltitudeLabel = new Label();
            this.txtAltitude = new TextBox();
            this.lblAltitudeUnits = new Label();

            // Airspeed
            this.lblAirspeedLabel = new Label();
            this.txtAirspeed = new TextBox();
            this.lblAirspeedUnits = new Label();

            // Heading
            this.lblHeadingLabel = new Label();
            this.txtHeading = new TextBox();
            this.lblHeadingUnits = new Label();

            // Vertical Speed
            this.lblVSLabel = new Label();
            this.txtVerticalSpeed = new TextBox();
            this.lblVSUnits = new Label();

            // Ground Speed
            this.lblGSLabel = new Label();
            this.txtGroundSpeed = new TextBox();
            this.lblGSUnits = new Label();

            // Latitude
            this.lblLatLabel = new Label();
            this.txtLatitude = new TextBox();
            this.lblLatUnits = new Label();

            // Longitude
            this.lblLonLabel = new Label();
            this.txtLongitude = new TextBox();
            this.lblLonUnits = new Label();

            // =====================================================
            // Engine Data
            // =====================================================
            this.grpEngineData = new GroupBox();
            this.tableEngineData = new TableLayoutPanel();

            this.lblRPMLabel = new Label();
            this.txtRPM = new TextBox();
            this.lblRPMUnits = new Label();

            this.lblThrottleLabel = new Label();
            this.txtThrottle = new TextBox();
            this.lblThrottleUnits = new Label();

            this.lblFuelFlowLabel = new Label();
            this.txtFuelFlow = new TextBox();
            this.lblFuelFlowUnits = new Label();

            this.lblFuelLabel = new Label();
            this.txtFuel = new TextBox();
            this.lblFuelUnits = new Label();

            // =====================================================
            // Autopilot Data
            // =====================================================
            this.grpAutopilot = new GroupBox();
            this.tableAutopilot = new TableLayoutPanel();

            this.lblAPMasterLabel = new Label();
            this.txtAPMaster = new TextBox();

            this.lblAPHdgLabel = new Label();
            this.txtAPHeading = new TextBox();
            this.lblAPHdgUnits = new Label();

            this.lblAPAltLabel = new Label();
            this.txtAPAltitude = new TextBox();
            this.lblAPAltUnits = new Label();

            this.lblAPSpdLabel = new Label();
            this.txtAPSpeed = new TextBox();
            this.lblAPSpdUnits = new Label();

            // =====================================================
            // Control Buttons
            // =====================================================
            this.grpControls = new GroupBox();
            this.flowControls = new FlowLayoutPanel();
            this.btnConnect = new Button();
            this.btnDisconnect = new Button();
            this.btnParkBrake = new Button();
            this.btnGear = new Button();
            this.btnLandingLights = new Button();
            this.btnNavLights = new Button();
            this.btnBeacon = new Button();
            this.btnStrobes = new Button();

            // =====================================================
            // Log Panel
            // =====================================================
            this.grpLog = new GroupBox();
            this.txtLog = new TextBox();
            this.btnClearLog = new Button();

            // =====================================================
            // Status Bar
            // =====================================================
            this.statusStrip = new StatusStrip();
            this.statusLabel = new ToolStripStatusLabel();
            this.statusRetryCount = new ToolStripStatusLabel();
            this.statusPlatform = new ToolStripStatusLabel();

            this.SuspendLayout();
            this.tableLayoutMain.SuspendLayout();

            // =====================================================
            // Configure Main Table Layout
            // =====================================================
            this.tableLayoutMain.Dock = DockStyle.Fill;
            this.tableLayoutMain.ColumnCount = 2;
            this.tableLayoutMain.RowCount = 3;
            this.tableLayoutMain.ColumnStyles.Add(
                new ColumnStyle(SizeType.Percent, 55F));
            this.tableLayoutMain.ColumnStyles.Add(
                new ColumnStyle(SizeType.Percent, 45F));
            this.tableLayoutMain.RowStyles.Add(
                new RowStyle(SizeType.Absolute, 70F));   // Header
            this.tableLayoutMain.RowStyles.Add(
                new RowStyle(SizeType.Percent, 55F));    // Data
            this.tableLayoutMain.RowStyles.Add(
                new RowStyle(SizeType.Percent, 45F));    // Log
            this.tableLayoutMain.Padding = new Padding(8);
            this.tableLayoutMain.CellBorderStyle =
                TableLayoutPanelCellBorderStyle.None;

            // =====================================================
            // Header
            // =====================================================
            this.panelHeader.Dock = DockStyle.Fill;
            this.panelHeader.BackColor = Color.FromArgb(30, 30, 45);
            this.panelHeader.Padding = new Padding(12, 8, 12, 8);
            this.tableLayoutMain.SetColumnSpan(this.panelHeader, 2);

            this.lblTitle.Text = "✈  Cross-Platform SimConnect";
            this.lblTitle.Font = new Font("Segoe UI", 16F,
                FontStyle.Bold);
            this.lblTitle.ForeColor = Color.White;
            this.lblTitle.AutoSize = true;
            this.lblTitle.Location = new Point(12, 6);

            this.lblConnectionStatus.Text = "● DISCONNECTED";
            this.lblConnectionStatus.Font = new Font("Segoe UI", 10F,
                FontStyle.Bold);
            this.lblConnectionStatus.ForeColor = Color.FromArgb(
                255, 80, 80);
            this.lblConnectionStatus.AutoSize = true;
            this.lblConnectionStatus.Location = new Point(12, 40);

            this.lblPlatform.Text = "";
            this.lblPlatform.Font = new Font("Segoe UI", 9F);
            this.lblPlatform.ForeColor = Color.LightGray;
            this.lblPlatform.AutoSize = true;
            this.lblPlatform.Anchor = AnchorStyles.Right |
                AnchorStyles.Top;
            this.lblPlatform.Location = new Point(500, 44);

            this.panelHeader.Controls.Add(this.lblTitle);
            this.panelHeader.Controls.Add(this.lblConnectionStatus);
            this.panelHeader.Controls.Add(this.lblPlatform);

            // =====================================================
            // Flight Data Group
            // =====================================================
            var dataFont = new Font("Consolas", 14F, FontStyle.Bold);
            var labelFont = new Font("Segoe UI", 9.5F);
            var unitFont = new Font("Segoe UI", 8.5F);
            var readOnlyBackColor = Color.FromArgb(20, 20, 35);
            var readOnlyForeColor = Color.FromArgb(0, 255, 128);
            var groupForeColor = Color.FromArgb(100, 180, 255);

            // Helper to configure data textboxes
            void ConfigDataTextBox(TextBox tb, string defaultText = "---")
            {
                tb.ReadOnly = true;
                tb.BackColor = readOnlyBackColor;
                tb.ForeColor = readOnlyForeColor;
                tb.Font = dataFont;
                tb.TextAlign = HorizontalAlignment.Right;
                tb.BorderStyle = BorderStyle.FixedSingle;
                tb.Text = defaultText;
                tb.Dock = DockStyle.Fill;
                tb.Margin = new Padding(2);
            }

            void ConfigLabel(Label lbl, string text)
            {
                lbl.Text = text;
                lbl.Font = labelFont;
                lbl.ForeColor = Color.LightGray;
                lbl.TextAlign = ContentAlignment.MiddleRight;
                lbl.Dock = DockStyle.Fill;
                lbl.Margin = new Padding(2);
            }

            void ConfigUnitLabel(Label lbl, string text)
            {
                lbl.Text = text;
                lbl.Font = unitFont;
                lbl.ForeColor = Color.Gray;
                lbl.TextAlign = ContentAlignment.MiddleLeft;
                lbl.Dock = DockStyle.Fill;
                lbl.Margin = new Padding(4, 2, 2, 2);
            }

            // --- Flight Data Table ---
            this.grpFlightData.Text = "  ✈  Flight Data  ";
            this.grpFlightData.Font = new Font("Segoe UI", 10F,
                FontStyle.Bold);
            this.grpFlightData.ForeColor = groupForeColor;
            this.grpFlightData.Dock = DockStyle.Fill;
            this.grpFlightData.Padding = new Padding(8);

            this.tableFlightData.Dock = DockStyle.Fill;
            this.tableFlightData.ColumnCount = 3;
            this.tableFlightData.ColumnStyles.Add(
                new ColumnStyle(SizeType.Absolute, 100F));
            this.tableFlightData.ColumnStyles.Add(
                new ColumnStyle(SizeType.Percent, 100F));
            this.tableFlightData.ColumnStyles.Add(
                new ColumnStyle(SizeType.Absolute, 55F));
            this.tableFlightData.RowCount = 7;
            for (int i = 0; i < 7; i++)
                this.tableFlightData.RowStyles.Add(
                    new RowStyle(SizeType.Percent, 14.28F));

            ConfigLabel(lblAltitudeLabel, "Altitude");
            ConfigDataTextBox(txtAltitude);
            ConfigUnitLabel(lblAltitudeUnits, "ft");

            ConfigLabel(lblAirspeedLabel, "Airspeed");
            ConfigDataTextBox(txtAirspeed);
            ConfigUnitLabel(lblAirspeedUnits, "kts");

            ConfigLabel(lblHeadingLabel, "Heading");
            ConfigDataTextBox(txtHeading);
            ConfigUnitLabel(lblHeadingUnits, "°");

            ConfigLabel(lblVSLabel, "V/S");
            ConfigDataTextBox(txtVerticalSpeed);
            ConfigUnitLabel(lblVSUnits, "fpm");

            ConfigLabel(lblGSLabel, "GS");
            ConfigDataTextBox(txtGroundSpeed);
            ConfigUnitLabel(lblGSUnits, "kts");

            ConfigLabel(lblLatLabel, "Lat");
            ConfigDataTextBox(txtLatitude);
            ConfigUnitLabel(lblLatUnits, "°");

            ConfigLabel(lblLonLabel, "Lon");
            ConfigDataTextBox(txtLongitude);
            ConfigUnitLabel(lblLonUnits, "°");

            // Add flight data rows
            int row = 0;
            this.tableFlightData.Controls.Add(lblAltitudeLabel, 0, row);
            this.tableFlightData.Controls.Add(txtAltitude, 1, row);
            this.tableFlightData.Controls.Add(lblAltitudeUnits, 2, row);
            row++;
            this.tableFlightData.Controls.Add(lblAirspeedLabel, 0, row);
            this.tableFlightData.Controls.Add(txtAirspeed, 1, row);
            this.tableFlightData.Controls.Add(lblAirspeedUnits, 2, row);
            row++;
            this.tableFlightData.Controls.Add(lblHeadingLabel, 0, row);
            this.tableFlightData.Controls.Add(txtHeading, 1, row);
            this.tableFlightData.Controls.Add(lblHeadingUnits, 2, row);
            row++;
            this.tableFlightData.Controls.Add(lblVSLabel, 0, row);
            this.tableFlightData.Controls.Add(txtVerticalSpeed, 1, row);
            this.tableFlightData.Controls.Add(lblVSUnits, 2, row);
            row++;
            this.tableFlightData.Controls.Add(lblGSLabel, 0, row);
            this.tableFlightData.Controls.Add(txtGroundSpeed, 1, row);
            this.tableFlightData.Controls.Add(lblGSUnits, 2, row);
            row++;
            this.tableFlightData.Controls.Add(lblLatLabel, 0, row);
            this.tableFlightData.Controls.Add(txtLatitude, 1, row);
            this.tableFlightData.Controls.Add(lblLatUnits, 2, row);
            row++;
            this.tableFlightData.Controls.Add(lblLonLabel, 0, row);
            this.tableFlightData.Controls.Add(txtLongitude, 1, row);
            this.tableFlightData.Controls.Add(lblLonUnits, 2, row);

            this.grpFlightData.Controls.Add(this.tableFlightData);

            // =====================================================
            // Right-side panel: Engine + Autopilot + Controls
            // =====================================================
            this.panelControls.Dock = DockStyle.Fill;
            var rightLayout = new TableLayoutPanel();
            rightLayout.Dock = DockStyle.Fill;
            rightLayout.ColumnCount = 1;
            rightLayout.RowCount = 3;
            rightLayout.RowStyles.Add(
                new RowStyle(SizeType.Percent, 40F));
            rightLayout.RowStyles.Add(
                new RowStyle(SizeType.Percent, 35F));
            rightLayout.RowStyles.Add(
                new RowStyle(SizeType.Percent, 25F));

            // --- Engine Data ---
            this.grpEngineData.Text = "  ⚙  Engine  ";
            this.grpEngineData.Font = new Font("Segoe UI", 10F,
                FontStyle.Bold);
            this.grpEngineData.ForeColor = groupForeColor;
            this.grpEngineData.Dock = DockStyle.Fill;
            this.grpEngineData.Padding = new Padding(8);

            this.tableEngineData.Dock = DockStyle.Fill;
            this.tableEngineData.ColumnCount = 3;
            this.tableEngineData.ColumnStyles.Add(
                new ColumnStyle(SizeType.Absolute, 90F));
            this.tableEngineData.ColumnStyles.Add(
                new ColumnStyle(SizeType.Percent, 100F));
            this.tableEngineData.ColumnStyles.Add(
                new ColumnStyle(SizeType.Absolute, 50F));
            this.tableEngineData.RowCount = 4;
            for (int i = 0; i < 4; i++)
                this.tableEngineData.RowStyles.Add(
                    new RowStyle(SizeType.Percent, 25F));

            var engDataFont = new Font("Consolas", 12F, FontStyle.Bold);

            void ConfigEngTextBox(TextBox tb)
            {
                tb.ReadOnly = true;
                tb.BackColor = readOnlyBackColor;
                tb.ForeColor = Color.FromArgb(255, 200, 50);
                tb.Font = engDataFont;
                tb.TextAlign = HorizontalAlignment.Right;
                tb.BorderStyle = BorderStyle.FixedSingle;
                tb.Text = "---";
                tb.Dock = DockStyle.Fill;
                tb.Margin = new Padding(2);
            }

            ConfigLabel(lblRPMLabel, "RPM");
            ConfigEngTextBox(txtRPM);
            ConfigUnitLabel(lblRPMUnits, "rpm");

            ConfigLabel(lblThrottleLabel, "Throttle");
            ConfigEngTextBox(txtThrottle);
            ConfigUnitLabel(lblThrottleUnits, "%");

            ConfigLabel(lblFuelFlowLabel, "Fuel Flow");
            ConfigEngTextBox(txtFuelFlow);
            ConfigUnitLabel(lblFuelFlowUnits, "gph");

            ConfigLabel(lblFuelLabel, "Fuel");
            ConfigEngTextBox(txtFuel);
            ConfigUnitLabel(lblFuelUnits, "lbs");

            row = 0;
            this.tableEngineData.Controls.Add(lblRPMLabel, 0, row);
            this.tableEngineData.Controls.Add(txtRPM, 1, row);
            this.tableEngineData.Controls.Add(lblRPMUnits, 2, row);
            row++;
            this.tableEngineData.Controls.Add(lblThrottleLabel, 0, row);
            this.tableEngineData.Controls.Add(txtThrottle, 1, row);
            this.tableEngineData.Controls.Add(lblThrottleUnits, 2, row);
            row++;
            this.tableEngineData.Controls.Add(lblFuelFlowLabel, 0, row);
            this.tableEngineData.Controls.Add(txtFuelFlow, 1, row);
            this.tableEngineData.Controls.Add(
                lblFuelFlowUnits, 2, row);
            row++;
            this.tableEngineData.Controls.Add(lblFuelLabel, 0, row);
            this.tableEngineData.Controls.Add(txtFuel, 1, row);
            this.tableEngineData.Controls.Add(lblFuelUnits, 2, row);

            this.grpEngineData.Controls.Add(this.tableEngineData);

            // --- Autopilot ---
            this.grpAutopilot.Text = "  🎯  Autopilot  ";
            this.grpAutopilot.Font = new Font("Segoe UI", 10F,
                FontStyle.Bold);
            this.grpAutopilot.ForeColor = groupForeColor;
            this.grpAutopilot.Dock = DockStyle.Fill;
            this.grpAutopilot.Padding = new Padding(8);

            this.tableAutopilot.Dock = DockStyle.Fill;
            this.tableAutopilot.ColumnCount = 3;
            this.tableAutopilot.ColumnStyles.Add(
                new ColumnStyle(SizeType.Absolute, 90F));
            this.tableAutopilot.ColumnStyles.Add(
                new ColumnStyle(SizeType.Percent, 100F));
            this.tableAutopilot.ColumnStyles.Add(
                new ColumnStyle(SizeType.Absolute, 50F));
            this.tableAutopilot.RowCount = 4;
            for (int i = 0; i < 4; i++)
                this.tableAutopilot.RowStyles.Add(
                    new RowStyle(SizeType.Percent, 25F));

            var apDataFont = new Font("Consolas", 12F, FontStyle.Bold);

            void ConfigAPTextBox(TextBox tb)
            {
                tb.ReadOnly = true;
                tb.BackColor = readOnlyBackColor;
                tb.ForeColor = Color.FromArgb(150, 200, 255);
                tb.Font = apDataFont;
                tb.TextAlign = HorizontalAlignment.Right;
                tb.BorderStyle = BorderStyle.FixedSingle;
                tb.Text = "---";
                tb.Dock = DockStyle.Fill;
                tb.Margin = new Padding(2);
            }

            ConfigLabel(lblAPMasterLabel, "AP");
            ConfigAPTextBox(txtAPMaster);
            var lblAPMasterUnits = new Label();
            ConfigUnitLabel(lblAPMasterUnits, "");

            ConfigLabel(lblAPHdgLabel, "HDG");
            ConfigAPTextBox(txtAPHeading);
            ConfigUnitLabel(lblAPHdgUnits, "°");

            ConfigLabel(lblAPAltLabel, "ALT");
            ConfigAPTextBox(txtAPAltitude);
            ConfigUnitLabel(lblAPAltUnits, "ft");

            ConfigLabel(lblAPSpdLabel, "SPD");
            ConfigAPTextBox(txtAPSpeed);
            ConfigUnitLabel(lblAPSpdUnits, "kts");

            row = 0;
            this.tableAutopilot.Controls.Add(lblAPMasterLabel, 0, row);
            this.tableAutopilot.Controls.Add(txtAPMaster, 1, row);
            this.tableAutopilot.Controls.Add(lblAPMasterUnits, 2, row);
            row++;
            this.tableAutopilot.Controls.Add(lblAPHdgLabel, 0, row);
            this.tableAutopilot.Controls.Add(txtAPHeading, 1, row);
            this.tableAutopilot.Controls.Add(lblAPHdgUnits, 2, row);
            row++;
            this.tableAutopilot.Controls.Add(lblAPAltLabel, 0, row);
            this.tableAutopilot.Controls.Add(txtAPAltitude, 1, row);
            this.tableAutopilot.Controls.Add(lblAPAltUnits, 2, row);
            row++;
            this.tableAutopilot.Controls.Add(lblAPSpdLabel, 0, row);
            this.tableAutopilot.Controls.Add(txtAPSpeed, 1, row);
            this.tableAutopilot.Controls.Add(lblAPSpdUnits, 2, row);

            this.grpAutopilot.Controls.Add(this.tableAutopilot);

            // --- Controls ---
            this.grpControls.Text = "  🎮  Controls  ";
            this.grpControls.Font = new Font("Segoe UI", 10F,
                FontStyle.Bold);
            this.grpControls.ForeColor = groupForeColor;
            this.grpControls.Dock = DockStyle.Fill;
            this.grpControls.Padding = new Padding(8);

            this.flowControls.Dock = DockStyle.Fill;
            this.flowControls.FlowDirection =
                FlowDirection.LeftToRight;
            this.flowControls.WrapContents = true;
            this.flowControls.AutoScroll = true;
            this.flowControls.Padding = new Padding(4);

            var btnFont = new Font("Segoe UI", 9F);
            var btnSize = new Size(105, 36);
            var btnBackColor = Color.FromArgb(45, 45, 65);
            var btnForeColor = Color.White;

            void ConfigButton(Button btn, string text, string emoji)
            {
                btn.Text = $"{emoji} {text}";
                btn.Font = btnFont;
                btn.Size = btnSize;
                btn.FlatStyle = FlatStyle.Flat;
                btn.FlatAppearance.BorderColor =
                    Color.FromArgb(80, 80, 120);
                btn.FlatAppearance.BorderSize = 1;
                btn.BackColor = btnBackColor;
                btn.ForeColor = btnForeColor;
                btn.Cursor = Cursors.Hand;
                btn.Margin = new Padding(3);
            }

            ConfigButton(btnConnect, "Connect", "🔌");
            btnConnect.BackColor = Color.FromArgb(30, 100, 30);
            btnConnect.Size = new Size(115, 36);

            ConfigButton(btnDisconnect, "Disconnect", "⏹");
            btnDisconnect.BackColor = Color.FromArgb(120, 30, 30);
            btnDisconnect.Enabled = false;
            btnDisconnect.Size = new Size(115, 36);

            ConfigButton(btnParkBrake, "Park Brake", "🅿");
            ConfigButton(btnGear, "Gear", "⚙");
            ConfigButton(btnLandingLights, "Land Lts", "💡");
            ConfigButton(btnNavLights, "Nav Lts", "🔦");
            ConfigButton(btnBeacon, "Beacon", "🔴");
            ConfigButton(btnStrobes, "Strobes", "⚡");

            this.flowControls.Controls.Add(btnConnect);
            this.flowControls.Controls.Add(btnDisconnect);
            this.flowControls.Controls.Add(btnParkBrake);
            this.flowControls.Controls.Add(btnGear);
            this.flowControls.Controls.Add(btnLandingLights);
            this.flowControls.Controls.Add(btnNavLights);
            this.flowControls.Controls.Add(btnBeacon);
            this.flowControls.Controls.Add(btnStrobes);

            this.grpControls.Controls.Add(this.flowControls);

            // Add to right panel
            rightLayout.Controls.Add(this.grpEngineData, 0, 0);
            rightLayout.Controls.Add(this.grpAutopilot, 0, 1);
            rightLayout.Controls.Add(this.grpControls, 0, 2);
            this.panelControls.Controls.Add(rightLayout);

            // =====================================================
            // Log Panel
            // =====================================================
            this.grpLog.Text = "  📋  Log  ";
            this.grpLog.Font = new Font("Segoe UI", 10F,
                FontStyle.Bold);
            this.grpLog.ForeColor = groupForeColor;
            this.grpLog.Dock = DockStyle.Fill;
            this.grpLog.Padding = new Padding(8);

            this.txtLog.Multiline = true;
            this.txtLog.ReadOnly = true;
            this.txtLog.ScrollBars = ScrollBars.Vertical;
            this.txtLog.BackColor = Color.FromArgb(15, 15, 25);
            this.txtLog.ForeColor = Color.FromArgb(180, 180, 180);
            this.txtLog.Font = new Font("Consolas", 8.5F);
            this.txtLog.BorderStyle = BorderStyle.FixedSingle;
            this.txtLog.Dock = DockStyle.Fill;
            this.txtLog.WordWrap = false;

            this.btnClearLog.Text = "Clear";
            this.btnClearLog.Font = new Font("Segoe UI", 8F);
            this.btnClearLog.Size = new Size(60, 24);
            this.btnClearLog.FlatStyle = FlatStyle.Flat;
            this.btnClearLog.FlatAppearance.BorderColor =
                Color.FromArgb(80, 80, 80);
            this.btnClearLog.BackColor = Color.FromArgb(50, 50, 50);
            this.btnClearLog.ForeColor = Color.LightGray;
            this.btnClearLog.Dock = DockStyle.Bottom;

            var logPanel = new Panel();
            logPanel.Dock = DockStyle.Fill;
            logPanel.Controls.Add(this.txtLog);
            logPanel.Controls.Add(this.btnClearLog);

            this.grpLog.Controls.Add(logPanel);

            // =====================================================
            // Status Bar
            // =====================================================
            this.statusStrip.BackColor = Color.FromArgb(25, 25, 40);
            this.statusStrip.SizingGrip = true;

            this.statusLabel.Text = "Disconnected";
            this.statusLabel.ForeColor = Color.LightGray;
            this.statusLabel.Spring = true;
            this.statusLabel.TextAlign =
                ContentAlignment.MiddleLeft;

            this.statusRetryCount.Text = "Retries: 0";
            this.statusRetryCount.ForeColor = Color.Gray;
            this.statusRetryCount.BorderSides =
                ToolStripStatusLabelBorderSides.Left;
            this.statusRetryCount.BorderStyle =
                Border3DStyle.Etched;

            this.statusPlatform.Text = "";
            this.statusPlatform.ForeColor = Color.LightBlue;
            this.statusPlatform.BorderSides =
                ToolStripStatusLabelBorderSides.Left;
            this.statusPlatform.BorderStyle =
                Border3DStyle.Etched;

            this.statusStrip.Items.Add(this.statusLabel);
            this.statusStrip.Items.Add(this.statusRetryCount);
            this.statusStrip.Items.Add(this.statusPlatform);

            // =====================================================
            // Assemble Main Layout
            // =====================================================
            this.tableLayoutMain.Controls.Add(
                this.panelHeader, 0, 0);
            this.tableLayoutMain.Controls.Add(
                this.grpFlightData, 0, 1);
            this.tableLayoutMain.Controls.Add(
                this.panelControls, 1, 1);
            this.tableLayoutMain.Controls.Add(
                this.grpLog, 0, 2);
            this.tableLayoutMain.SetColumnSpan(this.grpLog, 2);

            // =====================================================
            // Form
            // =====================================================
            this.AutoScaleDimensions = new SizeF(7F, 15F);
            this.AutoScaleMode = AutoScaleMode.Font;
            this.BackColor = Color.FromArgb(25, 25, 40);
            this.ClientSize = new Size(950, 750);
            this.Controls.Add(this.tableLayoutMain);
            this.Controls.Add(this.statusStrip);
            this.MinimumSize = new Size(800, 600);
            this.Name = "MainForm";
            this.Text =
                "Cross-Platform SimConnect — MSFS2024 / Prepar3D";
            this.StartPosition = FormStartPosition.CenterScreen;
            this.Font = new Font("Segoe UI", 9F);

            this.tableLayoutMain.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();
        }

        // =====================================================
        // Control Declarations
        // =====================================================
        private TableLayoutPanel tableLayoutMain;
        private Panel panelHeader;
        private Panel panelFlightData;
        private Panel panelControls;
        private Panel panelLog;

        // Header
        private Label lblTitle;
        private Label lblConnectionStatus;
        private Label lblPlatform;

        // Flight Data
        private GroupBox grpFlightData;
        private TableLayoutPanel tableFlightData;
        private Label lblAltitudeLabel;
        private TextBox txtAltitude;
        private Label lblAltitudeUnits;
        private Label lblAirspeedLabel;
        private TextBox txtAirspeed;
        private Label lblAirspeedUnits;
        private Label lblHeadingLabel;
        private TextBox txtHeading;
        private Label lblHeadingUnits;
        private Label lblVSLabel;
        private TextBox txtVerticalSpeed;
        private Label lblVSUnits;
        private Label lblGSLabel;
        private TextBox txtGroundSpeed;
        private Label lblGSUnits;
        private Label lblLatLabel;
        private TextBox txtLatitude;
        private Label lblLatUnits;
        private Label lblLonLabel;
        private TextBox txtLongitude;
        private Label lblLonUnits;

        // Engine Data
        private GroupBox grpEngineData;
        private TableLayoutPanel tableEngineData;
        private Label lblRPMLabel;
        private TextBox txtRPM;
        private Label lblRPMUnits;
        private Label lblThrottleLabel;
        private TextBox txtThrottle;
        private Label lblThrottleUnits;
        private Label lblFuelFlowLabel;
        private TextBox txtFuelFlow;
        private Label lblFuelFlowUnits;
        private Label lblFuelLabel;
        private TextBox txtFuel;
        private Label lblFuelUnits;

        // Autopilot
        private GroupBox grpAutopilot;
        private TableLayoutPanel tableAutopilot;
        private Label lblAPMasterLabel;
        private TextBox txtAPMaster;
        private Label lblAPHdgLabel;
        private TextBox txtAPHeading;
        private Label lblAPHdgUnits;
        private Label lblAPAltLabel;
        private TextBox txtAPAltitude;
        private Label lblAPAltUnits;
        private Label lblAPSpdLabel;
        private TextBox txtAPSpeed;
        private Label lblAPSpdUnits;

        // Controls
        private GroupBox grpControls;
        private FlowLayoutPanel flowControls;
        private Button btnConnect;
        private Button btnDisconnect;
        private Button btnParkBrake;
        private Button btnGear;
        private Button btnLandingLights;
        private Button btnNavLights;
        private Button btnBeacon;
        private Button btnStrobes;

        // Log
        private GroupBox grpLog;
        private TextBox txtLog;
        private Button btnClearLog;

        // Status Bar
        private StatusStrip statusStrip;
        private ToolStripStatusLabel statusLabel;
        private ToolStripStatusLabel statusRetryCount;
        private ToolStripStatusLabel statusPlatform;
    }
}