#include "ComponentTrackPiece.h"

namespace Kartaclysm
{
	ComponentTrackPiece::ComponentTrackPiece(
		HeatStroke::GameObject* p_pGameObject,
		float p_fWidthX,
		float p_fWidthZ,
		HeightFunction p_eHeightFunction,
		float p_fHeight1,
		float p_fHeight2,
		PositionFunction p_ePositionFunction,
		glm::vec3 p_vPivotPosition,
		glm::vec3 p_vPivotAxis,
		std::vector<OffroadSquare> p_vOffroadSquares,
		std::vector<OffroadTriangle> p_vOffroadTriangles)
		:
		Component(p_pGameObject),
		m_fWidthX(p_fWidthX),
		m_fWidthZ(p_fWidthZ),
		m_eHeightFunction(p_eHeightFunction),
		m_fHeight1(p_fHeight1),
		m_fHeight2(p_fHeight2),
		m_ePositionFunction(p_ePositionFunction),
		m_vPivotPosition(p_vPivotPosition),
		m_vPivotAxis(p_vPivotAxis),
		m_vOffroadSquares(p_vOffroadSquares),
		m_vOffroadTriangles(p_vOffroadTriangles)
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
			HeatStroke::EasyXML::GetRequiredFloatAttribute(p_pBaseNode->FirstChildElement("Height"), "height1", fHeight1);
			HeatStroke::EasyXML::GetRequiredFloatAttribute(p_pBaseNode->FirstChildElement("Height"), "height2", fHeight2);
			
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

		PositionFunction ePositionFunction = ParsePositionFunction(p_pBaseNode);
		glm::vec3 vPivotPosition;
		glm::vec3 vPivotAxis;
		if (ePositionFunction == Turn)
		{
			vPivotPosition = ParsePivotPosition(p_pBaseNode);
			vPivotAxis = ParsePivotAxis(p_pBaseNode);
		}

		tinyxml2::XMLElement* offroad = p_pBaseNode->FirstChildElement("Offroad");
		std::vector<OffroadSquare> vOffroadSquares;
		std::vector<OffroadTriangle> vOffroadTriangles;

		if (offroad != nullptr)
		{
			for (tinyxml2::XMLElement* offroadShape = offroad->FirstChildElement(); offroadShape != nullptr; offroadShape = offroadShape->NextSiblingElement())
			{
				std::string type;
				HeatStroke::EasyXML::GetRequiredStringAttribute(offroadShape, "type", type);

				if (type.compare("square") == 0)
				{
					OffroadSquare shape;

					HeatStroke::EasyXML::GetRequiredFloatAttribute(offroadShape, "x1", shape.x1);
					HeatStroke::EasyXML::GetRequiredFloatAttribute(offroadShape, "z1", shape.z1);
					HeatStroke::EasyXML::GetRequiredFloatAttribute(offroadShape, "x2", shape.x2);
					HeatStroke::EasyXML::GetRequiredFloatAttribute(offroadShape, "z2", shape.z2);

					vOffroadSquares.push_back(shape);
				}
				else if (type.compare("triangle") == 0)
				{
					OffroadTriangle shape;

					HeatStroke::EasyXML::GetRequiredFloatAttribute(offroadShape, "x1", shape.x1);
					HeatStroke::EasyXML::GetRequiredFloatAttribute(offroadShape, "z1", shape.z1);
					HeatStroke::EasyXML::GetRequiredFloatAttribute(offroadShape, "x2", shape.x2);
					HeatStroke::EasyXML::GetRequiredFloatAttribute(offroadShape, "z2", shape.z2);
					HeatStroke::EasyXML::GetRequiredFloatAttribute(offroadShape, "x3", shape.x3);
					HeatStroke::EasyXML::GetRequiredFloatAttribute(offroadShape, "z3", shape.z3);

					vOffroadTriangles.push_back(shape);
				}
			}
		}

		return new ComponentTrackPiece(p_pGameObject, fWidthX, fWidthZ, eHeightFunction, fHeight1, fHeight2, ePositionFunction, vPivotPosition, vPivotAxis, vOffroadSquares, vOffroadTriangles);
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

		if (m_eHeightFunction == Flat)
		{
			return baseHeight;
		}

		glm::vec3 checkPosition = p_pPosition;
		checkPosition = checkPosition - m_pGameObject->GetTransform().GetTranslation();
		checkPosition = checkPosition * -m_pGameObject->GetTransform().GetRotation();

		float progress = 1.0f - ((checkPosition.x / m_fWidthX) + 0.5f);
		
