
import pymultivesc as vesc
import json
import time

print("Starting VESC manager")
config = json.loads(open('./config/example.json').read())
manager = vesc.Manager(config)

# Get the motor object
atom1 = manager.motor("atomiser1")

print("Setting RPM to 1000")
atom1.set_rpm(12000)

print("Done")

# Wait forever
while True:
    print(f"Speed {atom1.rpm()}")
    time.sleep(1)




