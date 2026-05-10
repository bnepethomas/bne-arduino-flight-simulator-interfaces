// src/SimConnectCrossPlatform/Reconnection/ReconnectionConfig.cs

using System;

namespace SimConnectCrossPlatform.Reconnection
{
    /// <summary>
    /// Configuration for automatic reconnection behavior
    /// </summary>
    public class ReconnectionConfig
    {
        /// <summary>
        /// Initial delay before first retry (default: 2s)
        /// </summary>
        public TimeSpan InitialRetryDelay { get; set; }
            = TimeSpan.FromSeconds(2);

        /// <summary>
        /// Maximum delay between retries (default: 30s)
        /// </summary>
        public TimeSpan MaxRetryDelay { get; set; }
            = TimeSpan.FromSeconds(30);

        /// <summary>
        /// Multiplier for exponential backoff (default: 2.0)
        /// </summary>
        public double BackoffMultiplier { get; set; } = 2.0;

        /// <summary>
        /// Enable heartbeat monitoring (default: true)
        /// </summary>
        public bool EnableHeartbeat { get; set; } = true;

        /// <summary>
        /// Time without data before considering connection lost
        /// (default: 15s)
        /// </summary>
        public TimeSpan HeartbeatTimeout { get; set; }
            = TimeSpan.FromSeconds(15);

        /// <summary>
        /// Maximum retry attempts (-1 = infinite, default: -1)
        /// </summary>
        public int MaxRetryAttempts { get; set; } = -1;

        /// <summary>
        /// Automatically reconnect on disconnection (default: true)
        /// </summary>
        public bool AutoReconnect { get; set; } = true;

        /// <summary>
        /// Interval for processing SimConnect messages (default: 50ms)
        /// </summary>
        public TimeSpan ProcessInterval { get; set; }
            = TimeSpan.FromMilliseconds(50);

        /// <summary>
        /// Add random jitter to backoff delays (default: true)
        /// </summary>
        public bool EnableJitter { get; set; } = true;

        /// <summary>
        /// Jitter range as fraction of delay (default: 0.2 = ±20%)
        /// </summary>
        public double JitterFraction { get; set; } = 0.2;

        /// <summary>
        /// Validate configuration values
        /// </summary>
        public void Validate()
        {
            if (InitialRetryDelay <= TimeSpan.Zero)
                throw new ArgumentException(
                    "InitialRetryDelay must be positive");
            if (MaxRetryDelay < InitialRetryDelay)
                throw new ArgumentException(
                    "MaxRetryDelay must be >= InitialRetryDelay");
            if (BackoffMultiplier < 1.0)
                throw new ArgumentException(
                    "BackoffMultiplier must be >= 1.0");
            if (HeartbeatTimeout <= TimeSpan.Zero)
                throw new ArgumentException(
                    "HeartbeatTimeout must be positive");
            if (ProcessInterval <= TimeSpan.Zero)
                throw new ArgumentException(
                    "ProcessInterval must be positive");
        }
    }
}