#include "ADV7180.h"

extern char adv_conf_file_name[24];

ADV7180_VIDEO_DECODER::ADV7180_VIDEO_DECODER ()
{
    this->ready = false;
}

uint8_t ADV7180_VIDEO_DECODER::readReg (uint8_t reg)
{
	return this->i2cMemRead(reg);
}

void ADV7180_VIDEO_DECODER::writeReg (uint8_t reg, uint8_t data)
{
	this->i2cMemWrite(reg, data);
	#ifdef ADV_REGISTERS_CHECK
        
	#endif
}

ADV_ERRORS ADV7180_VIDEO_DECODER::init (uint8_t address)
{
    this->hal_init();
    
    this->ownAddress = address;
    
    this->setPwrDownPin(ADV_PWR_DOWN_ASSERT);
    this->setResetPin(ADV_RESET_ASSERT);
	this->delayMs(100);
	this->setPwrDownPin(ADV_PWR_DOWN_DEASSERT);
	this->delayMs(10);
	this->setResetPin(ADV_RESET_DEASSERT);
	this->delayMs(10);
	
	/*read silicon revision*/
	switch (this->readReg(adv_r_map::ident) ) {
		case 0:    this->revision_info = (char *)"UNKNOWN";
                   this->revision_id = 0;
			break;
		case 0x1b: this->revision_info = (char *)"INITIAL";
                   this->revision_id = 0x1b;
			break;  
		case 0x1c: this->revision_info = (char *)"ESD & PCD FIX";
                   this->revision_id = 0x1c;
			break;
		case 0x1e: this->revision_info = (char *)"48 OR 32 LEAD DEV";
                   this->revision_id = 0x1e;
			break;
		default:   this->revision_info = (char *)"UNKNOWN";
                   this->revision_id = -1;
                   return ADV7180_NOT_FOUND;
	}
        this->writeReg(adv_rw_map::inputControl, 					0x04); 	/*select ain 0, autodetect composite standart*/
        this->writeReg(adv_rw_map::autoDetectEnable, 				0xff); 	/*enable detecting of all composite standarts*/
        this->writeReg(adv_rw_map::defaultValueY, 					0xf2); 	/*enable free run and output blue screen*/
        this->writeReg(adv_rw_map::defaultValueC, 					0x70); 	/*enable free run an blue screen output*/
        this->writeReg(adv_rw_map::vs_fieldCtl_1, 					0x12);  /*default*/
        this->writeReg(adv_rw_map::vs_fieldCtl_2, 					0x41);  /*default*/
        this->writeReg(adv_rw_map::vs_fieldCtl_3, 					0x84);  /*default*/
        this->writeReg(adv_rw_map::hs_positionCtl_1, 				0x21);  /*default*/
        this->writeReg(adv_rw_map::hs_positionCtl_2, 				0x00);  /*default*/
        this->writeReg(adv_rw_map::hs_positionCtl_3, 				0x00);  /*default*/
        this->writeReg(adv_rw_map::polarity, 						0x01);  /*default*/
		this->writeReg(adv_rw_map::vs_fieldPinctl, 					0x01);  /*default*/
        this->writeReg(adv_rw_map::lockCount, 					    0x36);  /*default*/
        this->writeReg(adv_rw_map::miscGainCtl, 					0xa0);  /*default*/
        this->writeReg(adv_rw_map::cvbsTrim, 					    0x0d);  /*default*/
        this->writeReg(adv_rw_map::sdOffsetCb,   					0x00);  /*default*/
        this->writeReg(adv_rw_map::sdOffsetCr   , 					0x00);  /*default*/
        this->writeReg(adv_rw_map::ntscCombCtl, 					0x80);  /*default*/
        this->writeReg(adv_rw_map::palCombCtl, 						0xc0);  /*default*/
        this->writeReg(adv_rw_map::adcCtl, 							0x10);  /*default*/
        this->writeReg(adv_rw_map::manualWindowCtl, 				0x02);  /*default*/
		this->writeReg(adv_rw_map::pixelDelayCtl, 				    0x80);  /*default*/
        
        this->ready = true;
        
        if (adv_conf_file_name[0] != '\0') {
            this->setConfFromfile(adv_conf_file_name);
        }
    return ADV7180_OK;
}

void ADV7180_VIDEO_DECODER::powerDown ()
{
    this->setPwrDownPin(ADV_PWR_DOWN_ASSERT);
}

