#ifndef __INIT_H__
#define __INIT_H__



namespace init {
    
    void hal ();
    void clock ();
    void clock_enable ();
    void nvic ();
    void clock_out ();
    void tim_tsc_update ();
    void tim_hal_milis ();
    void fsmc ();
    void usb ();
    void gpio ();
    void rtc ();
    void audio ();
    void i2c1 ();
    
}; /*namespace init*/

void CAMERA_IRQHandler(void);
void CAMERA_DMA_IRQHandler(void);

#endif /*__INIT_H__*/


