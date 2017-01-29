#include "Emitter.h"

#include "AffectorFade.h"
#include "AffectorColor.h"
#include "AffectorScale.h"
#include "AffectorVelocity.h"

// static member setup
std::random_device HeatStroke::Emitter::s_Rand;
std::mt19937 HeatStroke::Emitter::s_RNGesus = std::mt19937(HeatStroke::Emitter::s_Rand());

HeatStroke::Emitter::Emitter(const std::string& p_strDefinitionFile)
	:
	m_bActive(true),
	m_pVB(nullptr),
	m_pIB(nullptr),
	m_pDecl(nullptr),
	m_pMat(nullptr),
	m_pFreeList(nullptr),
	m_pActiveHead(nullptr),
	m_pActiveTail(nullptr),
	m_fToSpawnAcc(0.0f),
	m_Transform("blah")
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError err = doc.LoadFile(p_strDefinitionFile.c_str());
#if _DEBUG
	assert(err == tinyxml2::XML_NO_ERROR);
#endif
	tinyxml2::XMLElement* pRootElement = doc.RootElement();

	EasyXML::GetRequiredIntAttribute(pRootElement, "num_particles", m_iNumParticles);
	
	InitFrequencyProperties(pRootElement);
	InitRenderProperties(pRootElement->FirstChildElement("Material"));
	InitSpawnProperties(pRootElement->FirstChildElement("Spawn_Properties"));
	InitAffectors(pRootElement->FirstChildElement("Affectors"));
	InitParticles();
}

HeatStroke::Emitter::~Emitter()
{
	delete[] m_pVerts;
	delete[] m_pIndices;

	delete m_pDecl;
	BufferManager::DestroyBuffer(m_pVB);
	BufferManager::DestroyBuffer(m_pIB);
	MaterialManager::DestroyMaterial(m_pMat);

	// TODO - delete particles
}

void HeatStroke::Emitter::Update(const float p_fDelta)
{
	if (m_bActive)
	{
		SpawnNewParticles(p_fDelta);
	}

	UpdateActiveParticles(p_fDelta);
}

void HeatStroke::Emitter::Render(const SceneCamera* p_pCamera)
{
	int v = 0;
	int i = 0;
	Particle* p = m_pActiveHead;
	while (p)
	{
		m_pVerts[v].vPos = (glm::vec3(-0.05f, -0.05f, 0.0f) * p->m_fSize) + p->m_vPos;
		m_pVerts[v].vUV = glm::vec2(0.0f, 0.0f);
		m_pVerts[v].vColor = glm::vec4(p->m_vColor, p->m_fFade);

		m_pVerts[v + 1].vPos = (glm::vec3(-0.05f, 0.05f, 0.0f) * p->m_fSize) + p->m_vPos;
		m_pVerts[v + 1].vUV = glm::vec2(0.0f, 1.0f);
		m_pVerts[v].vColor = glm::vec4(p->m_vColor, p->m_fFade);

		m_pVerts[v + 2].vPos = (glm::vec3(0.05f, 0.05f, 0.0f) * p->m_fSize) + p->m_vPos;
		m_pVerts[v + 2].vUV = glm::vec2(1.0f, 1.0f);
		m_pVerts[v].vColor = glm::vec4(p->m_vColor, p->m_fFade);

		m_pVerts[v + 3].vPos = (glm::vec3(0.05f, -0.05f, 0.0f) * p->m_fSize) + p->m_vPos;
		m_pVerts[v + 3].vUV = glm::vec2(1.0f, 0.0f);
		m_pVerts[v].vColor = glm::vec4(p->m_vColor, p->m_fFade);

		m_pIndices[i] = v;
		m_pIndices[i + 1] = v + 1;
		m_pIndices[i + 2] = v + 2;
		m_pIndices[i + 3] = v + 2;
		m_pIndices[i + 4] = v + 3;
		m_pIndices[i + 5] = v;

		v += 4;
		i += 6;
		p = p->m_pNext;
	}

	m_pVB->Write(m_pVerts, v * sizeof(Vertex));
	m_pIB->Write(m_pIndices, i * sizeof(GLuint));

	glm::mat4 mWorld =  glm::translate(m_Transform.GetTranslation()) * (glm::mat4)glm::transpose((glm::mat3)p_pCamera->GetViewMatrix());
	glm::mat4 mWorldViewTransform = p_pCamera->GetViewMatrix() * mWorld;
	glm::mat4 mWorldViewProjectionTransform = p_pCamera->GetProjectionMatrix() * mWorldViewTransform;

	m_pDecl->Bind();
	m_pMat->SetUniform("WorldViewProjectionTransform", mWorldViewProjectionTransform);
	m_pMat->Apply();

	glDrawElements(GL_TRIANGLES, i, GL_UNSIGNED_INT, 0);
}