void ADV7180_VIDEO_DECODER::powerUp ()
{
    this->setPwrDownPin(ADV_PWR_DOWN_DEASSERT);
}

bool ADV7180_VIDEO_DECODER::isReady ()
{
    return this->ready;
}

char *ADV7180_VIDEO_DECODER::getRevisionInfo ()
{
    return this->revision_info;
}

int32_t ADV7180_VIDEO_DECODER::getRevisionId ()
{
    return this->revision_id;
}
 
void ADV7180_VIDEO_DECODER::setLumaGain (uint16_t gain)
{
    this->writeReg(adv_rw_map::lumaGainCtl_1_WO, (gain >> 8) & 0xf);
    this->writeReg(adv_rw_map::lumaGainCtl_2_WO, gain & 0xff);
}

void ADV7180_VIDEO_DECODER::setChromaGain (uint16_t gain)
{
    this->writeReg(adv_rw_map::chromaGainCtl_2_WO, gain);
}

void ADV7180_VIDEO_DECODER::setBrightness (uint16_t value)
{
    this->writeReg(adv_rw_map::brightness, value);
}

void ADV7180_VIDEO_DECODER::setContrast (uint16_t value)
{
    this->writeReg(adv_rw_map::contrast, value);
}
 
bool ADV7180_VIDEO_DECODER::inLock ()
{
    return this->readReg(adv_r_map::status1) & 0x1;
}

bool ADV7180_VIDEO_DECODER::inFscLock ()
{
    return this->readReg(adv_r_map::status1) & 0x4;
}


uint16_t ADV7180_VIDEO_DECODER::getChromaGain ()
{
    return 0;
}

uint16_t ADV7180_VIDEO_DECODER::getLumaGain ()
{
    return 0;
}

uint16_t ADV7180_VIDEO_DECODER::getContrast ()
{
    return this->readReg(adv_rw_map::contrast);
}

uint16_t ADV7180_VIDEO_DECODER::getBrightnes ()
{
    return this->readReg(adv_rw_map::brightness);
}
 
char *ADV7180_VIDEO_DECODER::getDecodedStandard ()
{
    uint8_t status = (this->readReg(adv_r_map::status1) >> 4) & 0x7;
    char *ret = (char *)"";
    switch (status) {
        case 0 :    ret = (char *)"NTSC M/J";
            break;
        case 1 :    ret = (char *)"NTSC 4.43";
            break;
        case 2 :    ret = (char *)"PAL M";
            break;
        case 3 :    ret = (char *)"PAL 60";
            break;  
        case 4 :    ret = (char *)"PAL B/G/H/I/D";
            break;
        case 5 :    ret = (char *)"SECAM";
            break;
        case 6 :    ret = (char *)"PAL Combination N";
            break;
        case 7 :    ret = (char *)"SECAM 525";
            break;
    }
    return ret;
}


#include "ff.h"
#include "pp.h"

int ADV7180_VIDEO_DECODER::setConfFromfile (char *fileName)
{
    FRESULT res = FR_OK;
    FIL *file = new FIL;
    if (file == nullptr) {
        return -1;
    }
    res = f_open(file, fileName, FA_READ);
    if (res != FR_OK) {
        delete file;
        return -1;
    }
    char adv_header[12]; /*must be "ADV7180"*/
    char *buf = adv_header;
    int ret = -1;
    buf = f_gets(adv_header, 12, file);
    if (buf != adv_header) { /*empty file*/
        return -1;
    }
    touppers(buf);
    if (strncmp(buf, "ADV7180", 7) != 0) {
        return -1;
    }
    char key_buf[12];
    static uint8_t advReg = 0, advKey = 0;
    while(1) {
        buf = f_gets(key_buf, 12, file);
        if (buf != key_buf) {
            break;
        }
        this->parseReg(key_buf);
    }
    f_close(file);
    delete file;
    return 0;
}



int ADV7180_VIDEO_DECODER::parseReg (char *str)
{
    if (str[4] != '=') {
        return -1;
    }
    static uint8_t advReg = 0, advKey = 0;
    if (parse_hex8(str, &advReg) != 0) {
        return -1;
    }
    if (parse_hex8(str + 5, &advKey) != 0) {
        return -1;
    }
    this->writeReg(advReg, advKey);
    return 0;
}
