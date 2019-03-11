#include <stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif

#ifndef HWDEV
#define HWDEV (0)
#endif

typedef struct {
    uint8_t unused;
} enchw_device_t;

void enchw_setup(enchw_device_t *dev);
void enchw_select(enchw_device_t *dev);
void enchw_unselect(enchw_device_t *dev);
uint8_t enchw_exchangebyte(enchw_device_t *dev, uint8_t byte);


#ifdef __cplusplus
    }
#endif