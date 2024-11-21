
import os
import time
import smtplib
from email.mime.text import MIMEText

def measure_temp():
    temp = os.popen("vcgencmd measure_temp").readline()
    return float(temp.replace("temp=", "").replace("'C", ""))

def send_email_alert(temp):
    sender_email = "jfallerEE@gmail.com"
    receiver_email = "fallerj@sonoma.edu"
    password = "iskmlhzaomoacbmr"
    smtp_server = "smtp.gmail.com"
    smtp_port = 587

    try:
        msg = MIMEText(f"CPU temperature alert: {temp:.2f}°C")
        msg["Subject"] = "Raspberry Pi CPU Temperature Alert"
        msg["From"] = sender_email
        msg["To"] = receiver_email

        server = smtplib.SMTP(smtp_server, smtp_port)
        server.starttls()
        server.login(sender_email, password)
        server.sendmail(sender_email, receiver_email, msg.as_string())
        server.quit()
        print("Alert email sent.")
    except Exception as e:
        print(f"Failed to send email: {e}")

threshold = 37.0  # Set your temperature threshold here

while True:
    current_temp = measure_temp()
    print(f"Current temperature: {current_temp:.2f}°C")
    if current_temp > threshold:
        send_email_alert(current_temp)
    time.sleep(5)  # Check every 60 seconds
