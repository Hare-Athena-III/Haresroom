#include "ohos_init.h"
#include "cmsis_os2.h"
#include <stdio.h>

// 互斥锁ID（静态全局，限定作用域）
static osMutexId_t g_mutex_id = NULL;

/**
 * 互斥锁任务函数：循环获取→延迟→释放锁
 * @param arg：任务编号（区分任务1/2）
 */
void MutexTask(void *arg)
{
    uint32_t task_num = (uint32_t)arg;
    while (1) {
        printf("任务%d尝试获取锁\n", task_num);
        
        // 尝试获取互斥锁（永久阻塞，直到获取成功）
        osStatus_t status = osMutexAcquire(g_mutex_id, osWaitForever);
        if (status == osOK) {
            printf("任务%d尝试获取锁，获取成功\n", task_num);
            
            osDelay(500U);
            
            // 释放互斥锁
            osMutexRelease(g_mutex_id);
            printf("任务%d释放锁\n", task_num);
        } else {
            printf("任务%d尝试获取锁，获取失败\n", task_num);
        }
    }
}

/**
 * 实验初始化函数
 */
static void MutexExample(void)
{
    // 1. 创建互斥锁（初始状态为unlock）
    g_mutex_id = osMutexNew(NULL);
    if (g_mutex_id == NULL) {
        printf("锁创建失败！\n");
        return;
    }
    printf("锁创建成功，处于unlock状态\n");

    // 2. 定义任务属性
    osThreadAttr_t task_attr = {
        .stack_size = 1024 * 4,
        .priority = 25  // 两个任务优先级相同
    };

    // 3. 创建任务1
    task_attr.name = "MutexTask1";
    if (osThreadNew(MutexTask, (void*)1, &task_attr) != NULL) {
        printf("创建2个task，任务1启动成功\n");
    }

    // 4. 创建任务2
    task_attr.name = "MutexTask2";
    if (osThreadNew(MutexTask, (void*)2, &task_attr) != NULL) {
        printf("创建2个task，任务2启动成功\n");
    }
}

// OpenHarmony启动时自动执行实验
APP_FEATURE_INIT(MutexExample);