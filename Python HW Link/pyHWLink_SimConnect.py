from SimConnect import *

# Create SimConnect link
try: 
    sm = SimConnect()
    print("Sim Connected")    
    # Note the default _time is 2000 to be refreshed every 2 seconds
    print("Asking for aircraft info")
    aq = AircraftRequests(sm, _time=2000)
    print("Configured Aircraft Requests")
    # Use _time=ms where ms is the time in milliseconds to cache the data.
    # Setting ms to 0 will disable data caching and always pull new data from the sim.
    # There is still a timeout of 4 tries with a 10ms delay between checks.
    # If no data is received in 40ms the value will be set to None
    # Each request can be fine tuned by setting the time param.

    # To find and set timeout of cached data to 200ms:
    altitude = aq.find("PLANE_ALTITUDE")
    altitude.time = 200
    print("Grabbing Altitude")
   
except Exception as error:
   error_string = str(error)
   print(error_string)
   print("Sim not running?")

