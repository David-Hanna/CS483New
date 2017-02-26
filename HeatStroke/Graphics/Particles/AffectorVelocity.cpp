#include "AffectorVelocity.h"

HeatStroke::AffectorVelocity::AffectorVelocity()
{

}

HeatStroke::AffectorVelocity::~AffectorVelocity()
{

}

void HeatStroke::AffectorVelocity::Apply(const float p_fDelta, Particle* p) const
{
	p->m_vPos += p->m_vVelocity * p_fDelta;
}