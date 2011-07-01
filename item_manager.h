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
#include "steam.h"
#include "toggle_set.h"
#include "vertical_layout.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <vector>

const unsigned int BUTTON_SPACING = 5;

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

	// User interface handling.
	void CreateLayout();

	// Application running functions.
	void RunApplication();
	void SetThink( void (ItemManager::*thinkFunction)( void ) );
	void DoThink();
	void Loading();
	void Running();
	void Exiting();

	// Steam handling.
	void HandleCallbacks( void );
	void HandleMessage( uint32 id, void* message, size_t size );
	void HandleProtobuf( uint32 id, void* message, size_t size );

	// Input handling.
	void HandleKeyboard( void );

	// Mouse input handling.
	virtual bool OnLeftClicked( Mouse *mouse );
	virtual bool OnLeftReleased( Mouse *mouse );
	virtual bool OnMouseMoved( Mouse *mouse );

	// Popup handling.
	void OnPopupClicked( Popup* popup );
	void OnPopupReleased( Popup* popup );

	// Interface handling.
	Notice*	CreateNotice( const string& message );
	Alert*	CreateAlert( const string& message );

	// Item display.
	void UpdateItemDisplay( void );

	// Popup handling.
	void ShowPopup( Popup* popup );
	void HidePopup( Popup* popup );
	void RemovePopup( Popup* popup );
	void HandlePopup( Popup* popup );

private:

	// Application interfaces.
	Steam*				steam_;
	Backpack*			backpack_;

	// User interface members.
	SlotGridPages*		pagesView_;
	SlotGridView*		excludedView_;

	// Running think function.
	void (ItemManager::*thinkFunction_)( void );

	// Threaded loader for resources.
	DefinitionLoader*	definitionLoader_;

	// User interface stacks.
	std::vector<Popup*>	popups_;

	// Display for item information.
	ItemDisplay*		itemDisplay_;

	// Progress notice.
	Notice				*loadProgress_;

	// Information notifications.
	NotificationQueue	*notifications_;

	// Alerts.
	Alert				*alert_;
	Alert				*error_;

};

#endif // ITEM_MANAGER_H