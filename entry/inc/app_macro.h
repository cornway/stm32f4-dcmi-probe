#ifndef APP_MACRO_H
#define APP_MACRO_H

#define     GET_SPRITE(label, s, b, path, name) \
do { \
    tImage<color_t> image; \
    uint32_t w, h; \
    b = bmp.open(path, file); \
    if (b != nullptr) { \
        w = b->getWidth(); \
        h = b->getHeight(); \
        s = spriteRAM.allocSprite(w, h, name); \
        b->draw(s->ram, 2, 0); \
        bmp.close(b); \
        image.W = w; \
        image.H = h; \
        image.Image = s->ram; \
        label->setIcon(&image); \
    } \
} while (0) \

#define     SET_SPRITE(label, s) \
do { \
    tImage<color_t> image; \
    image.W = s->w; \
    image.H = s->h; \
    image.Image = s->ram; \
    label->setIcon(&image); \
} while (0) \


#define     APP_CLEANUP(obj, sensor) \
do { \
    vm::lock(MEMORY_ALLOC_LOCK_ID); \
    delete obj; \
    sensor.removeAllListeners(); \
    vm::unlock(MEMORY_ALLOC_LOCK_ID); \
} while (0)

#define SNAPSHOT(ret, name) \
int8_t ret = 0; \
do { \
    switch (applicationControl.snapshotBpp) { \
        case 1 : ret = snapshot_1bpp(name); \
            break; \
        case 4 : ret = snapshot_4bpp(name); \
            break; \
        default :  ret = -1; \
            break; \
    } \
} while (0)

#endif  /*APP_MACRO_H*/
