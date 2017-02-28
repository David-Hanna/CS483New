#include "Effect.h"

HeatStroke::Effect::Effect(const std::string& p_strDefinitionFile)
	:
	m_Transform("test")
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError err = doc.LoadFile(p_strDefinitionFile.c_str());
#if _DEBUG
	assert(err == tinyxml2::XML_NO_ERROR);
#endif

	ParseEmitters(doc.RootElement());
}

HeatStroke::Effect::~Effect()
{
	auto it = m_vEmitters.begin();
	auto end = m_vEmitters.end();
	for (; it != end; ++it)
	{
		delete (*it);
	}
	m_vEmitters.clear();
}

void HeatStroke::Effect::Update(const float p_fDelta)
{
	auto it = m_vEmitters.begin();
	auto end = m_vEmitters.end();
	for (; it != end; ++it)
	{
		(*it)->Update(p_fDelta);
	}
}

void HeatStroke::Effect::Render(const SceneCamera* p_pCamera)
{
	auto it = m_vEmitters.begin();
	auto end = m_vEmitters.end();
	for (; it != end; ++it)
	{
		(*it)->Render(p_pCamera);
	}
}

void HeatStroke::Effect::Start()
{
	auto it = m_vEmitters.begin();
	auto end = m_vEmitters.end();
	for (; it != end; ++it)
	{
		(*it)->Start();
	}
}

void HeatStroke::Effect::Stop()
{
	auto it = m_vEmitters.begin();
	auto end = m_vEmitters.end();
	for (; it != end; ++it)
	{
		(*it)->Stop();
	}
}

void HeatStroke::Effect::ParseEmitters(tinyxml2::XMLElement* p_pEffectElement)
{
	tinyxml2::XMLElement* pEmitterElement = p_pEffectElement->FirstChildElement("Emitter");
	while (pEmitterElement != nullptr)
	{
		std::string strEmitterDefinitionFile = ParseEmitterDefinitionFile(pEmitterElement);
		glm::vec3 vOffset = ParseEmitterOffset(pEmitterElement->FirstChildElement("Offset"));
		
		Emitter* pEmitter = new Emitter(strEmitterDefinitionFile);
		pEmitter->m_Transform.SetParent(&m_Transform);
		pEmitter->m_Transform.SetTranslation(vOffset);
		m_vEmitters.push_back(pEmitter);
		pEmitterElement = pEmitterElement->NextSiblingElement("Emitter");
	}
}

std::string HeatStroke::Effect::ParseEmitterDefinitionFile(const tinyxml2::XMLElement* p_pEmitterElement)
{
	std::string strEmitterDefinitionFile = "";
	EasyXML::GetRequiredStringAttribute(p_pEmitterElement, "source", strEmitterDefinitionFile);
	return strEmitterDefinitionFile;
}

glm::vec3 HeatStroke::Effect::ParseEmitterOffset(const tinyxml2::XMLElement* p_pOffsetElement)
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