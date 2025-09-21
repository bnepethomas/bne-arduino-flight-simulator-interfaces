# It looks like a number of events either aren't natively supported in the Simconnect python SDK
# or not supported in MSFS. eg NAV_LIGHTS_ON
# The good news is these are easily added
# Edit (after saving a copy) EventList.py
# C:\Users\admin\AppData\Local\Programs\Python\Python37\Lib\site-packages\SimConnect
# Added turn on Taxi Lights
#(b'TOGGLE_NAV_LIGHTS', "Toggle navigation lights", "All aircraft"),
#(b'NAV_LIGHTS_ON', "Turn on navigation lights", "All aircraft"),

# Check here https://docs.flightsimulator.com/html/Introduction/Introduction.htm
# https://docs.flightsimulator.com/html/Programming_Tools/Event_IDs/Aircraft_Misc_Events.htm#MASTER_CAUTION_TOGGLE
# https://docs.flightsimulator.com/html/Programming_Tools/Event_IDs/Event_IDs.htm

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
            #event_to_trigger = ae.find("TOGGLE_AVIONICS_MASTER")
            #event_to_trigger = ae.find("MASTER_BATTERY_OFF")
            #event_to_trigger = ae.find("MASTER_BATTERY_ON")
            #event_to_trigger = ae.find("ALTERNATOR_ON")
            #event_to_trigger = ae.find("ALTERNATOR_OFF")
            #event_to_trigger = ae.find("AVIONICS_MASTER_1_ON")
            #event_to_trigger = ae.find("AVIONICS_MASTER_1_OFF")
            #event_to_trigger = ae.find("STROBES_ON")
            #event_to_trigger = ae.find("STROBES_OFF")
            #event_to_trigger = ae.find("NAV_LIGHTS_ON")
            #event_to_trigger = ae.find("NAV_LIGHTS_OFF")
            #event_to_trigger = ae.find("PITOT_HEAT_ON")
            #event_to_trigger = ae.find("PITOT_HEAT_OFF")
            #event_to_trigger = ae.find("ANTI_ICE_ON")
            #event_to_trigger = ae.find("ANTI_ICE_OFF")
            #As per notes in 206 Dcoument - Hyd bindings aren't working
            #event_to_trigger = ae.find("HYDRAULIC_SWITCH_TOGGLE")
            #event_to_trigger = ae.find("MASTER_WARNING_TOGGLE")
            #event_to_trigger = ae.find("ELT_ON")
            #event_to_trigger = ae.find("ELT_OFF")
            #event_to_trigger = ae.find("LANDING_LIGHTS_OFF")
            event_to_trigger = ae.find("LANDING_LIGHTS_ON")

            
            event_to_trigger()


##  Read value of indicator - needed to do as FS only provides a toggle option
##            print("Getting Turn indicator state")
##            turn_indicator_switch_state = aq.get("TURN_INDICATOR_SWITCH")
##            print("Turn indicator state is :" + str(turn_indicator_switch_state))
##            if turn_indicator_switch_state == 1:
##                event_to_trigger = ae.find("TOGGLE_TURN_INDICATOR_SWITCH")
##                event_to_trigger()


##            starter_switch_state = aq.get("GENERAL_ENG_STARTER:1")
##            print("Starter1 state is :" + str(starter_switch_state))
##            if starter_switch_state == 0:
##                event_to_trigger = ae.find("TOGGLE_STARTER1")
##                event_to_trigger()


# Currently unable to find a sensible variable
##            fuel_valve_switch_state = aq.get("NEW_FUEL_SYSTEM")
##            print("FUEL VALVE 1 State is :" + str(fuel_valve_switch_state))
##            if fuel_valve_switch_state == 0:
##                event_to_trigger = ae.find("TOGGLE_FUEL_VALVE_ENG1")
##                event_to_trigger()        
            
            #event_to_trigger = ae.find("TOGGLE_AVIONICS_MASTER")
            #event_to_trigger()
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
