import paho.mqtt.client as mqtt
import requests

# MQTT Configuration
BROKER_URL = "broker.hivemq.com"  # Public HiveMQ broker
BROKER_PORT = 8000  # WebSocket port
TOPIC = "testtopic/temp/outTopic/starfoxesp"  # Topic to subscribe to (replace with the actual topic)

# PHP script URL to insert data into the database
PHP_URL = "https://chordsynthesizer.com/insertMQTTdata.php"  # Replace with your PHP URL

# Callback function for MQTT connection
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to HiveMQ broker!")
        # Subscribe to the topic once connected
        client.subscribe(TOPIC)
        print(f"Subscribed to topic: {TOPIC}")
    else:
        print(f"Failed to connect, return code {rc}")

# Callback function for receiving messages
def on_message(client, userdata, msg):
    print(f"Received message: {msg.payload.decode()} from topic: {msg.topic}")
    
    # Extract potentiometer value from the message (assuming it's in the format "Potentiometer Value: <value>")
    message = msg.payload.decode()
    try:
        # Extract the numeric value after "Potentiometer Value: "
        if "Potentiometer Value:" in message:
            value = int(message.split("Potentiometer Value:")[1].strip())  # Convert to integer
            # Push the potentiometer data to the PHP script
            push_value_to_php(value)
        else:
            print("Potentiometer value not found in message.")
    except ValueError:
        print("Error extracting potentiometer value from message.")

# Function to push potentiometer data to the PHP script
def push_value_to_php(value):
    try:
        # Construct the URL with the potentiometer value as a parameter
        url = f"{PHP_URL}?value={value}"
        
        # Send HTTP GET request to the PHP script
        response = requests.get(url)

        # Check if the request was successful
        if response.status_code == 200:
            print(f"Data successfully sent to PHP script: {response.text}")
        else:
            print(f"Failed to send data. Status code: {response.status_code}")
    except Exception as e:
        print(f"Error sending data to PHP: {e}")

# Create an MQTT client instance
client = mqtt.Client(transport="websockets")

# Attach callback functions
client.on_connect = on_connect
client.on_message = on_message

# Connect to the HiveMQ broker
print("Connecting to broker...")
client.connect(BROKER_URL, BROKER_PORT, 60)

# Start the MQTT client loop
client.loop_forever()
