# Tiva Launchpad Watchdog Example

This application was built using:

 - Code Composer 10.0.0.00010 
 - Tivaware 2.1.4.178 with 2.1.4.178.PATCH
 - PinMux Tool 4.0.1537

Watchdog timer creates interrupt every second.  The watchdog is fed by clearing the interrupt.  If SW1 is pressed, then feeding the interrupt stops.  At the next watchdog interrupt after the the watchdog has not been fed, the microcontroller resets.  It happens about 2 seconds after SW1 is pressed.