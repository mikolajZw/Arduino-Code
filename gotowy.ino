#include <SoftwareSerial.h>
#include <WiFiS3.h>

SoftwareSerial SoftSerial(2, 3); // Rx, Tx

float PM25 = 0.0;
float PM10 = 0.0;
float PM1 = 0.0;
float Temperature = 0.0;
float Light = 0.0;
float Pressure = 0.0;
float Humidity = 0.0;
float eTVOC = 0.0;
float eCO2 = 0.0;

char ssid[] = "SolarLab";
char pass[] = "SolarLab#2020";
char server[] = "esparduino-af1c98f2b313.herokuapp.com";   //"polar-wildwood-72630-ed5f60f11d8e.herokuapp.com";
int port = 80;
WiFiClient client;

// Struktura przechowująca dane z czujników
struct SensorData
{
  float PM25;
  float PM10;
  float PM1;
  float Temperature;
  float Light;
  float Pressure;
  float Humidity;
  float eTVOC;
  float eCO2;
};

SensorData sensorValues; // Zmienna przechowująca wartości z czujników

void setup()
{
  Serial.begin(9600);
  SoftSerial.begin(9600);

  // Connect to Wi-Fi
  while (WiFi.begin(ssid, pass) != WL_CONNECTED)
  {
    Serial.println("Connecting to WiFi...");
    delay(10000);
  }

  Serial.println("Connected to WiFi");
  printWifiStatus();
}

void loop()
{
  if (SoftSerial.available() > 0)
  {
    // Read incoming data from the sensor
    String receivedString = SoftSerial.readStringUntil('\n');

    // Parse the received data and update variables
    parseSensorData(receivedString);

    // Send the received data to your server
    sendToServer(receivedString);

    // Wait for a moment before sending the next data
    delay(10000); // Adjust this delay as needed
  }
}

void parseSensorData(String data)
{
  int separatorIndex = data.indexOf(':');
  if (separatorIndex != -1)
  {
    String key = data.substring(0, separatorIndex);
    String value = data.substring(separatorIndex + 2);

    float floatValue = value.toFloat();

    // Przypisz wartość do odpowiedniej zmiennej
    if (key == "PM25")
      sensorValues.PM25 = floatValue;
    else if (key == "PM10")
      sensorValues.PM10 = floatValue;
    else if (key == "PM1")
      sensorValues.PM1 = floatValue;
    else if (key == "Temperature")
      sensorValues.Temperature = floatValue;
    else if (key == "Light")
      sensorValues.Light = floatValue;
    else if (key == "Pressure")
      sensorValues.Pressure = floatValue;
    else if (key == "Humidity")
      sensorValues.Humidity = floatValue;
    else if (key == "eTVOC")
      sensorValues.eTVOC = floatValue;
    else if (key == "eCO2")
      sensorValues.eCO2 = floatValue;
  }
}

void sendToServer(String data)
{
  if (client.connect(server, port))
  {
    Serial.println("Connected to server");

    // Construct the HTTP request
    String url = "/";

    // Convert SensorData to JSON format
    String body = "{\"PM25\":" + String(sensorValues.PM25) +
                  ",\"PM10\":" + String(sensorValues.PM10) +
                  ",\"PM1\":" + String(sensorValues.PM1) +
                  ",\"Temperature\":" + String(sensorValues.Temperature) +
                  ",\"Light\":" + String(sensorValues.Light) +
                  ",\"Pressure\":" + String(sensorValues.Pressure) +
                  ",\"Humidity\":" + String(sensorValues.Humidity) +
                  ",\"eTVOC\":" + String(sensorValues.eTVOC) +
                  ",\"eCO2\":" + String(sensorValues.eCO2) + "}";

    // Make a HTTP POST request to the server
    client.println("POST " + url + " HTTP/1.1");
    client.println("Host: " + String(server));
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println(body.length());
    client.println();
    client.println(body);

    // Read and print the server's response
    readResponse();
     Serial.println(body);
  }
  else
  {
    Serial.println("Failed to connect to server");
  }

  // Close the connection
  client.stop();
}

void readResponse()
{
  Serial.println(client);
  while (client.available())
  {
    char c = client.read();
    Serial.println(c);
  }
}

void printWifiStatus()
{
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
