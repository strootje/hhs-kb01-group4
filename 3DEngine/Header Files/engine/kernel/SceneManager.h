#ifndef SCENEMANAGER_H_
#define SCENEMANAGER_H_

#include <map>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include "..\utils\explode.h"
#include "..\utils\fileExists.h"
#include "..\scene\Scene.h"
#include "..\renderer\DirectX9Renderer.h"

namespace engine
{
	/**
	 * The SceneManager is responsible for all the scenes that the engine contains or will contain.
	 */
	class SceneManager
	{
	private:
		ResourceManager* pResourceManager;
		std::map<std::string, Scene*> scenes;
	public:
		SceneManager(ResourceManager* argPResourceManager);
		~SceneManager();
		void CleanUp();
		
		int GetSceneCount();
		Scene* GetScene(std::string argSceneName);
		std::map<std::string, Scene*> GetScenes();

		void AddScene(std::string argSceneName, Scene* argPScene);
		Scene* LoadScene(Renderer* argPRenderer, std::string argSceneName, std::string argSceneFileName);
		void RemoveScene(std::string argSceneName);
	};
}

#endif