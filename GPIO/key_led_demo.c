#include <stdio.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h" // OpenHarmony 标准 GPIO 接口
#include "hi_io.h"    // Hi3861 原生 IO 接口 (用来设置上拉)

#define MY_INFO "Name: 张铭, ID: 2023192004"

#define LED_GPIO 9
#define KEY_GPIO 5

void KeyLedEntry(void) {
    // 1. 初始化 GPIO (使用标准接口)
    IoTGpioInit(LED_GPIO);
    IoTGpioInit(KEY_GPIO);

    // 2. 配置 LED 为输出 (使用标准接口)
    IoTGpioSetDir(LED_GPIO, IOT_GPIO_DIR_OUT);

    // 3. 配置按键为输入 (使用标准接口)
    IoTGpioSetDir(KEY_GPIO, IOT_GPIO_DIR_IN);

    // 4. 【关键】开启按键的上拉电阻 (使用原生接口 hi_io_set_pull)
    // 如果没有这一步，按键读数会乱跳
    hi_io_set_pull(KEY_GPIO, HI_IO_PULL_UP);

    printf("KeyLed Task Started! Waiting for key press...\n");

    IotGpioValue val = IOT_GPIO_VALUE1;
    IotGpioValue lastVal = IOT_GPIO_VALUE1;
    static int ledState = 0;

    while (1) {
        // 获取按键状态 (0表示按下，1表示松开)
        IoTGpioGetInputVal(KEY_GPIO, &val);

        // 检测下降沿 (上一次是1，这一次是0，说明刚刚按下)
        if (val == IOT_GPIO_VALUE0 && lastVal == IOT_GPIO_VALUE1) {
            
            // 翻转 LED
            ledState = !ledState;
            IoTGpioSetOutputVal(LED_GPIO, ledState);

            // 打印作业信息
            printf("Key Pressed! %s\n", MY_INFO);

            // 延时消抖
            usleep(200000); 
        }

        lastVal = val;
        usleep(10000); // 释放 CPU
    }
}

SYS_RUN(KeyLedEntry);