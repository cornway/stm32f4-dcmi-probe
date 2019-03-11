#include "wave_sample.h"


WaveSample::WaveSample (char *name, WAVE_DSC dsc, void *buffer)
{
    this->waveDsc = dsc;
    this->name = name;
    this->buffer = buffer;
}

void WaveSample::init (char *name, WAVE_DSC dsc, void *buffer)
{
    this->waveDsc = dsc;
    this->name = name;
    this->buffer = buffer;
}


WaveSample::~WaveSample ()
{
    
}

void WaveSample::play ()
{
    this->ll_play(this->waveDsc, this->buffer);
}

WAVE_DSC WaveSample::getDsc ()
{
    return this->waveDsc;
}

void *WaveSample::getBuffer ()
{
    return this->buffer;
}

void WaveSample::toUnsigned ()
{
    switch (this->waveDsc.bitRate) {
        case 8 :
            break;
        case 16 :   
                    uint16_t *dest = (uint16_t *)this->buffer;
                    uint16_t *src = (uint16_t *)this->buffer;
                    for (uint32_t i = 0; i < this->waveDsc.dataSize; i++) {
                        dest[i] = src[i] + 0x8000;
                    }
            break;
        case 24 :
            break;
        case 32:
            break;
        default :
            break;
    }
}

void WaveSample::wait ()
{
    this->ll_wait();
}