void HeatStroke::Emitter::Start()
{
	m_bActive = true;
}

void HeatStroke::Emitter::Stop()
{
	m_bActive = false;
}

void HeatStroke::Emitter::InitFrequencyProperties(tinyxml2::XMLElement* p_pFrequencyPropertiesElement)
{
	EasyXML::GetRequiredBoolAttribute(p_pFrequencyPropertiesElement, "continuous", m_bContinuous);
	if (m_bContinuous)
	{
		EasyXML::GetRequiredFloatAttribute(p_pFrequencyPropertiesElement, "birthrate", m_fBirthRate);
		EasyXML::GetRequiredFloatAttribute(p_pFrequencyPropertiesElement, "duration", m_fDuration);
	}
	else
	{
		EasyXML::GetRequiredBoolAttribute(p_pFrequencyPropertiesElement, "repeat", m_bRepeatBursts);
		EasyXML::GetOptionalBoolAttribute(p_pFrequencyPropertiesElement, "random_burst_rate", m_bRandomBurstTimer, false);
		if (m_bRandomBurstTimer)
		{
			EasyXML::GetRequiredFloatAttribute(p_pFrequencyPropertiesElement, "burst_rate_min", m_fBurstRateMin);
			EasyXML::GetRequiredFloatAttribute(p_pFrequencyPropertiesElement, "burst_rate_max", m_fBurstRateMax);
		}
		else
		{
			EasyXML::GetRequiredFloatAttribute(p_pFrequencyPropertiesElement, "burst_rate", m_fBurstRateMin);
			m_fBurstRateMax = m_fBurstRateMin;
		}

		GetTimeToNextBurst();
	}
}

void HeatStroke::Emitter::InitRenderProperties(tinyxml2::XMLElement* p_pRenderPropertiesElement)
{
	InitBuffers();
	InitVertexDeclaration();
	InitMaterial(p_pRenderPropertiesElement);
}

void HeatStroke::Emitter::InitBuffers()
{
	m_pVB = BufferManager::CreateVertexBuffer(0);
	m_pIB = BufferManager::CreateIndexBuffer(0);

	m_pVerts = new Vertex[m_iNumParticles * 4];
	m_pIndices = new GLuint[m_iNumParticles * 6];
}

void HeatStroke::Emitter::InitVertexDeclaration()
{
	m_pDecl = new VertexDeclaration();
	m_pDecl->Begin();
	m_pDecl->AppendAttribute(AT_Position, 3, CT_Float);
	m_pDecl->AppendAttribute(AT_TexCoord1, 2, CT_Float);
	m_pDecl->AppendAttribute(AT_Color, 4, CT_Float);
	m_pDecl->SetVertexBuffer(m_pVB);
	m_pDecl->SetIndexBuffer(m_pIB);
	m_pDecl->End();
}

