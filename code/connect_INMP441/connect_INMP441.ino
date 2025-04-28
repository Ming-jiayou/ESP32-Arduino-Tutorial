#include <driver/i2s.h>

// 定义I2S引脚
#define I2S_WS  15
#define I2S_SD  16
#define I2S_SCK 14

// I2S配置参数
const i2s_port_t I2S_PORT = I2S_NUM_0;
const int SAMPLE_RATE = 44100;  // 采样率
const int BUFFER_SIZE = 1024;   // 缓冲区大小

void setup() {
  Serial.begin(115200);
  
  // 配置I2S
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_24BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = BUFFER_SIZE,
    .use_apll = false
  };

  // 设置引脚
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,  // 不使用输出
    .data_in_num = I2S_SD
  };

  // 初始化I2S
  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);
}
void loop() {
  int32_t samples[BUFFER_SIZE];
  size_t bytes_read;
  
  // 读取I2S数据
  i2s_read(I2S_PORT, &samples, BUFFER_SIZE * sizeof(int32_t), &bytes_read, portMAX_DELAY);
  
  // 打印原始数据（示例）
  for (int i = 0; i < bytes_read / sizeof(int32_t); i++) {
    Serial.println(samples[i]);
  }
}