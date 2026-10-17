// src/SimConnectCrossPlatform/Reconnection/ReconnectingSimInterface.cs

using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Extensions.Logging;
using SimConnectCrossPlatform.Core;
using SimConnectCrossPlatform.Models;

namespace SimConnectCrossPlatform.Reconnection
{
    /// <summary>
    /// Wraps SimConnectInterface with automatic reconnection,
    /// state persistence across reconnections, and heartbeat monitoring.
    /// </summary>
    public class ReconnectingSimInterface : IDisposable
    {
        private readonly ReconnectionConfig _config;
        private readonly ILogger? _logger;
        private readonly Random _random = new();

        private SimConnectInterface? _simInterface;
        private CancellationTokenSource? _cts;
        private Task? _workerTask;
        private string _appName = string.Empty;
        private bool _disposed;

        // State
        private volatile ConnectionState _connectionState
            = ConnectionState.Disconnected;
        private int _retryCount;
        private DateTime _lastDataReceived = DateTime.UtcNow;
        private volatile bool _dataReceivedFlag;

        // Persisted state
        private readonly ConcurrentDictionary<uint, DataDefinition>
            _persistedDefinitions = new();
        private readonly ConcurrentDictionary<uint, PersistentRequest>
            _persistedRequests = new();

        /// <summary>
        /// A data request that persists across reconnections
        /// </summary>
        public class PersistentRequest
        {
            public uint RequestId { get; set; }
            public uint DefinitionId { get; set; }
            public UpdatePeriod Period { get; set; }
        }

        // ===== Events =====
        public event EventHandler<SimDataReceivedEventArgs>?
            DataReceived;
        public event EventHandler<ConnectionStateChangedEventArgs>?
            ConnectionStateChanged;
        public event EventHandler<ReconnectionAttemptEventArgs>?
            ReconnectionAttempt;
        public event EventHandler<SimConnectExceptionEventArgs>?
            ExceptionReceived;

        // ===== Properties =====
        public ConnectionState ConnectionState => _connectionState;
        public SimPlatform Platform =>
            _simInterface?.Platform ?? SimPlatform.Unknown;
        public string PlatformName =>
            _simInterface?.PlatformName ?? "Unknown (not connected)";
        public int RetryCount => _retryCount;
        public bool IsRunning =>
            _workerTask != null && !_workerTask.IsCompleted;
        public int PersistedDefinitionCount =>
            _persistedDefinitions.Count;
        public int PersistedRequestCount =>
            _persistedRequests.Count;

        // ===== Constructor =====

        public ReconnectingSimInterface(
            ReconnectionConfig? config = null,
            ILogger? logger = null)
        {
            _config = config ?? new ReconnectionConfig();
            _config.Validate();
            _logger = logger;
        }

        // =====================================================
        // Lifecycle
        // =====================================================

        /// <summary>
        /// Start the background connection/processing worker
        /// </summary>
        public void Start(string appName)
        {
            if (IsRunning)
                throw new InvalidOperationException(
                    "Already running. Call StopAsync() first.");

            _appName = appName
                ?? throw new ArgumentNullException(nameof(appName));
            _retryCount = 0;
            _cts = new CancellationTokenSource();

            _logger?.LogInformation(
                "Starting ReconnectingSimInterface for '{App}'",
                appName);

            _workerTask = Task.Run(
                () => WorkerLoopAsync(_cts.Token));
        }

        /// <summary>
        /// Stop the background worker and disconnect
        /// </summary>
        public async Task StopAsync()
        {
            _logger?.LogInformation("Stopping...");

            _cts?.Cancel();

            if (_workerTask != null)
            {
                try
                {
                    await _workerTask.ConfigureAwait(false);
                }
                catch (OperationCanceledException) { }
                catch (Exception ex)
                {
                    _logger?.LogError(
                        "Error stopping worker: {Error}", ex.Message);
                }
            }

            DisconnectInternal();

            _cts?.Dispose();
            _cts = null;
            _workerTask = null;

            SetConnectionState(ConnectionState.Disconnected);
            _logger?.LogInformation("Stopped.");
        }

        // =====================================================
        // Worker Loop
        // =====================================================