void HeatStroke::Emitter::InitMaterial(tinyxml2::XMLElement* p_pMaterialElement)
{
	std::string strMaterialName = "";
	std::string strVertexShader = "";
	std::string strFragmentShader = "";
	std::string strTexture = "";
	EasyXML::GetRequiredStringAttribute(p_pMaterialElement, "name", strMaterialName);
	EasyXML::GetRequiredStringAttribute(p_pMaterialElement, "vertex_shader", strVertexShader);
	EasyXML::GetRequiredStringAttribute(p_pMaterialElement, "fragment_shader", strFragmentShader);
	EasyXML::GetRequiredStringAttribute(p_pMaterialElement, "texture", strTexture);

	m_pMat = MaterialManager::CreateMaterial(strMaterialName);
	m_pMat->SetProgram(strVertexShader, strFragmentShader);
	m_pMat->SetTexture("Texture", TextureManager::CreateTexture(strTexture));

	m_pMat->SetBlend(true);
	m_pMat->SetBlendMode(HeatStroke::BM_SrcAlpha, HeatStroke::BM_One);
}

void HeatStroke::Emitter::InitSpawnProperties(tinyxml2::XMLElement* p_pSpawnPropertiesElement)
{
	InitSpawnPropertyColor(p_pSpawnPropertiesElement->FirstChildElement("Color"));
	InitSpawnPropertyFade(p_pSpawnPropertiesElement->FirstChildElement("Fade"));
	InitSpawnPropertyLifetime(p_pSpawnPropertiesElement->FirstChildElement("Lifetime"));
	InitSpawnPropertySize(p_pSpawnPropertiesElement->FirstChildElement("Size"));
	InitSpawnPropertyVelocity(p_pSpawnPropertiesElement->FirstChildElement("Velocity"));
}

void HeatStroke::Emitter::InitSpawnPropertyColor(tinyxml2::XMLElement* p_pSpawnPropertyColorElement)
{
#if _DEBUG
	assert(p_pSpawnPropertyColorElement != nullptr);
#endif

	std::string strType = "";
	EasyXML::GetRequiredStringAttribute(p_pSpawnPropertyColorElement, "type", strType);
	const char* cType = strType.c_str();
	if (strcmp(cType, "random") == 0)
	{
		m_bRandomColor = true;
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertyColorElement, "r_min", m_vColorMin.r);
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertyColorElement, "g_min", m_vColorMin.g);
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertyColorElement, "b_min", m_vColorMin.b);
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertyColorElement, "r_max", m_vColorMax.r);
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertyColorElement, "g_max", m_vColorMax.g);
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertyColorElement, "b_max", m_vColorMax.b);
	}
	else if (strcmp(cType, "constant") == 0)
	{
		m_bRandomColor = false;
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertyColorElement, "r", m_vColorMin.r);
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertyColorElement, "g", m_vColorMin.g);
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertyColorElement, "b", m_vColorMin.b);
		m_vColorMax = m_vColorMin;
	}
	else
	{
		printf("unknown type '%s' for spawn property 'color'\n", cType);
		m_bRandomColor = false;
		m_vColorMin= glm::vec3(1.0f, 1.0f, 1.0f);
		m_vColorMax = m_vColorMin;
	}
}

void HeatStroke::Emitter::InitSpawnPropertyFade(tinyxml2::XMLElement* p_pSpawnPropertyFadeElement)
{
#if _DEBUG
	assert(p_pSpawnPropertyFadeElement != nullptr);
#endif

	std::string strType = "";
	EasyXML::GetRequiredStringAttribute(p_pSpawnPropertyFadeElement, "type", strType);
	const char* cType = strType.c_str();
	if (strcmp(cType, "random") == 0)
	{
		m_bRandomFade = true;
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertyFadeElement, "min", m_fFadeMin);
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertyFadeElement, "max", m_fFadeMax);
	}
	else if (strcmp(cType, "constant") == 0)
	{
		m_bRandomFade = false;
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertyFadeElement, "value", m_fFadeMin);
		m_fFadeMax = m_fFadeMin;
	}
	else
	{
		printf("unknown type '%s' for spawn property 'fade'\n", cType);
		m_bRandomFade = false;
		m_fFadeMin = 1.0f;
		m_fFadeMax = m_fFadeMin;
	}
}

