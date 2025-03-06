String printCounts() {
  // Read the counter values from the "counters.txt" file
  File counterFile = SD.open(counterFilename);
  if (counterFile) {
    int newWriteCount = counterFile.parseInt();
    int newReadCount = counterFile.parseInt();
    counterFile.close();

    // Print the read counter values
    Serial.print("Write Count: ");
    Serial.println(newWriteCount);
    Serial.print("Read Count: ");
    Serial.println(newReadCount);
    String writeNo = String(newWriteCount);
    String readNo = String(newReadCount);

    // Read the latest delete event from "delete_log.txt" file and print date and time
    File deleteLogFile = SD.open(deleteLogFilename);
    if (deleteLogFile) {
      Serial.println("Latest Delete Event:");
      String latestDeleteEvent;
      while (deleteLogFile.available()) {
        latestDeleteEvent = deleteLogFile.readStringUntil('\n');
      }
      deleteLogFile.close();

    } else {
      Serial.println("Error opening delete log file");
    }
    purgedate = "18.08/1995";
    purgetime = "09:45 AM";
    String readWriteNo = writeNo + "," + readNo + "," + purgedate + "," + purgetime;
    // String readWriteNo = "writeNo, readNo,purgedate,purgetime";

    return (readWriteNo);
  } else {
    Serial.println("Error opening counter file for reading");
    return ("Fail to get Counter");
  }
}
