#ifndef SPRITE_H
#define SPRITE_H

#include "stdint.h"
#include "iterable.h"
#include "dimensions.h"
#include "device_conf.h"

#ifndef _PACKED
#define _PACKED __packed
#endif

#ifndef SPR_SIZE_T
#define SPR_SIZE_T uint16_t
#endif

#ifndef SPR_COLOR_T
#define SPR_COLOR_T uint16_t
#endif

#ifndef SPR_WORD_T
#define SPR_WORD_T uint32_t
#endif


class SpriteRAM;

typedef _PACKED struct {
    SPR_SIZE_T size;
    SPR_SIZE_T w, h;
    SPR_COLOR_T *ram;
    void *link;
    char name[1];
} Sprite;

class SpriteRAM {
    private :
        SPR_WORD_T mem_p;
        SPR_WORD_T mem_free;
        SPR_WORD_T mem_base;
        SPR_WORD_T mem_size;
    
        Sprite *link;
    public :
        SpriteRAM ();
        ~SpriteRAM ();
        void init (SPR_WORD_T mem_base, SPR_WORD_T mem_size);
    
        Sprite *allocSprite (SPR_SIZE_T w, SPR_SIZE_T h, const char *name);
        Sprite *getSprite (const char *name);
    
        template <typename I>
        I *getImage (Sprite *sprite)
        {
            static I i;
            i.W = sprite->w;
            i.H = sprite->h;
            i.Image = sprite->ram;
            return &i;
        }    
    
        void clear ();
};

#endif /*SPRITE_H*/

