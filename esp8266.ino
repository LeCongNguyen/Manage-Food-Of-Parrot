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
String jsonData = "";
int testBtn = 0; // button on Esp
int i;

// Keo's motor
Stepper_28BYJ_48 keoStepper(5, 4, 2, 14); // tương ứng với D1, D4, D5 của ESP8266

void setup()
{
    Serial.begin(115200);
    pinMode(testBtn, INPUT_PULLUP);
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
    // ESP sent request to Host to take json file
    retest:
    if (WiFi.status() == WL_CONNECTED)
    {
        // Connect to host through port 80
        WiFiClient client;
        if (!client.connect(host, 80))
        {
            Serial.println("Connect to server fail!");
            return;
        }
        // Sent GET request to host
        client.print(String("GET ") + path + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" +
                     "Connection: keep-alive\r\n\r\n");
        // Read data that host respond and show in serial monitor
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
            controlSteppers(result2);
        }
    }
    else
    {
        Serial.println("WiFi Disconnected");
        Serial.print(".");
        delay(250);
    }
    //Tạo delay kết hợp kiểm tra nút nhấn
    i = 120000;
    while (i >= 0)
    {
        if (digitalRead(testBtn) == 0)
        {
            Serial.println("Retest...");
            goto retest;
        }
        else {
            i--;
            delay(1);
        }
    }
}

void controlSteppers(String newData)
{
    if (newData != jsonData)
    {
        jsonData = newData;
        // Parse JSON
        int size = 1024; // Bắt đầu từ ArduinoJson 6.7.0 DynamicJsonDocument có dung lượng cố định (tìm DynamicJsonDocument để xem thêm chi tiết)
        char json[size];
        newData.toCharArray(json, size); //Ép kiểu từ String sang Array
        DynamicJsonDocument jsonBuffer(size);
        DeserializationError error = deserializeJson(jsonBuffer, json);
        if (error)
        {
            Serial.println("Parse JSON failed!");
        }
        // Keo's process
        int keoSteps = atoi(jsonBuffer["keoSet"]);
        if (strcmp(jsonBuffer["keo"], "on") == 0) // strcmp - string compare - so sánh 2 chuỗi, nếu bằng nhau thì trả về 0
        {
            keoStepper.step(keoSteps);
        }
        else
        {
            keoStepper.step(0 - keoSteps);
        }
    }
}