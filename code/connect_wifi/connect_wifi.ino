#include <WiFi.h>

const char* ssid = "TP-LINK_C5A2";
const char* password = "15750849198yy";

void setup()
{
    Serial.begin(115200);
    WiFi.begin(ssid,password);
    while(WiFi.status() != WL_CONNECTED){
        // statement
        delay(500);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("Connected to the WiFi network");

}

void loop()
{
    
}
