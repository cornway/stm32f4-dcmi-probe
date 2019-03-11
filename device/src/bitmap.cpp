#include "bitmap.h"
#include "string.h"

#ifdef EXCEPTIONS__
#include "exception.h"
#endif

#ifdef EXCEPTIONS__
    #define BRK(res, str) do { if (res != FR_OK) { throw exception::Throwable(str); }} while (0);
#else
    #define BRK(res, str) do { if (res != FR_OK) { return; }} while (0);
#endif
    
Bitmap::Bitmap (const char *path, FIL *file)
{
    FRESULT res;
    this->exist = false;
    this->info = nullptr;
    this->file = file;
    memset(this->name, '/0', BMP_NAME_LEN);
    strcpy(this->name, path);
    res = f_open(this->file, path, FA_READ);
    BRK(res, "no file");
    uint32_t f_bytes_read;
    res = f_read(this->file, (char *)&this->header, sizeof(BITMAPFILEHEADER), &f_bytes_read);
    BRK(res, "read denied");
    if ((this->header.bfType[0] != 'B') || (this->header.bfType[1] != 'M')) {
        BRK(!FR_OK, "not a bitmap");
    }
    if ((this->header.bfUnused[0] != 0) || (this->header.bfUnused[0] != 0)) {
        BRK(!FR_OK, "not a bitmap");
    }
    uint32_t tsize = 0;
    tsize = this->header.bfOffBits - sizeof(BITMAPFILEHEADER);
    this->info = (BITMAPINFOHEADER *) new char[tsize];
    if (this->info == nullptr) {
        BRK(!FR_OK, "not enought core");
    }
    res = f_read(this->file, this->info, tsize, &f_bytes_read);
    BRK(res, "read denied");
    f_close(this->file);
    this->exist = true;
}

Bitmap::~Bitmap ()
{
    if (this->info != nullptr) {
        delete this->info;
    }
}


