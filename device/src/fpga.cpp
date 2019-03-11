#include "fpga.h"
#include "ccpu.h"
#include "device_conf.h"

static void wait_cycles (int cycles)
{
    while (cycles > 0) {
        cycles--;
    }
}

#define FILTER_ARRAY_SIZE       (32)
#define FILTER_WORD_SIZE        (8)
#define FILTER_WORD_MAX         (0xff)
#define FILTER_WORD_T           uint8_t

static FILTER_WORD_T filter_shift_buffer[FILTER_ARRAY_SIZE]; /*8x32 = 256 samples*/

static void init_filter ()
{
    for (int i = 0; i < FILTER_ARRAY_SIZE; i++) {
        filter_shift_buffer[i] = 0;
    }
}

static void put_filter (bool state)
{
    int carry = (state == true ? 1 : 0);
    FILTER_WORD_T temp = 0;
    for (int i = 0; i < FILTER_ARRAY_SIZE; i++) {
        temp = (filter_shift_buffer[i] << 1) | carry;
        carry = (filter_shift_buffer[i] >> (FILTER_WORD_SIZE - 1)) & 1;
        filter_shift_buffer[i] = temp;
    }
}

static int get_filter ()
{
    for (int i = 0; i < FILTER_ARRAY_SIZE; i++) {
        if (filter_shift_buffer[i] != FILTER_WORD_MAX) {
            return -1;
        }
    }
    return 1;
}

Fpga::Fpga ()
{
    
}

Fpga::~Fpga ()
{
    
}

void Fpga::init_peripherals ()
{
    CCPU_DATA_T id = 0;
    CCPU_DATA_T reg = 0;
    id = CCPU_READ_REG(CCPU_READ_ID_REGISTER);
    while (id != CCPU_ID) {
        /*error*/
        id = CCPU_READ_REG(CCPU_READ_ID_REGISTER);
    }
    /*enable pll, core clock, and reset all peripherals*/
    CCPU_WRITE_REG(CCPU_WRITE_CONTROL_REGISTER, CCPU_PLL_RESET_BM | CCPU_PLL_ENABLE_BM);
    
    while ((reg & CCPU_PLL_LOCKED_BM) != CCPU_PLL_LOCKED_BM) {
        reg = CCPU_READ_REG(CCPU_READ_CONTROL_REGISTER);
    }
}

void Fpga::reconf (int image)
{
	#ifndef FPGA_NOINIT
    this->setNConfPin(false);
    this->setConfSelectPin(image);
    this->setNConfPin(true);
	init_filter();
    while (get_filter() < 0) {
        put_filter( this->getConfDonePin() );
        wait_cycles(1000);
    }
    wait_cycles(10000);
	#endif
}

void Fpga::init (int image)
{
    this->hal_init();
    this->reconf(image);
    this->init_peripherals();
}

void Fpga::restart (int image)
{
    this->reconf(image);
    this->init_peripherals();
}

void Fpga::IRQ_Handle ()
{
    
}
