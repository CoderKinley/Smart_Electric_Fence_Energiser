
// Callback function when data is received
void onDataReceived(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  Serial.print("Received data from: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Message: ");
  for (int i = 0; i < len; i++) {
    receivedData += (char)data[i];
  }

  // print the data
  Serial.println(receivedData );
  received_data = String(receivedData);
  receivedData = ""; 
  publish_status_flag = true;
}

// Callback function when data is sent
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

// Function to send relay status continuously
void sendRelayStatus() {
  const char *statusMessage = relayMode ? "Relay ON" : "Relay OFF";
  esp_err_t result = esp_now_send(peerAddress, (uint8_t *)statusMessage, strlen(statusMessage));

  if (result == ESP_OK) {
    Serial.println("Relay status sent successfully");
  } else {
    Serial.println("Error sending relay status");
  }
}