extern void bmp_mach_conv (void **dest, uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

static BITMAPSTREAM bmp_streams[MAX_STREAM_PAR];
static BITMAPSTREAM *cur_stream = nullptr;

static BITMAPSTREAM *get_free_stream ()
{
    for (uint32_t i = 0; i < MAX_STREAM_PAR; i++) {
        if (bmp_streams[i].lock == 0) {
            bmp_streams[i].lock = 1;
            return &bmp_streams[i];
        }
    }
    return nullptr;
}

static void free_stream (BITMAPSTREAM *stream)
{
    stream->lock = 0;
}

#define SENSE(x) do { if (x == 0) return 1; } while (0);



UINT streamer_1bpp (const BYTE *p, UINT btf)
{
    SENSE(btf);
    uint8_t byte = 0;
    uint8_t bit = 0;
    RGBQUAD *cp;
    cp = cur_stream->img->biClrMap;
    for (uint32_t i = 0; i < btf; i++) {
        byte = p[i];
        bit = 0;
        for (uint32_t bp = 0; bp < 8; bp++) {
            bit = (byte >> i) & 0x1;
            bmp_mach_conv(&cur_stream->dest, 0, 0, cp[bit].r, cp[bit].g, cp[bit].b, cp[bit].reserved);
        }
    }
    return 0;
}
UINT streamer_4bpp (const BYTE *p, UINT btf)
{
    SENSE(btf);
    uint8_t a, b;
    RGBQUAD *cp;
    cp = cur_stream->img->biClrMap;
    for (uint32_t i = 0; i < btf; i++) {
        a = p[i] & 0xf;
        b = (p[i] >> 4) & 0xf;
        bmp_mach_conv(&cur_stream->dest, 0, 0, cp[a].r, cp[a].g, cp[a].b, cp[a].reserved);
        bmp_mach_conv(&cur_stream->dest, 0, 0, cp[b].r, cp[b].g, cp[b].b, cp[b].reserved);
    }
    
    return 0;
}
UINT streamer_8bpp (const BYTE *p, UINT btf)
{
    SENSE(btf);
    RGBQUAD *cp;
    cp = cur_stream->img->biClrMap;
    for (uint32_t i = 0; i < btf; i++) {
        bmp_mach_conv(&cur_stream->dest, 0, 0, cp[p[i]].r, cp[p[i]].g, cp[p[i]].b, cp[p[i]].reserved);
    }
    return 0;
}
UINT streamer_24bpp (const BYTE *p, UINT btf)
{
    SENSE(btf);
    for (uint32_t i = 2; i < btf; i += 3) {
        bmp_mach_conv(&cur_stream->dest, 0, 0, p[i], p[i - 1], p[i - 2], 0);
    }
    return 0;
}

UINT (*stream_pack[4])(const BYTE *, UINT) = {
    streamer_1bpp,
    streamer_4bpp,
    streamer_8bpp,
    streamer_24bpp,
};


int Bitmap::draw (void *dest, uint8_t all, bool raw)
{   
    FRESULT res;
    res = f_open(this->file, this->name, FA_READ);
    if (res != FR_OK) {
        return -1;
    }
    res = f_lseek(this->file, this->header.bfOffBits);
    if (res != FR_OK) {
        return -1;
    }
    int8_t stream_select = 0;
    switch (this->info->biBitCount) {
        case 0 : 
        case 2 :
        case 3 : 
        case 5 : 
        case 6 : 
        case 7 : 
        case 9 : 
        case 10 : 
        case 11 : 
        case 12 : 
        case 13 : 
        case 14 : 
        case 15 : 
        case 16 : 
        case 17 : 
        case 18 : 
        case 19 : 
        case 20 : 
        case 21 : 
        case 22 : 
        case 23 : 
        case 25 : 
        case 26 : 
        case 27 : 
        case 28 : 
        case 29 : 
        case 30 : 
        case 31 : 
        case 32 :
                    stream_select = -1; break;
        
        case 1 : stream_select = 0; break;
        case 4 : stream_select = 1; break;
        case 8 : stream_select = 2; break;
        case 24 : stream_select = 3; break;
    }
    if (stream_select == -1) {
        return -1;
    }
    cur_stream = get_free_stream();
    if (cur_stream == nullptr) {
        return -2;
    }
    cur_stream->img = this->info;
    cur_stream->dest = dest;
    UINT dmy;
#if (_USE_FORWARD == 1)
    while ((res == FR_OK) && (!f_eof(&this->file))) {
        res = f_forward(&this->file, stream_pack[stream_select], BMP_MEDIA_SIZE, &dmy);
    }
#else /*_USE_FORWARD != 1*/
    static uint8_t f_buf[BMP_MEDIA_SIZE];
    while ((res == FR_OK) && (!f_eof(this->file))) {
        dmy = 0;
        res = f_read(this->file, f_buf, BMP_MEDIA_SIZE, &dmy);
        stream_pack[stream_select](f_buf, dmy);
        
    }
#endif /*_USE_FORWARD*/
    free_stream(cur_stream);
    //f_close(&this->file);
    return 0;
}

uint32_t Bitmap::getWidth ()
{
    return this->info->biWidth;
}

uint32_t Bitmap::getHeight ()
{
    return this->info->biHeight;
}



BitmapFactory::BitmapFactory ()
{
    
}

Bitmap *BitmapFactory::open (const char *path, FIL *file)
{
    Bitmap *bmp = nullptr;
    #ifdef EXCEPTIONS__
    try {
        bmp = new Bitmap(path);
        } catch (exception::Throwable *t) {
        return nullptr;
    }
     this->openBmps.add(bmp);   
    #else
        bmp = new Bitmap(path, file);
        if (bmp == nullptr) {
            return nullptr;
        }
        if (bmp->exist == false) {
            return nullptr;
        }
        this->openBmps.addFirst(bmp);
    #endif
    return bmp;
}

int BitmapFactory::close (Bitmap *bmp)
{
    Bitmap *b = this->openBmps.getFirst();
    while (b != nullptr) {
        if (b == bmp) {
            this->openBmps.remove(b);
            delete b;
            return 0;
        }
        b = b->next();
    }
    return -1;
}

int BitmapFactory::close (const char *path)
{
    
    return 0;
}

void BitmapFactory::closeAll ()
{
    
}


