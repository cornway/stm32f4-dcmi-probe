#include "gif.h"


bool cleanup;
class CleanUp {
    private :
        void *obj;
    public :
        CleanUp (void *obj)
        {
            this->obj = obj;
        }
        ~CleanUp ()
        {
            if (cleanup == true)
                delete this->obj;
        }
};

GIF_IMAGE_HEADER::GIF_IMAGE_HEADER (GIF_IMAGE_BLOCK *info, GIF_IMAGE_DATA_BLOCK *data)
{
    this->imageData = data;
    this->imageInfo = info;
}

Gif::Gif (const char *path)
{
    FRESULT res;
    res = f_open(&this->file, path, FA_READ);
    if (res != FR_OK) {
        return;
    }
    uint32_t f_bytes_read;
    res = f_read(&this->file, &this->header.giFileHeader, sizeof(GIF_FILE_HEADER), &f_bytes_read);
    uint8_t *sign = this->header.giFileHeader.giSignature;
    if (res != FR_OK) {
        return;
    }
    if (
            (sign[0] != 'G') ||
                (sign[1] != 'I') ||
                    (sign[2] != 'F') 
        ) {
            return;
        }
    sign = this->header.giFileHeader.giVersion;
    if (
            (sign[0] != '8') ||
                (sign[2] != 'a')
        ) {
            return;
        } else {
            if (sign[1] == '7') {
                this->version = GIF_STD_87A;
            } else if (sign[1] == '9') {
                this->version = GIF_STD_89A;
            } else  {
                return;
            }
        }
        uint8_t option = this->header.giFileHeader.giOption;
        bool gloColorTable = option & 0x1U ? true : false;
        bool interlace = option & 0x2U ? true : false;
        bool sort = option & 0x4U ? true : false;
        uint8_t gloColtableSize = (1 << (((option & 0xf0) >> 4) + 1));
        this->header.giData.giColorMap = new GIF_RGB_BLOCK[gloColtableSize];
        if (this->header.giData.giColorMap == nullptr) {
            return;
        }
        CleanUp(this->header.giData.giColorMap);
        res = f_read(&this->file, this->header.giData.giColorMap, gloColtableSize * sizeof(GIF_RGB_BLOCK), &f_bytes_read);
        if (res != FR_OK) {
            return;
        }
        uint8_t eof = 0xffU;
        do {
            GIF_IMAGE_BLOCK *info;
            GIF_IMAGE_DATA_BLOCK *data;
            info = new GIF_IMAGE_BLOCK;
            if (info == nullptr) {
                return;
            }
            CleanUp((void *)info);
            res = f_read(&this->file, info, sizeof(GIF_IMAGE_BLOCK), &f_bytes_read);
            if (res != FR_OK) {
                return;
            }
            option = info->giOption;
            
        } while (eof != 0);
        cleanup = false;
}

Gif::~Gif ()
{
    
}

int Gif::play (void *dest)
{
    return 0;
}
    


GifFactory::GifFactory ()
{
    
}

GifFactory::~GifFactory ()
{
    
}

Gif *GifFactory::open (const char *path)
{
    
    return nullptr;
}

int GifFactory::close (Gif *gif)
{
    return 0;
}

int GifFactory::close (const char *path)
{
    return 0;
}

