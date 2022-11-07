#include <Stepper_28BYJ_48.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

// Wifi-1 (ở nhà)
const char *ssid = "VNPT_HUYENTRANG 1";
const char *password = "07070707";

// Wifi-2 (phòng trọ)
// const char *ssid = "VNPT_2.4G";
// const char *password = "nguyen12345";

// Wifi-3 (cơ điện)
//  const char *ssid = "";
//  const char *password = "duoc6789";

// Host
String host = "managefoodofparrot.000webhostapp.com";
String path = "/feeder-status.json";
String status;

Stepper_28BYJ_48 stepper(16, 5, 4, 0); //tương ứng với D0. D1, D2, D3 của ESP8266

void setup()
{
    int i;
    for (i = 0; i <= 100; i++)
    {
        stepper.step(1);
    }

    Serial.begin(115200);
    // Connect to Wifi
    WiFi.begin(ssid, password);
    Serial.print("Connecting...");
    delay(1000);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(250);
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
}

void loop()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        WiFiClient client;
        if (!client.connect(host, 80))
        {
            Serial.println("Connect to server fail!");
            return;
        }
        client.print(String("GET ") + path + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" +
                     "Connection: keep-alive\r\n\r\n");
        while (client.available() == 0)
        {
        }
        while (client.available())
        {
            status = client.readString();
            int from1 = status.indexOf("Date");
            int to1 = from1 + 35;
            String result1 = status.substring(from1, to1);
            int from2 = status.indexOf("{");
            String result2 = status.substring(from2);
            Serial.println(result1);
            Serial.println(result2);
            Serial.println();
            client.stop();
        }
    }
    else
    {
        Serial.println("WiFi Disconnected");
        Serial.print(".");
        delay(250);
    }
    delay(600000);
}