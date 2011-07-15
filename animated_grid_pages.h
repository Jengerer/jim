#ifndef ANIMATED_GRID_PAGES_H
#define ANIMATED_GRID_PAGES_H

#include "slot_grid_pages.h"

class AnimatedGridPages : public SlotGridPages
{
public:

	AnimatedGridPages( unsigned int pageWidth, unsigned int pageHeight,
		unsigned int pageSpacing, unsigned int slotSpacing );

	void UpdateView();

private:

	float viewSpeed_;
	float viewPosition_;

};

#endif // ANIMATED_GRID_PAGES_H