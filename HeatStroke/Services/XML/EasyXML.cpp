//--------------------------------------------------------------------------------
// File:	EasyXML.cpp
// Author:	David Hanna
//
// Provides convenient functions for reading XML files.
//--------------------------------------------------------------------------------

#include "EasyXML.h"

namespace HeatStroke
{
	namespace EasyXML
	{
		void GetOptionalStringAttribute(const tinyxml2::XMLElement* p_pElement, const char* p_strAttrName, std::string& p_strOutValue, std::string& p_strDefault)
		{
			if (p_pElement == nullptr || p_strAttrName == nullptr)
			{
				p_strOutValue = p_strDefault;
				return;
			}

			const char* strAttributeText = p_pElement->Attribute(p_strAttrName);

			if (strAttributeText == nullptr)
			{
				p_strOutValue = p_strDefault;
				return;
			}

			p_strOutValue = std::string(strAttributeText);
		}

		void GetOptionalIntAttribute(const tinyxml2::XMLElement* p_pElement, const char* p_strAttrName, int& p_iOutValue, int p_iDefault)
		{
			if (p_pElement == nullptr || p_strAttrName == nullptr)
			{
				p_iOutValue = p_iDefault;
				return;
			}

			if (p_pElement->QueryIntAttribute(p_strAttrName, &p_iOutValue) == tinyxml2::XML_NO_ERROR)
			{
				return;
			}

			p_iOutValue = p_iDefault;
		}

		void GetOptionalUIntAttribute(const tinyxml2::XMLElement* p_pElement, const char* p_strAttrName, unsigned int& p_uiOutValue, unsigned int p_uiDefault)
		{
			if (p_pElement == nullptr || p_strAttrName == nullptr)
			{
				p_uiOutValue = p_uiDefault;
				return;
			}

			if (p_pElement->QueryUnsignedAttribute(p_strAttrName, &p_uiOutValue) == tinyxml2::XML_NO_ERROR)
			{
				return;
			}

			p_uiOutValue = p_uiDefault;
		}

		void GetOptionalBoolAttribute(const tinyxml2::XMLElement* p_pElement, const char* p_strAttrName, bool& p_bOutValue, bool p_bDefault)
		{
			if (p_pElement == nullptr || p_strAttrName == nullptr)
			{
				p_bOutValue = p_bDefault;
				return;
			}

			if (p_pElement->QueryBoolAttribute(p_strAttrName, &p_bOutValue) == tinyxml2::XML_NO_ERROR)
			{
				return;
			}

			p_bOutValue = p_bDefault;
		}

		void GetOptionalFloatAttribute(const tinyxml2::XMLElement* p_pElement, const char* p_strAttrName, float& p_fOutValue, float p_fDefault)
		{
			if (p_pElement == nullptr || p_strAttrName == nullptr)
			{
				p_fOutValue = p_fDefault;
				return;
			}

			if (p_pElement->QueryFloatAttribute(p_strAttrName, &p_fOutValue) == tinyxml2::XML_NO_ERROR)
			{
				return;
			}

			p_fOutValue = p_fDefault;
		}

		void GetRequiredStringAttribute(const tinyxml2::XMLElement* p_pElement, const char* p_strAttrName, std::string& p_strOutValue)
		{
			assert(p_pElement != nullptr);
			assert(p_strAttrName != nullptr);
			const char* strAttributeText = p_pElement->Attribute(p_strAttrName);
			assert(strAttributeText != nullptr);
			p_strOutValue = std::string(strAttributeText);
		}

		void GetRequiredIntAttribute(const tinyxml2::XMLElement* p_pElement, const char* p_strAttrName, int& p_iOutValue)
		{
			assert(p_pElement != nullptr);
			assert(p_strAttrName != nullptr);
			assert(p_pElement->QueryIntAttribute(p_strAttrName, &p_iOutValue) == tinyxml2::XML_NO_ERROR);
		}

		void GetRequiredUIntAttribute(const tinyxml2::XMLElement* p_pElement, const char* p_strAttrName, unsigned int& p_uiOutValue)
		{
			assert(p_pElement != nullptr);
			assert(p_strAttrName != nullptr);
			assert(p_pElement->QueryUnsignedAttribute(p_strAttrName, &p_uiOutValue) == tinyxml2::XML_NO_ERROR);
		}

		void GetRequiredBoolAttribute(const tinyxml2::XMLElement* p_pElement, const char* p_strAttrName, bool& p_bOutValue)
		{
			assert(p_pElement != nullptr);
			assert(p_strAttrName != nullptr);
			assert(p_pElement->QueryBoolAttribute(p_strAttrName, &p_bOutValue) == tinyxml2::XML_NO_ERROR);
		}

		void GetRequiredFloatAttribute(const tinyxml2::XMLElement* p_pElement, const char* p_strAttrName, float& p_fOutValue)
		{
			assert(p_pElement != nullptr);
			assert(p_strAttrName != nullptr);
			assert(p_pElement->QueryFloatAttribute(p_strAttrName, &p_fOutValue) == tinyxml2::XML_NO_ERROR);
		}

		tinyxml2::XMLNode* FindChildNode(tinyxml2::XMLNode* p_pNode, const char* p_szChildNodeName)
		{
			tinyxml2::XMLNode* p_pChildNode;
			for (p_pChildNode = p_pNode->FirstChild();
				p_pChildNode != nullptr && strcmp(p_pChildNode->Value(), p_szChildNodeName) != 0;
				p_pChildNode = p_pChildNode->NextSibling())
			{
			}
			return p_pChildNode;
		}
	}
}