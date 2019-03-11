#ifndef ADC_CORE_H
#define ADC_CORE_H

#include <stdint.h>
#include "adc_core_conf.h"
#include "fft.h"


extern adc_word_t *buffer_a, *buffer_b, *buffer_temp;
extern BaseComplex *buffer_complex;


#define CLIP(x, l, h) (x < l ? l : (x > h ? h : x))
#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)
#define ABS(a, b) (a > b ? a - b : b - a)


int adc_core_init ();
int adc_core_destroy ();

void fill_buffer_sin (adc_word_t *buffer, int size, float phase, float scale, float period);

void add_buf (adc_word_t *dest, adc_word_t *buffer_a, adc_word_t *buffer_b, int size, adc_word_t magnitude);
void addc_buf (adc_word_t *dest, adc_word_t *buffer_a, adc_word_t c, int size, adc_word_t magnitude);
void dif_buf (adc_word_t *dest, adc_word_t *buffer_a, adc_word_t *buffer_b, int size,  adc_word_t zero, adc_word_t magnitude);
void mul_buf (adc_word_t *dest, adc_word_t *buffer_a, adc_word_t *buffer_b, int size,  adc_word_t magnitude);
void div_buf (adc_word_t *dest, adc_word_t *buffer_a, adc_word_t *buffer_b, int size,  adc_word_t magnitude);

BaseComplex *make_fft (adc_word_t *buffer, int bsize, int size, int invert);

#endif  /*ADC_CORE_H*/
