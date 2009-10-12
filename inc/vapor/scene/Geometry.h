/************************************************************************
*
* vaporEngine (2008-2009)
*
*	<http://www.portugal-a-programar.org>
*
************************************************************************/

#pragma once

#include "vapor/Platform.h"
#include "vapor/scene/Transform.h"

#include "vapor/render/Renderable.h"

namespace vapor {
	namespace scene {

//-----------------------------------//

/**
 * Scene node that holds geometry data for rendering. It can holds various
 * Renderable objects and treat it as one big piece of geometry.
 */

class VAPOR_API Geometry : public Transformable
{
public:

	Geometry();

	Geometry( render::RenderablePtr rend );

	virtual ~Geometry();

	void addRenderable( render::RenderablePtr rend );

	const std::vector< render::RenderablePtr >& getRenderables() { return renderables; }

	virtual void update( double delta );

	virtual const std::string save(int indent = 0);

	virtual const std::string name() const;

protected:

	std::vector< render::RenderablePtr > renderables;
};

//-----------------------------------//

typedef tr1::shared_ptr< Geometry > GeometryPtr;

//-----------------------------------//

} } // end namespaces