void HeatStroke::Emitter::InitSpawnPropertyLifetime(tinyxml2::XMLElement* p_pSpawnPropertyLifetimeElement)
{
#if _DEBUG
	assert(p_pSpawnPropertyLifetimeElement != nullptr);
#endif

	std::string strType = "";
	EasyXML::GetRequiredStringAttribute(p_pSpawnPropertyLifetimeElement, "type", strType);
	const char* cType = strType.c_str();
	if (strcmp(cType, "random") == 0)
	{
		m_bRandomLifetime = true;
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertyLifetimeElement, "min", m_fLifetimeMin);
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertyLifetimeElement, "max", m_fLifetimeMax);
	}
	else if (strcmp(cType, "constant") == 0)
	{
		m_bRandomLifetime = false;
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertyLifetimeElement, "value", m_fLifetimeMin);
		m_fLifetimeMax = m_fLifetimeMin;
	}
	else
	{
		printf("unknown type '%s' for spawn property 'lifetime'\n", cType);
		m_bRandomLifetime = false;
		m_fLifetimeMin = 1.0f;
		m_fLifetimeMax = m_fLifetimeMin;
	}
}

void HeatStroke::Emitter::InitSpawnPropertySize(tinyxml2::XMLElement* p_pSpawnPropertySizeElement)
{
#if _DEBUG
	assert(p_pSpawnPropertyVelocityElement != nullptr);
#endif

	std::string strType = "";
	EasyXML::GetRequiredStringAttribute(p_pSpawnPropertySizeElement, "type", strType);
	const char* cType = strType.c_str();
	if (strcmp(cType, "random") == 0)
	{
		m_bRandomSize = true;
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertySizeElement, "min", m_fSizeMin);
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertySizeElement, "max", m_fSizeMax);
	}
	else if (strcmp(cType, "constant") == 0)
	{
		m_bRandomSize = false;
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertySizeElement, "value", m_fSizeMin);
		m_fSizeMax = m_fSizeMin;
	}
	else
	{
		printf("unknown type '%s' for spawn property 'size'\n", cType);
		m_bRandomSize = false;
		m_fSizeMin = 1.0f;
		m_fSizeMax = m_fSizeMin;
	}
}

void HeatStroke::Emitter::InitSpawnPropertyVelocity(tinyxml2::XMLElement* p_pSpawnPropertyVelocityElement)
{
#if _DEBUG
	assert(p_pSpawnPropertyVelocityElement != nullptr);
#endif

	std::string strType = "";
	EasyXML::GetRequiredStringAttribute(p_pSpawnPropertyVelocityElement, "type", strType);
	const char* cType = strType.c_str();
	if (strcmp(cType, "random") == 0)
	{
		m_bRandomVelocity = true;
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertyVelocityElement, "x_min", m_vVelocityMin.x);
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertyVelocityElement, "y_min", m_vVelocityMin.y);
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertyVelocityElement, "z_min", m_vVelocityMin.z);
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertyVelocityElement, "x_max", m_vVelocityMax.x);
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertyVelocityElement, "y_max", m_vVelocityMax.y);
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertyVelocityElement, "z_max", m_vVelocityMax.z);
	}
	else if (strcmp(cType, "constant") == 0)
	{
		m_bRandomVelocity = false;
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertyVelocityElement, "x", m_vVelocityMin.x);
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertyVelocityElement, "y", m_vVelocityMin.y);
		EasyXML::GetRequiredFloatAttribute(p_pSpawnPropertyVelocityElement, "z", m_vVelocityMin.z);
		m_vVelocityMax = m_vVelocityMin;
	}
	else {
		printf("unknown type '%s' for spawn property 'velocity'\n", cType);
		m_bRandomVelocity = false;
		m_vVelocityMin = glm::vec3(0.0f, 0.0f, 0.0f);
		m_vVelocityMax = m_vVelocityMin;
	}
}

void HeatStroke::Emitter::InitAffectors(tinyxml2::XMLElement* p_pAffectorsElement)
{
	InitAffectorColor(p_pAffectorsElement->FirstChildElement("Color"));
	InitAffectorFade(p_pAffectorsElement->FirstChildElement("Fade"));
	InitAffectorScale(p_pAffectorsElement->FirstChildElement("Scale"));
	InitAffectorVelocity();
}

