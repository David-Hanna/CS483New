#include "AffectorColor.h"

HeatStroke::AffectorColor::AffectorColor(const AFFECTOR_FUNCTION p_eFunction, const glm::vec3& p_vColor)
	:
	m_eFunction(p_eFunction),
	m_vColor(p_vColor)
{

}

HeatStroke::AffectorColor::~AffectorColor()
{

}

void HeatStroke::AffectorColor::Apply(const float p_fDelta, Particle* p) const
{
	switch (m_eFunction)
	{
		case HeatStroke::Affector::AF_CONSTANT:
			p->m_vColor += m_vColor * p_fDelta;
			break;
		case HeatStroke::Affector::AF_LIFETIME:
			p->m_vColor = p->m_vColorStart + (p->m_fAge * (m_vColor - p->m_vColorStart));
			break;
		default:
			break;
	}
}