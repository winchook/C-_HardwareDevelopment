#include <Arduino.h>
#include <TM1637Display.h>

// 1. 定义数码管连接的引脚
#define CLK 3
#define DIO 2
#define BUTTON_PIN 4 // 按键连接到 D4

// 2. 初始化数码管对象
TM1637Display display(CLK, DIO);

// 3. 设定倒计时时间（在这里修改时间）
const int TARGET_HOURS = 3;   // 设定的目标小时
const int TARGET_MINUTES = 5; // 设定的目标分钟

long remainingSeconds = 0;    // 剩余的总秒数
bool isRunning = false;       // 倒计时是否已经启动

void setup() {
  // 初始化按键引脚：使用内部上拉电阻
  // 这样按键未按下时是高电平，按下时变成低电平
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // 设置数码管亮度 (0-7)，4属于中等亮度
  display.setBrightness(4);
  
  // 计算总秒数
  remainingSeconds = (TARGET_HOURS * 3600L) + (TARGET_MINUTES * 60L);
  
  // 初始状态：显示设定好的总时间（小时和分钟）
  // 显示格式如：03:05
  int displayTime = (TARGET_HOURS * 100) + TARGET_MINUTES;
  display.showNumberDecEx(displayTime, 0b01000000, true); 
}

void loop() {
  // 4. 检查按键是否被按下
  // 因为使用了 INPUT_PULLUP，当按键被按下时，读取到的信号是 LOW
  if (digitalRead(BUTTON_PIN) == LOW && !isRunning) {
    delay(50); // 软件去抖动，防止小孩子按按键时由于机械抖动触发两次
    if (digitalRead(BUTTON_PIN) == LOW) {
      isRunning = true; // 启动定时器！
    }
  }

  // 5. 如果定时器启动了，开始倒计时
  if (isRunning && remainingSeconds >= 0) {
    
    long hours = remainingSeconds / 3600;
    long minutes = (remainingSeconds % 3600) / 60;
    long seconds = remainingSeconds % 60;
    
    int dataToShow = 0;
    
    // 逻辑优化：如果时间还很长，显示 小时:分钟
    if (remainingSeconds > 60) {
      dataToShow = (hours * 100) + minutes;
    } 
    // 如果只剩最后一分钟，显示 分钟:秒数，这样更刺激好玩！
    else {
      dataToShow = (minutes * 100) + seconds;
    }
    
    // 在数码管上显示数字，0b01000000 代表点亮中间的冒号 (:)
    display.showNumberDecEx(dataToShow, 0b01000000, true);
    
    // 如果时间到了 00:00，停止倒计时
    if (remainingSeconds == 0) {
      isRunning = false;
      // 这里预留了给下一阶段蜂鸣器响的代码位置
      // 可以在这里让蜂鸣器发出声音
    } else {
      remainingSeconds--; // 减少1秒
    }
    
    delay(1000); // 严格等待 1 秒钟
  }
}
