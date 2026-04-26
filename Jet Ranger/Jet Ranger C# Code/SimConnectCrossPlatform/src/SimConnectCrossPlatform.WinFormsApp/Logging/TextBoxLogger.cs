// File: src/SimConnectCrossPlatform.WinFormsApp/Logging/TextBoxLogger.cs

using System;
using System.Windows.Forms;
using Microsoft.Extensions.Logging;

namespace SimConnectCrossPlatform.WinFormsApp.Logging
{
    /// <summary>
    /// ILogger implementation that writes to a WinForms TextBox.
    /// Thread-safe via Control.Invoke.
    /// </summary>
    public class TextBoxLogger : ILogger
    {
        private readonly TextBox _textBox;
        private readonly string _categoryName;
        private readonly LogLevel _minLevel;
        private readonly object _lock = new();
        private const int MaxLines = 500;

        public TextBoxLogger(TextBox textBox, string categoryName,
            LogLevel minLevel = LogLevel.Debug)
        {
            _textBox = textBox
                ?? throw new ArgumentNullException(nameof(textBox));
            _categoryName = categoryName;
            _minLevel = minLevel;
        }

        public IDisposable? BeginScope<TState>(TState state)
            where TState : notnull => null;

        public bool IsEnabled(LogLevel logLevel) =>
            logLevel >= _minLevel;

        public void Log<TState>(
            LogLevel logLevel,
            EventId eventId,
            TState state,
            Exception? exception,
            Func<TState, Exception?, string> formatter)
        {
            if (!IsEnabled(logLevel)) return;

            var message = formatter(state, exception);
            if (string.IsNullOrEmpty(message)) return;

            var timestamp = DateTime.Now.ToString("HH:mm:ss.fff");
            var level = GetLevelString(logLevel);
            var line = $"[{timestamp}] [{level}] {message}";

            if (exception != null)
            {
                line += $"\n   Exception: {exception.Message}";
            }

            AppendLine(line);
        }

        private void AppendLine(string text)
        {
            if (_textBox.IsDisposed) return;

            try
            {
                if (_textBox.InvokeRequired)
                {
                    _textBox.BeginInvoke(new Action(() =>
                        AppendLineInternal(text)));
                }
                else
                {
                    AppendLineInternal(text);
                }
            }
            catch (ObjectDisposedException)
            {
                // Form was closed
            }
            catch (InvalidOperationException)
            {
                // Handle closed during invoke
            }
        }

        private void AppendLineInternal(string text)
        {
            lock (_lock)
            {
                try
                {
                    if (_textBox.IsDisposed) return;

                    // Trim old lines if too many
                    if (_textBox.Lines.Length > MaxLines)
                    {
                        var lines = _textBox.Lines;
                        var newLines = new string[MaxLines / 2];
                        Array.Copy(lines,
                            lines.Length - newLines.Length,
                            newLines, 0, newLines.Length);
                        _textBox.Lines = newLines;
                    }

                    _textBox.AppendText(text + Environment.NewLine);

                    // Auto-scroll to bottom
                    _textBox.SelectionStart = _textBox.Text.Length;
                    _textBox.ScrollToCaret();
                }
                catch (ObjectDisposedException) { }
            }
        }

        private static string GetLevelString(LogLevel level) =>
            level switch
            {
                LogLevel.Trace => "TRC",
                LogLevel.Debug => "DBG",
                LogLevel.Information => "INF",
                LogLevel.Warning => "WRN",
                LogLevel.Error => "ERR",
                LogLevel.Critical => "CRT",
                _ => "???"
            };
    }

    /// <summary>
    /// ILoggerProvider that creates TextBoxLogger instances
    /// </summary>
    public class TextBoxLoggerProvider : ILoggerProvider
    {
        private readonly TextBox _textBox;
        private readonly LogLevel _minLevel;

        public TextBoxLoggerProvider(TextBox textBox,
            LogLevel minLevel = LogLevel.Debug)
        {
            _textBox = textBox;
            _minLevel = minLevel;
        }

        public ILogger CreateLogger(string categoryName) =>
            new TextBoxLogger(_textBox, categoryName, _minLevel);

        public void Dispose() { }
    }
}