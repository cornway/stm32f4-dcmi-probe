#ifndef SNAKE_APP_H
#define SNAKE_APP_H

#include "app_macro.h"
#include "app_def.h"
#include "device_conf.h"
#include "vm.h"
#include "vmapi.h"
#include "gui.h"


#define SPRITE_COUNT (6U)

class _Sprite {
    private :
        tImage<color_t> icon;
    public :
        _Sprite ()
        {
            
        }
        
        ~_Sprite ()
        {
            
        }
        
        void setIcon (tImage<color_t> *icon)
        {
            this->icon.Image = icon->Image;
            this->icon.W = icon->W;
            this->icon.H = icon->H;
        }
        
        tImage<color_t> *
        getIcon ()
        {
            return &this->icon;
        }
};

class SnakeSegment : public Link<SnakeSegment>,
                     public Dimension<range_t> {
    private :
        
    public :
        SnakeSegment ()
        {
            
        }
        
        ~SnakeSegment ()
        {
            
        }
};
                     
enum {
    DIR_FORWARD = 1, 
    DIR_BACKWARD = -1,
    DIR_LEFT = 2,
    DIR_RIGHT = -2,
};


extern _Sprite megaSprites[3];

class Snake : public Dimension<range_t> {
    private :
        vector::Vector<SnakeSegment> segments;
        int8_t direction;
        tImage<color_t> icon;
        int exp;
    
        void subStep (SnakeSegment *seg)
        {
            Dimension<range_t> d = this->segments.getFirst()->getDimension();
            switch (this->direction) {
                    case DIR_FORWARD :  d.moveUp();
                                        if (d.y >= (this->y + this->h - d.h)) {
                                            d.y = this->y;
                                        }
                        break;
                    case DIR_BACKWARD : d.moveDown();
                                        if (d.y < this->y) {
                                            d.y = (this->y + this->h) - d.h;
                                        }
                                        
                        break;
                    case DIR_LEFT :     d.moveLeft();
                                        if (d.x < this->x) {
                                            d.x = (this->x + this->w) - d.w;
                                        }
                                        
                        break;
                    case DIR_RIGHT :    d.moveRight();
                                        if (d.x >= (this->x + this->w)) {
                                            d.x = this->x;
                                        }
                        break;
            }
            seg->setSize(d.getBox());
            this->segments.addFirst(seg);
        }
    
    public :
        Snake (Box<range_t> b)
        {
            this->setSize(b);
            this->direction = DIR_FORWARD;
            this->icon.W = 0;
            this->icon.H = 0;
            this->exp = 0;
        }
        
        ~Snake ()
        {
            SnakeSegment *seg = this->segments.getFirst();
            SnakeSegment *nseg = seg->next();
            while (seg != nullptr) {
                this->segments.remove(seg);
                delete seg;
                seg = nseg;
                nseg = nseg->next();
            }
        }
        
        void setIcon (tImage<color_t> *icon)
        {
            this->icon.Image = icon->Image;
            this->icon.W = icon->W;
            this->icon.H = icon->H;
        }
        
        void create (int count)
        {
            SnakeSegment *seg = nullptr;
            Dimension<range_t> d(0, 0, this->icon.W, this->icon.H);
            d.setOrigins(this->getOrigins());
            for (int i = 0; i < count; i++) {
                seg = new SnakeSegment();
                seg->setSize(d.getBox());
                this->segments.addFirst(seg);
                d.moveUp();
            }
            
        }
        
        template <typename G>
        void draw (G *g)
        {
            SnakeSegment *seg = this->segments.getFirst();
            while (seg != nullptr) {
                g->fill(seg->getBox(), &this->icon, COLOR_WHITE, 0, 1);
                seg = seg->next();
            }
        }
        
        int step ()
        {
            SnakeSegment *seg = this->segments.removeLast();
            this->subStep(seg);
        }
        
        int setDirection (int8_t direction)
        {
            if (this->direction == -direction) {
                return -1;
            }
            if (this->direction == direction) {
                return -1;
            }
            this->direction = direction;
            return 0;
        }
        
        void eat ()
        {
            SnakeSegment *seg = new SnakeSegment();
            this->subStep(seg);
        }
        
        bool testSelf ()
        {
            if (this->segments.size() < 4) {
                return 0;
            }
            auto seg = this->segments.get(3);
            Point<range_t> p = this->segments.getFirst()->getOrigins();
            while (seg != nullptr) {
                if (seg->testPoint(p) == true) {
                    return true;
                }
                seg = seg->next();
            }
            return false;
        }
        
