

#include "TFTili9486.h"

void TFT_ILI9486::DummyCycle (uint32_t __cnt)
{
	while (__cnt--);
}


TFT_ILI9486::TFT_ILI9486 ()
{
	
}

uint16_t TFT_ILI9486::window (uint16_t __X, uint16_t __Y, uint16_t __dW, uint16_t __dH)
	{
		__IntToByte __TransferA, __TransferB;
		__TransferA.Int = __Y;
		__TransferB.Int = __Y + __dH;
        
		this->writeCmd(LcdCmdCASET);
		this->writeData(__TransferA.Byte[1]);
		this->writeData(__TransferA.Byte[0]);
		this->writeData(__TransferB.Byte[1]);
		this->writeData(__TransferB.Byte[0]);
		__TransferA.Int = __X;
		__TransferB.Int = __X + __dW;
		this->writeCmd(LcdCmdPASET);
		this->writeData(__TransferA.Byte[1]);
		this->writeData(__TransferA.Byte[0]);
		this->writeData(__TransferB.Byte[1]);
		this->writeData(__TransferB.Byte[0]);
        
		return 0;
	}
	
uint16_t TFT_ILI9486::init ()
{
    this->hal_init();
	this->resetHard();
	this->writeCmd(LcdCmdSWRESET);//Soft Reset
	this->delay_ms(15);
	
	this->writeCmd(LcdCmdPWCTRL1);//power control
	this->writeData(0x15);//Vreg1out
	this->writeData(0x12);//Vreg2out
	
	this->writeCmd(LcdCmdPWCTRL2);//power control2
	this->writeData(0x41);//VGH/VGL
	
	this->writeCmd(LcdCmdVMCTRL);//VCOM Control 1
	this->writeData(0x00);//nVM
	this->writeData(0x4A);//VCM_REG [7:0] 
	this->writeData(0x80);//VCM_REG_EN 
	
	this->writeCmd(LcdCmdMADCTL);//memory access
	this->writeData(0x48);//
    
	this->writeCmd(LcdCmdCOLMOD);//Interface Pixel Format
	this->writeData(0x60);//
	
	this->writeCmd(LcdCmdIFMODE);//Interface Mode Control 
	this->writeData(0x02);//
	
	this->writeCmd(LcdCmdINVTR);//Display Inversion Control
	this->writeData(0x00);//
	
	this->writeCmd(LcdCmdDISCTRL);//RGB/MCU interface
	this->writeData(0x20);//
    this->writeData(0x0F);//source/Gate direction
	
	this->writeCmd(LcdCmdSETIMAGE);//set image function
    this->writeData(0x00);//disable 24 bit data input
	
	
	
    this->writeCmd(LcdCmdAdjustControl3);//adjust control
	this->writeData(0xA9);//
	this->writeData(0x52);//
	this->writeData(0x2C);//
	this->writeData(0x82);//
	
	
  
    this->writeCmd(LcdCmdPRCTR);//adjust control
	this->writeData(0x04);//
	this->writeData(0x04);//
	this->writeData(0x04);//
	this->writeData(0x04);//
    
    this->writeCmd(LcdCmdWRDISBV);//adjust control
	this->writeData(0x00);//
	this->writeData(0x80);//
    
    this->writeCmd(LcdCmdWRCTRLD);//adjust control
	this->writeData(1 << 5);//
    
	this->delay_ms(15);
	this->writeCmd(LcdCmdSLPOUT);//Sleep Out
	this->delay_ms(15);
	this->writeCmd(LcdCmdDISON);//Display On
	this->delay_ms(15);
    return 0;
}

void TFT_ILI9486::resetHard ()
{
	this->setReset(true);
    this->delay_ms(1);
	this->setReset(false);
	this->delay_ms(30);
	this->setReset(true);
    this->delay_ms(30);  
}

void TFT_ILI9486::writeCmd (uint8_t data)
{
    uint16_t reg = (0 << 8) | data;
	this->spiRxTx_9bit(reg);
}

void TFT_ILI9486::writeData (uint8_t data)
{
    uint16_t reg = (1 << 8) | data;
	this->spiRxTx_9bit(reg);
}

void TFT_ILI9486::Rotate90Clockwise ()
{
    this->writeCmd(LcdCmdMADCTL);
    this->writeData (1 << 5);
}

void TFT_ILI9486::setBackLight (int value)
{
    this->hal_backlight(value);
}

const char *TFT_ILI9486::getDriverInfo ()
{
    return "TFT DRIVER : ILI9486, 480 X 320, 18BPP";
}
