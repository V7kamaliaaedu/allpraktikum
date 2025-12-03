/*
  ---------------------------------
  IMPORTANT: CONFIGURATION REQUIRED
  ---------------------------------
  
  Before running this code, make sure to check the "secrets.h" file
  for important configuration details such as WiFi credentials and 
  Firebase settings.

  The "secrets.h" file should include:
  - Your WiFi SSID and Password
  - Your Firebase Realtime Database URL
  - (OPTIONAL) Firebase Authentication Token

  Ensure that "secrets.h" is properly configured and includes the correct
  information for your project. Failure to do so may result in connection
  errors or incorrect behavior of your application.
*/

#include "secrets.h"
#include <Firebase.h>
#include <ArduinoJson.h>

/* Use the following instance for Test Mode (No Authentication) */
Firebase fb(REFERENCE_URL);

/* Use the following instance for Locked Mode (With Authentication) */
// Firebase fb(REFERENCE_URL, AUTH_TOKEN);

void setup() {
    Serial.begin(115200);
    pinMode(D4, OUTPUT);
    
    // Board-specific initialization
    #if !defined(ARDUINO_UNOWIFIR4)
        WiFi.mode(WIFI_STA);
    #else
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, LOW);
    #endif
    
    WiFi.disconnect();
    delay(1000);

    /* Connect to WiFi */
    Serial.println();
    Serial.println();
    Serial.print("Connecting to: ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print("-");
        delay(500);
    }

    Serial.println();
    Serial.println("WiFi Connected");
    Serial.println();

    // Turn on built-in LED for UNO R4 WiFi
    #if defined(ARDUINO_UNOWIFIR4)
        digitalWrite(LED_BUILTIN, HIGH);
    #endif

    /* ===== JSON SERIALIZATION: CREATE AND SEND DATA ===== */
    
    Serial.println("Creating JSON data...");
    
    /*
      For guidance on serialization and deserialization, visit:
      https://arduinojson.org/v7/assistant/
    */

    // Create a JSON document to hold the output data
    StaticJsonDocument<255> docOutputLED;

    // Add various data types to the JSON document
    docOutputLED["LED1"] = 0;

    // Create a string to hold the serialized JSON data
    String output;

    // Serialize the JSON document to a string
    serializeJson(docOutputLED, output);

    Serial.println("JSON data created:");
    Serial.println(output);
    Serial.println();

    // Set the serialized JSON data in Firebase
    Serial.println("Sending JSON to Firebase...");
    int responseCode = fb.setJson("LED", output);
    Serial.print("Set JSON - Response Code: ");
    Serial.println(responseCode);

    if (responseCode == 200) {
        Serial.println("JSON data successfully sent to Firebase!");
    } else {
        Serial.println("Failed to send JSON data to Firebase!");
        Serial.print("Response code ");
        Serial.print(responseCode);
        Serial.println(" indicates an error occurred.");
        return; // Exit if we can't send data
    }

    Serial.println();
}

void loop() {
    // Nothing to do here
     /* ===== JSON DESERIALIZATION: RETRIEVE AND PARSE DATA ===== */
    
    Serial.println("Retrieving JSON from Firebase...");

    // Retrieve the serialized JSON data from Firebase
    String input;
    int responseCode = fb.getJson("LED", input);
    Serial.print("Get JSON - Response Code: ");
    Serial.println(responseCode);

    // Check if the retrieval was successful
    if (responseCode != 200) {
        Serial.println("Could not retrieve data from Firebase");
        Serial.print("Response code ");
        Serial.print(responseCode);
        Serial.println(" indicates an error occurred.");
        Serial.println("This might be due to:");
        Serial.println("- Network connectivity issues");
        Serial.println("- Incorrect Firebase URL or path");
        Serial.println("- Authentication problems");
        return;
    }

    Serial.println("JSON data retrieved:");
    Serial.println(input);
    Serial.println();

    // Create a JSON document to hold the deserialized data
    StaticJsonDocument<256> docInput;


    // Deserialize the JSON string into the JSON document
    DeserializationError error = deserializeJson(docInput, input);

    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
    }

    Serial.println("Parsing JSON data...");

    // Extract the values from the deserialized JSON document
    int lampu1 = docInput["LED1"];   // "Hello World!"


    /* Print the deserialized data */
    Serial.println("--- Parsed Data ---");
    Serial.print("Lampu 1: ");
    Serial.println(lampu1);

    Serial.println();
    digitalWrite(D4, lampu1);
}
