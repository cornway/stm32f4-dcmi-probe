#ifndef WAVE_H
#define WAVE_H

#include "ff.h"
#include <stdint.h>
#include "device_conf.h"



#pragma anon_unions

enum {
    WAVE_OK,
    WAVE_NOT_FOUND,
    WAVE_NOT_SUPPORTED,
    WAVE_UNDEFINED,
};

typedef __packed struct {
    __packed union {
        char chunkId[4]; /*"RIFF"*/
        uint32_t chunkIdU;
    };
    uint32_t chunkSize;
    __packed union {
        char format[4]; /*WAVE*/
        uint32_t formatU;
    };
    __packed union {
        char subChunkId[4]; /*"fmt "*/
        uint32_t subChunkIdU;
    };
    
    uint32_t subChunksize;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAllign;
    uint16_t bitsPerSample;
    uint16_t extraSize;
        __packed union {
            __packed struct {
                uint8_t *extra;
                __packed union {
                    char extraSubChunkDataId[4];
                    uint32_t extraSubChunkDataIdU;
                };
                uint32_t extraSubChunkDataSize;
            };
            __packed struct {
                __packed union {
                    char subChunkDataId[4];
                    uint32_t subChunkDataIdU;
                };
                uint32_t subChunkDataSize;
            };
        };
    uint8_t data[1];
} WAVE_HEADER_T;

typedef struct {
    uint32_t dataSize;
    uint8_t channels;
    uint16_t sampleRate;
    uint16_t byteRate;
    uint16_t bitRate;
} WAVE_DSC;

class WAVE {
    private :
        char *name;
        WAVE_HEADER_T waveHeader;
        uint32_t dataPointer;
        uint8_t *extra;
        bool ready;
    public :
        WAVE (char *name);
        ~WAVE ();
        int open (FIL *file);
        int refresh ();
        int load (FIL *file, uint8_t *buf);
        int loadSample (FIL *file, uint8_t *buf);
        WAVE_DSC getInfo ();
};

#endif /*WAVE_H*/
