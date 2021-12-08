# Let's talk MQTT in Python 
# first install : https://pypi.org/project/paho-mqtt/
# Author : G.MENEZ

import paho.mqtt.client as mqtt
import time
import json

#==================================================
client = mqtt.Client("control") # create new instance
client.username_pw_set(username="user", password="chiasse")


print("MQTT CONNECTION : {}".format("home.haysberg.io"))
client.connect("home.haysberg.io") # connect to broker

client.loop_start()     #------------ start the loop

#-------------------------------------
topicname = "control"

print("\nSubscribing to topic {}".format(topicname))
client.subscribe(topicname)

msg = "OFF"
for i in range(3) : # On publie 3 messages "off"
    print("\nPublishing message {} to topic {}".format(msg, topicname))
    print(client.publish(topicname, msg))
    if client.publish(topicname, msg)[0] == 0 :
        print("Success")
    else :
        print("Failed")
    time.sleep(5)

time.sleep(100)         # wait in seconds before 
client.loop_stop()      #------------ stop the loop and the script