void HeatStroke::Emitter::InitAffectorColor(tinyxml2::XMLElement* p_pAffectorColorElement)
{
#if _DEBUG
	assert(p_pAffectorColorElement != nullptr);
#endif

	Affector::AFFECTOR_FUNCTION eAffectorFunction = Affector::AFFECTOR_FUNCTION::AF_CONSTANT;
	std::string strAffectorFunction = "";
	EasyXML::GetRequiredStringAttribute(p_pAffectorColorElement, "function", strAffectorFunction);
	const char* cAffectorFunction = strAffectorFunction.c_str();
	if (strcmp(cAffectorFunction, "lifetime") == 0)
	{
		eAffectorFunction = Affector::AFFECTOR_FUNCTION::AF_LIFETIME;
	}

	glm::vec3 vColor = glm::vec3(0.0f, 0.0f, 0.0f);
	EasyXML::GetRequiredFloatAttribute(p_pAffectorColorElement, "r", vColor.r);
	EasyXML::GetRequiredFloatAttribute(p_pAffectorColorElement, "g", vColor.g);
	EasyXML::GetRequiredFloatAttribute(p_pAffectorColorElement, "b", vColor.b);
	m_vAffectors.push_back(new AffectorColor(eAffectorFunction, vColor));
}

void HeatStroke::Emitter::InitAffectorFade(tinyxml2::XMLElement* p_pAffectorFadeElement)
{
#if _DEBUG
	assert(p_pAffectorFadeElement != nullptr);
#endif

	Affector::AFFECTOR_FUNCTION eAffectorFunction = Affector::AFFECTOR_FUNCTION::AF_CONSTANT;
	std::string strAffectorFunction = "";
	EasyXML::GetRequiredStringAttribute(p_pAffectorFadeElement, "function", strAffectorFunction);
	const char* cAffectorFunction = strAffectorFunction.c_str();
	if (strcmp(cAffectorFunction, "lifetime") == 0)
	{
		eAffectorFunction = Affector::AFFECTOR_FUNCTION::AF_LIFETIME;
	}

	float fFade = 0.0f;
	EasyXML::GetRequiredFloatAttribute(p_pAffectorFadeElement, "value", fFade);
	m_vAffectors.push_back(new AffectorFade(eAffectorFunction, fFade));
}

void HeatStroke::Emitter::InitAffectorScale(tinyxml2::XMLElement* p_pAffectorScaleElement)
{
#if _DEBUG
	assert(p_pAffectorScaleElement != nullptr);
#endif

	Affector::AFFECTOR_FUNCTION eAffectorFunction = Affector::AFFECTOR_FUNCTION::AF_CONSTANT;
	std::string strAffectorFunction = "";
	EasyXML::GetRequiredStringAttribute(p_pAffectorScaleElement, "function", strAffectorFunction);
	const char* cAffectorFunction = strAffectorFunction.c_str();
	if (strcmp(cAffectorFunction, "lifetime") == 0)
	{
		eAffectorFunction = Affector::AFFECTOR_FUNCTION::AF_LIFETIME;
	}

	float fScale = 0.0f;
	EasyXML::GetRequiredFloatAttribute(p_pAffectorScaleElement, "amount", fScale);
	m_vAffectors.push_back(new AffectorScale(eAffectorFunction, fScale));
}

void HeatStroke::Emitter::InitAffectorVelocity()
{
	m_vAffectors.push_back(new AffectorVelocity());
}

void HeatStroke::Emitter::InitParticles()
{
	Particle* pParticles = new Particle[m_iNumParticles];
	for (int i = 0; i < m_iNumParticles; ++i)
	{
		AddParticleToPool(&pParticles[i]);
	}
}

void HeatStroke::Emitter::AddParticleToPool(Particle* p_pParticle)
{
	p_pParticle->m_pPrev = nullptr;
	p_pParticle->m_pNext = m_pFreeList;
	m_pFreeList = p_pParticle;
}

