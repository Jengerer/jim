#include "ToggleSet.h"

#define TOGGLE_SET_SPACING			10
#define TOGGLE_SET_PADDING			10
#define TOGGLE_SET_GRID_WIDTH		3

#define TOGGLE_SET_RADIUS			10
#define TOGGLE_SET_COLOUR			D3DCOLOR_XRGB( 42, 39, 37 )
#define TOGGLE_SET_STROKE_COLOUR	D3DCOLOR_RGBA( 255, 255, 255, 150 )
#define TOGGLE_SET_STROKE_SIZE		5

#define TOGGLE_SET_FONT_FACE		"TF2 Build"
#define TOGGLE_SET_FONT_SIZE		18
#define TOGGLE_SET_FONT_COLOUR		D3DCOLOR_XRGB( 255, 255, 255 )
#define TOGGLE_SET_FONT_BOLDED		false

#define TOGGLE_SET_MIN_WIDTH		150

Font *ToggleSet::titleFont_ = nullptr;

ToggleSet::ToggleSet( const string& nameSetA, const string& nameSetB, float x, float y ) : Popup( x, y )
{
	// Create titles.
	titleSetA_ = new Text( titleFont_ );
	titleSetA_->SetText( nameSetA );
	titleSetA_->Pack();

	titleSetB_ = new Text( titleFont_ );
	titleSetB_->SetText( nameSetB );
	titleSetB_->Pack();

	// Create rectangle.
	roundedRect_ = new RoundedRectangle( 0, 0, TOGGLE_SET_RADIUS, TOGGLE_SET_COLOUR );
	roundedRect_->SetStroke( TOGGLE_SET_STROKE_SIZE, TOGGLE_SET_STROKE_COLOUR );
	roundedRect_->SetLocalPosition( 0, 0 );
	Add( roundedRect_ );

	// Create okay and cancel buttons.
	okayButton_ = new LabelButton( "okay", nullptr );
	okayButton_->Pack();
	cancelButton_ = new LabelButton( "cancel", nullptr );
	cancelButton_->Pack();

	// Create layouts.
	//layoutSetA_ = new GridLayout( TOGGLE_SET_GRID_WIDTH );
	layoutSetA_ = new HorizontalLayout();
	layoutSetA_->SetSpacing( TOGGLE_SET_SPACING );

	//layoutSetB_ = new GridLayout( TOGGLE_SET_GRID_WIDTH );
	layoutSetB_ = new HorizontalLayout();
	layoutSetB_->SetSpacing( TOGGLE_SET_SPACING );

	buttonLayout_ = new HorizontalLayout();
	buttonLayout_->SetSpacing( TOGGLE_SET_SPACING );
	buttonLayout_->Add( okayButton_ );
	buttonLayout_->Add( cancelButton_ );
	buttonLayout_->Pack();

	setLayout_ = new VerticalLayout();
	setLayout_->SetSpacing( TOGGLE_SET_SPACING );
	setLayout_->SetAlignType( ALIGN_CENTER );
	setLayout_->SetLocalPosition( TOGGLE_SET_PADDING, TOGGLE_SET_PADDING );
	setLayout_->Add( titleSetA_ );
	setLayout_->Add( layoutSetA_ );
	setLayout_->Add( titleSetB_ );
	setLayout_->Add( layoutSetB_ );
	setLayout_->Add( buttonLayout_ );
	Add( setLayout_ );

	Pack();
}

ToggleSet::~ToggleSet( void )
{
	// Remove all buttons.
	while (!buttonSetA_.empty()) {
		Button *button = buttonSetA_.back();
		RemoveSetA( button );
	}

	while (!buttonSetB_.empty()) {
		Button *button = buttonSetB_.back();
		RemoveSetB( button );
	}
}

void ToggleSet::Pack( void )
{
	layoutSetA_->Pack();
	layoutSetB_->Pack();
	setLayout_->Pack();
	roundedRect_->SetSize( setLayout_->GetWidth() + TOGGLE_SET_PADDING * 2, setLayout_->GetHeight() + TOGGLE_SET_PADDING * 2 );
	SetSize( roundedRect_->GetWidth(), roundedRect_->GetHeight() );
}

