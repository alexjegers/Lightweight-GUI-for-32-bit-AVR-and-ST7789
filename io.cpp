/*
 * io.cpp
 *
 * Created: 10/16/2022 1:48:34 PM
 *  Author: AJ992
 */ 

#include <avr32/io.h>
#include <stdint.h>
#include "system.h"
/*
Function:
Params:
Returns:
Description:
*/


/*
Function: ioSetPinIO
Params: port: IO port A or B.
		pin: which pin within the port.
Returns: none
Description: sets pin x in port x to be controlled by the IO and not 
			a peripheral.
*/
void ioSetPinIO(avr32_gpio_port_t* port, uint32_t pin)
{
	port->gpers = pin;
}

/*
Function: ioSetPinPeripheral
Params: port: IO port A or B.
		pin: which pin within the port.
Returns: none
Description: sets pin x in port x to be overridden by a peripheral.
*/
void ioSetPinPeripheral(avr32_gpio_port_t* port, uint32_t pin)
{
	port->gperc = pin;
}

/*
Function: ioSetPeripheralFunction
Params: port: IO port A or B
		pin: pin within the port (ONLY ONE PIN)
		function: function A,B,C,or D from GPIO_PMR_FUNCTION_x.
Returns: none
Description: sets a specific pin to a specified multiplexing function,
			can only pass one pin at a time, i.e. pin = 1, NOT pin = 1 | 2.
*/
void ioSetPeripheralFunction(avr32_gpio_port_t* port, uint32_t pin, uint8_t function)
{
	port->pmr0c = pin;
	port->pmr1c = pin;
	port->pmr0s = (function << pin) & pin;
	port->pmr1s = (function << (pin - 1)) & pin;
}

/*
Function: ioSetPinOutput
Params: port: IO port A or B.
		pin: pin within port.
Returns: none
Description: sets pin as an output, enables output driver.
*/
void ioSetPinOutput(avr32_gpio_port_t* port, uint32_t pin)
{
	port->oders = pin;
}

/*
Function: ioSetPinInput
Params: port: IO port A or B.
		pin: pin within port.
Returns: none
Description: sets pin as an input.
*/
void ioSetPinInput(avr32_gpio_port_t* port, uint32_t pin)
{
	port->oderc = pin;
}

/*
Function: ioPinHigh
Params: port: IO port A or B.
		pin: pin within port.
Returns: none
Description: drives pin x high.
*/
void ioPinHigh(avr32_gpio_port_t* port, uint32_t pin)
{
	port->ovrs = pin;
}

/*
Function: ioPinLow
Params: port: IO port A or B.
		pin: pin within port.
Returns: none
Description: drives pin x low.
*/
void ioPinLow(avr32_gpio_port_t* port, uint32_t pin)
{
	port->ovrc = pin;
}

/*
Function: ioReadPort
Params: port: port A or B
Returns: the level of all the pins in selected port.
Description: see returns.
*/

uint32_t ioReadPort(avr32_gpio_port_t* port)
{
	return port->pvr;
}

/*
Function: ioEnableInterrupt
Params: port: port A or B.
		pin: which pin within the port to enable the interrupt on.
Returns: none
Description: enables interrupt on certain pin(s).
*/
void ioEnableInterrupt(avr32_gpio_port_t* port, uint32_t pin)
{
	port->iers = pin;
}

/*
Function: ioDisableInterrupt
Params: port: port A or B.
		pin: which pin within the port to disable the interrupt on.
Returns: none
Description: disables interrupt on certain pin(s).
*/
void ioDisableInterrupt(avr32_gpio_port_t* port, uint32_t pin)
{
	port->ierc = pin;
}

/*
Function: ioInterruptMode
Params: port: Port A or B.
		pin: which pin within the port.
		intMode: selects which edge or both to fire the interrupt.
Returns: none
Description: sets the interrupt mode for a certain pin.
*/
void ioInterruptMode(avr32_gpio_port_t* port, uint8_t intMode, uint32_t pin)
{
	port->imr0c = pin;
	port->imr1c = pin;
	if ((intMode & 1) == 1)
	{
		port->imr0s = pin;
	}
	if ((intMode & 2) == 2)
	{
		port->imr1s = pin;
	}
}

/*
Function: ioClearIntFlag
Params: port: Port A or B.
		pin: which pin within the port.
Returns: none
Description: clears the interrupt flag of a certain pin.
*/
void ioClearIntFlag(avr32_gpio_port_t* port, uint32_t pin)
{
	port->ifrc = pin;
}

/*
Function: ioIntFlags
Params: port: Port A or B.
		pin: which pin within the port.
Returns: the interrupt flag register
Description: Returns the interrupt flag register for a specific port.
*/
uint32_t ioIntFlags(avr32_gpio_port_t* port)
{
	return port->ifr;
}