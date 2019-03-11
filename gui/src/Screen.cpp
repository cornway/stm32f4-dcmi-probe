#include "Screen.h"

Screen::Screen (){}
	
void Screen::operator () (f2D rect, Frame2D<resolution_t> &frame)
{
	this->Dispose(rect);
	this->Frame = &frame;
	this->Fill(*this);
	this->Bitmap(*this);
	this->Circle(*this);
	this->Line(*this);
}

int32_t Screen::Refresh ()
{
	return this->Fill.Fill(this->back_color);
}

int32_t Screen::Normalize (f2D *rect)
{
	
	if (rect->x > this->W) {
		  rect->x = this->W;
	}
	if (rect->x + rect->w > this->W) {
		  rect->w = this->W - rect->x;
	}
  if (rect->y > this->H) {
		  rect->y = this->H;
	}
	if (rect->y + rect->h > this->H) {
		  rect->h = this->H - rect->h;
	}
	
	rect->x += this->X0;
	rect->y += this->Y0;
	return 0;
}

int32_t Screen::Normalize (float x0, float y0, float x, float y)
{
	
	if (x0 > this->W) {
		  x0 = this->W;
	}
	if (x > this->W) {
		  x = this->W;
	}
  if (x0 > this->H) {
		  x0 = this->H;
	}
	if (y > this->H) {
		  x = this->H;
	}
	
	x0 += this->X0;
	x0 += this->Y0;
	return 0;
}





ScreenManager::ScreenManager (){}
	
void ScreenManager::operator () (MemoryAllocator &heap)
{
	this->Heap = &heap;
	this->Unlock();
}

Screen *ScreenManager::Create ()
{
	Screen *screen = (Screen *)this->Heap->New(sizeof(Screen));
	*this + screen;
	return screen;
}

int32_t ScreenManager::Kill (Screen *screen)
{
	if (screen == (Screen *)0) return -1;
	*this - screen;
	this->Heap->Delete(screen);
	return 0;
}

int32_t ScreenManager::Refresh (uint32_t flags)
{
	if (this->Test()) return -1;
	register uint32_t i = this->Contain();
	while (i--)
		this->Get(i)->Refresh();
	return 0;
}
