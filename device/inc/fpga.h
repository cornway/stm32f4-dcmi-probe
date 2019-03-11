#ifndef FPGA_H
#define FPGA_H

#include <stdint.h>

static void init_filter ();
static void put_filter (bool state);
static int get_filter ();
static void wait_cycles (int cycles);


class Fpga {
	private :
		virtual void setConfSelectPin (int image); /*selects a configuration image. 0 - image 0, 1 - image 1*/
        virtual bool getConfDonePin (); /*
                                            The CONF_DONE pin
                                            drives low before and during configuration. After all
                                            configuration data is received without error and the
                                            initialization cycle starts, the CONF_DONE pin is
                                            released.
                                        */
        virtual bool getCrcErrPin (); /*read crc_err pin. 1 - error detected during configuration*/
        virtual void setNConfPin (bool state); /*
                                                    If you pull this pin low during user mode the device
                                                    will lose its configuration data, enter a reset state, and
                                                    tri-state all I/O pins. Pulling this pin to a logic-high
                                                    level initiates reconfiguration.
                                                */
        virtual bool getNStatusPin (); /*
                                        The device drives the nSTATUS pin low immediately
                                        after power up and releases the pin after power-on
                                        reset (POR) time.        
                                       */
        virtual void hal_init ();
        void init_peripherals ();
		void reconf (int image);
	public :
        Fpga ();
        ~Fpga ();
		void init (int image);
        void restart (int image);
    
        void IRQ_Handle ();
        
	
}; /*FpgaMax10*/

#endif /*FPGA_H*/

