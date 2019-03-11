#ifndef GPIO_ACCESS
#define GPIO_ACCESS

#include "stm32f4xx_hal.h"
#include <stdint.h>

/*
#define  DEV_Pin_Set(p, v)  do {\
	                                p->BSRR = v;\
                            } while (0)

#define  DEV_Pin_Clr(p, v)  do {\
	                                p->BRR = v << 16;\
                            } while (0)

#define  DEV_Pin_Tog(p, v)  do {\
	                                p->ODR ^= v;\
                            } while (0)

#define  DEV_Pin_Put(p, bm, v)  do {\
	                                if (v) DEV_Pin_Set (p, bm);\
	                                else DEV_Pin_Clr(p, bm);\
                            } while (0)

#define  DEV_Port_Out(p, v, m)  do {\
	                                p->BRR = m;\
	                                p->BSRR = v;\
                            } while (0)

#define  DEV_fPort_Out(p, v)  do {\
	                                p->ODR = v;\
                            } while (0)

#define  DEV_Pin_Read(p, v)  (p->IDR & v)	
*/

namespace gpio {

	void gpio_init 				(GPIO_TypeDef *port, uint32_t mode, uint32_t pull, uint32_t speed, uint32_t pin);
	void gpio_af_init     (GPIO_TypeDef *port, uint32_t pull, uint32_t speed, uint32_t af, uint32_t pin);
	void gpio_deinit 			(GPIO_TypeDef *port,  uint32_t pin);

	void gpio_pin_set 		(GPIO_TypeDef *port, uint32_t pin, bool value);
	void gpio_pin_set 		(GPIO_TypeDef *port,  uint32_t pin);
	void gpio_pin_reset 	(GPIO_TypeDef *port,  uint32_t pin);
	void gpio_pin_toggle 	(GPIO_TypeDef *port,  uint32_t pin);

	void gpio_port_set 		(GPIO_TypeDef *port,  uint32_t value);
	void gpio_port_reset 	(GPIO_TypeDef *port,  uint32_t value);
	void gpio_port_out 		(GPIO_TypeDef *port,  uint32_t value);
	
	bool gpio_pin_read (GPIO_TypeDef *port,  uint32_t pin);
	uint32_t gpio_port_read (GPIO_TypeDef *port);
	
	typedef struct {
		GPIO_TypeDef *port;
		uint32_t pin;
	} gpio_dsc;
	
	void init (gpio_dsc &dsc, uint32_t mode, uint32_t pull, uint32_t speed);
	void init_af (gpio_dsc &dsc, uint32_t pull, uint32_t speed, uint32_t af, uint32_t mode);
	void deinit (gpio_dsc &dsc);
	
	void pin_set (gpio_dsc &dsc);
	void pin_reset (gpio_dsc &dsc);
	void pin_set (gpio_dsc &dsc, bool value);
	void pin_toggle (gpio_dsc &dsc);
	
	void port_set (gpio_dsc &dsc);
	void port_reset (gpio_dsc &dsc);
	void port_set (gpio_dsc &dsc, bool value);
	void port_toggle (gpio_dsc &dsc);
	
	bool pin_read (gpio_dsc &dsc);
	uint32_t port_read (gpio_dsc &dsc); 
};



#endif /*GPIO_ACCESS*/


/*End of file*/

