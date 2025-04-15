#include <Wire.h>
#include "SSD1306Wire.h"
SSD1306Wire display(0x3c, 1, 0, GEOMETRY_128_32); 
String longText = "This is a very long text that cannot fit on the screen at once. It will scroll horizontally.";
int textPosition = 0;  // 当前滚动位置
unsigned long lastScrollTime = 0;
const int scrollDelay = 150;  // 滚动速度（毫秒）
void setup() {
  Serial.begin(115200);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);  // 使用小字体以显示更多内容
}
void loop() {
  if (millis() - lastScrollTime >= scrollDelay) {
    lastScrollTime = millis();
    
    display.clear();
    display.drawString(-textPosition, 0, longText);  // 负坐标实现左滚动
    display.display();
    
    textPosition++;  // 每次移动1像素
    
    // 如果文本完全滚出屏幕，重置位置
    if (textPosition > display.getStringWidth(longText)) {
      textPosition = 0;
    }
  }
}
