#include <U8g2lib.h>
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /*SCL=*/0, /*SDA=*/1, /*RESET=*/U8X8_PIN_NONE);
void setup() {
  Serial.begin(115200);
  u8g2.begin();
  u8g2.enableUTF8Print();
  // 使用更紧凑的字体
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  // 获取字体实际高度并计算安全 Y 坐标
  uint8_t fontHeight = u8g2.getMaxCharHeight();
  // uint8_t yPos = 32 - fontHeight; // 确保底部不超出屏幕
  uint8_t yPos = (32 + fontHeight) / 2;
  Serial.print("Font Height: ");
  Serial.println(fontHeight);
  Serial.print("Y Position: ");
  Serial.println(yPos);
  u8g2.clearBuffer();
  u8g2.setCursor(0, yPos);
  u8g2.print("你好世界");
  u8g2.sendBuffer();
  
  delay(100);
}
void loop() {}
