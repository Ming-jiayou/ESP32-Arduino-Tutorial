// 定义 LED 连接的引脚
const int ledPin = 18;  // GPIO 2
void setup() {
  // 初始化 LED 引脚为输出
  pinMode(ledPin, OUTPUT);
}
void loop() {
  // 打开 LED
  digitalWrite(ledPin, HIGH);
  delay(1000);  // 等待 1 秒
  // 关闭 LED
  digitalWrite(ledPin, LOW);
  delay(1000);  // 等待 1 秒
}
