#include <Wire.h>               // Only needed for Arduino 1.5 and earlier
#include "SSD1306Wire.h" 
// Initialize the OLED display
SSD1306Wire display(0x3c, 1, 0, GEOMETRY_128_32); 
bool showHelloWorld = true;     // 初始显示 "hello world"
int percentage = 1;             // 从 1% 开始
unsigned long lastUpdate = 0;   // 记录上次更新时间
const int updateInterval = 200; // 更新间隔（毫秒）
void setup() {
  Serial.begin(115200);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);
  
  // 初始显示 "hello world"
  display.clear();
  display.drawString(0, 0, "hello world");
  display.display();
  
  lastUpdate = millis(); // 记录初始时间
}
void loop() {
  unsigned long currentTime = millis();
  
  // 如果当前时间 - 上次更新时间 >= 间隔时间，则更新显示
  if (currentTime - lastUpdate >= updateInterval) {
    lastUpdate = currentTime;
    
    if (showHelloWorld) {
      // 显示 "hello world" 后，切换到百分比显示
      showHelloWorld = false;
    } else {
      // 更新百分比（1% ~ 100%）
      display.clear();
      display.drawString(0, 0, String(percentage) + "%");
      display.display();
      
      percentage++;
      if (percentage > 100) {
        percentage = 1; // 循环显示
      }
    }
  }
}
