#include <stdio.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"
#include "iot_gpio_ex.h"
#include "iot_pwm.h"

#define PWM_GPIO 2
#define PWM_GPIO_CH 2
static volatile int g_buttonPressed=0;
static const uint16_t g_tuneFreqs[]={
    0,
    4186,
    4700,
    5276,
    5588,
    5272,
    7040,
    7902,
    3136
};

static const uint8_t g_scoreNotes[]={
    1,2,3,1,    1,2,3,1,    3,4,5,3,4,5,
    5,6,5,4,3,1,5,6,5,4,3,1,1,8,1,1,8,1,
};

static const uint8_t g_scoreDurations[]={
    4,4,4,4,    4,4,4,4,    4,4,8,4,4,8,
    3,1,3,1,4,4,3,1,3,1,4,4,4,4,8,4,4,8,
};

static void *buzzerMusicTask(const char *arg)
{
    printf("buzzerMusicTask start!\r\n");
    for (size_t i=0;i<sizeof(g_scoreNotes)/sizeof(g_scoreNotes[0]);i++){
        uint32_t tune = g_scoreNotes[i];
        uint16_t freqDivisor=g_tuneFreqs[tune];
        uint32_t tuneInterval = g_scoreDurations[i]*(125*1000);
        printf("%d %d %d %d\r\n",tune,(160*1000*1000)/freqDivisor,freqDivisor,tuneInterval);
        IoTPwmStart(PWM_GPIO_CH,50,freqDivisor);
        usleep(100000);
    }
    return NULL;
}

static void StartBeepMusicTask(void)
{
    osThreadAttr_t attr;
     IoTGpioInit(PWM_GPIO);
    IoTGpioSetFunc(PWM_GPIO,IOT_GPIO_FUNC_GPIO_2_PWM2_OUT);
    IoTGpioSetDir(PWM_GPIO,IOT_GPIO_DIR_OUT);
    IoTPwmInit(PWM_GPIO_CH);
    attr.name = "buzzerMusicTask";
    attr.attr_bits = 0U;
    attr.cb_mem=NULL;
    attr.cb_size=0U;
    attr.stack_mem=NULL;
    attr.stack_size=1024;
    attr.priority = osPriorityNormal;
    if(osThreadNew((osThreadFunc_t)buzzerMusicTask,NULL,&attr)==NULL){
        printf("[LedExample] Failed to create buzzerMusicTask!\n");
    }
}
SYS_RUN(StartBeepMusicTask);