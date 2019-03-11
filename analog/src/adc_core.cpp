#include "adc_core.h"
#include <math.h>

adc_word_t *buffer_a, *buffer_b, *buffer_temp;
BaseComplex *buffer_complex;

template <typename I>
static void mem_set (I *array, I item, int size);

int adc_core_init ()
{
    buffer_a = (adc_word_t *)new uint8_t[(ADC_BUF_SIZE * sizeof(adc_word_t) * 3) + (ADC_BUF_SIZE * sizeof(BaseComplex))];
    if (buffer_a == nullptr) {
        return -1;
    }
    buffer_b = buffer_a + ADC_BUF_SIZE;
    buffer_temp = buffer_b + ADC_BUF_SIZE;
    buffer_complex = (BaseComplex *)(buffer_temp + ADC_BUF_SIZE);
    
    mem_set(buffer_a, (adc_word_t)0, ADC_BUF_SIZE);
    mem_set(buffer_b, (adc_word_t)0, ADC_BUF_SIZE);
    mem_set(buffer_temp, (adc_word_t)0, ADC_BUF_SIZE);
    
    BaseComplex c = {0.0, 0.0};
    mem_set(buffer_complex, c, ADC_BUF_SIZE);
    
    return 0;
}

int adc_core_destroy ()
{
    delete buffer_a;
    
    return 0;
}


void fill_buffer_sin (adc_word_t *buffer, int size, float phase, float scale, float period)
{
    for (int i = 0; i < size; i++) {
        buffer[i] = (int16_t)((100.0f * (sin(phase)  + 1.0f)) * scale);
        phase += period;
    }
}

void add_buf (adc_word_t *dest, adc_word_t *buffer_a, adc_word_t *buffer_b, int size,  adc_word_t magnitude)
{
    adc_word_t val = 0;
    for (int i = 0; i < size; i++) {
        val = buffer_a[i] + buffer_b[i];
        if (val > magnitude) {
            val = magnitude;
        }
        dest[i] = val;
    }
}

void addc_buf (adc_word_t *dest, adc_word_t *buffer_a, adc_word_t c, int size, adc_word_t magnitude)
{
    adc_word_t val = 0;
    for (int i = 0; i < size; i++) {
        val = buffer_a[i] + c;
        if (val > magnitude) {
            val = magnitude;
        } else if (val < 0) {
            val = 0;
        }
        dest[i] = val;
    }
}

void dif_buf (adc_word_t *dest, adc_word_t *buffer_a, adc_word_t *buffer_b, int size, adc_word_t zero, adc_word_t magnitude)
{
    adc_word_t val = 0;
    for (int i = 0; i < size; i++) {
        val = buffer_a[i] - buffer_b[i];
        val += zero;
        if (val > magnitude) {
            val = magnitude;
        }
        if (val < 0) {
            val = 0;
        }
        dest[i] = val;
    }
}

void mul_buf (adc_word_t *dest, adc_word_t *buffer_a, adc_word_t *buffer_b, int size,  adc_word_t magnitude)
{
    adc_word_t val = 0;
    for (int i = 0; i < size; i++) {
        val = buffer_a[i] * buffer_b[i];
        if (val > magnitude) {
            val = magnitude;
        }
        dest[i] = val;
    }
}

void div_buf (adc_word_t *dest, adc_word_t *buffer_a, adc_word_t *buffer_b, int size,  adc_word_t magnitude)
{
    adc_word_t val = 0;
    for (int i = 0; i < size; i++) {
        val = buffer_a[i] / (buffer_b[i] + 1);
        if (val > magnitude) {
            val = magnitude;
        }
        dest[i] = val;
    }
}




BaseComplex *make_fft (adc_word_t *buffer, int bsize, int size, int invert)
{
    int step = bsize / size;
    for (int i = 0, h = 0; i < size; i++, h+= step) {
        buffer_complex[i].real = (double)buffer[h];
        buffer_complex[i].imag = 0.0;
    }
    fft(buffer_complex, size, invert);
    for (int i = 0; i < size; i++) {
        buffer[i] = (int16_t)buffer_complex[i].real;
    }
    return buffer_complex;
}


template <typename I>
static void mem_set (I *array, I item, int size)
{
    for (int i = 0; i < size; i++) {
        array[i] = item;
    }
}