        private async Task WorkerLoopAsync(CancellationToken ct)
        {
            _logger?.LogDebug("Worker loop started");

            try
            {
                while (!ct.IsCancellationRequested)
                {
                    switch (_connectionState)
                    {
                        case ConnectionState.Disconnected:
                        case ConnectionState.Error:
                            await HandleDisconnectedStateAsync(ct);
                            break;

                        case ConnectionState.Connected:
                            await HandleConnectedStateAsync(ct);
                            break;

                        default:
                            await Task.Delay(100, ct);
                            break;
                    }
                }
            }
            catch (OperationCanceledException)
            {
                _logger?.LogDebug("Worker loop cancelled");
            }
            catch (Exception ex)
            {
                _logger?.LogError(
                    "Unhandled error in worker loop: {Error}",
                    ex.Message);
            }

            _logger?.LogDebug("Worker loop exited");
        }

        private async Task HandleDisconnectedStateAsync(
            CancellationToken ct)
        {
            // Check auto-reconnect
            if (!_config.AutoReconnect && _retryCount > 0)
            {
                await Task.Delay(500, ct);
                return;
            }

            // Check max retries
            if (_config.MaxRetryAttempts >= 0 &&
                _retryCount >= _config.MaxRetryAttempts)
            {
                _logger?.LogWarning(
                    "Max retry attempts ({Max}) reached",
                    _config.MaxRetryAttempts);
                SetConnectionState(ConnectionState.Error);
                await Task.Delay(1000, ct);
                return;
            }

            // Backoff delay (skip on first attempt)
            if (_retryCount > 0)
            {
                var delay = CalculateBackoff();

                _logger?.LogInformation(
                    "Reconnection attempt #{Attempt} in {Delay}s",
                    _retryCount, delay.TotalSeconds);

                ReconnectionAttempt?.Invoke(this,
                    new ReconnectionAttemptEventArgs(
                        _retryCount, delay));

                // Interruptible wait
                try
                {
                    await Task.Delay(delay, ct);
                }
                catch (OperationCanceledException)
                {
                    return;
                }
            }

            // Try to connect
            if (TryConnect())
            {
                _retryCount = 0;
                _lastDataReceived = DateTime.UtcNow;
                _dataReceivedFlag = false;

                RestoreState();
                SetConnectionState(ConnectionState.Connected);
            }
            else
            {
                _retryCount++;
            }
        }

        private async Task HandleConnectedStateAsync(
            CancellationToken ct)
        {
            try
            {
                _simInterface?.Process();
                UpdateHeartbeat();

                if (_config.EnableHeartbeat && IsHeartbeatExpired())
                {
                    _logger?.LogWarning(
                        "Heartbeat expired ({Timeout}s without data)",
                        _config.HeartbeatTimeout.TotalSeconds);
                    HandleDisconnection();
                    return;
                }

                await Task.Delay(_config.ProcessInterval, ct);
            }
            catch (OperationCanceledException)
            {
                throw; // Let it propagate to exit the loop
            }
            catch (AccessViolationException)
            {
                _logger?.LogError(
                    "Access violation — simulator likely crashed");
                HandleDisconnection();
            }
            catch (Exception ex)
            {
                _logger?.LogError(
                    "Error during processing: {Error}", ex.Message);
                HandleDisconnection();
            }
        }

        // =====================================================
        // Connection Management
        // =====================================================

        private bool TryConnect()
        {
            try
            {
                DisconnectInternal();

                _simInterface = new SimConnectInterface(_logger);

                // Wire data callback with heartbeat
                _simInterface.DataReceived += OnDataReceived;
                _simInterface.ConnectionStateChanged +=
                    OnInternalConnectionStateChanged;
                _simInterface.ExceptionReceived +=
                    OnExceptionReceived;

                var result = _simInterface.Connect(_appName);

                if (result)
                {
                    _logger?.LogInformation(
                        "Connected to {Platform}",
                        _simInterface.PlatformName);
                }
                else
                {
                    _logger?.LogDebug("Connection attempt failed");
                    DisconnectInternal();
                }

                return result;
            }
            catch (Exception ex)
            {
                _logger?.LogError(
                    "Connection attempt threw: {Error}", ex.Message);
                DisconnectInternal();
                return false;
            }
        }

