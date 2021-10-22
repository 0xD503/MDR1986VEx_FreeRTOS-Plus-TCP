## FreeRTOS-Plus-TCP stack port for MCUs MDR1986VE1T/3

This repository contains the following ports for MCUs MDR1986VE1T/3 by Milandr:
    1) FreeRTOS Kernel V10.4.3 (LTS)
    2) FreeRTOS+TCP V2.3.2

The following example provided to demonstrate the work of the port:
    Two tasks were created:
	    1)  UDP talker task - sends datagrams when receives notification from
            timer callback or listener task (loopback received packet)
        2)  UDP listener task - listens port for new datagrams and, when receives
            new datagram, sends notification to the talker task to loopback the
            received packet

#  Known issues
    - TCP sockets may not work or work incorrectly, because next sequence number generator is not implemented (use RFC to implement it)


## Feel free to contribute by making port improvements or bugs fixes