        bool test (Point<range_t> p)
        {
            auto seg = this->segments.getFirst();
            while (seg != nullptr) {
                if (seg->testPoint(p) == true) {
                    return true;
                }
                seg = seg->next();
            }
            return false;
        }
        
        SnakeSegment *
        getHead ()
        {
            return this->segments.getFirst();
        }
        
        void addExp (int exp)
        {
            this->exp += exp;
        }
        
        int getExp ()
        {
            return this->exp;
        }
};

class Bonus : public Link<Bonus>,
              public Dimension<range_t> {
    private :
        tImage<color_t> icon;
        int exp;
    public :
        Bonus (Box<range_t> box, int exp)
        {
            this->setSize(box);
            this->exp = exp;
        }
        ~Bonus ()
        {
            
        }
        
        void setIcon (tImage<color_t> *icon)
        {
            this->icon.Image = icon->Image;
            this->icon.W = icon->W;
            this->icon.H = icon->H;
        }
        
        tImage<color_t> *
        getIcon ()
        {
            return &this->icon;
        }
        
        int getExp ()
        {
            return this->exp;
        }
};
              
class BonusEngine : public Dimension<range_t> {
    private :
        vector::Vector<Bonus> bonuses;
        //tImage<color_t> icon;
    public :
        BonusEngine (Box<range_t> box)
        {
            this->setSize(box);
        }
        
        ~BonusEngine ()
        {
            Bonus *b = this->bonuses.getFirst();
            Bonus *bn = b->next();
            while (b != nullptr) {
                this->bonuses.remove(b);
                delete b;
                b = bn;
                bn = bn->next();
            }
        }
        
        template <typename G>
        void draw (G *g)
        {
            Bonus *b = this->bonuses.getFirst();
            while (b != nullptr) {
                g->fill(b->getBox(), b->getIcon(), COLOR_WHITE, 0, 1);
                b = b->next();
            }
        }
        
        bool test (Point<range_t> p)
        {
            auto b = this->bonuses.getFirst();
            while (b != nullptr) {
                if (b->testPoint(p) == true) {
                    return true;
                }
                b = b->next();
            }
            return false;
        }
        
        void addRandom (Snake *s, int seed, int spriteCount, _Sprite *sprites)
        {
            if (this->bonuses.size() > 7) {
                return;
            }
            
            static int _seed = 0;
            _seed += seed / (_seed + 1) + (seed % 100);
            
            int exp = 0;
            tImage<color_t> *image;
            int m = (seed % 50);
            if ((m < 5) || (m > 40)) {
                image = megaSprites[m % 3].getIcon();
                exp = 500 * (m % 3) + 500;
            } else {
                image = sprites[(_seed + (seed / 5) % 40) % spriteCount].getIcon();
                exp = 50;
            }
            
            
            range_t _w = this->w / image->W - 1;
            range_t _h = this->h / image->H - 1;
            
            static range_t _x, _y;
            bool collision = true;
            Point<range_t> p;
            while (collision) {
                _x = ((_seed + ((_y / seed) * _seed)) % _w) * image->W + this->x + image->W;
                _y = ((_seed + _x / _seed + (_x * _seed)) % _h) * image->H + this->y + image->H;
                p.x = _x;
                p.y = _y;
                collision = this->test(p);
                if (collision == true) {
                    continue;
                }
                collision = s->test(p);
                _seed += 10;
                _y += 9;
            }
            
            
            Box<range_t> box = {_x, _y, image->W, image->W};
            Bonus *b = new Bonus(box, exp);
            b->setIcon(image);
            this->bonuses.addFirst(b);
        }
        
        int test (Snake *s)
        {
            auto h = s->getHead();
            Bonus *b = this->bonuses.getFirst();
            while (b != nullptr) {
                if (b->testOver(h->getBox()) == true) {
                    s->eat();
                    s->addExp(b->getExp());
                    this->bonuses.remove(b);
                    delete b;  
                    return 1;
                }
                b = b->next();
            }
            return 0;
        }
};

extern FontArray fontArray;

INT_T snake_app (WORD_T size, void *argv);

static void start ();
static void draw_snake (NonPalette<color_t, range_t, COLOR_WHITE> *component);
static void snake_listener (abstract::Event e);
static void update_sprites ();

#endif  /*SNAKE_APP_H*/