#ifndef POPUP_LISTENER_HPP
#define POPUP_LISTENER_HPP

/*
 * Interface for handling popup events.
 */
class PopupListener
{

public:

	// Popup kill events.
	virtual bool on_popup_killed( Popup* popup ) = 0;

};

#endif // POPUP_LISTENER_HPP