
#ifndef GUI_DEFS_H
#define GUI_DEFS_H

#include <stdint.h>

#ifndef DIALOG_TEXT_SIZE
#define DIALOG_TEXT_SIZE    (48U)
#endif

#ifndef ALERT_TEXT_SIZE
#define ALERT_TEXT_SIZE    (48U)
#endif

#define REPAINT_WHEN_DIALOG (0U)

#define GCOMPONENT_NAME_LEN (12U)

#ifndef _PACKED
#define _PACKED __packed
#endif

#define GET_GREY_16BPP(rgb) ((rgb & 0xf8U) << 8) | ((rgb & 0xfcU) << 3) | ((rgb & 0xf8U) >> 3)
#define RGB24_TO_RGB16(r, g, b) ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3)
#define RGB16_GET_R(c)          ((c >> 11) & 0x1f)
#define RGB16_GET_G(c)          ((c >> 5) & 0x3f)
#define RGB16_GET_B(c)          ((c >> 0) & 0x1f)
#define GET_ALPHA_16BPP(a, c) \
do { \
    uint8_t r, g, b; \
    r = RGB16_GET_R(c); \
    g = RGB16_GET_G(c); \
    b = RGB16_GET_B(c); \
    a = (r + g + b) / 3; \
    a = a << 2; \
} while (0)

#pragma anon_unions

enum {
    GTEXT_ALLIGN_NONE,
    GTEXT_ALLIGN_TOP,
    GTEXT_ALLIGN_BOTTOM,
    GTEXT_ALLIGN_LEFT,
    GTEXT_ALLIGN_RIGHT,
    GTEXT_ALLIGN_CENTER,
};

template <typename Color>
_PACKED struct  tImage {
    const Color *Image;
    uint16_t W, H;
};

template <typename Color>
_PACKED struct  tAnimation {
    const tImage<Color> **imagesSet;
    uint16_t imagesCount;
};


_PACKED struct tChar {
    const uint8_t Code;
    const void *Image;       
};

_PACKED struct tFont {
    const uint8_t elements;
    const tChar *imagesSet;   
    const uint8_t w, h; 
};


#define GUI_min(a, b)  (a < b ? a : b)   
#define GUI_max (a, b)  (a >= b ? a : b)   


#define ColorMaskReference ColorWhite

typedef char DefaultCharStorage_t;

/*
typedef tFont<ColorDepth> DefaultFontStorage;
*/

#include "iterable.h"

template <typename F, typename Set>
    class FontCharSet : public Link<FontCharSet<F, Set> > { 
        private :
            Set *set;
            const F * font;

            
            
        public :
            FontCharSet (Set *set, const F *f)
            {
                this->set = set;
                this->font = f;
            }
            ~FontCharSet ()
            {
                delete this->set;
            }
            Set get_UTF8 (uint16_t code)
            {
                return set[(uint8_t)code];
            }
            
            
            
            uint32_t getW ()
            {
                return this->font->w;
            }
            uint32_t getH ()
            {
                return this->font->h;
            }
            const F *getFont ()
            {
                return this->font;
            }
    };

#endif /*GUI_DEFS_H*/
