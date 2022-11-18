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

// Domain và path file cần get
String host = "managefoodofparrot.infinityfreeapp.com";
String path = "/feeder-status.json";
// status - nội dung file feeder-status.json
String status;
// timeAct - thời gian đọc file
String timeAct;
// relay - rơ le cấp nguồn cho động cơ bước và mạch điều khiển (pin 12)
int relay = 12;
// i - biến đếm dùng cho các vòng lặp
int i;
// iWifiDis - biến đếm số lần wifi disconnect
int iWifiDis;
// led - led trên mạch ESP8266 (pin 2)
int led = 2;

// Keo's motor
// 5, 4, 13, 14 tương ứng với D1, D2, D7, D5 của ESP8266
Stepper_28BYJ_48 keoStepper(5, 4, 13, 14);

void setup()
{
    Serial.begin(115200);
    pinMode(relay, OUTPUT);
    digitalWrite(relay, LOW);
    pinMode(led, OUTPUT);
    connectToWifi();
    iWifiDis = 0;
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
        // ESP sent GET request to host
        client.print(String("GET ") + path + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" +
                     // Trường hợp này cần thêm cookie để hiển thị nội dung feeder-status.json (tìm hiểu thêm)
                     "Cookie: _test=1a274e73b2f6d4bad76fdece11d742c8\r\n" +
                     "Connection: keep-alive\r\n\r\n");
        while (client.available() == 0)
        {
        }
        while (client.available())
        {
            //Đọc response được gửi tới ESP từ host
            status = client.readString();
            // Tách là thời gian
            timeAct = status.substring(status.indexOf("Date"), status.indexOf("GMT"));
            // Tách ra nội dung file feeder-status.json
            status = status.substring(status.indexOf("{"));
            Serial.println();
            Serial.print(timeAct);
            Serial.println("GMT");
            Serial.println(status);
            client.stop();
            controlSteppers(status);
        }
    }
    else
    {
        Serial.println("WiFi Disconnected!");
        digitalWrite(led, LOW);
        delay(1000);
        digitalWrite(led, HIGH);
        delay(1000);
        iWifiDis++;
        if (iWifiDis == 100)
        {
            // Ngủ 60s
            ESP.deepSleep(60000000);
        }
        return;
    }
    // Ngủ 15 phút
    ESP.deepSleep(60000000);
}

// Kết nối Wifi
void connectToWifi()
{
    WiFi.begin(ssid, password);
    Serial.print("Connecting...");
    delay(250);
    i = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        // led nháy khi đang trong quá trình kết nối
        digitalWrite(led, LOW);
        delay(250);
        digitalWrite(led, HIGH);
        delay(250);
        // nếu sau 100 dấu chấm mà chưa kết nối được thì chờ 60s sau thử kết nối lại
        i++;
        if (i == 100)
        {
            // Ngủ 60s
            ESP.deepSleep(60000000);
        }
    }
    // led sáng liên tục khi đã kết nối thành công
    digitalWrite(led, LOW);
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
}

void controlSteppers(String newStatus)
{
    // Parse JSON
    // Bắt đầu từ ArduinoJson 6.7.0 DynamicJsonDocument có dung lượng cố định (tìm DynamicJsonDocument để xem thêm chi tiết)
    int size = 1024;
    char json[size];
    // Ép kiểu từ String sang Array
    newStatus.toCharArray(json, size);
    DynamicJsonDocument jsonBuffer(size);
    DeserializationError error = deserializeJson(jsonBuffer, json);
    if (error)
    {
        Serial.println("Parse JSON failed!");
    }
    if (jsonBuffer["done"] == "0")
    {
        // Điều khiển động cơ của Kẹo
        // atoi - hàm chuyển một chuỗi số sang kiểu int
        int keoSteps = atoi(jsonBuffer["keoSet"]);
        // strcmp - string compare - so sánh 2 chuỗi, nếu bằng nhau thì trả về 0
        if (strcmp(jsonBuffer["keo"], "on") == 0)
        {
            Serial.println("on!!!");
            digitalWrite(relay, HIGH);
            delay(100);
            keoStepper.step(keoSteps);
            digitalWrite(relay, LOW);
        }
        else
        {
            Serial.println("off!!!");
            digitalWrite(relay, HIGH);
            delay(100);
            keoStepper.step(0 - keoSteps);
            digitalWrite(relay, LOW);
        }
        // Thông báo lên server đã thực hiệnWiFiClient client;
        int iNotDone = 0;
        while (1)
        {
        start:
            if (WiFi.status() == WL_CONNECTED)
            {
                WiFiClient client;
                if (!client.connect(host, 80))
                {
                    Serial.println("Connect to server fail to update \"done\"!");
                    goto start;
                }
                // ESP sent GET request to host
                client.print(String("GET ") + "/index.php?done=1" + " HTTP/1.1\r\n" +
                             "Host: " + host + "\r\n" +
                             // Cần có Cookie
                             "Cookie: _test=1a274e73b2f6d4bad76fdece11d742c8\r\n" +
                             "Content-Type: application/x-www-form-urlencoded\r\n" +
                             "Connection: close\r\n\r\n");
                client.stop();
                Serial.println("Update \"done\" is OK!");
                break;
            }
            else
            {
                Serial.println("WiFi Disconnected!");
                Serial.println("Can not update \"done\"!");
                digitalWrite(led, LOW);
                delay(2000);
                digitalWrite(led, HIGH);
                delay(2000);
                iNotDone++;
                if (iNotDone == 100)
                {
                    delay(60000);
                    iNotDone = 0;
                    connectToWifi();
                    
                }
                goto start;
            }
        }
    }
}