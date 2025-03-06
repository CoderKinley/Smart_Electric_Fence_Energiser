// Function to publish random data
void publishData(String voltage_data) {
  if (!mqttClient.connected() && !connectToMQTT()) {
    return;
  } 

  String voltagedata = String(voltage_data); 
  String timeData = getCurrentDateTime();
  String location = getCellTowerLocation();
  
  String message = timeData + ", " + voltagedata +", "+ location;
  Serial.println(message);

  // logDataToSD(message); //save each data to the sd card
  
  if (mqttClient.publish(mqtt_topic_publish, message.c_str())) {
    SerialMon.println("Published: " + message + " to topic: " + mqtt_topic_publish);
  } 
  else {
    SerialMon.println("Failed to publish message");
  }
}

// write data to the SD card
void logDataToSD(String dataString) {
  File dataFile = SD.open("energizerDataLog.txt", FILE_WRITE);

  if (dataFile) {
    dataFile.println(dataString);  
    dataFile.close();              
    Serial.println("Data written to SD: " + dataString);  
  } 
  else {
    Serial.println("Error opening energizerDataLog.txt");
  }
}