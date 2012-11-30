#ifndef KERNEL_H_
#define KERNEL_H_

#include "WindowManager.h"
#include "Logger.h"
#include "TextLog.h"
#include "ConsoleLog.h" 
#include "SceneManager.h"
#include "InputManager.h"
#include "ResourceManager.h"

namespace engine {
	/**
	 * This class is responsible for managing the entire engine.
	 * It will check the status of, and communicate with all the sub-managers.
	 * These sub-managers are in turn responsible for running their respective parts of the engine.
	 */
	class Kernel
	{
	private:
		Logger logger;
		WindowManager* windowManager;
		SceneManager* sceneManager;
		ResourceManager* resourceManager;
		InputManager* inputManager;
	public:
		Kernel();
		~Kernel();
		void Run();
		Logger GetLogger();
	};
}
#endif
