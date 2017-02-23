#include "ComponentParticleEffect.h"

#include "GameObject.h"

HeatStroke::ComponentParticleEffect::ComponentParticleEffect(
	HeatStroke::GameObject* p_pGameObject,
	const std::vector<std::pair<std::string, std::string>>& p_vEffectDefinitionFiles,
	const glm::vec3& p_vOffset/* = {0.0f, 0.0f, 0.0f}*/)
	:
	Component(p_pGameObject)
{
	int iSize = p_vEffectDefinitionFiles.size();
	for (int i = 0; i < iSize; ++i)
	{
		Effect* pEffect = new Effect(p_vEffectDefinitionFiles[i].first);
		m_mEffects.insert(std::pair<std::string, Effect*>(p_vEffectDefinitionFiles[i].second, pEffect));
		pEffect->m_Transform.SetParent(&(p_pGameObject->GetTransform()));
		pEffect->m_Transform.SetTranslation(p_vOffset);
		SceneManager::Instance()->AddParticleEffect(pEffect);
	}
}

HeatStroke::ComponentParticleEffect::~ComponentParticleEffect()
{
	for (std::pair<std::string, Effect*> effect : m_mEffects)
	{
		Effect* pEffect = effect.second;
		SceneManager::Instance()->RemoveParticleEffect(pEffect);
		delete pEffect;
		pEffect = nullptr;
	}
	m_mEffects.clear();
}

HeatStroke::Component* HeatStroke::ComponentParticleEffect::CreateComponent(
	HeatStroke::GameObject* p_pGameObject,
	tinyxml2::XMLNode* p_pBaseNode,
	tinyxml2::XMLNode* p_pOverrideNode)
{
	tinyxml2::XMLElement* pEffectsRoot = p_pBaseNode->FirstChildElement("Effects");
	tinyxml2::XMLElement* pEffectElement = pEffectsRoot->FirstChildElement("Effect");
	std::vector<std::pair<std::string, std::string>> vEffectDefinitionFiles;
	while (pEffectElement != nullptr)
	{
		std::pair<std::string, std::string> strEffectDefinition = ParseEffectDefintionFile(pEffectElement);
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
	for (std::pair<std::string, Effect*> effect : m_mEffects)
	{
		effect.second->Update(p_fDelta);
	}
}

void HeatStroke::ComponentParticleEffect::Start()
{
	for (std::pair<std::string, Effect*> effect : m_mEffects)
	{
		effect.second->Start();
	}
}

void HeatStroke::ComponentParticleEffect::Stop()
{
	for (std::pair<std::string, Effect*> effect : m_mEffects)
	{
		effect.second->Stop();
	}
}

HeatStroke::Effect* HeatStroke::ComponentParticleEffect::GetEffect(const std::string& p_strEffectId)
{
	auto pEffect = m_mEffects.find(p_strEffectId);
	return pEffect != m_mEffects.end() ? pEffect->second : nullptr;
}

std::pair<std::string, std::string> HeatStroke::ComponentParticleEffect::ParseEffectDefintionFile(const tinyxml2::XMLElement* p_pEffectElement)
{
	std::string strEffectFile = "";
	std::string strEffectId = "";
	EasyXML::GetRequiredStringAttribute(p_pEffectElement, "path", strEffectFile);
	EasyXML::GetRequiredStringAttribute(p_pEffectElement, "id", strEffectId);
	return std::pair<std::string, std::string>(strEffectFile, strEffectId);
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