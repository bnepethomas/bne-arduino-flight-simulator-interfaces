from SimConnect import *
import time
import socket
import json
import sys

# --- SimVar Definitions ---
# This dictionary maps a descriptive name to the SimConnect variable name and its unit.
# The index ':1' is used for the first engine/system where applicable.
# Some SimVars might need adjustment based on the specific aircraft model.
SIMVAR_DATA = {
    "Airspeed Indicator":       ("AIRSPEED INDICATED", "Knots"),
    "Attitude Indicator Pitch": ("AIRCRAFT PITCH", "Degrees"),
    "Attitude Indicator Bank":  ("AIRCRAFT BANK", "Degrees"),
    "Altimeter":                ("INDICATED ALTITUDE", "Feet"),
    "Radar Altimeter":          ("RADIO ALTITUDE", "Feet"),
    "Vertical Velocity":        ("VERTICAL SPEED", "Feet per second"),
    "Turn Rate":                ("TURN INDICATOR", "Degrees per second"), # For Turn/Slip, Turn Rate
    "Slip/Skid Angle":          ("SIDESLIP ANGLE", "Degrees"),    # For Turn/Slip, Slip component
    "HSI Heading":              ("HEADING INDICATOR", "Degrees"), # For HSI
    "HSI NAV1 CDI":             ("NAV1 CDI", "Number"),           # For HSI, Course Deviation Indicator
    "HSI NAV1 Glideslope":      ("NAV1 GLIDE SLOPE", "Number"),   # For HSI, Glideslope Deviation
    "ADF Active Freq":          ("ADF ACTIVE FREQUENCY:1", "Hz"),
    "ADF Bearing":              ("ADF BEARING:1", "Degrees"),
    "N1 Speed":                 ("TURB ENG N1:1", "Percent"),     # Gas Producer (N1) speed
    "EGT":                      ("GENERAL ENG EXHAUST GAS TEMPERATURE:1", "Rankine"), # Exhaust Gas Temperature
    "N2 Speed":                 ("TURB ENG N2:1", "Percent"),     # Power Turbine (N2) speed
    "Rotor RPM":                ("ROTOR RPM", "RPM"),             # Rotor (NR) speed (for helicopters)
    "Engine Torque":            ("GENERAL ENG TORQUE:1", "Foot pounds"),
    "Engine Oil Temp":          ("GENERAL ENG OIL TEMPERATURE:1", "Rankine"),
    "Engine Oil Pressure":      ("GENERAL ENG OIL PRESSURE:1", "Psi"),
    "Transmission Oil Temp":    ("TRANSMISSION OIL TEMPERATURE:1", "Rankine"), # Note: May not be universally supported by all aircraft models.
    "Transmission Oil Pressure":("TRANSMISSION OIL PRESSURE:1", "Psi"), # Note: May not be universally supported by all aircraft models.
    "Fuel Level":               ("FUEL TOTAL QUANTITY", "Gallons"),
    "Fuel Pressure":            ("GENERAL ENG FUEL PRESSURE:1", "Psi"),
    "Electrical Load":          ("ELECTRICAL TOTAL LOAD", "Amps")
}

# --- UDP Configuration ---
UDP_IP = "127.0.0.1"  # Target IP address (e.g., localhost)
UDP_PORT = 5005       # Target UDP port
RECONNECT_DELAY = 5   # Seconds to wait before attempting to reconnect

def read_simconnect_data_and_send_udp():
    """
    Connects to SimConnect, reads specified aircraft instrument data every 100 milliseconds,
    sends it over UDP, and handles disconnects with automatic reconnection.
    """
    udp_socket = None
    try:
        udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP socket
        print(f"UDP socket created. Sending data to {UDP_IP}:{UDP_PORT}")
    except socket.error as e:
        print(f"Error creating UDP socket: {e}. Exiting.")
        sys.exit(1)

    while True: # Outer loop for continuous operation and reconnection
        sm = None
        try:
            print("Attempting to connect to SimConnect...")
            sm = SimConnect(True) # Initialize SimConnect. True enables verbose logging.
            print("Successfully connected to SimConnect.")

            aq = AircraftRequests(sm, _time=100) # Refresh time of 100ms
            for name, (simvar, unit) in SIMVAR_DATA.items():
                aq.add_request(name, simvar, unit)

            print(f"Reading data every {aq._time}ms. Press Ctrl+C to stop.")

            while True: # Inner loop for data reading and sending when connected
                try:
                    data = aq.get_data() # Get all requested data

                    # Prepare data for UDP
                    formatted_data = {}
                    for name, (simvar, unit) in SIMVAR_DATA.items():
                        value = data.get(name)
                        if value is not None:
                            formatted_data[name] = round(value, 2) # Round for cleaner output
                        else:
                            formatted_data[name] = "N/A"

                    json_data = json.dumps(formatted_data)
                    udp_socket.sendto(json_data.encode('utf-8'), (UDP_IP, UDP_PORT))

                    # Optional: Print to console for verification
                    # print("\n--- Instrument Readings (Sent via UDP) ---")
                    # for key, value in formatted_data.items():
                    #     print(f"{key}: {value}")

                    time.sleep(0.1) # Sleep for 100ms to match the update frequency for display/sending

                except SimConnect.SimConnectError as e:
                    print(f"SimConnect communication error: {e}. Attempting to reconnect...")
                    break # Break inner loop to trigger reconnection
                except Exception as e:
                    print(f"An unexpected error occurred during data processing or UDP send: {e}. Attempting to reconnect...")
                    break # Break inner loop to trigger reconnection

        except SimConnect.SimConnectError as e:
            print(f"SimConnect connection error: {e}. Retrying in {RECONNECT_DELAY} seconds...")
        except Exception as e:
            print(f"An unexpected error occurred during SimConnect initialization: {e}. Retrying in {RECONNECT_DELAY} seconds...")
        finally:
            if sm:
                sm.exit() # Ensure SimConnect connection is properly closed
                print("Disconnected from SimConnect.")
            time.sleep(RECONNECT_DELAY) # Wait before retrying connection

if __name__ == "__main__":
    read_simconnect_data_and_send_udp()

    