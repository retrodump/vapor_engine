/************************************************************************
*
* vaporEngine (2008-2009)
*
*	<http://www.portugal-a-programar.org>
*
************************************************************************/

#include "vapor/PCH.h"

#ifdef VAPOR_RENDERER_OPENGL

#include "vapor/render/Device.h"
#include "vapor/render/Adapter.h"
#include "vapor/render/BufferManager.h"

#include "vapor/render/GL.h"

namespace vapor {
	namespace render {

//-----------------------------------//

Device::Device()
	: clearColor( math::Colors::White ), adapter( nullptr ),
	window( nullptr ), bufferManager( nullptr ), activeTarget( nullptr )
{

}

//-----------------------------------//

void Device::init()
{
	info( "render::gl", "Creating OpenGL rendering device" );

	if( !activeTarget || !window ) 
	{
		warn( "render::gl", "No current OpenGL context found, stuff may fail" );
	}

	checkExtensions();

	adapter = new Adapter();
	bufferManager = new BufferManager();
	TextureManager::getInstance();

	glEnable(GL_DEPTH_TEST);
}

//-----------------------------------//

Device::~Device()
{
	info("render::opengl", "Closing OpenGL rendering device");

	// TODO: delete all OpenGL resources (shaders, textures...)

	delete adapter;
	delete window;
	delete bufferManager;
}

//-----------------------------------//

Window* Device::getWindow() const
{
	return window;
}

//-----------------------------------//

Adapter* Device::getAdapter() const
{
	return adapter;
}

//-----------------------------------//

void Device::setClearColor(math::Color c)
{
	clearColor = c;
}

//-----------------------------------//

void Device::updateTarget()
{
	activeTarget->update();
}

//-----------------------------------//

void Device::setRenderTarget(RenderTarget* renderTarget)
{
	activeTarget = renderTarget;
}

//-----------------------------------//

BufferManager* Device::getBufferManager() const
{
	return bufferManager;
}

//-----------------------------------//

TextureManager* Device::getTextureManager() const
{
	return const_cast<TextureManager*>( &textureManager );
}

//-----------------------------------//

void Device::render( RenderQueue& queue, math::Matrix4 viewMatrix ) 
{
	activeTarget->makeCurrent();

	glClear( GL_DEPTH_BUFFER_BIT );

	foreach( RenderState state, queue )
	{
		ProgramPtr program = state.renderable->getMaterial()->getProgram();

		//program->setUniform( "vp_ProjectionMatrix", state.modelMatrix );
		program->setUniform( "vp_ModelMatrix", state.modelMatrix );
		program->setUniform( "vp_ViewMatrix", viewMatrix );
		program->setUniform( "vp_ModelViewMatrix", viewMatrix * state.modelMatrix );

		state.renderable->render( *this );
	}
}

//-----------------------------------//

void Device::checkExtensions()
{
	// init GLEW (OpenGL Extension Wrangler)
	GLenum err = glewInit();
	
	if( err != GLEW_OK )
	{
		error( "render::opengl", 
			"Failed to initialize GLEW: %s", glewGetErrorString( err ) );

		return;
	}

	info( "render::opengl", "Using GLEW version %s", 
		glewGetString( GLEW_VERSION ) );

	//if( !GLEW_VERSION_2_1 )
	//{
	//	log::Log::MessageDialog( "You need at least OpenGL 2.1 to run this.",
	//		log::LogLevel::Error );

	//	exit( 1 );
	//	
	//	// TODO: exit program in a structured manner
	//}
}

//-----------------------------------//

render::Target* Device::getRenderTarget() const
{
	return activeTarget;
}

//-----------------------------------//

//RenderDevice::render( RenderQueue& queue, Matrix4x4 invCamera = Matrix4x4::Identity ) // ?
//{
//    // 1. sort renderables by state
//    sortByMaterial( queue );
//        
//    // 2. for each node render
//    foreach ( renderable in queue )
//    {
//        // change state if needed
//        changeState( renderable );
//        
//        // concantenate camera and renderable matrices
//        glLoadMatrix( renderable->second * invCamera );
//        
//        // render it
//        renderable->first.render( *this );
//    }
//}

//-----------------------------------//

Window& Device::createWindow( const WindowSettings& settings )
{
	Window& window = Window::createWindow( settings );

	this->window = &window;

	setRenderTarget(&window);

	return window;
}

//-----------------------------------//

void Device::clearTarget()
{
	glClearColor( clearColor.r, clearColor.g, clearColor.b, clearColor.a );
	glClear( GL_COLOR_BUFFER_BIT );
}

//-----------------------------------//

} } // end namespaces

#endif