#include <U8g2lib.h>
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /*SCL=*/0, /*SDA=*/1, /*RESET=*/U8X8_PIN_NONE);
// 要显示的中文长文本
const char *longText = "这是一个很长很长的中文文本，它将会在屏幕上水平滚动显示。";
// 滚动相关的变量
int16_t textWidth;        // 文本的实际宽度
int16_t scrollPosition = 0; // 当前滚动位置
unsigned long lastScrollTime = 0; // 上次滚动的时间
const unsigned long scrollInterval = 5; // 滚动间隔时间（毫秒） - 减少间隔
const int scrollIncrement = 5;       // 每次滚动的像素数 - 减少增量
uint8_t fontHeight;          //字体高度
uint8_t yPos;              //Y坐标
void setup() {
  Serial.begin(115200);
  u8g2.begin();
  u8g2.enableUTF8Print();
  // 使用更紧凑的字体，适合中文显示
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  // 获取字体实际高度并计算安全 Y 坐标
  fontHeight = u8g2.getMaxCharHeight();
  yPos = 32 - fontHeight; // 确保底部不超出屏幕
  Serial.print("Font Height: ");
  Serial.println(fontHeight);
  Serial.print("Y Position: ");
  Serial.println(yPos);
  // 计算文本的实际宽度
  textWidth = u8g2.getUTF8Width(longText);
  Serial.print("Text Width: ");
  Serial.println(textWidth);
}
void loop() {
  // 获取当前时间
  unsigned long currentTime = millis();
  // 检查是否需要滚动
  if (currentTime - lastScrollTime >= scrollInterval) {
    lastScrollTime = currentTime;
    // 更新滚动位置
    scrollPosition += scrollIncrement; // 使用增量
    // 如果滚动到文本末尾，则重置滚动位置
    if (scrollPosition > textWidth) {
      scrollPosition = 0;
    }
    // 重绘屏幕
    drawScrollingText();
  }
}
// 绘制滚动文本的函数
void drawScrollingText() {
  u8g2.clearBuffer();
  // 计算绘制文本的起始 X 坐标（负数表示文本部分在屏幕外）
  int16_t xPos = 0 - scrollPosition;
  // 绘制文本
  u8g2.setCursor(xPos, yPos);
  u8g2.print(longText);
  u8g2.sendBuffer();
}