		switch (m_eHeightFunction)
		{
			case Linear:
				return baseHeight + m_fHeight1 + (m_fHeight2 * progress);
				break;
			case Sin1:
				return baseHeight + m_fHeight1 + (m_fHeight2 * sinf(progress*(PI / 2.0f)));
				break;
			case Sin2:
				return baseHeight + m_fHeight2 + (m_fHeight1 * sinf((PI / 2.0f) + (progress*(PI / 2.0f))));
				break;
			case Sin3:
				return baseHeight + m_fHeight2 + (m_fHeight1 * (sinf(PI + (progress*(PI / 2.0f))) + 1.0f));
				break;
			case Sin4:
				return baseHeight + m_fHeight1 + (m_fHeight2 * (sinf((PI * 1.5f) + (progress*(PI / 2.0f))) + 1.0f));
				break;
			default:
				return baseHeight;
				break;
		}
	}

	bool ComponentTrackPiece::IsOffroadAtPosition(const glm::vec3 &p_pPosition)
	{
		glm::vec3 checkPosition = p_pPosition;
		checkPosition = checkPosition - m_pGameObject->GetTransform().GetTranslation();
		checkPosition = checkPosition * -m_pGameObject->GetTransform().GetRotation();

		float x = (checkPosition.x / m_fWidthX) * 2.0f;
		float z = (checkPosition.z / m_fWidthZ) * 2.0f;

		for (unsigned int i = 0; i < m_vOffroadSquares.size(); i++)
		{
			OffroadSquare square = m_vOffroadSquares[i];

			if (((x >= square.x1 && x <= square.x2) || (x >= square.x2 && x <= square.x1))
				&& ((z >= square.z1 && z <= square.z2) || (z >= square.z2 && z <= square.z1)))
			{
				return true;
			}
		}

		for (unsigned int i = 0; i < m_vOffroadTriangles.size(); i++)
		{
			OffroadTriangle triangle = m_vOffroadTriangles[i];

			if (PointInTriangle(glm::vec2(x, z), glm::vec2(triangle.x1, triangle.z1), glm::vec2(triangle.x2, triangle.z2), glm::vec2(triangle.x3, triangle.z3)))
			{
				return true;
			}
		}

		return false;
	}

	bool ComponentTrackPiece::PointInTriangle(const glm::vec2 &p, const glm::vec2 &p0, const glm::vec2 &p1, const glm::vec2 &p2)
	{
		// Shamelessly stolen from stack overflow
		// Eat a dick, internet
		
		float s = p0.y * p2.x - p0.x * p2.y + (p2.y - p0.y) * p.x + (p0.x - p2.x) * p.y;
		float t = p0.x * p1.y - p0.y * p1.x + (p0.y - p1.y) * p.x + (p1.x - p0.x) * p.y;

		if ((s < 0) != (t < 0))
			return false;

		float A = -p1.y * p2.x + p0.y * (p2.x - p1.x) + p0.x * (p1.y - p2.y) + p1.x * p2.y;
		if (A < 0.0)
		{
			s = -s;
			t = -t;
			A = -A;
		}
		return s > 0 && t > 0 && (s + t) <= A;
	}

	bool ComponentTrackPiece::IsAhead(const glm::vec3& p_vFirstRacerPosition, const glm::vec3& p_vSecondRacerPosition) const
	{
		switch (m_ePositionFunction)
		{
			case Straight:
				return IsAheadOnStraight(p_vFirstRacerPosition, p_vSecondRacerPosition);
			case Turn:
				return IsAheadOnTurn(p_vFirstRacerPosition, p_vSecondRacerPosition);
			default:
				printf("Unknown position function\n");
				return false;
				break;
		}
	}

	bool ComponentTrackPiece::IsAheadOnStraight(const glm::vec3& p_vFirstRacerPosition, const glm::vec3& p_vSecondRacerPosition) const
	{
		glm::vec4 vPlane = CalculatePlane();

		float fFirstRacerDistance = GetDistanceToPlane(vPlane, p_vFirstRacerPosition);
		float fSecondRacerDistance = GetDistanceToPlane(vPlane, p_vSecondRacerPosition);

		return fFirstRacerDistance <= fSecondRacerDistance;
	}

	bool ComponentTrackPiece::IsAheadOnTurn(const glm::vec3& p_vFirstRacerPosition, const glm::vec3& p_vSecondRacerPosition) const
	{
		glm::vec3 vPivot = CalculatePivot();
		glm::vec3 vPivotAxis = CalculatePivotAxis();

		glm::vec3 v1 = glm::normalize(p_vFirstRacerPosition - vPivot);
		glm::vec3 v2 = glm::normalize(p_vSecondRacerPosition - vPivot);

		return glm::dot(vPivotAxis, v1) <= glm::dot(vPivotAxis, v2);
	}

	//TODO would be great to calculate this once and store it for all future uses
	glm::vec4 ComponentTrackPiece::CalculatePlane() const
	{
		HeatStroke::HierarchicalTransform trackTransform = m_pGameObject->GetTransform();
		glm::quat qTrackRotation = trackTransform.GetRotation();
		qTrackRotation.y *= -1.0f;
		glm::vec3 vNormal = glm::vec3(1.0f, 0.0f, 0.0f) * qTrackRotation;
		glm::vec3 vPlaneCenter = trackTransform.GetTranslation() + (glm::vec3(-5.0f, 0.0f, 0.0f) * qTrackRotation);
		return glm::vec4(vNormal, -(vNormal.x * vPlaneCenter.x) - (vNormal.y * vPlaneCenter.y) - (vNormal.z * vPlaneCenter.z));
	}

	float ComponentTrackPiece::GetDistanceToPlane(const glm::vec4& p_vPlane, const glm::vec3& p_vPoint) const
	{
		return ((p_vPlane.x * p_vPoint.x) + (p_vPlane.y * p_vPoint.y) + (p_vPlane.z * p_vPoint.z) + p_vPlane.w) /
			(sqrt(pow(p_vPlane.x, 2) + pow(p_vPlane.y, 2) + pow(p_vPlane.z, 2)));
	}

	//TODO would be great to calculate this once and store it for all future uses
	glm::vec3 ComponentTrackPiece::CalculatePivot() const
	{
		HeatStroke::HierarchicalTransform trackTransform = m_pGameObject->GetTransform();
		glm::quat qTrackRotation = trackTransform.GetRotation();
		qTrackRotation.y *= -1.0f;
		return trackTransform.GetTranslation() + (m_vPivotPosition * qTrackRotation);
	}

	//TODO would be great to calculate this once and store it for all future uses
	glm::vec3 ComponentTrackPiece::CalculatePivotAxis() const
	{
		HeatStroke::HierarchicalTransform trackTransform = m_pGameObject->GetTransform();
		glm::quat qTrackRotation = trackTransform.GetRotation();
		qTrackRotation.y *= -1.0f;
		return glm::normalize(m_vPivotAxis * qTrackRotation);
	}

	PositionFunction ComponentTrackPiece::ParsePositionFunction(const tinyxml2::XMLNode* p_pNode)
	{
		std::string strHeightFunction = "";
		HeatStroke::EasyXML::GetRequiredStringAttribute(p_pNode->FirstChildElement("Position"), "function", strHeightFunction);
		if (strHeightFunction.compare("straight") == 0)
		{
			return Straight;
		}
		else if (strHeightFunction.compare("turn") == 0)
		{
			return Turn;
		}
		else
		{
			assert(false && "Unknown track position function.");
			return Straight;
		}
	}

	glm::vec3 ComponentTrackPiece::ParsePivotPosition(const tinyxml2::XMLNode* p_pNode)
	{
		glm::vec3 vPivotPosition;
		const tinyxml2::XMLElement* pPositionElement = p_pNode->FirstChildElement("Position");
		HeatStroke::EasyXML::GetRequiredFloatAttribute(pPositionElement, "pivotX", vPivotPosition.x);
		HeatStroke::EasyXML::GetRequiredFloatAttribute(pPositionElement, "pivotY", vPivotPosition.y);
		HeatStroke::EasyXML::GetRequiredFloatAttribute(pPositionElement, "pivotZ", vPivotPosition.z);
		return vPivotPosition;
	}

	glm::vec3 ComponentTrackPiece::ParsePivotAxis(const tinyxml2::XMLNode* p_pNode)
	{
		glm::vec3 vPivotAxis;
		const tinyxml2::XMLElement* pPositionElement = p_pNode->FirstChildElement("Position");
		HeatStroke::EasyXML::GetRequiredFloatAttribute(pPositionElement, "pivotAxisX", vPivotAxis.x);
		HeatStroke::EasyXML::GetRequiredFloatAttribute(pPositionElement, "pivotAxisY", vPivotAxis.y);
		HeatStroke::EasyXML::GetRequiredFloatAttribute(pPositionElement, "pivotAxisZ", vPivotAxis.z);
		return vPivotAxis;
	}
}