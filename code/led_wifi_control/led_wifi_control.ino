#include <WiFi.h>
#include <WebServer.h>

// 定义 LED 连接的引脚
const int ledPin = 18;  // GPIO 2

// WiFi 连接参数
const char* ssid = "TP-LINK_C5A2";
const char* password = "15750849198yy";

// Web 服务器端口
const int serverPort = 80;

// 创建 WiFi 客户端和 Web 服务器对象

WebServer server(serverPort);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  // 初始化 LED 引脚为输出
  pinMode(ledPin, OUTPUT);

  // 连接 WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // 设置 Web 服务器路由
  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);

  // 开始 Web 服务器
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  // 处理 Web 服务器请求
  server.handleClient();
}

// 处理根路由
void handleRoot() {
  String html = "<html><head><title>ESP32 LED Control</title></head><body>";
  html += "<h1>ESP32 LED Control</h1>";
  html += "<a href=\"/on\"><button>Turn On</button></a>";
  html += "<a href=\"/off\"><button>Turn Off</button></a>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

// 处理 LED 开启请求
void handleOn() {
  digitalWrite(ledPin, HIGH);
  server.send(200, "text/plain", "LED is ON");
}

// 处理 LED 关闭请求
void handleOff() {
  digitalWrite(ledPin, LOW);
  server.send(200, "text/plain", "LED is OFF");
}
