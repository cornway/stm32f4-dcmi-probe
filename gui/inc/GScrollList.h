#ifndef GSCROLL_LIST_CLASS
#define GSCROLL_LIST_CLASS

#include "iterable.h"
#include "graphic.h"
#include "gComponent.h"
#include "gPaletteComponent.h"
#include "gSelectable.h"


template <typename Range, typename Color, Color white>
class GScrollList :  public Link<GScrollList<Color, Range, white> >,
										 public GComponent<Color, Range>,
										 public virtual GPaletteComponent<Color, Range, white>,
										 public GSelectable<Color, Range, white> 
										{
	
										 private :
											 
										 public :
											 GScrollList (Graphic<Color, Range, white> *graphic) : GComponent<Color, Range> (),
                                                      GPaletteComponent<Color, Range, white> (graphic),
                                                      GSelectable<Color, Range, white> (graphic)
											 {
													 value = 50;
													 topValue = 100;
													 bottomValue = 0;
													 stripColor = 0x7a77;
													 trackColor = 0xFccc;
													 trackDelayColor = 0x000F;
													 DelayValue = value;
													 anchor = 5;
													 anchorLevel = 53;
													 link = nullptr;
													 this->setSelectBorderWidth(5);
											 } 
};

#endif /*GSCROLL_LIST_CLASS*/

/**/

