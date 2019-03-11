#ifndef GIF_H
#define GIF_H

#include "abstract.h"
#include "iterable.h"
#include "ff.h"
#include "stdint.h"

#define GIF_PACKED  __packed

typedef GIF_PACKED struct {
    uint8_t r, g, b;
} GIF_RGB_BLOCK;

typedef GIF_PACKED struct {
    uint8_t giSignature[3]; /*"gif"*/
    uint8_t giVersion[3];
    uint16_t giLogicWidth;
    uint16_t giLogicHeight;
    uint8_t giOption;
    uint8_t giBackGround;
    uint8_t giAspect;
} GIF_FILE_HEADER;

typedef GIF_PACKED struct {
    GIF_RGB_BLOCK *giColorMap;
    uint8_t *giBlocks;
    uint8_t *trailer;
} GIF_DATA;


typedef GIF_PACKED struct {
    GIF_FILE_HEADER giFileHeader;
    GIF_DATA giData;
} GIF_HEADER;


typedef GIF_PACKED struct {
    uint8_t giSeparator;
    uint16_t giLeft;
    uint16_t giTop;
    uint16_t giWidth;
    uint16_t giHeight;
    uint8_t giOption;
    
} GIF_IMAGE_BLOCK;

typedef GIF_PACKED struct {
    GIF_RGB_BLOCK *giColorMap;
    uint8_t giLZWCodeSize;
    uint8_t giBlockSize;
    uint8_t *giImageData;
    uint8_t giTerminator;
} GIF_IMAGE_DATA_BLOCK;


typedef GIF_PACKED struct {
    uint8_t giExtIntro;
    uint8_t giGraphCtlLabel;
    uint8_t giBlkSize;
    uint8_t giOption;
    uint16_t giDelayTime;
    uint8_t giTraColorIndex;
    uint8_t giTerminator;
} GIF_CTRL_EXT;

typedef GIF_PACKED struct {
    uint8_t giExtIntro;
    uint8_t giCommentLabel;
    uint8_t giBlkSizeC;
    uint8_t giBlkSize;
    uint8_t *giCommData;
    uint8_t *giTerminator;
} GIF_COMM_EXT;

typedef GIF_PACKED struct {
    uint8_t giExtIntro;
    uint8_t giPlaTextLabel;
    uint8_t giBlkSize;
    uint16_t giTxtGridLeftPos;
    uint16_t giTxtGridTopPos;
    uint16_t giTxtGridWidth;
    uint16_t giTxtGridHeight;
    uint8_t giCharCellWidth;
    uint8_t giCharCellHight;
    uint8_t giTxtForeground;
    uint8_t giTxtBackGround;
    uint8_t giPlainSize;
    uint8_t *giPlaTxtData;
    uint8_t *giTrminator;
} GIF_TEXT_EXT;

enum {
    GIF_STD_87A,
    GIF_STD_89A,
};

typedef GIF_PACKED struct {
    uint8_t giExtIntro;
    uint8_t giAppLabel;
    uint8_t giBlkSizeC;
    uint8_t giAppIdent[8];
    uint8_t giBlkSize;
    uint8_t *giAppData;
    uint8_t *giTrminator;
} GIF_APP_EXT;

class GifFactory;
class Gif;

class GIF_IMAGE_HEADER : public Link<GIF_IMAGE_HEADER> {
    private :
        GIF_IMAGE_BLOCK *imageInfo;
        GIF_IMAGE_DATA_BLOCK *imageData; 
        friend class Gif;
    public :
        GIF_IMAGE_HEADER (GIF_IMAGE_BLOCK *info, GIF_IMAGE_DATA_BLOCK *data);
};

class GifFactory;

class Gif : public Link<Gif> {
    private :
            GIF_HEADER header;
            GIF_DATA *data;
            vector::Vector<GIF_IMAGE_HEADER> imagesArray;
            FIL file;
            int8_t version;
            Gif (const char *path);
            ~Gif ();
    public :
        int play (void *dest);
};


class GifFactory {
    private :
        
    public :
        GifFactory ();
        ~GifFactory ();
        Gif *open (const char *path);
        int close (Gif *gif);
        int close (const char *path);
};

#endif /*GIF_H*/

