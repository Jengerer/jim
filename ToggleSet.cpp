#include "ToggleSet.h"

#define TOGGLE_SET_SPACING			10
#define TOGGLE_SET_PADDING			10
#define BUTTON_SPACING				10

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

ToggleSet::ToggleSet( const string& nameSetA, const string& nameSetB )
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
	Add( roundedRect_ );

	// Create layouts.
	layoutSetA_ = new VerticalLayout();
	layoutSetA_->SetSpacing( BUTTON_SPACING );
	layoutSetA_->SetMinimumWidth( TOGGLE_SET_MIN_WIDTH );
	layoutSetA_->Add( titleSetA_ );

	layoutSetB_ = new VerticalLayout();
	layoutSetB_->SetSpacing( BUTTON_SPACING );
	layoutSetB_->SetMinimumWidth( TOGGLE_SET_MIN_WIDTH );
	layoutSetB_->Add( titleSetB_ );

	setLayout_ = new HorizontalLayout();
	setLayout_->SetSpacing( TOGGLE_SET_SPACING );
	setLayout_->SetAlignType( ALIGN_TOP );
	setLayout_->Add( layoutSetA_ );
	setLayout_->Add( layoutSetB_ );
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

void ToggleSet::UpdatePosition( void )
{
	roundedRect_->SetPosition( GetX(), GetY() );
	setLayout_->SetPosition( GetX() + TOGGLE_SET_PADDING, GetY() + TOGGLE_SET_PADDING );
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

const vector<Button*>* ToggleSet::GetSetA( void ) const
{
	return &buttonSetA_;
}

const vector<Button*>* ToggleSet::GetSetB( void ) const
{
	return &buttonSetB_;
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

	return true;
}

bool ToggleSet::OnLeftClicked( Mouse *mouse )
{
	return mouse->IsTouching( this );
}

bool ToggleSet::OnLeftReleased( Mouse *mouse )
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

	Pack();
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