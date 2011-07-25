#include "toggle_set.h"

const unsigned int TOGGLE_SET_SPACING		= 10;
const unsigned int TOGGLE_SET_PADDING		= 10;
const unsigned int TOGGLE_SET_GRID_WIDTH	= 3;

const unsigned int TOGGLE_SET_RADIUS		= 10;
const Colour TOGGLE_SET_COLOUR				= Colour( 42, 39, 37 );
const unsigned int TOGGLE_SET_STROKE_SIZE	= 5;

const char* TOGGLE_SET_FONT_FACE			= "fonts/tf2build.ttf";
const unsigned int TOGGLE_SET_FONT_SIZE		= 18;
const Colour TOGGLE_SET_FONT_COLOUR			= COLOUR_WHITE;
const bool TOGGLE_SET_FONT_BOLDED			= false;

const char* TOGGLE_SET_BUTTON_FONT_FACE			= "fonts/tf2build.ttf";
const unsigned int TOGGLE_SET_BUTTON_FONT_SIZE	= 16;
const Colour TOGGLE_SET_BUTTON_FONT_COLOUR		= Colour( 42, 39, 37 );
const bool TOGGLE_SET_BUTTON_FONT_BOLDED		= false;

const unsigned int TOGGLE_SET_MIN_WIDTH			= 150;

Font* ToggleSet::titleFont_ = nullptr;
Font* ToggleSet::buttonFont_ = nullptr;

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
	roundedRect_->SetStroke( TOGGLE_SET_STROKE_SIZE, COLOUR_WHITE );
	Add( roundedRect_ );
	SetConstraint( roundedRect_, 0.0f, 0.0f );

	// Create okay and cancel buttons.
	okayButton_ = Button::CreateLabelButton( "okay", buttonFont_ );
	cancelButton_ = Button::CreateLabelButton( "cancel", buttonFont_ );
	okayButton_->Pack();
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
	setLayout_->Add( titleSetA_ );
	setLayout_->Add( layoutSetA_ );
	setLayout_->Add( titleSetB_ );
	setLayout_->Add( layoutSetB_ );
	setLayout_->Add( buttonLayout_ );
	Add( setLayout_ );
	SetConstraint( setLayout_, TOGGLE_SET_PADDING, TOGGLE_SET_PADDING );

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

bool ToggleSet::MouseMoved( Mouse *mouse )
{
	// Pass message to children.
	ButtonList::iterator i, end;
	for (i = buttonSetA_.begin(), end = buttonSetA_.end(); i != end; ++i) {
		Button *button = *i;
		button->MouseMoved( mouse );
	}

	for (i = buttonSetB_.begin(), end = buttonSetB_.end(); i != end; ++i) {
		Button *button = *i;
		button->MouseMoved( mouse );
	}

	okayButton_->MouseMoved( mouse );
	cancelButton_->MouseMoved( mouse );

	return true;
}

bool ToggleSet::MouseClicked( Mouse *mouse )
{
	SetState( POPUP_STATE_KILLED );
	return false;
}

bool ToggleSet::MouseReleased( Mouse *mouse )
{
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

void ToggleSet::Precache()
{
	titleFont_ = FontFactory::create_font( TOGGLE_SET_FONT_FACE, TOGGLE_SET_FONT_SIZE );
	buttonFont_ = FontFactory::create_font( TOGGLE_SET_BUTTON_FONT_FACE, TOGGLE_SET_BUTTON_FONT_SIZE );
}

void ToggleSet::Release( void )
{
	if (titleFont_ != nullptr) {
		delete titleFont_;
		titleFont_ = nullptr;
	}

	if (buttonFont_ != nullptr) {
		delete buttonFont_;
		buttonFont_ = nullptr;
	}
}