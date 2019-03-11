#include "sprite.h"
#include "string.h"





SpriteRAM::SpriteRAM ()
{

}

SpriteRAM::~SpriteRAM ()
{
    
}

void SpriteRAM::init (SPR_WORD_T mem_base, SPR_WORD_T mem_size)
{
    this->mem_base = mem_base;
    this->mem_size = mem_size;
    this->mem_p = mem_base;
    this->mem_free = mem_size;
    this->link = nullptr;
}

Sprite *SpriteRAM::allocSprite (SPR_SIZE_T w, SPR_SIZE_T h, const char *name)
{
    SPR_WORD_T imgSize = w * h * sizeof(SPR_COLOR_T);
    int name_len = strlen(name);
    Sprite *sprite = (Sprite *)this->mem_p;
    sprite->size = sizeof(Sprite) + name_len;
    sprite->ram = (SPR_COLOR_T *)(this->mem_p + sprite->size);
    sprite->w = w;
    sprite->h = h;
    memcpy(sprite->name, name, name_len);
    this->mem_p = (SPR_WORD_T)sprite->ram + imgSize;
    
    if (this->link == nullptr) {
        this->link = sprite;
    } else {
        this->link->link = sprite;
    }
    sprite->link = nullptr;
    return sprite;
}

Sprite *SpriteRAM::getSprite (const char *name)
{
    if (this->link == nullptr) {
        return nullptr;
    }
    Sprite *sprite = (Sprite *)this->mem_p;
    while (sprite != nullptr) {
        if (strcmp(sprite->name, name) == 0) {
            return sprite;
        }
        sprite = (Sprite *)sprite->link;
    }
    return nullptr;
}


void SpriteRAM::clear ()
{
    this->mem_p = this->mem_base;
    this->mem_free = this->mem_size;
    this->link = nullptr;
}

            

