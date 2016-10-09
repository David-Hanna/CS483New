//--------------------------------------------------------------------------------
// File:	EasyXML.h
// Author:	David Hanna
//
// Provides convenient functions for reading XML files.
//--------------------------------------------------------------------------------

#ifndef EASYXML_H
#define EASYXML_H

#include <string>
#include <tinyxml2.h>
#include <assert.h>

namespace HeatStroke
{
	namespace EasyXML
	{
		// Returns an attribute from the given element. If the attribute does not exist, the default is returned.
		void GetOptionalStringAttribute(const tinyxml2::XMLElement* p_pElement, const char* p_strAttrName, std::string& p_strOutValue, std::string& p_strDefault);
		void GetOptionalIntAttribute(const tinyxml2::XMLElement* p_pElement, const char* p_strAttrName, int& p_iOutValue, int p_iDefault);
		void GetOptionalUIntAttribute(const tinyxml2::XMLElement* p_pElement, const char* p_strAttrName, unsigned int& p_uiOutValue, unsigned int p_uiDefault);
		void GetOptionalBoolAttribute(const tinyxml2::XMLElement* p_pElement, const char* p_strAttrName, bool& p_bOutValue, bool p_bDefault);
		void GetOptionalFloatAttribute(const tinyxml2::XMLElement* p_pElement, const char* p_strAttrName, float& p_fOutValue, float p_fDefault);

		// Returns an attribute from the given element. If the attribute does not exist, an assertion is made.
		void GetRequiredStringAttribute(const tinyxml2::XMLElement* p_pElement, const char* p_strAttrName, std::string& p_strOutValue);
		void GetRequiredIntAttribute(const tinyxml2::XMLElement* p_pElement, const char* p_strAttrName, int& p_iOutValue);
		void GetRequiredUIntAttribute(const tinyxml2::XMLElement* p_pElement, const char* p_strAttrName, unsigned int& p_uiOutValue);
		void GetRequiredBoolAttribute(const tinyxml2::XMLElement* p_pElement, const char* p_strAttrName, bool& p_bOutValue);
		void GetRequiredFloatAttribute(const tinyxml2::XMLElement* p_pElement, const char* p_strAttrName, float& p_fOutValue);

		// Helps find a child node of a given node by tagname.
		tinyxml2::XMLNode* FindChildNode(tinyxml2::XMLNode* p_pNode, const char* p_szChildNodeName);
	}
}

#endif // EASYXML_H