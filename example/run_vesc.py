
import pymultivesc as vesc
import json
import time

config = json.loads(open('config.json').read())
manager = vesc.Manager(config)

atom1 = manager.motor("atomiser1")

atom1.set_rpm(1000)

# Wait forever
while True:
    time.sleep(1)



