/*
 * Watchdog Example
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_watchdog.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/watchdog.h"

bool g_bWatchDogReset;
volatile bool g_bWatchdogFeed = 1;

//*****************************************************************************
// Watchdog Functions
//
// The interrupt handler for the watchdog.  This feeds the dog (so that the
// processor does not get reset) and winks the LED connected to GPIO F2.
//
//*****************************************************************************
void WatchdogIntHandler(void)
{
    // Clear the watchdog interrupt.
    if (g_bWatchdogFeed)
    {
        WatchdogIntClear(WATCHDOG0_BASE);
    }
}

int main(void)
{
    // Set the clocking to run directly from the crystal.
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    //*****************************************************************************
    // GPIO Setup
    //*****************************************************************************

    // Enable Peripheral Clocks
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Configure the GPIO Pin Mux for PF4
    // for GPIO_PF4
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Configure the GPIO Pin Mux for PF2
    // for GPIO_PF2
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);

    // Enable processor interrupts.
    IntMasterEnable();

    //*****************************************************************************
    // Watchdog Setup
    //*****************************************************************************

    // Enable the watchdog peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG0);

    // Wait for the Watchdog 0 module to be ready.
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_WDOG0)) {}

    // Enable the watchdog interrupt
    IntEnable(INT_WATCHDOG);

    // Check to see if the registers are locked, and if so, unlock them
    if(WatchdogLockState(WATCHDOG0_BASE) == true)
    {
        WatchdogUnlock(WATCHDOG0_BASE);
    }

    // Enable Watchdog Interrupts
    WatchdogIntEnable(WATCHDOG0_BASE);
    WatchdogIntTypeSet(WATCHDOG0_BASE, WATCHDOG_INT_TYPE_INT);

    // Set the period of the watchdog timer for 1 s (2 s to reset)
    WatchdogReloadSet(WATCHDOG0_BASE, SysCtlClockGet()/3);

    // Enable the processor reset if watchdog is not fed (feeding is clearing interrupt)
    WatchdogResetEnable(WATCHDOG0_BASE);

    // Prevent changes to the setup values
    WatchdogLock(WATCHDOG0_BASE);

    // Enable Watchdog
    WatchdogEnable(WATCHDOG0_BASE);

    //*****************************************************************************
    // Main Code
    //*****************************************************************************

    // Blink light on boot
    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,GPIO_PIN_2);
    SysCtlDelay(SysCtlClockGet()/150); // Delay 20 ms
    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,0);

    // Get cause for last reset
    uint32_t ui32ResetCause = SysCtlResetCauseGet();
    SysCtlResetCauseClear(ui32ResetCause);

    // If reset due to watchdog, set flag
    if ( SYSCTL_CAUSE_WDOG == (ui32ResetCause & SYSCTL_CAUSE_WDOG) )
    {
        g_bWatchDogReset = 1;
    }

    while(1) // Runs this code repeatedly (called polling)
    {
        if ( GPIO_PIN_4 != GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4) )
        {
            g_bWatchdogFeed = 0;
        }
    }
}
