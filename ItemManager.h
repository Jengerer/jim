#ifndef ITEM_MANAGER_H
#define ITEM_MANAGER_H

#include "Application.h"
#include "Notification.h"
#include "Alert.h"
#include "Button.h"

#include "Backpack.h"

#define BUTTON_SPACING	2
#define BUTTON_Y		380

class ItemManager: public Application
{
public:
	ItemManager( void );
	virtual ~ItemManager( void );

	// Initializing and closing.
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
	virtual bool OnRightClicked( Mouse *mouse );
	virtual bool OnRightReleased( Mouse *mouse );
	virtual bool OnMouseMoved( Mouse *mouse );

	// Interface handling.
	Notification*	CreateNotification( const string& message );
	Alert*	CreateAlert( const string& message );
	Button*	CreateButton( const string& caption, Texture *texture = 0, float x = 0.0f, float y = 0.0f );

	void	ShowPopup( Popup *popup );
	void	HidePopup( Popup *popup );
	void	HandlePopup( Popup *popup );
	void	RemovePopup( Popup* popup );

private:
	// Application interfaces.
	Backpack*		backpack_;

	// User interface stacks.
	deque<Popup*>	popupStack_;
	vector<Popup*>	popupList_;
	vector<Button*>	buttonList_;

	// Keyboard booleans.
	bool			leftPressed_;
	bool			rightPressed_;
	bool			enterPressed_;

	// Interface variables.
	Notification	*loadProgress_;
	Alert			*alert_;
	Alert			*error_;
	Button			*equipButton_, *craftButton_, *sortButton_;
};

#endif // ITEM_MANAGER_H