void ToggleSet::AddSetA( Button *button )
{
	layoutSetA_->Add( button );
	buttonSetA_.push_back( button );
}

void ToggleSet::AddSetB( Button *button )
{
	layoutSetB_->Add( button );
	buttonSetB_.push_back( button );
}

void ToggleSet::RemoveSetA( Button *button )
{
	RemoveSet( &buttonSetA_, button );
	layoutSetA_->Remove( button );
}

void ToggleSet::RemoveSetB( Button *button )
{
	RemoveSet( &buttonSetB_, button );
	layoutSetB_->Remove( button );
}

void ToggleSet::RemoveSet( ButtonList *set, Button *button )
{
	ButtonList::iterator i, end;
	for (i = set->begin(), end = set->end(); i != end; ++i) {
		if (*i == button) {
			set->erase( i );
			return;
		}
	}
}

bool ToggleSet::InSetA( Button *button ) const
{
	return InSet( &buttonSetA_, button );
}

bool ToggleSet::InSetB( Button *button ) const
{
	return InSet( &buttonSetB_, button );
}

bool ToggleSet::InSet( const ButtonList *set, Button *button ) const
{
	vector<Button*>::const_iterator i, end;
	for (i = set->begin(), end = set->end(); i != end; ++i) {
		if (*i == button) {
			return true;
		}
	}

	return false;
}

bool ToggleSet::OnMouseMoved( Mouse *mouse )
{
	// Pass message to children.
	ButtonList::iterator i, end;
	for (i = buttonSetA_.begin(), end = buttonSetA_.end(); i != end; ++i) {
		Button *button = *i;
		button->OnMouseMoved( mouse );
	}

	for (i = buttonSetB_.begin(), end = buttonSetB_.end(); i != end; ++i) {
		Button *button = *i;
		button->OnMouseMoved( mouse );
	}

	okayButton_->OnMouseMoved( mouse );
	cancelButton_->OnMouseMoved( mouse );

	return true;
}

bool ToggleSet::OnLeftClicked( Mouse *mouse )
{
	if (mouse->IsTouching( this )) {
		return Draggable::OnLeftClicked( mouse );
	}

	SetState( POPUP_STATE_KILLED );
	return false;
}

bool ToggleSet::OnLeftReleased( Mouse *mouse )
{
	Draggable::OnLeftReleased( mouse );
	if (!mouse->IsTouching( this )) {
		return false;
	}

	// Check children.
	ButtonList::iterator i, end;
	bool hitButton_ = false;
	for (i = buttonSetA_.begin(), end = buttonSetA_.end(); i != end; ++i) {
		Button *button = *i;
		if (mouse->IsTouching( button )) {
			RemoveSetA( button );
			AddSetB( button );
			hitButton_ = true;
			break;
		}
	}

	if (!hitButton_) {
		for (i = buttonSetB_.begin(), end = buttonSetB_.end(); i != end; ++i) {
			Button *button = *i;
			if (mouse->IsTouching( button )) {
				RemoveSetB( button );
				AddSetA( button );
				break;
			}
		}
	}

	// Get old size for repositioning.
	int oldWidth = GetWidth();
	int oldHeight = GetHeight();

	Pack();

	// Reposition aligned center.
	// TODO: Maybe have a component-based alignment feature.
	/*SetPosition( 
		GetX() - ((GetWidth() - oldWidth) >> 1),
		GetY() - ((GetHeight() - oldHeight) >> 1 ) );*/

	return true;
}

void ToggleSet::Precache( DirectX *directX )
{
	titleFont_ = directX->CreateFont( 
		TOGGLE_SET_FONT_FACE, 
		TOGGLE_SET_FONT_SIZE, 
		TOGGLE_SET_FONT_BOLDED );
}

void ToggleSet::Release( void )
{
	if (titleFont_ != nullptr) {
		delete titleFont_;
		titleFont_ = nullptr;
	}
}