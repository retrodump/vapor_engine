/************************************************************************
*
* vapor3D Engine � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#include "vapor/PCH.h"

//---------------------------------------------------------------------//
// Audio-related Includes
//---------------------------------------------------------------------//

#include <vapor/audio/Device.h>
#include <vapor/audio/Buffer.h>
#include <vapor/audio/Context.h>
#include <vapor/audio/Source.h>

//---------------------------------------------------------------------//
// Math-related Includes
//---------------------------------------------------------------------//

#include <vapor/math/Math.h>
#include <vapor/math/Vector2.h>
#include <vapor/math/Vector3.h>
#include <vapor/math/Ray.h>
#include <vapor/math/BoundingBox.h>
#include <vapor/math/BoundingSphere.h>
#include <vapor/math/Noise.h>

//---------------------------------------------------------------------//
// Resource-related Includes
//---------------------------------------------------------------------//

#include <vapor/resources/Image.h>
#include <vapor/resources/MS3D.h>
#include <vapor/resources/Sound.h>
#include <vapor/resources/Font.h>

//---------------------------------------------------------------------//
// Scene-related Includes
//---------------------------------------------------------------------//

#include <vapor/scene/Group.h>
#include <vapor/scene/Scene.h>
#include <vapor/scene/Camera.h>
#include <vapor/scene/FirstPersonCamera.h>
#include <vapor/scene/Sound.h>
#include <vapor/scene/Listener.h>
#include <vapor/scene/Geometry.h>
#include <vapor/scene/Model.h>
#include <vapor/scene/Grid.h>
#include <vapor/scene/Light.h>
#include <vapor/scene/Skydome.h>
#include <vapor/scene/Water.h>
#include <vapor/scene/Billboard.h>
#include <vapor/scene/Body.h>
#include <vapor/scene/Tags.h>

//---------------------------------------------------------------------//
// Render-related Includes
//---------------------------------------------------------------------//

#include <vapor/render/Viewport.h>
#include <vapor/render/Target.h>
#include <vapor/render/VertexBuffer.h>
#include <vapor/render/Texture.h>
#include <vapor/render/Program.h>
#include <vapor/render/Renderable.h>
#include <vapor/render/Shader.h>
#include <vapor/render/Quad.h>
#include <vapor/render/TextureManager.h>
#include <vapor/render/ProgramManager.h>
#include <vapor/render/FBO.h>
#include <vapor/render/FrameStatistics.h>
#include <vapor/render/DebugGeometry.h>

//---------------------------------------------------------------------//
// Input-related Includes
//---------------------------------------------------------------------//

#include <vapor/input/InputManager.h>
#include <vapor/input/InputMap.h>
#include <vapor/input/Keyboard.h>
#include <vapor/input/Mouse.h>

//---------------------------------------------------------------------//
// VFS-related Includes
//---------------------------------------------------------------------//

#include <vapor/vfs/FileSystem.h>
#include <vapor/vfs/FileWatcher.h>
#include <vapor/vfs/File.h>

//---------------------------------------------------------------------//
// GUI-related Includes
//---------------------------------------------------------------------//

#include <vapor/gui/Overlay.h>
#include <vapor/gui/Label.h>
#include <vapor/gui/Window.h>
#include <vapor/gui/Console.h>

//---------------------------------------------------------------------//
// Scripting-related Includes
//---------------------------------------------------------------------//

#include <vapor/script/State.h>
#include <vapor/script/Script.h>

//---------------------------------------------------------------------//
// Physics-related Includes
//---------------------------------------------------------------------//

#include <vapor/physics/Physics.h>

//---------------------------------------------------------------------//
// Terrain-related Includes
//---------------------------------------------------------------------//

#include <vapor/terrain/Terrain.h>
#include <vapor/terrain/Cell.h>

//---------------------------------------------------------------------//
// Paging-related Includes
//---------------------------------------------------------------------//

#include <vapor/paging/PageManager.h>
#include <vapor/paging/Page.h>

//---------------------------------------------------------------------//
// General Includes
//---------------------------------------------------------------------//

#include <vapor/Engine.h>
#include <vapor/Log.h>
#include <vapor/Profiler.h>
#include <vapor/TaskManager.h>