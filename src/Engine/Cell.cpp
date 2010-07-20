/************************************************************************
*
* vapor3D Engine � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#include "vapor/PCH.h"
#include "vapor/terrain/Cell.h"
#include "vapor/terrain/Terrain.h"
#include "vapor/math/Math.h"
#include "vapor/math/Color.h"

namespace vapor {

//-----------------------------------//

Cell::Cell( const TerrainSettings& settings, 
			const std::vector<float>& heights, short x, short y )
	: Renderable( Primitive::Triangles )
	, settings( settings )
	, heights( heights )
	, x(x), y(y)
{
	// Create a new VBO and upload triangle data
	VertexBufferPtr vb( new VertexBuffer() );
	IndexBufferPtr ib( new IndexBuffer() );

	setVertexBuffer( vb );
	setIndexBuffer( ib );

	{
		//Profiler p( "terrain build" );

		rebuildGeometry();
		rebuildFaceNormals();
		rebuildAveragedNormals();
	}
}

//-----------------------------------//

void Cell::rebuildGeometry()
{
	info( "cell", "Rebuilding geometry of cell (%hd, %hd)", x, y );

	rebuildVertices();
	rebuildIndices();
}

//-----------------------------------//

void Cell::rebuildVertices()
{
	assert( vb != nullptr );

	// Vertex data
	std::vector<Vector3> vertex;
	std::vector<Vector3> texCoords;

	const ushort numTiles = settings.NumberTiles;
	const ushort sizeCell = settings.CellSize;

	float offsetX = x * sizeCell;
	float offsetZ = y * sizeCell;

	const float tileSize = sizeCell / numTiles;
	const ushort numExpectedVertices = (numTiles+1)*(numTiles+1);

	for( uint i = 0; i < numExpectedVertices; i++ )
	{
		uint row = i % (numTiles+1);
		uint col = i / (numTiles+1);

		float X = offsetX + tileSize*row;
		float Z = offsetZ + tileSize*col;
		float Y = heights[i] * settings.MaxHeight;
		
		vertex.push_back( Vector3(X, Y, Z) );
		texCoords.push_back( Vector2(X/sizeCell, Z/sizeCell) );
	}

	assert( vertex.size() == numExpectedVertices );
	assert( texCoords.size() == numExpectedVertices );

	// Vertex buffer setup.
	vb->set( VertexAttribute::Position, vertex );
	vb->set( VertexAttribute::TexCoord0, texCoords );
}

//-----------------------------------//

void Cell::rebuildIndices()
{
	assert( ib != nullptr );

	// Vertex data
	std::vector<ushort> indices;

	const ushort numTiles = settings.NumberTiles;
	
	for( short col = 0; col < numTiles; col++ )
	{
		for( short row = 0; row < numTiles; row++ )
		{
			uint i = col*(numTiles+1)+row;

			// First triangle
			indices.push_back( i) ;
			indices.push_back( i+(numTiles+1) );
			indices.push_back( i+1 );

			// Second triangle
			indices.push_back( i+1 );
			indices.push_back( i+(numTiles+1) ) ;
			indices.push_back( i+(numTiles+2) );
		}
	}

	// Index buffer setup.
	ib->set( indices );
}

//-----------------------------------//

void Cell::rebuildFaceNormals()
{
	assert( vb && ib );

	const std::vector<Vector3>& vs = vb->getVertices();
	assert( !vs.empty() );

	const std::vector<ushort>& ind = ib->getIndices16();
	assert( !ind.empty() );

	faceNormals.clear();

	info( "cell", "Rebuilding face normals of cell (%hd, %hd)", x, y );

	for( uint i = 0; i < ind.size(); i += 3 )
	{
		Vector3 v1 = vs[ind[i]];
		Vector3 v2 = vs[ind[i+1]];
		Vector3 v3 = vs[ind[i+2]];

		Vector3 normal = calculateTriangleNormal(v1, v2, v3);
		faceNormals.push_back( normal );
	}

	const uint numTiles = settings.NumberTiles;
	assert( faceNormals.size() == numTiles*numTiles*2 );
}

//-----------------------------------//

// Resources:
// http://www.gamedev.net/community/forums/topic.asp?topic_id=163625
// http://www.gamedev.net/reference/articles/article2264.asp
//

#define isRegular(x,y) ((x>=1u) && (x<=(numTiles-1u)) \
						&& (y>=1u) && (y<=(numTiles-1u)))

std::vector<uint> Cell::getNeighborFaces( uint i )
{
	std::vector<uint> n;

	const ushort numTiles = settings.NumberTiles;
	uint facesPerRow = numTiles*2;
	
	uint row = i / (numTiles+1);
	uint col = i % (numTiles+1);
	
	if( isRegular(col, row) )
	{
		uint startFace = (row-1)*facesPerRow+(col*2);

		n.push_back(startFace-1);
		n.push_back(startFace);
		n.push_back(startFace+1);

		n.push_back(startFace+facesPerRow-2);
		n.push_back(startFace+facesPerRow-1);
		n.push_back(startFace+facesPerRow);
	}
	else
	{
		n.push_back(0);
	}

	return n;
}

//-----------------------------------//

void Cell::rebuildAveragedNormals()
{
	assert( vb && ib );

	const std::vector<Vector3>& vs = vb->getVertices();
	assert( !vs.empty() );

	const std::vector<ushort>& ind = ib->getIndices16();
	assert( !ind.empty() );

	// Averaged per-vertex normals.
	std::vector<Vector3> normals;

	info( "cell", "Rebuilding average per-vertex normals of cell (%hd, %hd)", x, y );
	
	for(uint i = 0; i < vs.size(); i++)
	{
		std::vector<uint> ns = getNeighborFaces(i);

		Vector3 average;
		foreach( const uint& n, ns )
			average += faceNormals[n];

		average /= ns.size();
		average.normalize();

		normals.push_back( average );
	}

	assert( normals.size() == vs.size() );
	vb->set( VertexAttribute::Normal, normals );
}

//-----------------------------------//

} // end namespace