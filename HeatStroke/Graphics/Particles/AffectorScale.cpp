#include "AffectorScale.h"

HeatStroke::AffectorScale::AffectorScale(const AFFECTOR_FUNCTION p_eFunction, const float p_fSize)
	:
	m_eFunction(p_eFunction),
	m_fSize(p_fSize)
{

}

HeatStroke::AffectorScale::~AffectorScale()
{

}

void HeatStroke::AffectorScale::Apply(const float p_fDelta, Particle* p) const
{
	switch (m_eFunction)
	{
		case HeatStroke::Affector::AF_CONSTANT:
			p->m_fSize += m_fSize * p_fDelta;
			break;
		case HeatStroke::Affector::AF_LIFETIME:
			p->m_fSize = p->m_fSizeStart + (p->m_fAge * (m_fSize - p->m_fSizeStart));
			break;
		default:
			break;
	}
}