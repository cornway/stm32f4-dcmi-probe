#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>
#include "gui_defs.h"
#include "dimensions.h"

#define BMP_MEDIA_SIZE  (1536U)

#define MAX_STREAM_PAR  (4)

#define _BMP_PACKED __packed

#ifndef BYTE
#define BYTE uint8_t
#endif

#ifndef BMP_NAME_LEN
#define BMP_NAME_LEN    (44U)
#endif

typedef _BMP_PACKED struct {
    uint8_t b, g, r, reserved;
} RGBQUAD;

typedef _BMP_PACKED struct  {
    uint8_t bfType[2];
    uint32_t bfSize;
    uint16_t bfUnused[2];
    uint32_t bfOffBits;
} BITMAPFILEHEADER;

typedef _BMP_PACKED struct {
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXRes;
    uint32_t biYRes;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
    RGBQUAD biClrMap[1];
} BITMAPINFOHEADER;



typedef struct {
    void *dest;
    BITMAPINFOHEADER *img;
    int8_t lock;
} BITMAPSTREAM;

#include "ff.h"
#include "abstract.h"
#include "iterable.h"

class BitmapFactory;

class Bitmap : public Link<Bitmap> {
    private :
        BITMAPFILEHEADER header;
        BITMAPINFOHEADER *info;
        char name[BMP_NAME_LEN];
        FIL *file;
    
        bool exist;
    
        virtual void preprocess (void *input, void *output, uint32_t color_cnt)
        {
            *(uint32_t *)output = *(uint32_t *)input;
        }
    
        Bitmap (const char *path, FIL *file);
        ~Bitmap ();
        
        friend class BitmapFactory;
    public :
        int draw (void *dest, uint8_t all, bool raw);
        template <typename COLOR, typename RANGE>
        void
        getImage (tImage<COLOR> *image)
        {
            image->W = this->info->biWidth;
            image->H = this->info->biHeight;
            this->draw((void *)image->Image, 2, 0);
        }
        template <typename RANGE>
        void
        getSize (Point<RANGE> *p)
        {
            p->w = this->info->biWidth;
            p->h = this->info->biHeight;
        }
        
        uint32_t getWidth ();
        uint32_t getHeight ();
};

class BitmapFactory {
    private :
        vector::Vector<Bitmap> openBmps;
    public :
        BitmapFactory ();
        Bitmap *open (const char *path, FIL *file);
        int close (Bitmap *bmp);
        int close (const char *name);
        void closeAll ();
        
};

#endif /*BITMAP_H*/


