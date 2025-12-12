#include "ohos_init.h"
#include "cmsis_os2.h"
#include <stdio.h>

// 定义2个事件标志（模拟事件集的不同事件）
#define FLAGS_MSK1  0x00000001U  // 事件1标志
#define FLAGS_MSK2  0x00000002U  // 事件2标志

// 事件集ID（静态全局，限定作用域）
static osEventFlagsId_t g_evt_set_id = NULL;

/**
 * 任务1：事件发送方→循环发送不同事件
 */
void EventSenderTask(void *arg)
{
    (void)arg;
    uint32_t send_flag = FLAGS_MSK1; // 初始发送事件1

    while (1) {
        // 向事件集发送当前事件
        osEventFlagsSet(g_evt_set_id, send_flag);
        printf("任务1向事件集发送事件：0x%08x\n", send_flag);

        // 交替切换发送的事件（事件1→事件2→事件1...）
        send_flag = (send_flag == FLAGS_MSK1) ? FLAGS_MSK2 : FLAGS_MSK1;

        osDelay(300U); // 每3秒发送一次事件
    }
}

/**
 * 任务2：事件接收方→接收事件并解析执行逻辑
 */
void EventReceiverTask(void *arg)
{
    (void)arg;
    uint32_t recv_flags;

    printf("任务2等待事件集的事件...\n");
    while (1) {
        // 阻塞等待事件集（任意事件触发）
        recv_flags = osEventFlagsWait(
            g_evt_set_id,          // 事件集ID
            FLAGS_MSK1 | FLAGS_MSK2, // 等待的事件掩码（事件1或事件2）
            osFlagsWaitAny,        // 任意事件满足即可唤醒
            osWaitForever          // 永久阻塞等待
        );

        // 解析收到的事件，执行对应逻辑
        if (recv_flags & FLAGS_MSK1) {
            printf("任务2接收事件成功→解析到事件1，执行【逻辑A：打印事件1】\n");
        }
        if (recv_flags & FLAGS_MSK2) {
            printf("任务2接收事件成功→解析到事件2，执行【逻辑B：打印事件2】\n");
        }
    }
}

/**
 * 实验初始化函数
 */
static void EventSetExample(void)
{
    // 1. 创建事件集（初始值为空）
    g_evt_set_id = osEventFlagsNew(NULL);
    if (g_evt_set_id == NULL) {
        printf("事件集创建失败！\n");
        return;
    }
    printf("事件集创建成功，初始值为空\n");

    // 2. 定义任务属性
    osThreadAttr_t task_attr = {
        .stack_size = 1024 * 4,
        .priority = 25
    };

    // 3. 创建任务1（事件发送方）
    task_attr.name = "EventSenderTask";
    if (osThreadNew(EventSenderTask, NULL, &task_attr) != NULL) {
        printf("创建2个task，任务1启动成功\n");
    }

    // 4. 创建任务2（事件接收方）
    task_attr.name = "EventReceiverTask";
    if (osThreadNew(EventReceiverTask, NULL, &task_attr) != NULL) {
        printf("创建2个task，任务2启动成功\n");
    }
}

// OpenHarmony启动时自动执行实验
APP_FEATURE_INIT(EventSetExample);