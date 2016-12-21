//============================================================================
// ModelManager
// Author: David Hanna
//
// Manages loaded models. 
// Ensures models are only loaded from disc once (unless flushed).
// Ensures models are all deleted when this object is destroyed.
//============================================================================

#ifndef MODEL_MANAGER_H
#define MODEL_MANAGER_H

#include "Common.h"
#include "Model.h"
#include "EasyXML.h"

#include <map>

namespace HeatStroke
{
	class ModelManager
	{
	public:
		static void CreateInstance();
		static void DestroyInstance();
		static ModelManager* Instance();

		void Preload(const std::string& p_strPreloadFileName);
		Model* GetOrCreateModel(const std::string& p_strOBJFileName);
		void Flush();

	private:
		static ModelManager* s_pModelManagerInstance;

		typedef std::map<std::string, Model*> LoadedModels;
		LoadedModels m_mLoadedModels;

		ModelManager();
		~ModelManager();
	};
}

#endif