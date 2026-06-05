#include <Arduino.h>
#include <TM1637Display.h>

// 1. 定义引脚（保持不变）
#define CLK 3
#define DIO 2
#define BUTTON_PIN 4 

TM1637Display display(CLK, DIO);

// 2. 设定倒计时时间：1分钟（一共 60 秒）
const long TOTAL_SECONDS = 60; 
long remainingSeconds = TOTAL_SECONDS;

// 3. 状态记录变量（核心升级）
bool isRunning = false;        // 记录当前是“正在倒计时”还是“暂停”
bool lastButtonState = HIGH;   // 记录上一次按键的状态（用来检测松开和按下的瞬间）
unsigned long lastTickTime = 0;// 用来精准记录时间，代替容易卡死的 delay(1000)

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  display.setBrightness(4);
  
  // 初始状态显示：01:00（1分钟）
  showTime(remainingSeconds);
}

void loop() {
  // --- 需求 1：按键控制 启动 / 暂停 ---
  int currentButtonState = digitalRead(BUTTON_PIN);

  // 检测按键被按下的“那一瞬间”（从高电平变成低电平）
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    delay(50); // 消除按键机械抖动
    if (digitalRead(BUTTON_PIN) == LOW) {
      
      // 核心魔术：状态反转！
      // 如果原来是运行(true)，就变成暂停(false)；反之亦然。
      isRunning = !isRunning; 
      
      // 如果时间已经结束了，再次按下就复位到 1 分钟，方便重复玩
      if (remainingSeconds <= 0) {
        remainingSeconds = TOTAL_SECONDS;
        isRunning = true;
      }
    }
  }
  // 记录这次的状态，留给下一次循环对比
  lastButtonState = currentButtonState;


  // --- 需求 2：实时秒数倒计时 ---
  // 如果是启动状态，且时间没到 0
  if (isRunning && remainingSeconds > 0) {
    
    // 检查是否过去了 1 秒钟（1000毫秒）
    // 这种写法比 delay(1000) 好一万倍，因为在等待的 1 秒内，大脑还能随时响应你的“暂停”按键！
    if (millis() - lastTickTime >= 1000) {
      remainingSeconds--;   // 减去 1 秒
      showTime(remainingSeconds); // 刷新屏幕显示
      lastTickTime = millis();    // 重新计时下 1 秒
    }
  }

  // 如果时间到了 00:00，自动停止
  if (remainingSeconds == 0 && isRunning) {
    isRunning = false;
    // 【下一阶段提示】：以后可以在这里让蜂鸣器大声响起来！
  }
}

// 4. 专门用来把“总秒数”转换成“分:秒”并显示出来的魔法函数
void showTime(long totalSecs) {
  long minutes = totalSecs / 60;
  long seconds = totalSecs % 60;
  
  // 组合成 4 位数字，比如 1分5秒 变成 0105
  int dataToShow = (minutes * 100) + seconds;
  
  // 显示在屏幕上，0b01000000 代表点亮中间的冒号 (:)
  display.showNumberDecEx(dataToShow, 0b01000000, true);
}