/************************************************************************
*
* vaporEngine (2008-2009)
*
*	<http://www.portugal-a-programar.org>
*
************************************************************************/

#include "vapor/PCH.h"

#include "vapor/render/Material.h"

namespace vapor {
	namespace render {

//-----------------------------------//

Material::Material( const std::string& name, ProgramPtr program )
	: name( name ), program( program )
{

}

//-----------------------------------//

Material::~Material()
{

}

//-----------------------------------//

const std::string& Material::getName() const
{
	return name;
}

//-----------------------------------//

void Material::addTexture( TexturePtr tex )
{
	textures.push_back( tex );
}

//-----------------------------------//

ProgramPtr Material::getProgram() const
{
	return program;
}

//-----------------------------------//

} } // end namespaces