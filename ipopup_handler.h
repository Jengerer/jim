#ifndef IPOPUP_HANDLER_H
#define IPOPUP_HANDLER_H

class IPopupHandler
{

public:

	// Mouse input handling.
	virtual void on_popup_clicked( Popup* popup ) = 0;
	virtual void on_popup_released( Popup* popup ) = 0;

	// Key input handling.
	virtual void on_popup_key_pressed( Popup* popup ) = 0;
	virtual void on_popup_key_released( Popup* popup ) = 0;

};

#endif // IPOPUP_HANDLER_H