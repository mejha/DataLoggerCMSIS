// https://blog.feabhas.com/2013/02/developing-a-generic-hard-fault-handler-for-arm-cortex-m3cortex-m4/

#include <stdint.h>
#include "stm32f10x.h"
#include "core_cm3.h"

void printErrorMsg(const char *errMsg)
{
    while(*errMsg != '\0')
    {
        ITM_SendChar(*errMsg);
        ++errMsg;
    }
}

void HardFault_Handler(void)
{
    printErrorMsg("In Hard Fault Handler");
    __ASM volatile("BKPT #01");     // enter debug state
    while(1);
}