        private void HandleDisconnection()
        {
            _logger?.LogInformation(
                "Handling disconnection — will attempt reconnect");
            DisconnectInternal();
            SetConnectionState(ConnectionState.Disconnected);
        }

        private void DisconnectInternal()
        {
            if (_simInterface != null)
            {
                _simInterface.DataReceived -= OnDataReceived;
                _simInterface.ConnectionStateChanged -=
                    OnInternalConnectionStateChanged;
                _simInterface.ExceptionReceived -=
                    OnExceptionReceived;

                try
                {
                    _simInterface.Disconnect();
                }
                catch (Exception ex)
                {
                    _logger?.LogDebug(
                        "Error during disconnect: {Error}",
                        ex.Message);
                }

                _simInterface.Dispose();
                _simInterface = null;
            }
        }

        // =====================================================
        // State Restoration
        // =====================================================

        private void RestoreState()
        {
            _logger?.LogInformation(
                "Restoring {DefCount} definitions, " +
                "{ReqCount} requests",
                _persistedDefinitions.Count,
                _persistedRequests.Count);

            // Re-register all definitions
            foreach (var def in _persistedDefinitions.Values)
            {
                try
                {
                    if (!_simInterface!.RegisterDataDefinition(def))
                    {
                        _logger?.LogError(
                            "Failed to restore definition {Id}",
                            def.Id);
                    }
                }
                catch (Exception ex)
                {
                    _logger?.LogError(
                        "Error restoring definition {Id}: {Error}",
                        def.Id, ex.Message);
                }
            }

            // Re-request all persistent requests
            foreach (var req in _persistedRequests.Values)
            {
                try
                {
                    if (!_simInterface!.RequestData(
                        req.RequestId, req.DefinitionId, req.Period))
                    {
                        _logger?.LogError(
                            "Failed to restore request {Id}",
                            req.RequestId);
                    }
                }
                catch (Exception ex)
                {
                    _logger?.LogError(
                        "Error restoring request {Id}: {Error}",
                        req.RequestId, ex.Message);
                }
            }
        }

        // =====================================================
        // Heartbeat
        // =====================================================

        private void UpdateHeartbeat()
        {
            if (_dataReceivedFlag)
            {
                _lastDataReceived = DateTime.UtcNow;
                _dataReceivedFlag = false;
            }
        }

        private bool IsHeartbeatExpired()
        {
            if (!_config.EnableHeartbeat) return false;
            return (DateTime.UtcNow - _lastDataReceived)
                > _config.HeartbeatTimeout;
        }

        // =====================================================
        // Backoff
        // =====================================================

        private TimeSpan CalculateBackoff()
        {
            double delayMs =
                _config.InitialRetryDelay.TotalMilliseconds;

            for (int i = 1; i < _retryCount; i++)
            {
                delayMs *= _config.BackoffMultiplier;
            }

            // Cap at max
            delayMs = Math.Min(delayMs,
                _config.MaxRetryDelay.TotalMilliseconds);

            // Optional jitter
            if (_config.EnableJitter)
            {
                double jitter =
                    (_random.NextDouble() - 0.5) * 2.0
                    * _config.JitterFraction;
                delayMs *= (1.0 + jitter);
            }

            // Ensure positive
            return TimeSpan.FromMilliseconds(
                Math.Max(100, delayMs));
        }

        // =====================================================
        // Internal Event Handlers
        // =====================================================

        private void OnDataReceived(object? sender,
            SimDataReceivedEventArgs e)
        {
            _dataReceivedFlag = true;
            DataReceived?.Invoke(this, e);
        }

        private void OnInternalConnectionStateChanged(object? sender,
            ConnectionStateChangedEventArgs e)
        {
            if (e.NewState == ConnectionState.Disconnected)
            {
                _logger?.LogInformation(
                    "Simulator sent disconnect/quit");
                // Don't call HandleDisconnection here —
                // the worker loop will detect it
                _connectionState = ConnectionState.Disconnected;
            }
        }

