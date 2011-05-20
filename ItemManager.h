#ifndef ITEM_MANAGER_H
#define ITEM_MANAGER_H

#include "Alert.h"
#include "Application.h"
#include "Backpack.h"
#include "Button.h"
#include "ItemDisplay.h"
#include "LabelButton.h"
#include "Notice.h"
#include "NotificationQueue.h"
#include "Notification.h"
#include "ToggleSet.h"

#define BUTTON_SPACING	5
#define BUTTON_Y		380

class ItemManager: public Application
{
public:
	ItemManager( void );
	virtual ~ItemManager( void );

	// Starting up.
	void LoadInterfaces( HINSTANCE instance );
	void CloseInterfaces( void );

	void LoadDefinitions( void );
	void LoadItemsFromWeb( void );

	void RunApplication( void );

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

	// User interface stacks.
	deque<Popup*>	popupStack_;
	vector<Popup*>	popupList_; // TODO: These might be dep'd.
	vector<Button*>	buttonList_;

	// Keyboard booleans.
	bool			leftPressed_;
	bool			rightPressed_;
	bool			enterPressed_;

	// Display for item information.
	ItemDisplay		*itemDisplay_;

	// Progress notice.
	Notice				*loadProgress_;

	// Information notifications.
	NotificationQueue	*notifications_;

	// Alerts.
	Alert				*alert_;
	Alert				*error_;

	// Inventory buttons.
	LabelButton			*equipButton_;
	LabelButton			*craftButton_;
	LabelButton			*sortButton_;

	// Equipment management.
	ToggleSet			*equipSet_;
	Button				*equipSoldier_;
	Button				*equipDemoman_;
};

#endif // ITEM_MANAGER_H