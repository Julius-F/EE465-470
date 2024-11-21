import os
import psutil
import requests
import time

# ThingSpeak API setup
WRITE_API_KEY = 'l3kb38f8ork3oh0x' #your Write API key
CHANNEL_ID = '2756503' # Replace with your ThingSpeak Channel ID
URL = f'https://api.thingspeak.com/update?api_key={WRITE_API_KEY}'

def get_cpu_temperature():
    # Reading CPU temperature
    temp = os.popen("vcgencmd measure_temp").readline()
    return float(temp.replace("temp=","").replace("'C\n", ""))

def get_cpu_usage():
    # Reading CPU usage
    return psutil.cpu_percent()

def send_data_to_thingspeak(temp, cpu_usage):
    # Sending data to ThingSpeak
    params = {
        'field1': temp,
        'field2': cpu_usage
    }
    response = requests.get(URL, params=params)
    return response

# Continuously send data to ThingSpeak every 60 seconds
while True:
    # Get CPU temperature and usage
    temp = get_cpu_temperature()
    cpu_usage = get_cpu_usage()

    # Send data to ThingSpeak
    send_data_to_thingspeak(temp, cpu_usage)
    
    # Print data to terminal
    print(f"CPU Temperature: {temp} C")
    print(f"CPU Usage: {cpu_usage} %")
    # Wait for 5 seconds before sending the next data point
    time.sleep(5)
 
