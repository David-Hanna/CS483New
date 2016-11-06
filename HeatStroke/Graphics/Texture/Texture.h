//-----------------------------------------------------------------------------
// File:			Texture.h
// Original Author:	Gordon Wood
//
// Class encapsulating a texture
//-----------------------------------------------------------------------------
#ifndef TEXTURE_H
#define TEXTURE_H

#include "Types.h"
#include <string>

namespace HeatStroke
{
	class Texture
	{
		friend class TextureManager;

	public:
		enum Format
		{
			FMT_888,
			FMT_8888,
			FMT_565,
			FMT_4444,
			FMT_5551,
			FMT_NUM_FORMATS,
			FMT_INVALID
		};

		enum WrapMode
		{
			WM_Repeat,
			WM_Clamp,
			WM_MirroredRepeat,
			WM_NUM_WRAP_MODES,
			WM_Invalid
		};

		enum FilterMode
		{
			FM_Nearest,
			FM_Linear,
			FM_NearestMipmap,
			FM_LinearMipmap,
			FM_TrilinearMipmap,
			FM_NUM_FILTER_MODES,
			FM_Invalid
		};

	public:
		void Bind() const;

		void SetWrapMode(WrapMode p_eWrapU, WrapMode p_eWrapV = WM_Invalid);
		void SetFilterMode(FilterMode p_eFilterMin, FilterMode p_eFilterMag = FM_Invalid);

		unsigned int GetWidth() const { return m_uiWidth; }
		unsigned int GetHeight() const { return m_uiHeight; }

	private:
		GLuint			m_uiTex;
		WrapMode		m_eWrapU;
		WrapMode		m_eWrapV;
		FilterMode		m_eFilterMin;
		FilterMode		m_eFilterMag;
		unsigned int	m_uiWidth;
		unsigned int    m_uiHeight;

		// Made private to enforce creation/destruction via TextureManager
		Texture(const std::string& p_strFile);
		Texture(void* p_pData, unsigned int p_uiWidth, unsigned int p_uiHeight, Format p_eFormat);
		virtual ~Texture();

		void LoadFromTGA(const std::string& p_strFile);
	};
}

#endif

