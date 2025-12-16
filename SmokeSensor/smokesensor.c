#include <stdio.h>          // printf
#include <unistd.h>         // usleep / sleep
#include "cmsis_os2.h"      // osThreadNew / osThreadAttr_t
#include "ohos_init.h"
#include "iot_gpio.h"
#include "iot_gpio_ex.h"
#include "iot_pwm.h"
#include "iot_adc.h"
#include "iot_errno.h"

#define PWM_GPIO 2
#define PWM_GPIO_CH 2
#define SMOKE_ADC_GPIO 7
#define SMOKE_ADC_CHANNEL 3
#define ADC_VREF_VOL 1.8
#define ADC_COEFFICIENT 4 
#define ADC_RATIO 4096
#define ADC_TASK_DELAY_1S 1000000
#define BEER_WARNING_NUM 5
#define SMOKE_VOLTAGE 0.3
#define SMOKE_FREQ 4000
#define SMOKE_WARM_DELAY 30

static float GetVoltage(void)
{
    unsigned int ret;
    unsigned short data;

    ret = IoTAdcRead(SMOKE_ADC_CHANNEL,&data,IOT_ADC_EQU_MODEL_8,IOT_ADC_CUR_BAIS_DEFAULT,0xff);
    if (ret != IOT_SUCCESS){
        printf("ADC Read Fail\n");
    }

    return (float)data * ADC_VREF_VOL * ADC_COEFFICIENT / ADC_RATIO;
}

void SmokeWarning(int delay)
{
    printf("SmokeSensorTask Warning!\r\n");
    for (size_t i=0;i<delay;i++){
        IoTPwmStart(PWM_GPIO_CH,50,SMOKE_FREQ);
        usleep(ADC_TASK_DELAY_1S);
        IoTPwmStop(PWM_GPIO_CH);
        usleep(200000);
    }
}

static void *SmokeSensorTask(const char *arg)
{
    int Warning=0;
    printf("SmokeSensorTast start!\r\n");
    SmokeWarning(BEER_WARNING_NUM);
    printf("SmokeSensor warming wait %d S...\r\n",SMOKE_WARM_DELAY);
    for (size_t i =0;i<SMOKE_WARM_DELAY;i++){
        usleep(ADC_TASK_DELAY_1S);
    }
    while(1)
    {
        float voltage = GetVoltage();
        printf("vlt:%.3fV\n",voltage);
        if (voltage > SMOKE_VOLTAGE)
        {
            SmokeWarning(BEER_WARNING_NUM);
        }
        usleep(ADC_TASK_DELAY_1S);
    }

    return NULL;
}

static void StartSmokeSensorTask(void)
{
    osThreadAttr_t attr;
    IoTGpioSetPull(SMOKE_ADC_GPIO,IOT_GPIO_PULL_UP);
    IoTGpioInit(PWM_GPIO);
    IoTGpioSetFunc(PWM_GPIO,IOT_GPIO_FUNC_GPIO_2_PWM2_OUT);
    IoTGpioSetDir(PWM_GPIO,IOT_GPIO_DIR_OUT);
    IoTPwmInit(PWM_GPIO_CH);
    attr.name = "SmokeSensorTask";
    attr.attr_bits = 0U;
    attr.cb_mem=NULL;
    attr.cb_size=0U;
    attr.stack_mem=NULL;
    attr.stack_size=1024;
    attr.priority = osPriorityNormal;
    if(osThreadNew((osThreadFunc_t)SmokeSensorTask,NULL,&attr)==NULL){
        printf("[LedExample] Failed to create SmokeSensorTask!\n");
    }
}
SYS_RUN(StartSmokeSensorTask);
