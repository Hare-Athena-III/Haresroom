#include "ohos_init.h"
#include "cmsis_os2.h"
#include <stdio.h>

// 信号量ID（静态全局，避免作用域污染）
static osSemaphoreId_t g_sem_id = NULL;

/**
 * 信号量任务函数：尝试获取信号量→成功后延迟5秒→释放信号量→循环
 * @param arg：任务编号（区分任务1~4）
 */
void SemaphoreTask(void *arg)
{
    uint32_t task_num = (uint32_t)arg; // 提取任务编号

    while (1) {
        // 尝试获取信号量（阻塞等待，直到成功）
        osStatus_t status = osSemaphoreAcquire(g_sem_id, osWaitForever);
        if (status == osOK) {
            printf("任务%d尝试获取信号量，获取成功\n", task_num);
            
            // 延迟5秒（CMSIS-RTOS2中1tick=1ms，5000tick=5秒）
            osDelay(500U);
            
            // 释放信号量（计数+1）
            osSemaphoreRelease(g_sem_id);
            printf("任务%d释放信号量\n", task_num);
        } else {
            // 因超时设为osWaitForever，此分支一般不会执行（除非信号量被销毁）
            printf("任务%d获取信号量失败\n", task_num);
        }
    }
}

/**
 * 实验初始化函数：创建信号量+4个任务
 */
static void SemaphoreExample(void)
{
    printf("2023192004 张铭");
    // 1. 创建信号量：最大计数2，初始计数2
    g_sem_id = osSemaphoreNew(2, 2, NULL);
    if (g_sem_id == NULL) {
        printf("信号量创建失败\n");
        return;
    }
    printf("信号量创建成功，初始值为2\n");

    // 2. 定义任务属性（4个任务共用相同栈大小、优先级）
    osThreadAttr_t task_attr = {
        .stack_size = 1024 * 4,  // 栈大小（足够运行基础逻辑）
        .priority = 25,          // 任务优先级（4个任务优先级相同）
    };

    // 3. 创建4个任务（传递不同编号区分任务）
    // 任务1
    task_attr.name = "SemaphoreTask1";
    if (osThreadNew(SemaphoreTask, (void*)1, &task_attr) != NULL) {
        printf("创建4个task，任务1启动成功\n");
    }
    // 任务2
    task_attr.name = "SemaphoreTask2";
    if (osThreadNew(SemaphoreTask, (void*)2, &task_attr) != NULL) {
        printf("创建4个task，任务2启动成功\n");
    }
    // 任务3
    task_attr.name = "SemaphoreTask3";
    if (osThreadNew(SemaphoreTask, (void*)3, &task_attr) != NULL) {
        printf("创建4个task，任务3启动成功\n");
    }
    // 任务4
    task_attr.name = "SemaphoreTask4";
    if (osThreadNew(SemaphoreTask, (void*)4, &task_attr) != NULL) {
        printf("创建4个task，任务4启动成功\n");
    }
}

// OpenHarmony初始化宏：启动时自动执行实验逻辑
APP_FEATURE_INIT(SemaphoreExample);