#include <WiFi.h>
#include <HTTPClient.h>
#include <U8g2lib.h>
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, 0, 1, U8X8_PIN_NONE);
// WiFi配置
const char* ssid = "TP-LINK_C5A2";
const char* password = "15750849198yy";
const char* serverUrl = "http://192.168.0.101:5000/OutputClassicalPoetry";
// 显示相关变量
uint8_t fontHeight;
uint8_t yPos;
const uint16_t screenWidth = 128;
const unsigned long segmentDisplayTime = 2000;
unsigned long lastSegmentChange = 0;
int currentSegmentStart = 0;
bool needFullCycleReset = false;  // 新增循环重置标志
// 文本存储
String receivedText = "";
bool newTextReceived = false;
// UTF-8字符测量
int utf8CharBytes(char c) {
  if ((c & 0x80) == 0) return 1;
  else if ((c & 0xE0) == 0xC0) return 2;
  else if ((c & 0xF0) == 0xE0) return 3;
  else if ((c & 0xF8) == 0xF0) return 4;
  return 1;
}
// 分段计算（增加循环重置检测）
int getNextSegmentStart(const char* text, int startIndex, int maxWidth) {
  if (!text || strlen(text) == 0 || startIndex >= strlen(text)) return 0;
  
  int bytePos = startIndex;
  int segmentWidth = 0;
  int lastValidPos = bytePos;
  bool hasAdvanced = false;  // 检测是否前进过
  
  while (text[bytePos] != '\0') {
    hasAdvanced = true;
    int charBytes = utf8CharBytes(text[bytePos]);
    char buffer[10] = {0};
    for (int i = 0; i < charBytes; i++) buffer[i] = text[bytePos + i];
    
    int charWidth = u8g2.getUTF8Width(buffer);
    if (segmentWidth + charWidth > maxWidth) break;
    
    segmentWidth += charWidth;
    lastValidPos = bytePos + charBytes;
    bytePos += charBytes;
  }
  
  // 当没有前进且不是初始位置时，说明需要重置
  if (!hasAdvanced && startIndex != 0) return 0;
  return (text[bytePos] == '\0') ? 0 : lastValidPos;
}
// 显示处理
void drawSegment(const char* text, int startIndex) {
  if (!text || strlen(text) == 0) {
    u8g2.clearBuffer();
    u8g2.setCursor(0, yPos);
    u8g2.print("无内容");
    u8g2.sendBuffer();
    return;
  }
  
  u8g2.clearBuffer();
  char segment[256] = {0};
  int bytePos = startIndex;
  int segmentWidth = 0;
  int segIndex = 0;
  
  while (text[bytePos] != '\0') {
    int charBytes = utf8CharBytes(text[bytePos]);
    char temp[10] = {0};
    for (int i = 0; i < charBytes; i++) temp[i] = text[bytePos + i];
    int charWidth = u8g2.getUTF8Width(temp);
    
    if (segmentWidth + charWidth > screenWidth) break;
    
    for (int i = 0; i < charBytes; i++) segment[segIndex++] = text[bytePos + i];
    segmentWidth += charWidth;
    bytePos += charBytes;
  }
  segment[segIndex] = '\0';
  
  u8g2.setCursor(0, yPos);
  u8g2.print(segment);
  u8g2.sendBuffer();
}
// 数据获取
void fetchTextFromServer() {
  if (WiFi.status() != WL_CONNECTED) return;
  
  HTTPClient http;
  http.begin(serverUrl);
  Serial.print("[GET] 发送请求至: ");
  Serial.println(serverUrl);
  
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    String newText = http.getString();
    Serial.print("[GET] 接收结果: ");
    Serial.println(newText);
    
    if (newText != receivedText) {
      receivedText = newText;
      newTextReceived = true;
      needFullCycleReset = true;  // 标记需要完整循环
    }
  }
  http.end();
}
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("连接WiFi: ");
  Serial.println(ssid);
  
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("WiFi连接成功");
  
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  
  fontHeight = u8g2.getMaxCharHeight();
  yPos = (32 + fontHeight) / 2;
  drawSegment("等待数据...", 0);
  fetchTextFromServer();
}
void loop() {
  static unsigned long lastFetchTime = 0;
  unsigned long currentTime = millis();
  // 每30秒获取数据
  if (currentTime - lastFetchTime >= 60000) {
    lastFetchTime = currentTime;
    fetchTextFromServer();
  }
  // 处理新文本
  if (newTextReceived) {
    newTextReceived = false;
    currentSegmentStart = 0;
    lastSegmentChange = currentTime;
    drawSegment(receivedText.c_str(), currentSegmentStart);
    needFullCycleReset = false;  // 重置标志
  }
  
  // 分段切换逻辑
  if (currentTime - lastSegmentChange >= segmentDisplayTime) {
    int prevStart = currentSegmentStart;
    currentSegmentStart = getNextSegmentStart(receivedText.c_str(), currentSegmentStart, screenWidth);
    
    // 完整循环后重置
    if (currentSegmentStart == 0 && prevStart != 0) {
      needFullCycleReset = true;
    }
    
    // 执行实际切换
    if (needFullCycleReset) {
      currentSegmentStart = 0;
      needFullCycleReset = false;
      lastSegmentChange = currentTime;  // 重置计时器
    } else {
      lastSegmentChange = currentTime;
    }
    
    drawSegment(receivedText.c_str(), currentSegmentStart);
  }
  
  delay(100);
}