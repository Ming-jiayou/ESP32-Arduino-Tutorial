#include <U8g2lib.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
// WiFi 设置
const char* ssid = "TP-LINK_C5A2";
const char* password = "15750849198yy";
// NTP 设置
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 28800, 60000); // GMT+8, 60秒更新一次
// OLED 设置
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /*SCL=*/0, /*SDA=*/1, /*RESET=*/U8X8_PIN_NONE);
void setup() {
  Serial.begin(115200);
  
  // 初始化 OLED
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  
  // 连接 WiFi
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  // 初始化 NTP
  timeClient.begin();
  while (!timeClient.update()) {
    delay(500);
    Serial.print(".");
  }
  setTime(timeClient.getEpochTime());
  Serial.println("NTP time synced");
}
void loop() {
  static unsigned long lastNtpUpdate = 0;
  unsigned long now = millis();
  // 每分钟同步一次 NTP 时间
  if (now - lastNtpUpdate >= 60000) {
    timeClient.update();
    setTime(timeClient.getEpochTime());
    lastNtpUpdate = now;
  }
  // 获取当前时间
  time_t t = now();
  char timeStr[20]; // 足够存储 "2023年12月31日 23:59:59"
  
  // 格式化时间字符串
  snprintf(timeStr, sizeof(timeStr), 
           "%04d年%02d月%02d日 %02d:%02d:%02d",
           year(t), month(t), day(t),
           hour(t), minute(t), second(t));
  // 显示时间
  u8g2.clearBuffer();
  u8g2.setCursor(0, (32 + u8g2.getMaxCharHeight()) / 2);
  u8g2.print(timeStr);
  u8g2.sendBuffer();
  
  delay(200); // 适当降低刷新率以减少闪烁
}