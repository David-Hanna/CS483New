#include "AffectorFade.h"

HeatStroke::AffectorFade::AffectorFade(const AFFECTOR_FUNCTION p_eFunction, float p_fFade)
	:
	m_eFunction(p_eFunction),
	m_fFade(p_fFade)
{

}

HeatStroke::AffectorFade::~AffectorFade()
{

}

void HeatStroke::AffectorFade::Apply(const float p_fDelta, Particle* p) const
{
	switch (m_eFunction)
	{
		case HeatStroke::Affector::AF_CONSTANT:
			p->m_fFade += m_fFade * p_fDelta;
			break;
		case HeatStroke::Affector::AF_LIFETIME:
			p->m_fFade = p->m_fFadeStart + (p->m_fAge * (m_fFade- p->m_fFadeStart));
			break;
		default:
			break;
	}
}