void HeatStroke::Emitter::SpawnNewParticles(const float p_fDelta)
{
	int iNumParticlesToSpawn = 0;

	if (m_bContinuous)
	{
		m_fDuration -= p_fDelta;
		if (m_fDuration <= 0.0f && m_fDuration > -1.0f)
		{
			Stop();
			return;
		}

		m_fToSpawnAcc += m_fBirthRate * p_fDelta;
		iNumParticlesToSpawn = (int)m_fToSpawnAcc;
	}
	else
	{
		m_fTimeToBurst -= p_fDelta;
		if (m_fTimeToBurst <= 0.0f)
		{
			iNumParticlesToSpawn = m_iNumParticles;

			if (m_bRepeatBursts)
			{
				GetTimeToNextBurst();
			}
			else
			{
				Stop();
			}
		}
	}

	for (int i = 0; i < iNumParticlesToSpawn; ++i)
	{
		SpawnParticle();
	}
	m_fToSpawnAcc -= iNumParticlesToSpawn;
}

void HeatStroke::Emitter::UpdateActiveParticles(const float p_fDelta)
{
	Particle* pParticle = m_pActiveHead;
	while (pParticle)
	{
		pParticle->m_fAge += p_fDelta * pParticle->m_fAgeFactor;
		if (pParticle->m_fAge >= 1.0f)
		{
			Particle* pOldParticle = pParticle;
			pParticle = pParticle->m_pNext;
			KillParticle(pOldParticle);
		}
		else
		{
			auto it = m_vAffectors.begin();
			auto end = m_vAffectors.end();
			for (; it != end; ++it)
			{
				(*it)->Apply(p_fDelta, pParticle);
			}
			pParticle = pParticle->m_pNext;
		}
	}
}

void HeatStroke::Emitter::SpawnParticle()
{
	Particle* pParticle = GetFreeParticle();

	//TEMP
	SetRandomPosition(pParticle);
	//pParticle->m_vPos = m_Transform.GetTranslation();

	ApplySpawnProperties(pParticle);

	AddToActive(pParticle);
}

HeatStroke::Particle* HeatStroke::Emitter::GetFreeParticle()
{
	if (!m_pFreeList)
	{
		KillParticle(m_pActiveTail);
	}

	Particle* pRet = m_pFreeList;
	m_pFreeList = m_pFreeList->m_pNext;
	if (m_pFreeList)
	{
		m_pFreeList->m_pPrev = nullptr;
	}
	pRet->m_pNext = nullptr;
	pRet->m_pPrev = nullptr;
	return pRet;
}

void HeatStroke::Emitter::AddToActive(Particle* p_pParticle)
{
	p_pParticle->m_pNext = m_pActiveHead;
	if (m_pActiveHead)
	{
		m_pActiveHead->m_pPrev = p_pParticle;
	}
	else
	{
		m_pActiveTail = p_pParticle;
		m_pActiveTail->m_pPrev = nullptr;
	}
	m_pActiveHead = p_pParticle;
	m_iNumActiveParticles++;
}

void HeatStroke::Emitter::KillParticle(Particle* p_pParticle)
{
	RemoveFromActive(p_pParticle);
	AddParticleToPool(p_pParticle);
}

void HeatStroke::Emitter::RemoveFromActive(Particle* p_pParticle)
{
	if (p_pParticle->m_pNext)
	{
		p_pParticle->m_pNext->m_pPrev = p_pParticle->m_pPrev;
	}
	else
	{
		m_pActiveTail = p_pParticle->m_pPrev;
	}

	if (p_pParticle->m_pPrev)
	{
		p_pParticle->m_pPrev->m_pNext = p_pParticle->m_pNext;
	}
	else
	{
		m_pActiveHead = p_pParticle->m_pNext;
	}
	m_iNumActiveParticles--;
}

