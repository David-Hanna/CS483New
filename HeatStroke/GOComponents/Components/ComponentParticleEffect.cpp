#include "ComponentParticleEffect.h"

#include "GameObject.h"

HeatStroke::ComponentParticleEffect::ComponentParticleEffect(
	HeatStroke::GameObject* p_pGameObject,
	const std::string& p_strEffectDefinitionFile)
	:
	Component(p_pGameObject)
{
	m_pEffect = new Effect(p_strEffectDefinitionFile);
	m_pEffect->m_Transform.SetParent(&(p_pGameObject->GetTransform()));
	SceneManager::Instance()->AddParticleEffect(m_pEffect);
}

HeatStroke::ComponentParticleEffect::~ComponentParticleEffect()
{
	SceneManager::Instance()->RemoveParticleEffect(m_pEffect);
	delete m_pEffect;
}

HeatStroke::Component* HeatStroke::ComponentParticleEffect::CreateComponent(
	HeatStroke::GameObject* p_pGameObject,
	tinyxml2::XMLNode* p_pBaseNode,
	tinyxml2::XMLNode* p_pOverrideNode)
{
	tinyxml2::XMLElement* pEffectElement = p_pBaseNode->FirstChildElement("Effect");
	std::string strEffectDefinitionFile = ParseEffectDefintionFile(pEffectElement);
	return new ComponentParticleEffect(p_pGameObject, strEffectDefinitionFile);
}

void HeatStroke::ComponentParticleEffect::Init()
{

}

void HeatStroke::ComponentParticleEffect::Update(const float p_fDelta)
{
	m_pEffect->Update(p_fDelta);
}

void HeatStroke::ComponentParticleEffect::Start()
{
	m_pEffect->Start();
}

void HeatStroke::ComponentParticleEffect::Stop()
{
	m_pEffect->Stop();
}

std::string HeatStroke::ComponentParticleEffect::ParseEffectDefintionFile(const tinyxml2::XMLElement* p_pEffectElement)
{
	std::string strEffectFile = "";
	EasyXML::GetRequiredStringAttribute(p_pEffectElement, "path", strEffectFile);
	return strEffectFile;
}