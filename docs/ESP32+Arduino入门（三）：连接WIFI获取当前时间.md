# ESP32+Arduino入门（三）：连接WIFI获取当前时间

ESP32内置了WIFI模块连接WIFI非常简单方便。

代码如下：

```c
#include <WiFi.h>

const char* ssid = "WIFI名称";
const char* password = "WIFI密码";

void setup()
{
    Serial.begin(115200);
    WiFi.begin(ssid,password);
    while(WiFi.status() != WL_CONNECTED){
        // statement
        delay(500);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("Connected to the WiFi network");

}

void loop()
{
    
}
```

本次分享的是在OLED屏上显示当前时间的案例。

需要用到NTPClient。

需要先安装一下这个库。

![image-20250410151043063](https://mingupupup.oss-cn-wuhan-lr.aliyuncs.com/imgs/image-20250410151043063.png)

NTPClient 是一个客户端程序，它使用网络时间协议（NTP）从网络时间服务器获取当前时间。 

网络时间协议（NTP）：NTP 是一个互联网协议，用于在计算机系统之间同步时钟。

```c
NTPClient timeClient(ntpUDP, "pool.ntp.org", 28800, 60000); // GMT+8 时区, 更新间隔 60 秒
```

- `pool.ntp.org` 是一个公共 NTP 服务器池，它会将您的请求重定向到附近的 NTP 服务器。 这是一个方便且常用的选择，因为您不需要选择特定的 NTP 服务器。
- **`28800`**:  这是一个整数，表示时区偏移量，单位为秒。 在本例中，`28800` 秒等于 8 小时 (28800 / 3600 = 8)。 这表明代码被配置为使用 GMT+8 时区，例如中国标准时间 (CST)。
- **`60000`**:  这是一个整数，表示时间更新的间隔，单位为毫秒。 在本例中，`60000` 毫秒等于 60 秒。 这意味着 `timeClient` 对象配置为每 60 秒从 NTP 服务器更新一次时间。

还需要安装一个Time库。

![image-20250410151431919](https://mingupupup.oss-cn-wuhan-lr.aliyuncs.com/imgs/image-20250410151431919.png)



在 **Arduino** 环境中，Time是一个轻量级的库，主要用于处理 时间戳和 日期时间的计算、格式化和同步。

全部代码如下：

```c
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
```

效果：

![](https://mingupupup.oss-cn-wuhan-lr.aliyuncs.com/imgs/%E6%98%BE%E7%A4%BA%E6%97%B6%E9%97%B4.jpg)