void HeatStroke::Emitter::ApplySpawnProperties(Particle* p_pParticle)
{
	ApplySpawnPropertyColor(p_pParticle);
	ApplySpawnPropertyFade(p_pParticle);
	ApplySpawnPropertyLifetime(p_pParticle);
	ApplySpawnPropertySize(p_pParticle);
	ApplySpawnPropertyVelocity(p_pParticle);
}

void HeatStroke::Emitter::ApplySpawnPropertyColor(Particle* p_pParticle)
{
	if (m_bRandomColor)
	{
		auto rangeR = std::uniform_real_distribution<float>(m_vColorMin.r, m_vColorMax.r);
		auto rangeG = std::uniform_real_distribution<float>(m_vColorMin.g, m_vColorMax.g);
		auto rangeB = std::uniform_real_distribution<float>(m_vColorMin.b, m_vColorMax.b);
		p_pParticle->m_vColor.r = rangeR(s_RNGesus);
		p_pParticle->m_vColor.g = rangeG(s_RNGesus);
		p_pParticle->m_vColor.b = rangeB(s_RNGesus);
	}
	else
	{
		p_pParticle->m_vColor = m_vColorMax;
	}
	p_pParticle->m_vColorStart = p_pParticle->m_vColor;
}

void HeatStroke::Emitter::ApplySpawnPropertyFade(Particle* p_pParticle)
{
	if (m_bRandomFade)
	{
		auto range = std::uniform_real_distribution<float>(m_fFadeMin, m_fFadeMax);
		p_pParticle->m_fFade = range(s_RNGesus);
	}
	else
	{
		p_pParticle->m_fFade = m_fFadeMax;
	}
	p_pParticle->m_fFadeStart = p_pParticle->m_fFade;
}

void HeatStroke::Emitter::ApplySpawnPropertyLifetime(Particle* p_pParticle)
{
	if (m_bRandomLifetime)
	{
		auto range = std::uniform_real_distribution<float>(m_fLifetimeMin, m_fLifetimeMax);
		p_pParticle->m_fAgeFactor = 1.0f / range(s_RNGesus);
	}
	else
	{
		p_pParticle->m_fAgeFactor = 1.0f / m_fLifetimeMax;
	}
	p_pParticle->m_fAge = 0.0f;
}

void HeatStroke::Emitter::ApplySpawnPropertySize(Particle* p_pParticle)
{
	if (m_bRandomSize)
	{
		auto range = std::uniform_real_distribution<float>(m_fSizeMin, m_fSizeMax);
		p_pParticle->m_fSize = range(s_RNGesus);
	}
	else
	{
		p_pParticle->m_fSize = m_fSizeMax;
	}
	p_pParticle->m_fSizeStart = p_pParticle->m_fSize;
}

void HeatStroke::Emitter::ApplySpawnPropertyVelocity(Particle* p_pParticle)
{
	if (m_bRandomVelocity)
	{
		auto rangeX = std::uniform_real_distribution<float>(m_vVelocityMin.x, m_vVelocityMax.x);
		auto rangeY = std::uniform_real_distribution<float>(m_vVelocityMin.y, m_vVelocityMax.y);
		auto rangeZ = std::uniform_real_distribution<float>(m_vVelocityMin.z, m_vVelocityMax.z);
		p_pParticle->m_vVelocity.x = rangeX(s_RNGesus);
		p_pParticle->m_vVelocity.y = rangeY(s_RNGesus);
		p_pParticle->m_vVelocity.z = rangeZ(s_RNGesus);
	}
	else
	{
		p_pParticle->m_vVelocity = m_vVelocityMax;
	}
}

void HeatStroke::Emitter::GetTimeToNextBurst()
{
	if (m_bRandomBurstTimer)
	{
		auto range = std::uniform_real_distribution<float>(m_fBurstRateMin, m_fBurstRateMax);
		m_fTimeToBurst = range(s_RNGesus);
	}
	else
	{
		m_fTimeToBurst = m_fBurstRateMax;
	}
}

void HeatStroke::Emitter::SetRandomPosition(Particle* p_pParticle)
{
	p_pParticle->m_vPos = glm::vec3(0.0f, 0.05f, 0.0f);
}