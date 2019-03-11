/*===================================================================*/
/*                                                                   */
/*  pspad.h : PS Pad function header file                            */
/*                                                                   */
/*  2016/1/20  Racoon                                                */
/*                                                                   */
/*===================================================================*/
#ifndef PSPAD_H
#define PSPAD_H

// Initialize interface
void pspad_init();

// Read PS Pad state
// SE -- -- ST U R D L L2 R2 L1 R1 TR O X SQ
void pspad_read(unsigned short *pad1, unsigned short *pad2);

#endif

