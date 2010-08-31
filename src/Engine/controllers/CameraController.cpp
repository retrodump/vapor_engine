/************************************************************************
*
* vapor3D Engine � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#include "vapor/PCH.h"
#include "vapor/controllers/CameraController.h"

namespace vapor {

//-----------------------------------//

BEGIN_CLASS_PARENT_ABSTRACT(CameraController, Controller)
	FIELD_PRIMITIVE(CameraController, float, lookSensivity)
	FIELD_PRIMITIVE(CameraController, float, moveSensivity)
END_CLASS()

//-----------------------------------//

static const float DEFAULT_MOVE_SENSIVITY = 100.0f;
static const float DEFAULT_LOOK_SENSIVITY = 20.0f;

//-----------------------------------//

CameraController::CameraController()
	: moveSensivity( DEFAULT_MOVE_SENSIVITY )
	, lookSensivity( DEFAULT_LOOK_SENSIVITY )
{ }

//-----------------------------------//

} // end namespace