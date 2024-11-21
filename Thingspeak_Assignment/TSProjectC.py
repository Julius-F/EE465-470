import RPi.GPIO as GPIO
import time
import requests

# Set the GPIO mode
GPIO.setmode(GPIO.BCM)

# Set up GPIO 27 as an output pin
GPIO.setup(27, GPIO.OUT)

# ThingSpeak channel details
channelID = '2756669'  # Replace with your ThingSpeak channel ID
readAPIKey = 'av6j9qqniby2eqv8'  # Replace with your ThingSpeak Read API Key
fieldNumber = 1  # The field where LED status is stored (1 for ON, 0 for OFF)

# Function to read data from ThingSpeak
def get_led_status():
    url = f"https://api.thingspeak.com/channels/{channelID}/fields/{fieldNumber}.json?api_key={readAPIKey}&results=1"
    response = requests.get(url)
    data = response.json()
    
    # Extract the latest value from the field
    if data['feeds']:
        led_status = int(data['feeds'][0]['field1'])  # Assuming field 1 stores LED status
        return led_status
    else:
        print("Error fetching data from ThingSpeak.")
        return None

# Function to toggle the LED based on the ThingSpeak value
def toggle_led():
    led_status = get_led_status()
    
    if led_status is not None:
        if led_status == 1:
            GPIO.output(27, GPIO.HIGH)  # Turn LED ON
            print("LED is ON")
        else:
            GPIO.output(27, GPIO.LOW)  # Turn LED OFF
            print("LED is OFF")
    else:
        print("Error: Could not read LED status.")

# Loop to check and control the LED state based on ThingSpeak data
try:
    while True:
        toggle_led()  # Check and toggle the LED based on ThingSpeak value
        time.sleep(10)  # Wait 10 seconds before checking again (adjust as needed)

finally:
    # Clean up GPIO settings before exiting
    GPIO.cleanup()
