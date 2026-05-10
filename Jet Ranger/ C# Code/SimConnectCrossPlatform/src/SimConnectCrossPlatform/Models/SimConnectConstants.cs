// src/SimConnectCrossPlatform/Models/SimConnectConstants.cs

namespace SimConnectCrossPlatform.Models
{
    /// <summary>
    /// SimConnect protocol constants shared across platforms
    /// </summary>
    internal static class SimConnectConstants
    {
        // Data types
        public const uint DATATYPE_FLOAT32 = 0;
        public const uint DATATYPE_FLOAT64 = 1;
        public const uint DATATYPE_INT32 = 2;
        public const uint DATATYPE_INT64 = 3;
        public const uint DATATYPE_STRING256 = 4;

        // Object IDs
        public const uint OBJECT_ID_USER = 0;

        // Periods
        public const uint PERIOD_ONCE = 0;
        public const uint PERIOD_VISUAL_FRAME = 1;
        public const uint PERIOD_SIM_FRAME = 2;
        public const uint PERIOD_SECOND = 3;

        // SimObject types
        public const uint SIMOBJECT_TYPE_USER = 0;

        // Priorities
        public const uint GROUP_PRIORITY_HIGHEST = 0x01000000;

        // Flags
        public const uint EVENT_FLAG_GROUPID_IS_PRIORITY = 0x00000010;

        // Special values
        public const uint UNUSED = 0xFFFFFFFF;

        // Recv IDs
        public const uint RECV_ID_NULL = 0x00000000;
        public const uint RECV_ID_EXCEPTION = 0x00000001;
        public const uint RECV_ID_OPEN = 0x00000002;
        public const uint RECV_ID_QUIT = 0x00000003;
        public const uint RECV_ID_EVENT = 0x00000004;
        public const uint RECV_ID_EVENT_OBJECT_ADDREMOVE = 0x00000005;
        public const uint RECV_ID_EVENT_FILENAME = 0x00000006;
        public const uint RECV_ID_EVENT_FRAME = 0x00000007;
        public const uint RECV_ID_SIMOBJECT_DATA = 0x00000008;
        public const uint RECV_ID_SIMOBJECT_DATA_BYTYPE = 0x00000009;
        public const uint RECV_ID_SYSTEM_STATE = 0x0000000C;

        // Recv struct offsets (byte offsets into raw data)
        // SIMCONNECT_RECV base
        public const int RECV_OFFSET_DWSIZE = 0;
        public const int RECV_OFFSET_DWVERSION = 4;
        public const int RECV_OFFSET_DWID = 8;

        // SIMCONNECT_RECV_SIMOBJECT_DATA extends RECV
        public const int DATA_OFFSET_REQUEST_ID = 12;
        public const int DATA_OFFSET_OBJECT_ID = 16;
        public const int DATA_OFFSET_DEFINE_ID = 20;
        public const int DATA_OFFSET_FLAGS = 24;
        public const int DATA_OFFSET_ENTRY_NUMBER = 28;
        public const int DATA_OFFSET_OUT_OF = 32;
        public const int DATA_OFFSET_DEFINE_COUNT = 36;
        public const int DATA_OFFSET_DATA_START = 40;

        // SIMCONNECT_RECV_EXCEPTION
        public const int EXCEPTION_OFFSET_EXCEPTION = 12;
        public const int EXCEPTION_OFFSET_SENDID = 16;
        public const int EXCEPTION_OFFSET_INDEX = 20;
    }
}