#define BLYNK_TEMPLATE_ID "TMPL6YAS5_ss3"
#define BLYNK_TEMPLATE_NAME "IoT Smart Exhaust Fan"
#define BLYNK_PRINT Serial  // Directs Blynk messages to the serial port
 
#include <WiFi.h>  // Include WiFi library for ESP32
#include <BlynkSimpleEsp32.h>  // Include Blynk ESP32 library
#include "DHT.h"  // Include DHT sensor library
 
// Blynk and WiFi credentials
const char auth[] = "jbfjmdnbjb";  // Blynk authentication token
const char ssid[] = "Rohit";  // WiFi SSID
const char pass[] = "1234567890";  // WiFi password
 
// Pin configuration
#define DHTPIN 22  // DHT sensor pin
#define DHTTYPE DHT11  // Type of DHT sensor
 
const int gasSensorPin = 34;  // Gas sensor pin
const int relayPin = 23;  // Relay module pin
 
const int gasThreshold = 20;  // Gas level threshold for triggering the relay
 
DHT dht(DHTPIN, DHTTYPE);  // Initialize DHT sensor
 
bool manualMode = false;  // Flag to track if the relay is in manual mode
 
void setup()
{
  Serial.begin(115200);  // Start serial communication at 115200 baud rate
  dht.begin();  // Initialize DHT sensor
  Blynk.begin(auth, ssid, pass);  // Connect to Blynk using WiFi
 
  pinMode(relayPin, OUTPUT);  // Set relay pin as output
  pinMode(gasSensorPin, INPUT);  // Set gas sensor pin as input
 
  digitalWrite(relayPin, LOW);  // Ensure relay is off initially
 
  delay(2000);  // Wait for 2 seconds before proceeding
}
 
// Function to handle manual relay control from Blynk app
BLYNK_WRITE(V4)
{
  int relayControl = param.asInt();  // Get value from Blynk app
  manualMode = (relayControl == 1);  // Set manual mode based on app input
 
  if (manualMode)
  {
    digitalWrite(relayPin, HIGH);  // Turn on relay in manual mode
  }
  else
  {
    digitalWrite(relayPin, LOW);  // Turn off relay when exiting manual mode
  }
 
  Blynk.virtualWrite(V3, relayControl);  // Update relay status on Blynk
}
 
void loop()
{
  Blynk.run();  // Run Blynk
 
  int sensorValue = analogRead(gasSensorPin);  // Read gas sensor value
  int gas_percentage = map(sensorValue, 0, 4095, 0, 100);  // Convert to percentage
 
  float humidity = dht.readHumidity();  // Read humidity from DHT sensor
  float temperature = dht.readTemperature();  // Read temperature from DHT sensor
 
  // Check if sensor readings are valid
  if (isnan(humidity) || isnan(temperature))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
 
  // Print sensor values to the Serial Monitor
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");
 
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println("°C ");
 
  Serial.print("Gas sensor value: ");
  Serial.println(sensorValue);
 
  Serial.print("Gas Percentage: ");
  Serial.print(gas_percentage);
  Serial.println("%");
 
  Serial.println();
 
  // Automatic control logic based on gas levels
  if (!manualMode)
  {
    if (gas_percentage > gasThreshold)
    {
      digitalWrite(relayPin, HIGH);  // Activate relay if gas above threshold
      Blynk.virtualWrite(V3, HIGH);  // Update Blynk app
    }
    else
    {
      digitalWrite(relayPin, LOW);  // Deactivate relay if gas below threshold
      Blynk.virtualWrite(V3, LOW);  // Update Blynk app
    }
  }
 
  // Send sensor values to Blynk
  Blynk.virtualWrite(V0, gas_percentage);  // Send gas percentage
  Blynk.virtualWrite(V1, temperature);  // Send temperature
  Blynk.virtualWrite(V2, humidity);  // Send humidity
 
  delay(1000);  // Wait for a second before next loop iteration
}