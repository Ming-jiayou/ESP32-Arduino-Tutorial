# ESP32+Arduino入门（一）：开发环境搭建

## 前言

作为一名硬件小白，在动手组装了一下小智AI之后，感觉还挺有意思的，也想开始学习ESP32了。

![](https://mingupupup.oss-cn-wuhan-lr.aliyuncs.com/imgs/%E5%B0%8F%E6%99%BAAI%E7%BB%84%E8%A3%85%E5%AE%8C%E6%88%90%E5%9B%BE.jpg)

## ESP32介绍

ESP32 是乐鑫科技（Espressif Systems）推出的一款高性能、低功耗的 Wi-Fi 和蓝牙双模微控制器芯片，广泛应用于物联网（IoT）、智能家居、可穿戴设备等领域。

经过调研推荐小白使用Arduino入门。

## Arduino介绍

Arduino 是一个基于易于使用的硬件和软件的开源电子平台。Arduino 开发板能够读取输入——例如传感器上的光线、按钮上的手指或 Twitter 消息——并将其转换为输出——例如激活电机、点亮 LED 或在线发布内容。你可以通过向开发板上的微控制器发送一组指令来告诉你的开发板做什么。为此，你使用基于 Wiring 的 Arduino 编程语言，以及基于 Processing 的 Arduino 软件（IDE）。

多年来，Arduino 已成为数千个项目的大脑，从日常物品到复杂的科学仪器。全球范围内的创客社区——包括学生、业余爱好者、艺术家、程序员和专业人士——围绕这个开源平台聚集，他们的贡献积累了大量的可访问知识，对初学者和专家都非常有帮助。

Arduino 诞生于伊夫雷亚交互设计学院，作为一种快速原型制作的简易工具，面向没有电子和编程背景的学生。当它被更广泛的社区采用时，Arduino 开发板开始改变以适应新的需求和挑战，从简单的 8 位开发板发展到适用于物联网应用、可穿戴设备、3D 打印和嵌入式环境的产品。

开始使用Arduino IDE，下载地址：https://www.arduino.cc/en/software

![image-20250409140157515](https://mingupupup.oss-cn-wuhan-lr.aliyuncs.com/imgs/image-20250409140157515.png)

下载完成之后打开如下所示：

![image-20250409140253263](https://mingupupup.oss-cn-wuhan-lr.aliyuncs.com/imgs/image-20250409140253263.png)

如果是英文的，在文件——》首选项中设置为中文：

![image-20250409140602758](https://mingupupup.oss-cn-wuhan-lr.aliyuncs.com/imgs/image-20250409140602758.png)

现在需要安装一下对应的开发板。

打开工具——》开发板——》开发板管理器。

安装这个：

![image-20250409140729954](https://mingupupup.oss-cn-wuhan-lr.aliyuncs.com/imgs/image-20250409140729954.png)

我这里一直安装不上，最后是直接通过网盘下载这个进行安装的。

网盘地址：https://pan.baidu.com/s/1RZkrNGdLaF2psiLQD174yg

提取码：v2ey

下载完成如下所示：

![image-20250409141106074](https://mingupupup.oss-cn-wuhan-lr.aliyuncs.com/imgs/image-20250409141106074.png)

直接打开。

将开发板连接电脑，如图所示，忽略OLED屏和接线：

![](https://mingupupup.oss-cn-wuhan-lr.aliyuncs.com/imgs/ESP32%E8%BF%9E%E6%8E%A5%E7%94%B5%E8%84%91.jpg)

然后选择对应的开发板与端口：

![](https://mingupupup.oss-cn-wuhan-lr.aliyuncs.com/imgs/%E9%80%89%E6%8B%A9%E5%BC%80%E5%8F%91%E6%9D%BF%E5%92%8C%E7%AB%AF%E5%8F%A3.gif)

开始写第一个程序：

```c
void setup() 
{ 
  Serial.begin(115200); 
  Serial.println("Hello world!");
}

void loop() 
{
  Serial.println("Hello world!");
  delay(2000);
}
```

![image-20250409143016541](https://mingupupup.oss-cn-wuhan-lr.aliyuncs.com/imgs/image-20250409143016541.png)

点击这个箭头就会将程序编译并烧录到开发板上。

现在打开串口监视器：

![image-20250409143134296](https://mingupupup.oss-cn-wuhan-lr.aliyuncs.com/imgs/image-20250409143134296.png)

如下所示：

![](https://mingupupup.oss-cn-wuhan-lr.aliyuncs.com/imgs/%E7%AC%AC%E4%B8%80%E4%B8%AA%E7%A8%8B%E5%BA%8F%E6%95%88%E6%9E%9C.gif)

至此就完成了第一个程序的编写了。

set up函数只会执行一次。

loop函数会一直执行。

以上就完成了ESP32入门开发环境的搭建。

希望对开发ESP32感兴趣的朋友有所帮助。
