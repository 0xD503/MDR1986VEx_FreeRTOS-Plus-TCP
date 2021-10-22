/**
  * This example demonstarates the work of FreeRTOS-Plus-TCP stack port for MCU
  * MDR1986VE1/3.
  * Two tasks were created:
  *     1)  UDP talker task - sends datagrams when receives notification from
  *         timer callback or listener task (loopback received packet)
  *     2)  UDP listener task - listens port for new datagrams and, when receives
  *         new datagram, sends notification to the talker task to loopback the
  *         received packet
  *
  * One software timer was created to send UDP datagrams periodically
  *
  * Versions:
  *     SPL - 1.5.1 with some additions
  *     FreeRTOS - FreeRTOS Kernel V10.4.3 (LTS)
  *     FreeRTOS+TCP V2.3.2
  *
  * Known issues of the port:
  *     - TCP sockets may not work or work incorrectly, because next sequence
  *     number generator is not implemented (use RFC to implement it)
  */


/// RTOS headers
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/// App specific headers
#include "app_includes.h"
#include "main.h"
#include "bsp.h"
#include "app_tasks.h"
#include "utils.h"



int main (int argc, char *argv[])
{
    prvSetupHardware(NULL);  /// init hardware/software


    /// create talker task
    xTaskCreate(vTalker_Task, "talker",
                (unsigned short) 128,
                NULL,                   /// no params
                MEDIUM_PRIORITY,        ///
                &xDataExchange_Handle);

    /// create listener task
    xTaskCreate(vListener_Task, "listenr",
                (unsigned short) 128,
                NULL,                   /// no params
                MEDIUM_PRIORITY,        ///
                &xOE_Listener_Handle);


    /// create timer to periodically send UDP packets
    xTx_Tmr = xTimerCreate("UDP_tmr",
                           pdMS_TO_TICKS(3000),
                           pdTRUE, NULL,//(tmrID *) (NULL????),
                           prvStandbyTxTimer_Callback);


    /// check that semaphores/timers were created
    if ((xTx_Tmr != NULL))
    {
        prvStartHardware(NULL);     /// enable peripheral devices

        if (xTimerStart(xTx_Tmr, 0) == pdPASS)   /// start standby timer
        {
            vTaskStartScheduler();  /// go into RTOS event loop

            while (true)
            {
                /// program shouldn't be here
            }
        }
    }
    else
    {
        while (true)
        {
            /// program shouldn't be here
        }
    }


    return (0);
}
