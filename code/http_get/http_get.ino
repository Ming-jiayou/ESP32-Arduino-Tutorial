#include <WiFi.h>
#include <HTTPClient.h>
const char* ssid = "TP-LINK_C5A2";
const char* password = "15750849198yy";
const char* serverUrl = "http://192.168.0.101:5000/OutputClassicalPoetry";
void setup() {
  Serial.begin(115200);
  
  // 连接WiFi
  WiFi.begin(ssid, password);
  Serial.print("正在连接到WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi已连接");
  Serial.print("IP地址: ");
  Serial.println(WiFi.localIP());
}
void loop() {
  // 确保WiFi保持连接
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi连接断开，尝试重连...");
    WiFi.reconnect();
    delay(2000);  // 等待重连
    return;
  }
  HTTPClient http;
  
  Serial.println("\n正在发送GET请求...");
  http.begin(serverUrl); 
  // 设置超时时间
  http.setTimeout(10000);
  
  // 发送请求并获取状态码
  int httpCode = http.GET();
  
  // 处理响应
  if (httpCode > 0) {
    Serial.printf("HTTP状态码: %d\n", httpCode);
    
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println("响应内容:");
      Serial.println(payload);
    } else {
      Serial.println("非200响应，完整响应头:");
      Serial.println(http.getString());  // 输出错误信息
    }
  } else {
    Serial.printf("请求失败，错误: %s\n", http.errorToString(httpCode).c_str());
  }
  
  http.end();  // 必须释放资源
  
  delay(5000); // 等待5秒
}

