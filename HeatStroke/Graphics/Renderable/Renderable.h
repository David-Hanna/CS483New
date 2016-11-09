//=================================================================================
// Renderable
// Author: David Hanna
//
// Provides an interface for all things implementing a Render method.
// Passing a camera is defaulted to nullptr. Some Renderables do not need it.
// Note, however, that many require it.
//=================================================================================

#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "SceneCamera.h"

namespace HeatStroke
{
	class Renderable
	{
	public:
		virtual ~Renderable() = 0 {}

		virtual void Render(const SceneCamera* p_pCamera = nullptr) = 0;
	};
}

#endif // W_RENDERABLE_H