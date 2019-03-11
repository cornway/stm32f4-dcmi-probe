#include "time.h"
#include "main.h"

void time::delay_ns (uint32_t value)
{
	while (value-- > 0) {}
}

void time::delay_us (uint32_t value)
{
	while (value-- > 0) {
		time::delay_ns(20);
	}
}

void time::delay_ms (uint32_t value)
{
	HAL_Delay (value);
}

void time::delay_s (uint32_t value)
{
	while (value-- > 0) {
		time::delay_ms(1000);
	}
}






