#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <EEPROM.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

const char* ssid = "Orange-8D1B"; // Enter your WiFi SSID
const char* password = "F1HR6701Q96"; // Enter your WiFi password

 char* PARAM_EMAIL = "email";
 char* PARAM_PASSWORD = "password";

// EEPROM addresses to start writing the strings
const int EEPROM_EMAIL_ADDRESS = 0;
const int EEPROM_PASSWORD_ADDRESS = 100;


void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void setup() {
    EEPROM.begin(512);
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("WiFi Failed!\n");
        return;
    }

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", "<form action='/submit' method='post'>Email:<input type='text' name='email'><br>Password:<input type='password' name='password'><br><input type='submit' value='Submit'></form>");
    });

    server.on("/submit", HTTP_POST, [](AsyncWebServerRequest *request){
        String email;
        String password;
        if (request->hasParam(PARAM_EMAIL, true)) {
            email = request->getParam(PARAM_EMAIL, true)->value();
            
        } else {
            email = "No email sent";
        }
        if (request->hasParam(PARAM_PASSWORD, true)) {
            password = request->getParam(PARAM_PASSWORD, true)->value();
            
            
        } else {
            password = "No password sent";
        }
        // Write email to EEPROM
        for (int i = 0; i < email.length(); ++i) {
            EEPROM.write(EEPROM_EMAIL_ADDRESS + i, email[i]);
        }
        EEPROM.write(EEPROM_EMAIL_ADDRESS + email.length(), '\0'); // Null-terminate the email string
        
        // Write password to EEPROM
        for (int i = 0; i < password.length(); ++i) {
            EEPROM.write(EEPROM_PASSWORD_ADDRESS + i, password[i]);
        }
        EEPROM.write(EEPROM_PASSWORD_ADDRESS + password.length(), '\0'); // Null-terminate the password string
        
        EEPROM.commit(); // Commit the changes
        request->send(200, "text/plain", "Email: " + email + ", Password: " + password);
        
         
    });
        server.on("/readEmail", HTTP_GET, [](AsyncWebServerRequest *request){
        String email;
        for (int i = 0; ; ++i) {
            char c = EEPROM.read(EEPROM_EMAIL_ADDRESS + i);
            if (c == '\0') {
                break;
            }
            email += c;
        }
        request->send(200, "text/plain", "Email read from EEPROM: " + email);
    });

    server.on("/readPassword", HTTP_GET, [](AsyncWebServerRequest *request){
        String password;
        for (int i = 0; ; ++i) {
            char c = EEPROM.read(EEPROM_PASSWORD_ADDRESS + i);
            if (c == '\0') {
                break;
            }
            password += c;
        }
        request->send(200, "text/plain", "Password read from EEPROM: " + password);
    });

    
    server.onNotFound(notFound);

    server.begin();
}

void loop() {
  
}
