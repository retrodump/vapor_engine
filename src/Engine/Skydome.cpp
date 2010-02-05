/************************************************************************
*
* vaporEngine (2008-2010)
*
*	<http://www.portugal-a-programar.org>
*
************************************************************************/

#include "vapor/PCH.h"
#include "vapor/scene/Skydome.h"
#include "vapor/math/Math.h"

namespace vapor { namespace scene {

using namespace vapor::math;
using namespace vapor::render;

//-----------------------------------//

const std::string& Skydome::type = "Skydome";

//-----------------------------------//

Skydome::Skydome( render::MaterialPtr mat )
{
	RenderablePtr rend( 
		new Renderable( Primitive::Quads, generateSphere() ) );
	
	MaterialPtr matSky( new Material( "SkydomeSimple" ) );
	
	//matSky->setDepthWrite( false );
	matSky->setBackfaceCulling( false );
	matSky->setTexture( 0, "sky.png" );
	matSky->setProgram( "tex" );

	rend->setMaterial( matSky );
	//rend->setPolygonMode( PolygonMode::Wireframe );

	addRenderable( rend );
}

//-----------------------------------//

Skydome::~Skydome()
{

}

//-----------------------------------//

static const short div_phi = 50;
static const short div_theta = 50;
static const short radius = 600;

render::VertexBufferPtr Skydome::generateSphere()
{
	// We need to generate the sphere geometry for the skydome.
	// For that we use the sphere equations, which give us any
	// point in the sphere surface, given different patemeters.
	// For more details check the Sphere equations:
	// http://en.wikipedia.org/wiki/Sphere#Equations_in_R3

	// Create a new VBO and upload triangle data
	VertexBufferPtr vb( new VertexBuffer() );

	// Vertex position data
	std::vector< Vector3 > vertex;
	//vertex.reserve( div_theta*div_phi/**4*/ );

	// Vertex color data
	std::vector< Vector3 > colors;

	short d_phi = (90 / div_phi);
	short d_theta = (360 / div_theta);

	for( short phi = 90; phi >= 0; phi -= d_phi )
	{
		// We need to convert ϕ and θ to radian values since that's
		// what the trignometric functions sin and cos expect.
		
		float r_phi = math::degreeToRadian( phi );
		float s_phi = math::sinf( r_phi );
		float c_phi = math::cosf( r_phi );

		float r_phi1 = math::degreeToRadian( short(phi+d_phi) );
		float s_phi1 = math::sinf( r_phi1 );
		float c_phi1 = math::cosf( r_phi1 );

		for( short theta = 0; theta <= 360; theta += d_theta )
		{
			float r_theta = math::degreeToRadian( theta );
			float s_theta = math::sinf( r_theta );
			float c_theta = math::cosf( r_theta );

			float r_theta1 = math::degreeToRadian( short(theta+d_theta) );
			float s_theta1 = math::sinf( r_theta1 );
			float c_theta1 = math::cosf( r_theta1 );

			Vector3 v1(
				radius * s_phi * c_theta,
				radius * c_phi,
				radius * s_phi * s_theta );

			vertex.push_back( v1 );
			colors.push_back( Vector3( 1.0f, 1.0f, 1.0f ) );

			Vector3 v2(
				radius * s_phi * c_theta1,
				radius * c_phi,
				radius * s_phi * s_theta1 );

			vertex.push_back( v2 );
			colors.push_back( Vector3( 1.0f, 1.0f, 1.0f ) );

			Vector3 v3(
				radius * s_phi1 * c_theta1,
				radius * c_phi1,
				radius * s_phi1 * s_theta1 );

			vertex.push_back( v3 );
			colors.push_back( Vector3( 1.0f, 1.0f, 1.0f ) );

			Vector3 v4(
				radius * s_phi1 * c_theta,
				radius * c_phi1,
				radius * s_phi1 * s_theta );

			vertex.push_back( v4 );
			colors.push_back( Vector3( 1.0f, 1.0f, 1.0f ) );
		}
	}

	// Let's generate spherical texture coordinates for our dome.
	// See http://www.mvps.org/directx/articles/spheremap.htm

	std::vector< Vector3 > tex;

	for( uint i = 0; i < vertex.size(); i += 4 )
	{
		Vector3 normal1 = calculateTriangleNormal( 
			vertex[i], vertex[i+1], vertex[i+2] );

		Vector3 normal2 = calculateTriangleNormal( 
			vertex[i+1], vertex[i+2], vertex[i+3] );

		Vector3 normal( ((normal1+normal2) / 2).normalize() );

		// calculate texture coordinates
		Vector3 uv( math::asinf( normal.x ) / math::PI+0.5f,
					math::asinf( normal.y ) / math::PI+0.5f,
					0.0f );

		tex.push_back( uv );
		tex.push_back( uv );
		tex.push_back( uv );
		tex.push_back( uv );
	}

	// Vertex buffer setup
	vb->set( VertexAttribute::Vertex, vertex );
	vb->set( VertexAttribute::Color, colors );
	vb->set( VertexAttribute::MultiTexCoord0, tex );

	return vb;
}

//-----------------------------------//

const std::string& Skydome::getType() const
{
	return type;
}

//-----------------------------------//

} } // end namespaces
