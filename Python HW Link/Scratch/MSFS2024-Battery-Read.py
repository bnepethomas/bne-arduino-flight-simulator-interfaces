import time
import threading
from SimConnect import SimConnect, AircraftRequests, AircraftEvents  # or from pysimconnect
# from SimConnect import SIMCONNECT_PERIOD, SIMCONNECT_DATA_REQUEST_FLAG

from SimConnect import *

def connect():
    global ae
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
    global ae
    """
    Get the battery voltage for named battery (if supported).
    Returns float voltage in volts, or None if unavailable.
    """
    try:
        # Using SimVar: (A:ELECTRICAL BATTERY VOLTAGE:'Battery_1'_n, Volts)
        # Replace 'Battery_1' with the correct component name or index
        # print("Getting Battery Voltage.")
        #simvar_name = ("PLANE_LATITUDE")
        #simvar_name = ("ELECTRICAL_BATTERY_VOLTAGE")
        #simvar_name = ("ELECTRICAL_MAIN_BUS_VOLTAGE")
        #simvar_name = ("ROTOR_RPM_PCT")
        #simvar_name = ("COLLECTIVE_POSITION")
        #simvar_name = ("GENERAL_ENG_THROTTLE_LEVER_POSITION:1")
        # Throttle position reports 0 against stop
        #simvar_name = ("GENERAL_ENG_EXHAUST_GAS_TEMPERATURE:1")
        
        simvar_name = ("AVIONICS_MASTER_SWITCH")

        
        voltage = aq.get(simvar_name)
        print(simvar_name + " : " +  str(voltage))

        if voltage == 0: 
            print("Avonics off")
            event_to_trigger = ae.find("TOGGLE_AVIONICS_MASTER")
            #event_to_trigger = ae.find("MASTER_BATTERY_OFF")
            #AVIONICS = ae.Avionics.AVIONICS_MASTER_SET
            #AVIONICS.value = 1


            event_to_trigger()
        return voltage
    except Exception as e:
        print(f"Error getting battery voltage: {e}")
        return None




def monitor_loop(battery_name="Battery_1", pump_index=1, voltage_threshold=10.0):
    sim, aq, ae = connect()

    while True:
        try:
            voltage = get_battery_voltage(aq, battery_name)
            if voltage is None:
                # maybe aircraft doesn’t have battery by that name, or variable unavailable
                print("Battery voltage unavailable.")


            # Sleep for 200 ms
            time.sleep(1)
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


    # Keep main thread alive
    try:
        while True:
            monitor_loop()
            
            time.sleep(1)
    except KeyboardInterrupt:
        print("Exiting...")
