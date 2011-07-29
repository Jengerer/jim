#include "backpack.h"

#include <json/json.h>

#include "protobuf/base_gcmessages.pb.h"
#include "protobuf/gcsdk_gcmessages.pb.h"
#include "protobuf/steammessages.pb.h"

// Navigation constants.
const int PAGE_CHANGE_DELAY	= 500;
const int BUTTON_SPACING	= 5;

// Spring constants.
const float SPRING_STRENGTH	= 0.1f;
const float SPRING_DAMPING	= 0.7f;

const int BACKPACK_PADDING		= 25;
const int BACKPACK_PADDING_TOP	= 50;

const int SLOT_SPACING			= 5;
const int PAGE_SPACING			= BACKPACK_PADDING * 2;

Backpack::Backpack( unsigned int excludedSize ) : Inventory( 0, excludedSize )
{
	SetLoaded( false );
}

SlotGridPages* Backpack::CreateInventoryView( unsigned int width, unsigned int height ) const
{
	SlotGridPages* inventoryView = new AnimatedGridPages( width, height,
		PAGE_SPACING, SLOT_SPACING );
	inventoryView->AddPages( inventorySlots_ );
	return inventoryView;
}

SlotGridView* Backpack::CreateExcludedView() const
{
	SlotGridView* excludedView = new SlotGridView( excludedSlots_->GetSlotCount(), SLOT_SPACING );
	excludedView->AddSlots( excludedSlots_ );
	return excludedView;
}

bool Backpack::IsLoaded( void ) const
{
	return isLoaded_;
}

void Backpack::SetLoaded( bool isLoaded )
{
	isLoaded_ = isLoaded;
}

/*void Backpack::MoveCamera( void )
{
	// Add elastic speed.
	float cameraDistance = cameraDest_ - pages_->GetLocalX();
	cameraSpeed_ += cameraDistance*SPRING_STRENGTH;
	cameraSpeed_ *= SPRING_DAMPING;

	// Stop the camera if slowing and near destination.
	float pagesX;
	float pagesY = pages_->GetLocalY();
	if (abs( cameraSpeed_ ) < 2.0f && abs( cameraDistance ) < 1.0f) {
		cameraSpeed_ = 0.0f;
		pagesX = cameraDest_;
	}
	else {
		pagesX = pages_->GetLocalX() + cameraSpeed_;
	}

	pages_->SetLocalPosition( pagesX, pagesY );
	backpackLayout_->UpdateChild( pages_ );
}*/