        private void OnExceptionReceived(object? sender,
            SimConnectExceptionEventArgs e)
        {
            ExceptionReceived?.Invoke(this, e);
        }

        // =====================================================
        // Public API — Persisted Operations
        // =====================================================

        /// <summary>
        /// Register a data definition (persisted across reconnections)
        /// </summary>
        public bool RegisterDataDefinition(DataDefinition definition)
        {
            if (definition == null)
                throw new ArgumentNullException(nameof(definition));

            _persistedDefinitions[definition.Id] = definition;
            _logger?.LogDebug(
                "Persisted definition {Id} ({Count} vars)",
                definition.Id, definition.Variables.Count);

            if (_connectionState == ConnectionState.Connected
                && _simInterface != null)
            {
                return _simInterface.RegisterDataDefinition(definition);
            }

            return true;
        }

        /// <summary>
        /// Unregister a data definition
        /// </summary>
        public bool UnregisterDataDefinition(uint definitionId)
        {
            _persistedDefinitions.TryRemove(definitionId, out _);

            // Remove associated requests
            foreach (var kvp in _persistedRequests)
            {
                if (kvp.Value.DefinitionId == definitionId)
                {
                    _persistedRequests.TryRemove(kvp.Key, out _);
                }
            }

            if (_connectionState == ConnectionState.Connected
                && _simInterface != null)
            {
                return _simInterface
                    .UnregisterDataDefinition(definitionId);
            }

            return true;
        }

        /// <summary>
        /// Request data (periodic requests persisted across reconnections)
        /// </summary>
        public bool RequestData(uint requestId, uint definitionId,
            UpdatePeriod period = UpdatePeriod.Once)
        {
            // Persist periodic requests (not one-shot)
            if (period != UpdatePeriod.Once)
            {
                _persistedRequests[requestId] = new PersistentRequest
                {
                    RequestId = requestId,
                    DefinitionId = definitionId,
                    Period = period
                };
                _logger?.LogDebug(
                    "Persisted request {ReqId} (def={DefId}, " +
                    "period={Period})",
                    requestId, definitionId, period);
            }

            if (_connectionState == ConnectionState.Connected
                && _simInterface != null)
            {
                return _simInterface.RequestData(
                    requestId, definitionId, period);
            }

            return true;
        }

        /// <summary>
        /// Stop a persistent request
        /// </summary>
        public bool StopRequest(uint requestId)
        {
            _persistedRequests.TryRemove(requestId, out _);
            return true;
        }

        /// <summary>
        /// Send a simulator event (only when connected)
        /// </summary>
        public bool SendEvent(string eventName, uint data = 0)
        {
            if (_connectionState != ConnectionState.Connected
                || _simInterface == null)
            {
                _logger?.LogDebug(
                    "Cannot send event '{Event}' — not connected",
                    eventName);
                return false;
            }

            return _simInterface.SendEvent(eventName, data);
        }

        /// <summary>
        /// Set data on the simulation (only when connected)
        /// </summary>
        public bool SetData(uint definitionId,
            IReadOnlyDictionary<string, SimValue> values)
        {
            if (_connectionState != ConnectionState.Connected
                || _simInterface == null)
            {
                return false;
            }

            return _simInterface.SetData(definitionId, values);
        }

        // =====================================================
        // Helpers
        // =====================================================

        private void SetConnectionState(ConnectionState newState)
        {
            var old = _connectionState;
            _connectionState = newState;
            if (old != newState)
            {
                ConnectionStateChanged?.Invoke(this,
                    new ConnectionStateChangedEventArgs(
                        old, newState, Platform));
            }
        }

        /// <summary>
        /// Reset retry counter (useful for manual reconnect triggers)
        /// </summary>
        public void ResetRetryCount()
        {
            _retryCount = 0;
        }

        // =====================================================
        // Dispose
        // =====================================================

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!_disposed)
            {
                if (disposing)
                {
                    _cts?.Cancel();

                    try
                    {
                        _workerTask?.Wait(TimeSpan.FromSeconds(5));
                    }
                    catch { }

                    DisconnectInternal();

                    _cts?.Dispose();
                }
                _disposed = true;
            }
        }
    }
}