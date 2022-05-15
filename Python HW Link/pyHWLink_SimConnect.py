from SimConnect import *
import time

Radians = 57.295779513

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
    print("Grabbing Altitude")
    altitude = aq.find("PLANE_ALTITUDE")
    altitude.time = 200
    print("Altitude is: " + str(altitude.value))


    print("Grabbing HEADING_DEGREES_TRUE")
    HEADING_DEGREES_TRUE = aq.find("PLANE_HEADING_DEGREES_MAGNETIC")
    HEADING_DEGREES_TRUE.time = 200
    print("HEADING_DEGREES_TRUE is: " + str(HEADING_DEGREES_TRUE.value * Radians))

    print("Grabbing AIRSPEED_TRUE")
    AIRSPEED_TRUE = aq.find("AIRSPEED_TRUE")
    AIRSPEED_TRUE.time = 200
    print("AIRSPEED_TRUE is: " + str(AIRSPEED_TRUE.value))

    print("Grabbing LATITUDE")
    LATITUDE = aq.find("PLANE_LATITUDE")
    LATITUDE.time = 200
    print("LATITUDE is: " + str(LATITUDE.value))

    print("Grabbing LONGITUDE")
    LONGITUDE = aq.find("PLANE_LONGITUDE")
    LONGITUDE.time = 200
    print("LONGITUDE is: " + str(LONGITUDE.value))



    for x in range(0, 50):
        print("Altitude is: " + str(altitude.value))
        print("AIRSPEED_TRUE is: " + str(AIRSPEED_TRUE.value))
        print("HEADING_DEGREES_TRUE is: " + str(HEADING_DEGREES_TRUE.value * Radians))
        print("LATITUDE is: " + str(LATITUDE.value))
        print("LONGITUDE is: " + str(LONGITUDE.value))
        time.sleep(1)

    
   
except Exception as error:
   error_string = str(error)
   print(error_string)
   print("Sim not running?")

