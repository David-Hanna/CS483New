#include "ComponentParticleEffect.h"

#include "GameObject.h"

HeatStroke::ComponentParticleEffect::ComponentParticleEffect(
	HeatStroke::GameObject* p_pGameObject,
	const std::string& p_strEffectDefinitionFile,
	const glm::vec3& p_vOffset/* = {0.0f, 0.0f, 0.0f}*/)
	:
	Component(p_pGameObject)
{
	m_pEffect = new Effect(p_strEffectDefinitionFile);
	m_pEffect->m_Transform.SetParent(&(p_pGameObject->GetTransform()));
	m_pEffect->m_Transform.SetTranslation(p_vOffset);
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

	tinyxml2::XMLElement* pOffsetElement = p_pBaseNode->FirstChildElement("Offset");
	glm::vec3 vOffset = ParseEffectOffset(pOffsetElement);

	return new ComponentParticleEffect(p_pGameObject, strEffectDefinitionFile, vOffset);
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

glm::vec3 HeatStroke::ComponentParticleEffect::ParseEffectOffset(const tinyxml2::XMLElement* p_pOffsetElement)
{
	glm::vec3 vOffset = glm::vec3(0.0f, 0.0f, 0.0f);

	if (p_pOffsetElement != nullptr)
	{
		EasyXML::GetRequiredFloatAttribute(p_pOffsetElement, "x", vOffset.x);
		EasyXML::GetRequiredFloatAttribute(p_pOffsetElement, "y", vOffset.y);
		EasyXML::GetRequiredFloatAttribute(p_pOffsetElement, "z", vOffset.z);
	}
	return vOffset;
}