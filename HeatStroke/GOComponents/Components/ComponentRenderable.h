//------------------------------------------------------------------------
// ComponentRenderable
//
// Created:	2012/12/14
// Author:	Carel Boers
//	
// This class provides the base class for all renderable-type components.
//------------------------------------------------------------------------

#ifndef COMPONENTRENDERABLE_H
#define COMPONENTRENDERABLE_H

#include "Model.h"
#include "Component.h"

namespace HeatStroke
{
	class ComponentRenderable : public Component
	{
	public:
		//------------------------------------------------------------------------------
		// Public methods.
		//------------------------------------------------------------------------------
		virtual const std::string FamilyID() const override	{ return std::string("GOC_Renderable"); }

		virtual ~ComponentRenderable() = 0 {} // Pure virtual class.

		// Game Loop Methods
		// ComponentRenderable adds a PreRender step to the game loop.
		virtual void Init() override = 0;
		virtual void Update(const float p_fDelta) override = 0;
		virtual void PreRender()
		{
			SyncTransform();
			// More steps in PreRender may be added in the future or by subclasses.
		}

	protected:
		//------------------------------------------------------------------------------
		// Protected methods
		//------------------------------------------------------------------------------
		// Prevent public construction.
		ComponentRenderable(GameObject* p_pGameObject) : Component(p_pGameObject) {}
		
		// SyncTransform is a mandatory part of PreRender.
		virtual void SyncTransform() = 0;

	private:
		//------------------------------------------------------------------------------
		// Private methods
		//------------------------------------------------------------------------------
		// Prevent copying
		ComponentRenderable(const ComponentRenderable&) = delete;
		ComponentRenderable& operator=(const ComponentRenderable&) = delete;
	};
}

#endif // COMPONENTRENDERABLE_H