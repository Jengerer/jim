#ifndef INVENTORY_LISTENER_H
#define INVENTORY_LISTENER_H

class IInventoryListener
{
public:

	virtual void OnItemMove( Item* item ) = 0;


#endif // INVENTORY_LISTENER_H