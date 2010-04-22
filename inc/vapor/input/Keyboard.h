/************************************************************************
*
* vapor3D Engine � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#pragma once

#include "vapor/input/Device.h"
#include "vapor/input/KeyboardEvents.h"

namespace vapor { namespace input {

//-----------------------------------//

/**
 * Keyboard device.
 */

class VAPOR_API Keyboard : public Device
{
public:	

	Keyboard();

	// Returns the current state of the keys.
	IMPLEMENT_GETTER(KeyState, const std::vector< bool >&, keyState)

	// Checks if the given key is pressed.
	bool isKeyPressed( Keys::Enum keyCode ) const;

	//-----------------------------------//
	// Events
	//-----------------------------------//

	fd::delegate< void( const KeyEvent& ) > onKeyPress;
	fd::delegate< void( const KeyEvent& ) > onKeyRelease;

private:
	
	// Processes an event (only if it's a keyboard event).
	virtual void processEvent( const input::Event& event );

	// Return this device as a keyboard.
	IMPLEMENT_GETTER(Type, const DeviceType::Enum, DeviceType::Keyboard)

	// Handles a key press event.
	void keyPressed( const KeyEvent& keyEvent );
	
	// Handles a key release event.
	void keyReleased( const KeyEvent& keyEvent );
	
	// Current key state.
	std::vector< bool > keyState;
	
	// Last key pressed.
	Keys::Enum lastKey;
};

//-----------------------------------//

} } // end namespaces
