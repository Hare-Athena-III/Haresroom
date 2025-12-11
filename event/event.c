#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "los_event.h"
#include "los_task.h"
#include "ohos_init.h"
#include "cmsis_os2.h"

#define FLAGS_MSK1 0x00000001U
osEventFlagsId_t evt_id;

void Thread_EventSender(void *argument)
{
    (void)argument;
    while(1)
    {
        printf("Example_TaskEntry write event.\n");
        osEventFlagsSet(evt_id, FLAGS_MSK1);

        osDelay(500U);
    }
}

void Thread_EventReceiver(void *argument)
{
    (void)argument;
    uint32_t flags;
    printf("ExampleEvent wait event 0x%x \n", FLAGS_MSK1);
    while(1)
    {
        flags = osEventFlagsWait(evt_id, FLAGS_MSK1, osFlagsWaitAny, osWaitForever);
        printf("Example_Event,read event :0x%x\n",flags);

    }
}

static void Event_example(void)
{
    evt_id = osEventFlagsNew(NULL);
    if (evt_id == NULL)
    {
        printf("Failed to create EventFlags!\n");
    }

    osThreadAttr_t attr;

    attr.attr_bits = 0U;
    attr.cb_mem= NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 4;
    attr.priority= 25;

    attr.name = "Thread_EventSender";
    if (osThreadNew(Thread_EventSender, NULL , &attr) == NULL)
    {
        printf("Failed to create Thread_EventSender!\n");
    }
    attr.name = "Thread_EventReceiver";
     if (osThreadNew(Thread_EventReceiver, NULL , &attr) == NULL)
    {
        printf("Failed to create Thread_EventReceiver!\n");
    }
}

APP_FEATURE_INIT(Event_example);