#ifndef ITEM_MANAGER_H
#define ITEM_MANAGER_H

#include "alert.h"
#include "application.h"
#include "backpack.h"
#include "button.h"
#include "definition_loader.h"
#include "item_display.h"
#include "notice.h"
#include "notification_queue.h"
#include "notification.h"
#include "toggle_set.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#define BUTTON_SPACING	5
#define BUTTON_Y		380

typedef std::map<EClassEquip, Button*> ButtonMap;

class ItemManager: public Application
{
public:
	ItemManager( void );
	virtual ~ItemManager( void );

	// Starting up.
	void LoadInterfaces( HINSTANCE instance );
	void CloseInterfaces( void );

	// Inventory and definition loading.
	void LoadResources( void );
	void LoadDefinitions( void );
	void LoadItemsFromWeb( void );

	// Application running functions.
	void RunApplication();
	void SetThink( void (ItemManager::*thinkFunction)( void ) );
	void DoThink();
	void Loading();
	void Running();
	void Exiting();

	// Steam handling.
	void HandleCallbacks( void );
	void HandleMessages( void );

	// Input handling.
	void HandleKeyboard( void );

	// Mouse input handling.
	virtual bool OnLeftClicked( Mouse *mouse );
	virtual bool OnLeftReleased( Mouse *mouse );
	virtual bool OnMouseMoved( Mouse *mouse );

	// Interface handling.
	Notice*			CreateNotice( const string& message );
	Alert*			CreateAlert( const string& message );
	void			CreateEquipSet( const Item *item );

	// Item display.
	void	UpdateItemDisplay( void );

	// Popup handlign.
	void	ShowPopup( Popup *popup );
	void	HidePopup( Popup *popup );
	void	HandlePopup( Popup *popup );
	void	RemovePopup( Popup* popup );

private:
	// Application interfaces.
	Backpack*		backpack_;
	void (ItemManager::*thinkFunction_)( void );

	// Threaded loader for resources.
	DefinitionLoader*	definitionLoader_;

	// User interface stacks.
	std::vector<Popup*>	popupStack_;

	// Display for item information.
	ItemDisplay			*itemDisplay_;

	// Progress notice.
	Notice				*loadProgress_;

	// Information notifications.
	NotificationQueue	*notifications_;

	// Alerts.
	Alert				*alert_;
	Alert				*error_;

	// Equipment management.
	ToggleSet			*equipSet_;
	ButtonMap			equipButtons_;
};

#endif // ITEM_MANAGER_H