#define TINY_GSM_MODEM_SIM7600
#include <WiFi.h>
#include <PubSubClient.h>
#include <TinyGsmClient.h>
#include <HTTPClient.h>
#include <Update.h>
#include <SD.h>
#include <SPI.h>
#include <time.h>
#include <esp_now.h>

#define SerialAT Serial2
#define SerialMon Serial

#define RXD2 27
#define TXD2 26
#define powerPin 4
#define reSetPin 32

#define CHANNEL 1
uint8_t peerAddress[] = { 0x7C, 0x9E, 0xBD, 0x54, 0x7E, 0x78 };

esp_now_peer_info_t peerInfo;
bool relayMode = false;

String receivedData = "";
String received_data = "";

const char* ssid = "You all";
const char* password = "kinley123";

const char* mqtt_server = "119.2.119.202";
const int mqtt_port = 1883;
const char* mqtt_topic_publish = "Energizer/data";
const char* mqtt_topic_subscribe = "Energizer/commands";

TinyGsm modem(SerialAT);
TinyGsmClient gsmClient(modem);
WiFiClient wifiClient;
PubSubClient mqttClient(gsmClient);

const int redPin = 25;
const int greenPin = 33;

const char* counterFilename = "/counters.txt";
const char* deleteLogFilename = "/delete_log.txt";
int writeCount = 0;
int readCount = 0;
bool counterInitialized = false;
String readWriteCount;
String purgedate;
String purgetime;

bool useGSM = true;
bool use4G = true;

float usedPercentage;

bool pulse_high = false;

bool publish_status_flag = false;

void setReadMode() {
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, HIGH);
}

void setWriteMode() {
  digitalWrite(greenPin, LOW);
  digitalWrite(redPin, HIGH);
}

bool restartModem() {
  SerialMon.println("Restarting modem...");
  digitalWrite(reSetPin, HIGH);
  delay(2500);
  digitalWrite(reSetPin, LOW);
  delay(3000);

  if (!modem.restart()) {
    SerialMon.println("Modem restart failed");
    return false;
  }
  return true;
}

int checkSignalStrength() {
  int signalQuality = modem.getSignalQuality();
  SerialMon.print("Signal quality: ");
  SerialMon.println(signalQuality);
  return signalQuality;
}

bool connectGSMNetwork() {
  SerialMon.println("Connecting to GSM network...");
  if (!modem.waitForNetwork(60000L)) {
    SerialMon.println("GSM Network connection failed");
    return false;
  }
  if (modem.isNetworkConnected()) {
    SerialMon.println("GSM Network connected");

    if (!modem.gprsConnect("internet", "", "")) {
      SerialMon.println("GPRS Connection failed");
      return false;
    }
    SerialMon.println("GPRS Connected");
  }
  setReadMode();
  return true;
}

// listen to the mqtt of subscribe to it for the relay mode
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  SerialMon.print("Message arrived [");
  SerialMon.print(topic);
  SerialMon.print("] ");
  SerialMon.println(message);

  if (String(topic) == mqtt_topic_subscribe) {
    if (message == "ON") {
      relayMode = false;
    } 
    else if (message == "OFF") {
      relayMode = true;
    }
    sendRelayCommand(relayMode);
  }
}

bool connectToMQTT() {
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqttCallback);

  SerialMon.println("Attempting to connect to MQTT");

  if (mqttClient.connect("ESP32Client")) {
    SerialMon.println("Connected to MQTT broker");
    
    if (mqttClient.subscribe(mqtt_topic_subscribe)) {
      SerialMon.println("Subscribed to topic: " + String(mqtt_topic_subscribe));
    } else {
      SerialMon.println("Failed to subscribe to topic");
    }
    
    return true;
  } else {
    SerialMon.print("Failed to connect to MQTT, rc=");
    SerialMon.println(mqttClient.state());
    return false;
  }
}

