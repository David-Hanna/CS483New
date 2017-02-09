#include "ComponentParticleEffect.h"

#include "GameObject.h"

HeatStroke::ComponentParticleEffect::ComponentParticleEffect(
	HeatStroke::GameObject* p_pGameObject,
	const std::vector<std::string>& p_vEffectDefinitionFile,
	const glm::vec3& p_vOffset/* = {0.0f, 0.0f, 0.0f}*/)
	:
	Component(p_pGameObject)
{
	int iSize = p_vEffectDefinitionFile.size();
	for (int i = 0; i < iSize; ++i)
	{
		m_pEffects.push_back(new Effect(p_vEffectDefinitionFile[i]));
		m_pEffects[i]->m_Transform.SetParent(&(p_pGameObject->GetTransform()));
		m_pEffects[i]->m_Transform.SetTranslation(p_vOffset);
		SceneManager::Instance()->AddParticleEffect(m_pEffects[i]);
	}
}

HeatStroke::ComponentParticleEffect::~ComponentParticleEffect()
{
	for (Effect* pEffect : m_pEffects)
	{
		SceneManager::Instance()->RemoveParticleEffect(pEffect);
		delete pEffect;
		pEffect = nullptr;
	}
	m_pEffects.clear();
}

HeatStroke::Component* HeatStroke::ComponentParticleEffect::CreateComponent(
	HeatStroke::GameObject* p_pGameObject,
	tinyxml2::XMLNode* p_pBaseNode,
	tinyxml2::XMLNode* p_pOverrideNode)
{
	tinyxml2::XMLElement* pEffectsRoot = p_pBaseNode->FirstChildElement("Effects");
	tinyxml2::XMLElement* pEffectElement = pEffectsRoot->FirstChildElement("Effect");
	std::vector<std::string> vEffectDefinitionFiles;
	while (pEffectElement != nullptr)
	{
		std::string strEffectDefinition = ParseEffectDefintionFile(pEffectElement);
		vEffectDefinitionFiles.push_back(strEffectDefinition);

		pEffectElement = pEffectElement->NextSiblingElement("Effect");
	}

	tinyxml2::XMLElement* pOffsetElement = p_pBaseNode->FirstChildElement("Offset");
	glm::vec3 vOffset = ParseEffectOffset(pOffsetElement);

	return new ComponentParticleEffect(p_pGameObject, vEffectDefinitionFiles, vOffset);
}

void HeatStroke::ComponentParticleEffect::Init()
{

}

void HeatStroke::ComponentParticleEffect::Update(const float p_fDelta)
{
	for (Effect* pEffect : m_pEffects)
	{
		pEffect->Update(p_fDelta);
	}
}

void HeatStroke::ComponentParticleEffect::Start()
{
	for (Effect* pEffect : m_pEffects)
	{
		pEffect->Start();
	}
}

void HeatStroke::ComponentParticleEffect::Stop()
{
	for (Effect* pEffect : m_pEffects)
	{
		pEffect->Stop();
	}
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