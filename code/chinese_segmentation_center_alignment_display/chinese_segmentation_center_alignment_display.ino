#include <U8g2lib.h>

// 定义屏幕对象（根据实际使用的屏幕和接口调整构造函数参数）
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /*SCL=*/0, /*SDA=*/1, /*RESET=*/U8X8_PIN_NONE);

// 要显示的中文长文本
const char *longText = "这是一个很长很长的中文文本，它将会在屏幕上分段显示。";

// 显示相关的全局变量
uint8_t fontHeight;         // 字体高度
uint8_t yPos;               // 文本在 Y 轴上的显示位置
const uint16_t screenWidth = 128; // OLED 屏幕宽度

// 控制段显示的时间（毫秒）
const unsigned long segmentDisplayTime = 2000;  // 每段显示 2 秒
unsigned long lastSegmentChange = 0;            // 上一次切换段的时间

// 记录当前段在文本中的起始字节索引
int currentSegmentStart = 0;

// 用于测量 UTF-8 字符占用的字节数
int utf8CharBytes(char c) {
  if ((c & 0x80) == 0) return 1;          // ASCII
  else if ((c & 0xE0) == 0xC0) return 2;    // 2 字节
  else if ((c & 0xF0) == 0xE0) return 3;    // 3 字节
  else if ((c & 0xF8) == 0xF0) return 4;    // 4 字节
  return 1; // 默认返回 1
}

// 根据当前起始位置和屏幕宽度，计算下一段的起始位置
int getNextSegmentStart(const char* text, int startIndex, int maxWidth) {
  int bytePos = startIndex;
  int segmentWidth = 0;
  int lastValidPos = bytePos;
  
  // 累计添加字符直到超出屏幕宽度
  while (text[bytePos] != '\0') {
    int charBytes = utf8CharBytes(text[bytePos]);
    char buffer[10] = {0};  // 临时存放单个字符，最多支持 4 字节编码
    for (int i = 0; i < charBytes; i++) {
      buffer[i] = text[bytePos + i];
    }
    int charWidth = u8g2.getUTF8Width(buffer);
    
    // 如果再加当前字符会超过最大宽度，则退出循环
    if (segmentWidth + charWidth > maxWidth) {
      break;
    }
    segmentWidth += charWidth;
    lastValidPos = bytePos + charBytes;
    bytePos += charBytes;
  }
  
  // 如果到达文本末尾，则下次从0开始
  if (text[bytePos] == '\0') {
    return 0;
  }
  return lastValidPos;
}

// 将当前段字符提取并显示到屏幕
void drawSegment(const char* text, int startIndex) {
  u8g2.clearBuffer();
  char segment[256] = {0};  // 存放本段字符串，注意长度根据文本长度自行调整
  int bytePos = startIndex;
  int segmentWidth = 0;
  int segIndex = 0;
  
  // 逐字符读取，直到累计宽度超过屏幕宽度或遇到字符串终结符
  while (text[bytePos] != '\0') {
    int charBytes = utf8CharBytes(text[bytePos]);
    char temp[10] = {0};
    for (int i = 0; i < charBytes; i++) {
      temp[i] = text[bytePos + i];
    }
    int charWidth = u8g2.getUTF8Width(temp);
    
    // 达到最大宽度就停止
    if (segmentWidth + charWidth > screenWidth) {
      break;
    }
    
    // 将当前字符复制到 segment 中
    for (int i = 0; i < charBytes; i++) {
      segment[segIndex++] = text[bytePos + i];
    }
    segmentWidth += charWidth;
    bytePos += charBytes;
  }
  segment[segIndex] = '\0';  // 末尾加上结束符
  
  // 将段文本显示在屏幕上，Y 坐标保持之前计算的值
  u8g2.setCursor(0, yPos);
  u8g2.print(segment);
  u8g2.sendBuffer();
}

void setup() {
  Serial.begin(115200);
  u8g2.begin();
  u8g2.enableUTF8Print();
  
  // 设置支持中文的字体
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  
  // 获取字体高度，计算 Y 坐标（确保文字不会超出屏幕）
  fontHeight = u8g2.getMaxCharHeight();
  //yPos = 32 - fontHeight; // OLED 分辨率 128x32
  yPos = (32 + fontHeight) / 2;
  Serial.print("Font Height: ");
  Serial.println(fontHeight);
  
  // 如果需要第一次直接显示部分文本，可在 setup 中调用 drawSegment()
  drawSegment(longText, currentSegmentStart);
}

void loop() {
  unsigned long currentTime = millis();
  
  // 判断是否到达切换段的时间
  if (currentTime - lastSegmentChange >= segmentDisplayTime) {
    lastSegmentChange = currentTime;
    
    // 根据当前段计算下段起始位置
    currentSegmentStart = getNextSegmentStart(longText, currentSegmentStart, screenWidth);
  }
  
  drawSegment(longText, currentSegmentStart);
}