#include "ComponentTrackPiece.h"

namespace Kartaclysm
{
	ComponentTrackPiece::ComponentTrackPiece(
		HeatStroke::GameObject* p_pGameObject,
		float p_fWidthX,
		float p_fWidthZ,
		HeightFunction p_eHeightFunction,
		float p_fHeight1,
		float p_fHeight2)
		:
		Component(p_pGameObject),
		m_fWidthX(p_fWidthX),
		m_fWidthZ(p_fWidthZ),
		m_eHeightFunction(p_eHeightFunction),
		m_fHeight1(p_fHeight1),
		m_fHeight2(p_fHeight2)
	{
	}

	ComponentTrackPiece::~ComponentTrackPiece()
	{
	}

	HeatStroke::Component* ComponentTrackPiece::CreateComponent(HeatStroke::GameObject* p_pGameObject, tinyxml2::XMLNode* p_pBaseNode, tinyxml2::XMLNode* p_pOverrideNode)
	{
		float fWidthX;
		HeatStroke::EasyXML::GetRequiredFloatAttribute(p_pBaseNode->FirstChildElement("Bounds"), "widthX", fWidthX);

		float fWidthZ;
		HeatStroke::EasyXML::GetRequiredFloatAttribute(p_pBaseNode->FirstChildElement("Bounds"), "widthZ", fWidthZ);

		std::string sHeightFunction;
		HeatStroke::EasyXML::GetRequiredStringAttribute(p_pBaseNode->FirstChildElement("Height"), "function", sHeightFunction);

		HeightFunction eHeightFunction;
		float fHeight1;
		float fHeight2;
		if (sHeightFunction.compare("flat") == 0)
		{
			eHeightFunction = Flat;
			fHeight1 = 0.0f;
			fHeight2 = 0.0f;
		}
		else
		{
			HeatStroke::EasyXML::GetRequiredFloatAttribute(p_pBaseNode->FirstChildElement("Bounds"), "height1", fHeight1);
			HeatStroke::EasyXML::GetRequiredFloatAttribute(p_pBaseNode->FirstChildElement("Bounds"), "height2", fHeight2);
			
			if (sHeightFunction.compare("linear") == 0)
			{
				eHeightFunction = Linear;
			}
			else if (sHeightFunction.compare("sin1") == 0)
			{
				eHeightFunction = Sin1;
			}
			else if (sHeightFunction.compare("sin2") == 0)
			{
				eHeightFunction = Sin2;
			}
			else if (sHeightFunction.compare("sin3") == 0)
			{
				eHeightFunction = Sin3;
			}
			else if (sHeightFunction.compare("sin4") == 0)
			{
				eHeightFunction = Sin4;
			}
		}

		return new ComponentTrackPiece(p_pGameObject, fWidthX, fWidthZ, eHeightFunction, fHeight1, fHeight2);
	}

	void ComponentTrackPiece::Init()
	{
	}

	void ComponentTrackPiece::Update(float p_fDelta)
	{
	}

	bool ComponentTrackPiece::CheckInBounds(glm::vec3 p_pPosition)
	{
		float xMin, xMax, zMin, zMax;

		glm::vec3 origin = m_pGameObject->GetTransform().GetTranslation();
		xMin = origin.x - (m_fWidthX / 2.0f);
		xMax = origin.x + (m_fWidthX / 2.0f);
		zMin = origin.z - (m_fWidthZ / 2.0f);
		zMax = origin.z + (m_fWidthZ / 2.0f);

		if (p_pPosition.x >= xMin && p_pPosition.x < xMax && p_pPosition.z >= zMin && p_pPosition.z < zMax)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	float ComponentTrackPiece::HeightAtPosition(glm::vec3 p_pPosition)
	{
		float baseHeight = m_pGameObject->GetTransform().GetTranslation().y;
		
		switch (m_eHeightFunction)
		{
			case Flat:
				return baseHeight;
				break;
			default:
				return 0.0f;
				break;
		}
	}
}