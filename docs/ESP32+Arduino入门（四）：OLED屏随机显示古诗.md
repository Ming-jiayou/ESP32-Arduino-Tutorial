# ESP32+Arduino入门（四）：OLED屏随机显示古诗

## 前言

我觉得去做一些简单又好玩的案例是入门很好的选择。

在实践的过程中会碰到很多需求很多问题在解决这些需求这些问题的过程就是在学习的过程。

今天我来分享一个随机显示古诗的案例，如果对此感兴趣可以跟我一起动手实践起来。

案例效果：

![](https://mingupupup.oss-cn-wuhan-lr.aliyuncs.com/imgs/%E6%98%BE%E7%A4%BA%E5%8F%A4%E8%AF%97%E6%95%88%E6%9E%9C.jpg)

## 拆解任务

现在我们先来拆解一下需求。

就是向一个web服务发送一个Get请求获取数据然后将数据显示在OLED屏上，为了效果更好采用分段显示。

![image-20250411113018703](https://mingupupup.oss-cn-wuhan-lr.aliyuncs.com/imgs/image-20250411113018703.png)

首先我们的OLED屏需要连接到一个OLED屏，这个在教程（二）中写了。

[ESP32+Arduino入门教程（二）：连接OLED屏](https://mp.weixin.qq.com/s/MK8S8LtWYq3afJ4Ucid7Ag)

如何输出古诗或者其他内容呢？

需要创建一个Web服务，然后随便接入一个大模型，让其输出古诗或其他内容再返回。

就算你从来没有写过Web服务也没关系，这个教程使用Python来创建一个非常简单的Web服务，小白也能实现。

在Arduino中编写代码，ESP32连接WIFI然后发送一个Get请求获取结果，将结果分段显示在OLED屏上。

搭建开发环境在教程（一）中写了。

[ESP32+Arduino入门（一）：开发环境搭建](https://mp.weixin.qq.com/s/BabUbbM0VmIhE2fXzfYTXg)

## 创建Web服务

创建一个Python虚拟环境，安装一下flask与openai。

写一个app.py如下所示：

```python
from flask import Flask
from openai import OpenAI

app = Flask(__name__)

client = OpenAI(api_key="你的api key", 
                base_url="https://api.siliconflow.cn/v1")



@app.route('/OutputClassicalPoetry', methods=['GET'])
def output_classical_poetry():
    response = client.chat.completions.create(
    model="internlm/internlm2_5-20b-chat",
    messages=[
        {'role': 'user', 
        'content': "输出一句古诗。"}
    ],
    stream=False,
    temperature=0.7
    )
    response_text = response.choices[0].message.content
    return response_text

if __name__ == '__main__':
    app.run(host='0.0.0.0', debug=True)
```

运行之后，查看一下自己的ip地址。

打开cmd输入ipconfig：

![image-20250411175926507](https://mingupupup.oss-cn-wuhan-lr.aliyuncs.com/imgs/image-20250411175926507.png)

打开浏览器，输入`http://你的ip地址:5000/OutputClassicalPoetry`，即可看到如下的效果：

![image-20250411180147479](https://mingupupup.oss-cn-wuhan-lr.aliyuncs.com/imgs/image-20250411180147479.png)

就说明这个简单的Web服务已经搞定了。

## ESP32连接WIFI并发送Get请求

代码如下：

```c
#include <WiFi.h>
#include <HTTPClient.h>
const char* ssid = "你的WIFI名称";
const char* password = "你的WIFI密码";
const char* serverUrl = "上面的接口地址";
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
```

电脑需要连接同一个WIFI。

烧录到ESP32运行。

现在打开串口监视器查看效果：

![image-20250411181807720](https://mingupupup.oss-cn-wuhan-lr.aliyuncs.com/imgs/image-20250411181807720.png)

## 分段显示在OLED屏

代码如下：

```c
#include <WiFi.h>
#include <HTTPClient.h>
#include <U8g2lib.h>
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, 0, 1, U8X8_PIN_NONE);
// WiFi配置
const char* ssid = "你的WIFI名称";
const char* password = "你的WIFI密码";
const char* serverUrl = "上面的接口地址";
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
  // 每60秒获取数据
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
```

烧录到ESP32运行即可得到上面的效果。

```c
 // 每60秒获取数据
  if (currentTime - lastFetchTime >= 60000) {
    lastFetchTime = currentTime;
    fetchTextFromServer();
  }
```

这里可以更改获取数据的时间。















































