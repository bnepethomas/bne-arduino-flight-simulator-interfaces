using System.Reflection;

namespace FSUIPCTest
{
    /// <summary>The About window: the program's name, version, build number and the date and time it was built.</summary>
    internal sealed class AboutForm : Form
    {
        /// <summary>A value the project file stamps into the program (BuildNumber, BuildDateTime).</summary>
        public static string Metadata(string key)
            => Assembly.GetExecutingAssembly().GetCustomAttributes<AssemblyMetadataAttribute>().FirstOrDefault(a => a.Key == key)?.Value ?? "unknown";

        /// <summary>"1.0.0 build 3".</summary>
        public static string VersionText()
            => (Assembly.GetExecutingAssembly().GetName().Version?.ToString(3) ?? "?") + " build " + Metadata("BuildNumber");

        public AboutForm(string programName, string description)
        {
            Text = "About " + programName;
            FormBorderStyle = FormBorderStyle.FixedDialog;
            MaximizeBox = false;
            MinimizeBox = false;
            ShowInTaskbar = false;
            StartPosition = FormStartPosition.CenterParent;
            AutoScaleMode = AutoScaleMode.Dpi;
            try { Icon = Icon.ExtractAssociatedIcon(Application.ExecutablePath) ?? Icon; } catch { }   // the bridge icon (bridge.ico, set as the application icon in the project file)
            Font = new Font("Segoe UI", 9f);
            ClientSize = new Size(420, 220);

            var name = new Label { Text = programName, Font = new Font("Segoe UI", 13f, FontStyle.Bold), AutoSize = true, Location = new Point(16, 14) };
            var about = new Label { Text = description, AutoSize = false, Size = new Size(388, 40), Location = new Point(18, 48) };
            var table = new TableLayoutPanel { Location = new Point(18, 96), Size = new Size(388, 76), ColumnCount = 2, RowCount = 3 };
            table.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 110));
            table.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            void Row(int row, string label, string value)
            {
                table.Controls.Add(new Label { Text = label, AutoSize = true, Margin = new Padding(0, 3, 3, 3) }, 0, row);
                table.Controls.Add(new Label { Text = value, AutoSize = true, Font = new Font("Segoe UI", 9f, FontStyle.Bold), Margin = new Padding(0, 3, 3, 3) }, 1, row);
            }
            Row(0, "Version", Assembly.GetExecutingAssembly().GetName().Version?.ToString(3) ?? "?");
            Row(1, "Build number", Metadata("BuildNumber"));
            Row(2, "Built", Metadata("BuildDateTime"));
            var ok = new Button { Text = "OK", DialogResult = DialogResult.OK, Size = new Size(88, 28), Location = new Point(316, 180) };
            AcceptButton = ok;
            CancelButton = ok;
            Controls.AddRange(new Control[] { name, about, table, ok });
        }
    }
}
