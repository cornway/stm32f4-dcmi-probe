#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>
#include "memory_alloc.h"
#include "Screen_Driver.h"

class Screen : {
	public:
		Screen ();
	  void operator () (f2D, Frame2D<resolution_t> &); 
	  int32_t Refresh ();
    Fill_Class       <Screen> Fill;
	  Bitmap_Class     <Screen> Bitmap;
	  Circle_Class     <Screen> Circle;
	  Line_Class       <Screen> Line;
	private:
		Frame2D<resolution_t> *Frame;
	  ColorTypeDef back_color;
	  int32_t Normalize (f2D *);
	  int32_t Normalize (float, float, float, float);
	
	  friend class Bitmap_Class     <Screen>;
	  friend class Fill_Class       <Screen>;
	  friend class Circle_Class     <Screen>;
	  friend class Line_Class       <Screen>;
	  friend class Screen_Obj       <Screen>;
};
							 
class ScreenManager : public Locker ,
	                    public List_Iterator<Screen>{
  public:
		ScreenManager ();
	  void operator () (MemoryAllocator &);
	  Screen *Create ();
	  int32_t Kill (Screen *);
	  int32_t Refresh (uint32_t);
	private:
		MemoryAllocator *Heap;
	  
	
};



#endif /*SCREEN_H*/




