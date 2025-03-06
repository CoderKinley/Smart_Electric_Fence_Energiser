// get the current date and time using the cclk command
String getCurrentDateTime() {
  modem.sendAT("+CCLK?");                                  // Send command to get date and time
  if (modem.waitResponse(10000L, "+CCLK: ") == 1) {        // Wait for the response
    String response = modem.stream.readStringUntil('\n');  // Read the response
    response.trim();
    if (response.startsWith("+CCLK: ")) {
      String dateTime = response.substring(response.indexOf("\"") + 1, response.lastIndexOf("\""));
      // Extract date and time parts
      String year = dateTime.substring(0, 2);
      String month = dateTime.substring(3, 5);
      String day = dateTime.substring(6, 8);
      String hour24 = dateTime.substring(9, 11);
      String minute = dateTime.substring(12, 14);
      String second = dateTime.substring(15, 17);

      // Format the date and time
      char buffer[30];  // Buffer size to accommodate the formatted string
      snprintf(buffer, sizeof(buffer), "%s-%s-20%s, %s:%s:%s", day.c_str(), month.c_str(), year.c_str(), hour24.c_str(), minute.c_str(), second.c_str());
      return String(buffer);
    } else {
      String dateTime = response.substring(response.indexOf("\"") + 1, response.lastIndexOf("\""));
      // Extract date and time parts
      String year = dateTime.substring(0, 2);
      String month = dateTime.substring(3, 5);
      String day = dateTime.substring(6, 8);
      String hour24 = dateTime.substring(9, 11);
      String minute = dateTime.substring(12, 14);
      String second = dateTime.substring(15, 17);

      // Format the date and time
      char buffer[30];  // Buffer size to accommodate the formatted string
      snprintf(buffer, sizeof(buffer), "%s-%s-20%s, %s:%s:%s", day.c_str(), month.c_str(), year.c_str(), hour24.c_str(), minute.c_str(), second.c_str());
      return String(buffer);
    }
  }

  return "Error getting time";
}

// get location data
String getCellTowerLocation() {
  modem.sendAT("+CLBS=1,1");
  if (modem.waitResponse(30000L, "+CLBS: ") == 1) {
    String response = modem.stream.readStringUntil('\n');
    response.trim();
    
    // Parse the response
    // Expected format: +CLBS: <result>,<time>,<latitude>,<longitude>,<accuracy>
    int commaCount = 0;
    int startIndex = 0;
    String latitude, longitude;
    
    for (int i = 0; i < response.length(); i++) {
      if (response.charAt(i) == ',') {
        commaCount++;
        if (commaCount == 2) {
          startIndex = i + 1;
        } else if (commaCount == 3) {
          latitude = response.substring(startIndex, i);
          startIndex = i + 1;
        } else if (commaCount == 4) {
          longitude = response.substring(startIndex, i);
          break;
        }
      }
    }
    
    if (!latitude.isEmpty() && !longitude.isEmpty()) {
      char buffer[50];
      snprintf(buffer, sizeof(buffer), "Lat: %s, Lon: %s", latitude.c_str(), longitude.c_str());
      return String(buffer);
    }
  }
  // rturning the default location when there is no location information
  return "27.4727924, 89.6392863"; 
}
