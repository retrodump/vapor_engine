/************************************************************************
*
* vapor3D Engine � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#include "vapor/PCH.h"

#ifdef VAPOR_SHADER_GLSL

#include "vapor/render/GLSL_Program.h"
#include "vapor/resources/GLSL_Text.h"
#include "vapor/render/GL.h"
#include "vapor/Utilities.h"
#include "ReferenceCount.h"
#include <algorithm>

namespace vapor {

//-----------------------------------//

GLSL_Program::GLSL_Program( const GLSL_TextPtr& text )
	: text(text)
	, linkError(false)
{
	assert( text != nullptr );
	
	create();
	createShaders();
	updateShadersText();
}

//-----------------------------------//

GLSL_Program::~GLSL_Program()
{
	// Detach shaders.
	for( uint i = 0; i < shaders.size(); i++ )
	{
		const GLSL_ShaderPtr& shader = shaders[i];
	
		if( attached[shader] )
			glDetachShader( id, shader->id() );

		if( glHasError("Could not detach shader object") )
			continue;
	}

	glDeleteProgram( id );

	if( glHasError("Could not delete program object") )
		return;
}

//-----------------------------------//

bool GLSL_Program::create()
{
	id = glCreateProgram( );

	if( glHasError( "Could not create a new program object" ) )
		return false;

	return true;
}

//-----------------------------------//

void GLSL_Program::addShader( const GLSL_ShaderPtr& shader )
{
	ShaderVector::iterator it =
		std::find(shaders.begin(), shaders.end(), shader);

	if( it == shaders.end() )
	{
		shaders.push_back( shader );
	}

	if( !attached[shader] )
	{
		glAttachShader( id, shader->id() );

		if( glHasError("Could not attach shader") )
			return;

		attached[shader] = true;
	}
}

//-----------------------------------//

void GLSL_Program::createShaders()
{
	if( !text )
		return;

	vertex = new GLSL_Shader();
	vertex->setType( ShaderType::Vertex );
	vertex->create();
	
	fragment = new GLSL_Shader();
	fragment->setType( ShaderType::Fragment );
	fragment->create();

	addShader( (GLSL_ShaderPtr&) vertex );
	addShader( (GLSL_ShaderPtr&) fragment );
}

//-----------------------------------//

void GLSL_Program::updateShadersText()
{
	assert( text != nullptr );
	
	assert( vertex != nullptr );
	vertex->setText( text->getVertexSource() );
	
	assert( fragment != nullptr );
	fragment->setText( text->getFragmentSource() );

	for( uint i = 0; i < shaders.size(); i++ )
	{
		const ShaderPtr& shader = shaders[i];
		shader->forceRecompile();
	}

	linkError = false;
	linked = false;
}

//-----------------------------------//

bool GLSL_Program::attachShaders()
{
	// Make sure all shaders are compiled.
	for( uint i = 0; i < shaders.size(); i++ )
	{
		const GLSL_ShaderPtr& shader = shaders[i];

		if( shader->isCompiled() )
			 continue;
		
		std::string type = String::toLowerCase(
			ShaderType::getString( shader->getType()));

		if( !shader->compile() )
		{
			Log::error( "Error compiling %s shader '%s': %s",
				type.c_str(), text->getBasePath().c_str(),
				shader->getLog().c_str() );

			linkError = true;
			linked = false;

			return false;
		}
		else
		{
			Log::info( "Compiled %s shader '%s' with no errors",
				type.c_str(), text->getBasePath().c_str() );
		}
	}

	return true;
}

//-----------------------------------//

bool GLSL_Program::link()
{
	// If the program is already linked, return.
	if( isLinked() ) 
		return true;

	// If we already tried to link and were not succesful, 
	// don't try to link again until the program is updated.
	if( linkError ) 
		return false;

	// No shaders, don't try to link.
	if( shaders.empty() ) 
		return false;

	if( !attachShaders() )
		return false;

	bindDefaultAttributes();

	glLinkProgram( id );

	// Check that the linking was good
	if( glHasError("Could not link program object") )
	{
		linked = false;
		linkError = true;
		return false;
	}

	getLogText();

	int status;
	glGetProgramiv( id, GL_LINK_STATUS, &status );

	if( status != GL_TRUE )
	{
		Log::warn( "Could not link program object '%d': %s", id, log.c_str() );
		linked = false;
		linkError = true;
		return false;
	}

	linked = true;
	linkError = false;

	return true;
}

//-----------------------------------//

bool GLSL_Program::validate()
{
	if( !isLinked() ) return false;
	
	glValidateProgram( id );
	
	int status;
	glGetProgramiv( id, GL_VALIDATE_STATUS, &status );

	if( status != GL_TRUE )
	{
		getLogText();

		Log::warn( "Could not validate program object '%d': %s", id, log.c_str() );
		return false;
	}

	return true;
}

//-----------------------------------//

void GLSL_Program::bind()
{
	if( !isLinked() ) return;

	glUseProgram( id );

	if( glHasError("Could not bind program object") )
		return;
}

//-----------------------------------//

void GLSL_Program::unbind()
{
	glUseProgram( 0 );

	if( glHasError("Could not unbind program object") )
		return;
}

//-----------------------------------//

void GLSL_Program::getLogText()
{
	// Get linking log size.
	GLint size;
	glGetProgramiv( id, GL_INFO_LOG_LENGTH, &size );

	if( size == 0 )
	{
		log = "(no message)";
		return;
	}

	// TODO: move directly to string...

	GLchar* info = new char[size];
	GLsizei length;

	glGetProgramInfoLog( id, size, &length, info );

	log.assign( info );
	delete[] info;
}

//-----------------------------------//

void GLSL_Program::bindDefaultAttributes()
{
	#pragma TODO("Shader uniforms and attributes should only be bound if needed")

	setAttribute( "vp_Vertex", VertexAttribute::Position );
	setAttribute( "vp_Normal", VertexAttribute::Normal );
	setAttribute( "vp_Color", VertexAttribute::Color );
	setAttribute( "vp_TexCoord0", VertexAttribute::TexCoord0 );
	setAttribute( "vp_BoneIndex", VertexAttribute::FogCoord );
}

//-----------------------------------//

void GLSL_Program::setAttribute( const std::string& name, VertexAttribute::Enum attr )
{
	//if( !isLinked() ) return;

	//bind();

	glBindAttribLocation( id, attr, name.c_str() );

	if( glHasError("Could not bind attribute variable") )
		return;

	//unbind();
}

//-----------------------------------//

void GLSL_Program::setUniform( const std::string& slot, int data )
{
	if( !isLinked() ) return;

	//bind();

	GLint loc = glGetUniformLocation( id, slot.c_str() );

	if( loc == -1 )
	{
		// Log::warn( "Could not locate uniform location in program object '%d'", id );
		return;
	}

	glUniform1i( loc, data );

	//unbind();
}

//-----------------------------------//

void GLSL_Program::setUniform( const std::string& slot, float data )
{
	if( !isLinked() ) return;

	//bind();

	GLint loc = glGetUniformLocation( id, slot.c_str() );

	if( loc == -1 )
	{
		// Log::warn( "Could not locate uniform location in program object '%d'", id );
		return;
	}

	glUniform1f( loc, data );

	//unbind();
}

//-----------------------------------//

void GLSL_Program::setUniform( const std::string& slot, const std::vector<Vector3> vec )
{
	if( !isLinked() ) return;

	assert( sizeof(vec[0]) == 3*sizeof(float) );

	//bind();

	GLint loc = glGetUniformLocation( id, slot.c_str() );

	if( loc == -1 )
	{
		// Log::warn( "Could not locate uniform location in program object '%d'", id );
		return;
	}

	glUniform3fv( loc, vec.size(), reinterpret_cast<const float*>(&vec[0]) );

	//unbind();
}

//-----------------------------------//

void GLSL_Program::setUniform( const std::string& slot, const std::vector<Color> vec )
{
	if( !isLinked() ) return;

	assert( sizeof(vec[0]) == 4*sizeof(float) );

	//bind();

	GLint loc = glGetUniformLocation( id, slot.c_str() );

	if( loc == -1 )
	{
		//// Log::warn( "Could not locate uniform location in program object '%d'", id );
		return;
	}

	glUniform4fv( loc, vec.size(), reinterpret_cast<const float*>(&vec[0]) );

	//unbind();
}

//-----------------------------------//

void GLSL_Program::setUniform( const std::string& slot, const Vector3& vec )
{
	if( !isLinked() ) return;

	//bind();

	GLint loc = glGetUniformLocation( id, slot.c_str() );

	if( loc == -1 )
	{
		//// Log::warn( "Could not locate uniform location in program object '%d'", id );
		return;
	}

	glUniform3f( loc, vec.x, vec.y, vec.z );

	//unbind();
}

//-----------------------------------//

void GLSL_Program::setUniform( const std::string& slot, const EulerAngles& ang )
{
	Vector3 vec( ang.x, ang.y, ang.z );
	setUniform(slot, vec);
}

//-----------------------------------//

void GLSL_Program::setUniform( const std::string& slot, const Matrix4x3& matrix )
{
	if( !isLinked() ) return;

	//bind();

	GLint loc = glGetUniformLocation( id, slot.c_str() );

	if( loc == -1 )
	{
		//// Log::warn( "Could not locate uniform location in program object '%d'", id );
		return;
	}

	//if(glUniformMatrix4x3fv)
	//{
	//	glUniformMatrix4x3fv( loc, 1, true, &matrix.m11 );
	//}
	//else
	{
		Matrix4x4 mat( matrix );
		glUniformMatrix4fv( loc, 1, true, &mat.m11 );
	}
}

//-----------------------------------//

void GLSL_Program::setUniform( const std::string& slot, const Matrix4x4& matrix )
{
	if( !isLinked() ) return;

	//bind();

	GLint loc = glGetUniformLocation( id, slot.c_str() );

	if( loc == -1 )
	{
		//// Log::warn( "Could not locate uniform location in program object '%d'", id );
		return;
	}

	glUniformMatrix4fv( loc, 1, true, &matrix.m11 );
	
	//float test[16];
	//glGetUniformfv( id, loc, test );

	//unbind();
}

//-----------------------------------//

void GLSL_Program::setUniform( const std::string& slot, const std::vector<Matrix4x4>& vec )
{
	if( !isLinked() ) return;

	if( vec.empty() )
		return;
	//bind();

	GLint loc = glGetUniformLocation( id, slot.c_str() );

	if( loc == -1 )
	{
		// Log::warn( "Could not locate uniform location in program object '%d'", id );
		return;
	}

	glUniformMatrix4fv( loc, vec.size(), true, &(vec[0].m11) );

	//unbind();
}

//-----------------------------------//

} // end namespace

#endif