String sdCardReading() {
  if (SD.begin(5)) {
    Serial.println("SD CARD Memory");

    uint64_t totalBytes = SD.totalBytes();
    uint64_t usedBytes = SD.usedBytes();

    usedPercentage = (float)usedBytes / totalBytes * 100.0;
    String usedMemoryPercent = String(usedPercentage);

    Serial.printf("Total space: %lluMB\n", totalBytes / (1024 * 1024));
    Serial.printf("Used space: %lluMB (%.2f%%)\n", usedBytes / (1024 * 1024), usedPercentage);
    float totalMemory = (float)totalBytes / (1024 * 1024);
    String totalMemoryStr = String(totalMemory);
    float usedMemory = (float)usedBytes / (1024 * 1024);
    String usedMemoryStr = String(usedMemory);

    readWriteCount = printCounts();

    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE) {
      SerialMon.println("No SD card attached");
    } else {
      SerialMon.print("SD Card Type: ");
      if (cardType == CARD_MMC) {
        SerialMon.println("MMC");
      } else if (cardType == CARD_SD) {
        SerialMon.println("SDSC");
      } else if (cardType == CARD_SDHC) {
        SerialMon.println("SDHC");
      } else {
        SerialMon.println("UNKNOWN");
      }

      uint64_t cardSize = SD.cardSize() / (1024 * 1024);
      SerialMon.printf("SD Card Size: %lluMB\n", cardSize);
    }

    String sdCardValue = "1," + totalMemoryStr + "," + usedMemory + "," + usedMemoryPercent + "," + readWriteCount;
    Serial.println("SD card Details: ");
    Serial.println(sdCardValue);
    return (sdCardValue);
  } else {
    Serial.println("No SD card present");
    return ("No SD Card Present");
  }
}

void setup() {
  SerialMon.begin(9600);
  Serial1.begin(9600, SERIAL_8N1, 16, 17);
  delay(10);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW initialization failed");
    return;
  }

  esp_now_register_recv_cb(onDataReceived);
  esp_now_register_send_cb(onDataSent);

  peerInfo.channel = CHANNEL;
  peerInfo.encrypt = false;
  memcpy(peerInfo.peer_addr, peerAddress, 6);

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }else if(esp_now_add_peer(&peerInfo) == ESP_OK){
    Serial.println("Successfully Added peer");
  }

  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, HIGH);

  pinMode(reSetPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  digitalWrite(reSetPin, HIGH);
  SerialMon.println("Reset Pin High");
  delay(2500);

  digitalWrite(reSetPin, LOW);
  delay(3000);

  SerialAT.begin(115200, SERIAL_8N1, RXD2, TXD2);
  delay(3000);

  SerialMon.println("Initializing modem...");
  if (!restartModem()) {
    SerialMon.println("Failed to restart modem after multiple attempts");
    useGSM = false;
  }

  while (!connectGSMNetwork()) {
    SerialMon.println("No network available. Retrying in 30 seconds...");
    setWriteMode();
    delay(30000);
  }

  String sd_data = sdCardReading();
  SerialMon.println(sd_data);

  if (!connectToMQTT()) {
    SerialMon.println("Initial MQTT connection failed. Continuing...");
  }
}

void loop() {
  static unsigned long lastCheck = 0;
  static unsigned long lastPublish = 0;
  const unsigned long checkInterval = 30000;
  const unsigned long publishInterval = 5000;

  unsigned long currentMillis = millis();

  if (currentMillis - lastCheck >= checkInterval) {
    lastCheck = currentMillis;

    if (useGSM) {
      if (!modem.isNetworkConnected() || !modem.isGprsConnected()) {
        SerialMon.println("GSM network connection lost. Attempting to reconnect...");
        setWriteMode();
        if (!connectGSMNetwork()) {
          SerialMon.println("GSM reconnection failed. Trying Again...");
          useGSM = false;
        }
      }
    } else {
      if (!connectGSMNetwork()) {
        SerialMon.println("GSM connection failed. Restarting...");
        ESP.restart();
      }
    }

    if (useGSM) {
      mqttClient.setClient(gsmClient);
    } else {
      mqttClient.setClient(wifiClient);
    }
  }

  if (!mqttClient.connected()) {
    connectToMQTT();
  }
  mqttClient.loop();

  if (publish_status_flag) {
    publishData(received_data);
    publish_status_flag = false;
  }
}

// send the relay data tot the other side 
void sendRelayCommand(bool relay_mode){
  String relay_stat = "";
  if(relay_mode){
    relay_stat = "OFF";
  }
  else if (!relay_mode){
    relay_stat = "ON";
  }

  // Serial.println(relayMode);
  esp_err_t result = esp_now_send(peerAddress, (uint8_t *)relay_stat.c_str(), relay_stat.length());
  if (result == ESP_OK) {
      Serial.println("Data sent: " + relay_stat);
  } 
  else {
      Serial.println("Error sending data");
  }
}

// data received = date, time, voltage, relay_status, latitude , longitude