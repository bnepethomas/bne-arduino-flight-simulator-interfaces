// src/SimConnectCrossPlatform/Loader/SimConnectDelegates.cs

using System;
using System.Runtime.InteropServices;

namespace SimConnectCrossPlatform.Loader
{
    /// <summary>
    /// Unmanaged function pointer delegate types matching
    /// SimConnect API signatures
    /// </summary>
    internal static class SimConnectDelegates
    {
        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int SimConnect_Open(
            out IntPtr phSimConnect,
            [MarshalAs(UnmanagedType.LPStr)] string szName,
            IntPtr hWnd,
            uint UserEventWin32,
            IntPtr hEventHandle,
            uint ConfigIndex);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int SimConnect_Close(
            IntPtr hSimConnect);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int SimConnect_AddToDataDefinition(
            IntPtr hSimConnect,
            uint DefineID,
            [MarshalAs(UnmanagedType.LPStr)] string DatumName,
            [MarshalAs(UnmanagedType.LPStr)] string UnitsName,
            uint DatumType,
            float fEpsilon,
            uint DatumID);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int SimConnect_ClearDataDefinition(
            IntPtr hSimConnect,
            uint DefineID);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int SimConnect_RequestDataOnSimObjectType(
            IntPtr hSimConnect,
            uint RequestID,
            uint DefineID,
            uint dwRadiusMeters,
            uint type);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int SimConnect_RequestDataOnSimObject(
            IntPtr hSimConnect,
            uint RequestID,
            uint DefineID,
            uint ObjectID,
            uint Period,
            uint Flags,
            uint origin,
            uint interval,
            uint limit);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int SimConnect_SetDataOnSimObject(
            IntPtr hSimConnect,
            uint DefineID,
            uint ObjectID,
            uint Flags,
            uint ArrayCount,
            uint cbUnitSize,
            IntPtr pDataSet);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int SimConnect_MapClientEventToSimEvent(
            IntPtr hSimConnect,
            uint EventID,
            [MarshalAs(UnmanagedType.LPStr)] string EventName);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int SimConnect_TransmitClientEvent(
            IntPtr hSimConnect,
            uint ObjectID,
            uint EventID,
            uint dwData,
            uint GroupID,
            uint Flags);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int SimConnect_GetNextDispatch(
            IntPtr hSimConnect,
            out IntPtr ppData,
            out uint pcbData);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int SimConnect_SubscribeToSystemEvent(
            IntPtr hSimConnect,
            uint EventID,
            [MarshalAs(UnmanagedType.LPStr)] string SystemEventName);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int SimConnect_UnsubscribeFromSystemEvent(
            IntPtr hSimConnect,
            uint EventID);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int SimConnect_Text(
            IntPtr hSimConnect,
            uint type,
            float fTimeSeconds,
            uint EventID,
            uint cbUnitSize,
            IntPtr pDataSet);
    }
}