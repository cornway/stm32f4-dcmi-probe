#include "gpio.h"
#include "init.h"

void gpio::gpio_init (GPIO_TypeDef *port, uint32_t mode, uint32_t pull, uint32_t speed, uint32_t pin)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.Mode  = mode;
  GPIO_InitStruct.Pull  = pull;
  GPIO_InitStruct.Speed = speed;

  GPIO_InitStruct.Pin = pin;
  HAL_GPIO_Init(port, &GPIO_InitStruct);
}

void gpio::gpio_af_init (GPIO_TypeDef *port, uint32_t pull, uint32_t speed, uint32_t af, uint32_t pin)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.Mode  = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull  = pull;
    GPIO_InitStruct.Speed = speed;
	GPIO_InitStruct.Alternate = af;

    GPIO_InitStruct.Pin = pin;
    HAL_GPIO_Init(port, &GPIO_InitStruct);
}

void gpio::gpio_deinit (GPIO_TypeDef *port, uint32_t pin)
{
	/*to do*/
}

void gpio::gpio_pin_set (GPIO_TypeDef *port, uint32_t pin, bool value)
{
	if ( value == true ) {
		port->BSRR |= pin;
	} else {
		port->BSRR |= pin << 16;
	}
}

void gpio::gpio_pin_set (GPIO_TypeDef *port, uint32_t pin)
{
	port->BSRR |= pin;
}

void gpio::gpio_pin_reset (GPIO_TypeDef *port, uint32_t pin)
{
	port->BSRR |= pin << 16;
}

void gpio::gpio_pin_toggle (GPIO_TypeDef *port, uint32_t pin)
{
	port->ODR ^= pin;
}

bool gpio::gpio_pin_read (GPIO_TypeDef *port, uint32_t pin)
{
	if ((port->IDR & pin) == pin) {
		return true;
	} else {
		return false;
	}
}

void gpio::gpio_port_set (GPIO_TypeDef *port, uint32_t value)
{
	port->BSRR |= value;
}

void gpio::gpio_port_reset (GPIO_TypeDef *port, uint32_t value)
{
	port->BSRR |= value << 16;
}

void gpio::gpio_port_out (GPIO_TypeDef *port, uint32_t value)
{
	port->ODR = value;
}

uint32_t gpio::gpio_port_read (GPIO_TypeDef *port)
{
	return port->IDR;
}












void gpio::init (gpio_dsc &dsc, uint32_t mode, uint32_t pull, uint32_t speed)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.Mode  = mode;
    GPIO_InitStruct.Pull  = pull;
    GPIO_InitStruct.Speed = speed;

    GPIO_InitStruct.Pin = dsc.pin;
    HAL_GPIO_Init(dsc.port, &GPIO_InitStruct);
}

void gpio::init_af (gpio_dsc &dsc, uint32_t pull, uint32_t speed, uint32_t af, uint32_t mode)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.Mode  = mode;
    GPIO_InitStruct.Pull  = pull;
    GPIO_InitStruct.Speed = speed;
	GPIO_InitStruct.Alternate = af;

    GPIO_InitStruct.Pin = dsc.pin;
    HAL_GPIO_Init(dsc.port, &GPIO_InitStruct);
}

void gpio::deinit (gpio_dsc &dsc)
{
	/*to do*/
}

void gpio::pin_set (gpio_dsc &dsc, bool value)
{
	if ( value == true ) {
		dsc.port->BSRR |= dsc.pin;
	} else {
		dsc.port->BSRR |= dsc.pin << 16;
	}
}

void gpio::pin_set (gpio_dsc &dsc)
{
	dsc.port->BSRR |= dsc.pin;
}

void gpio::pin_reset (gpio_dsc &dsc)
{
	dsc.port->BSRR |= dsc.pin << 16;
}

void gpio::pin_toggle (gpio_dsc &dsc)
{
	dsc.port->ODR ^= dsc.pin;
}

bool gpio::pin_read (gpio_dsc &dsc)
{
	if ((dsc.port->IDR & dsc.pin) == dsc.pin) {
		return true;
	} else {
		return false;
	}
}

void gpio::port_set (gpio_dsc &dsc)
{
	dsc.port->BSRR |= dsc.pin;
}

void gpio::port_reset (gpio_dsc &dsc)
{
	dsc.port->BSRR |= dsc.pin;
}


uint32_t gpio::port_read (gpio_dsc &dsc)
{
	return dsc.port->IDR;
}





