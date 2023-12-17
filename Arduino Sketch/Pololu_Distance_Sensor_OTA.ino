#include <PubSubClient.h>
#include <WiFiClient.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#define LED_PIN_RED 02
#define LED_PIN_GREEN 0
const char compile_date[] = __DATE__ " " __TIME__;
WiFiClient wifiClient;
// Replace the next variables with your SSID/Password combination
char* ssid = "ssid";
char* password = "password";

const char* mqtt_server = "IP Broker";
AsyncWebServer server(80);

//int LED_PIN_RED=02,LED_PIN_GREEN=0;
const uint8_t sensorPin = 4;

WiFiClient espClient;
PubSubClient client(espClient);

char hexMAC[3 + 12 + 1] = "espxxxxxxxxxxxx";  // length of "esp" + 12 bytes for MAC + 1 byte '\0' terminator
uint8_t mac[6];

char msg[50];
int value = 0;
unsigned long lastMillis = 0;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect(hexMAC)) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");
}



void setup() {
  Serial.begin(115200);
  Serial.println("Let's go!");
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  pinMode(sensorPin, INPUT);
  pinMode(LED_PIN_RED, OUTPUT);
  pinMode(LED_PIN_GREEN, OUTPUT);

  //pinMode(kIndicatorRed, OUTPUT);
  //pinMode(kIndicatorGreen, OUTPUT);
  //pinMode(kIndicatorBlue, OUTPUT);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


      server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(200, "text/plain", "To Update Firmware Please go to : " + WiFi.localIP().toString() + "/update");
      });

  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
  Serial.println("HTTP server started");

  WiFi.macAddress(mac);

  for (int i = 0; i < 6; i++) {
    hexMAC[3 + i * 2] = hex_digit(mac[i] >> 4);
    hexMAC[3 + i * 2 + 1] = hex_digit(mac[i] & 0x0f);
  }
  Serial.println(hexMAC);
}
void reconnect() {
  // Loop until we're reconnected
  client.loop();
  delay(10);

  if (!client.connected()) {
    connect();
  }
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(hexMAC)) {
      Serial.println("connected");
      // Subscribe
     // client.subscribe("sensor_distance/test");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {

  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }

  {
    int16_t t = pulseIn(sensorPin, HIGH);

    if (t == 0) {
      // pulseIn() did not detect the start of a pulse within 1 second.
      Serial.println("timeout");
    } else if (t > 1850) {//1850
      // No detection.
      Serial.println(-1);
      //client.publish("sensor_distance/sensor_distance", "-1");
      //digitalWrite(LED_PIN_RED, HIGH);
    } else {

      // Valid pulse width reading. Convert pulse width in microseconds to distance in millimeters.
      int16_t d = (t - 1000) / 2.5;

      // Limit minimum distance to 0.
      if (d < 0) { d = 0; }
      delay(2000);
      Serial.print(d);
      Serial.println(" cm");
      client.publish("sensor_distance/Distance", String(d).c_str());
      //client.publish("sensor_distance/sensor_distance","0");
      //digitalWrite(LED_PIN_RED, LOW);
      //digitalWrite(LED_PIN_GREEN, HIGH);

      if  (d > 200) {
      client.publish("sensor_distance/Status","1");
      digitalWrite(LED_PIN_RED, LOW);
      digitalWrite(LED_PIN_GREEN, HIGH);
       }

      //if  (d < 1900) 
      else {
      client.publish("sensor_distance/Status","0");
      digitalWrite(LED_PIN_RED, HIGH);
      digitalWrite(LED_PIN_GREEN, LOW);
       }

      }
      

  }
}
char hex_digit(uint8_t aValue) {
  char values[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
  if (aValue < 16) {
    return values[aValue];
  } else {
    return 'x';
  }
}
