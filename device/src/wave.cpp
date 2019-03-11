#include "wave.h"
#include <string.h>


WAVE::WAVE (char *name)
{
    this->name = name;
    this->extra = nullptr;
    this->dataPointer = 0;
    this->ready = false;
}

WAVE::~WAVE ()
{
    if (this->extra != nullptr) {
        delete this->extra;
    }
}

#define WAVE_RET(file, res) do { f_close(file); return res; } while (0)

int WAVE::open (FIL *file)
{
    FRESULT res = FR_OK;
    res = f_open(file, this->name, FA_READ);
    if (res != FR_OK) {
        return WAVE_NOT_FOUND;
    }
    uint32_t f_bytesread = 0;
    res = f_read(file, &this->waveHeader, sizeof(WAVE_HEADER_T), &f_bytesread);
    if (res != FR_OK) {
        WAVE_RET(file, WAVE_UNDEFINED);
    }
    if (this->waveHeader.chunkIdU != 0x46464952U) {
        WAVE_RET(file, WAVE_UNDEFINED);
    }
    #if (WAVE_PCM_ONLY == 1U)
        if (this->waveHeader.audioFormat != 1) {
            WAVE_RET(file, WAVE_NOT_SUPPORTED);
        }
        if (this->waveHeader.subChunkDataIdU != 0x61746164U) {
            WAVE_RET(file, WAVE_NOT_SUPPORTED);
        }
    #else
    
    #endif
        
    if (this->waveHeader.formatU != 0x45564157U) {
        WAVE_RET(file, WAVE_NOT_SUPPORTED);
    }
    if (this->waveHeader.subChunkIdU != 0x20746d66U) {
        WAVE_RET(file, WAVE_NOT_SUPPORTED);
    }
    
        
    #if (WAVE_MONO == 1U)
        if (this->waveHeader.numChannels > 1) {
            WAVE_RET(file, WAVE_NOT_SUPPORTED);
        }
    #else
        
    #endif
       
    #ifdef WAVE_SAMPLE_RATE
    if (this->waveHeader.sampleRate != WAVE_SAMPLE_RATE) {
         WAVE_RET(file, WAVE_NOT_SUPPORTED);
    }
    #endif
    #ifdef WAVE_BIT_RATE
    if (this->waveHeader.bitsPerSample != WAVE_BIT_RATE) {
        WAVE_RET(file, WAVE_NOT_SUPPORTED);
    }
    #endif
    this->ready = true;    
    WAVE_RET(file, WAVE_OK);    
}

int WAVE::refresh ()
{
    this->dataPointer = 0;
    return WAVE_OK;
}

int WAVE::load (FIL *file, uint8_t *data)
{
    
    return WAVE_OK;
}

int WAVE::loadSample (FIL *file, uint8_t *data)
{
    if (this->ready == false) {
        return WAVE_NOT_FOUND;
    }
    uint32_t offset = 0;
    uint32_t bytesToRead = 0;
    #if (WAVE_PCM_ONLY == 1U)
        offset = 44U;
        bytesToRead = this->waveHeader.subChunkDataSize;
    #else
        
    #endif
    
    FRESULT res = FR_OK;
    res = f_open(file, this->name, FA_READ);
    if (res != FR_OK) {
        return WAVE_NOT_FOUND;
    }
    res = f_lseek(file, offset);
    if (res != FR_OK) {
        WAVE_RET(file, WAVE_NOT_FOUND); 
    }
    uint32_t f_bytesRead = 0;
    
    res = f_read(file, data, bytesToRead, &f_bytesRead);
    if (res != FR_OK) {
        WAVE_RET(file, WAVE_NOT_FOUND); 
    }
    
    WAVE_RET(file, WAVE_OK); 
}

WAVE_DSC WAVE::getInfo ()
{
    WAVE_DSC dsc;
    dsc.byteRate = this->waveHeader.byteRate;
    dsc.channels = this->waveHeader.numChannels;
    #if (WAVE_PCM_ONLY == 1U)
        dsc.dataSize = this->waveHeader.subChunkDataSize;
    #else
    
    #endif
    dsc.sampleRate = this->waveHeader.sampleRate;
    dsc.bitRate = this->waveHeader.bitsPerSample;
    return dsc;
}

