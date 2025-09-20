import time
import threading
from SimConnect import SimConnect, AircraftRequests, AircraftEvents  # or from pysimconnect
# from SimConnect import SIMCONNECT_PERIOD, SIMCONNECT_DATA_REQUEST_FLAG

from SimConnect import *

def connect():
    """
    Try to establish a connection to MSFS SimConnect, retrying on failure.
    Returns sim, aq (AircraftRequests), ae (AircraftEvents)
    """
    while True:
        try:
            sim = SimConnect()
            aq = AircraftRequests(sim, _time=200)  # request caching / refresh time 200ms
            ae = AircraftEvents(sim)
            print("Connected to MSFS")
            return sim, aq, ae
        except Exception as e:
            print(f"Failed to connect: {e}")
            time.sleep(5)  # wait 5 seconds then retry

def get_battery_voltage(aq, battery_name="Battery_1"):
    """
    Get the battery voltage for named battery (if supported).
    Returns float voltage in volts, or None if unavailable.
    """
    try:
        # Using SimVar: (A:ELECTRICAL BATTERY VOLTAGE:'Battery_1'_n, Volts)
        # Replace 'Battery_1' with the correct component name or index
        simvar_name = f"ELECTRICAL BATTERY VOLTAGE:'{battery_name}'_n"
        voltage = aq.get(simvar_name)
        return voltage
    except Exception as e:
        print(f"Error getting battery voltage: {e}")
        return None

def is_fuel_pump_off(aq, pump_index=1):
    """
    Check if the fuel pump is off. Returns True if off, False if on, None if unavailable.
    Using simvar or LVar / event variable depending on aircraft.
    """
    try:
        # Two possible checks:
        # 1. If there is a simvar like "FUELSYSTEM PUMP SWITCH:1" which returns Bool
        # 2. Alternatively an L-var or aircraft-specific variable
        simvar_name = f"FUELSYSTEM PUMP SWITCH:{pump_index}"
        pump_status = aq.get(simvar_name)
        # pump_status likely boolean: True means switch ON, False OFF
        if pump_status is None:
            return None
        # Here: if True means it's ON → so OFF means False
        return (pump_status == False)
    except Exception as e:
        print(f"Error checking fuel pump status: {e}")
        return None

def send_warning(ae, message="Warning: Fuel pump is off while battery voltage is above threshold"):
    """
    Sends a warning message. Could be a SimConnect event or a custom display.
    This function is a stub; adapt to how you want to display the warning.
    """
    # Option A: use SimConnect event if application / aircraft supports
    # Option B: print to console / write to log / trigger some UI within add-on
    print(message)
    # Example of sending a custom event (if defined in your aircraft / panel)
    # event = ae.find("YOUR_CUSTOM_WARNING_EVENT_ID")
    # if event:
    #     event()

def monitor_loop(battery_name="Battery_1", pump_index=1, voltage_threshold=10.0):
    sim, aq, ae = connect()

    while True:
        try:
            voltage = get_battery_voltage(aq, battery_name)
            if voltage is None:
                # maybe aircraft doesn’t have battery by that name, or variable unavailable
                print("Battery voltage unavailable.")
            else:
                # Debug
                # print(f"Battery voltage: {voltage:.2f} V")
                if voltage > voltage_threshold:
                    pump_off = is_fuel_pump_off(aq, pump_index)
                    if pump_off is None:
                        print("Fuel pump status unavailable.")
                    elif pump_off:
                        send_warning(ae)
            # Sleep for 200 ms
            time.sleep(0.2)
        except Exception as e:
            print(f"Error in monitor loop: {e}")
            # Perhaps connection lost; try to reconnect
            try:
                sim.close()
            except:
                pass
            print("Attempting to reconnect...")
            sim, aq, ae = connect()

if __name__ == "__main__":
    # You can configure battery_name, pump_index, threshold here
    monitor_thread = threading.Thread(target=monitor_loop,
                                      args=("Battery_1", 1, 10.0),
                                      daemon=True)
    monitor_thread.start()

    # Keep main thread alive
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("Exiting...")
