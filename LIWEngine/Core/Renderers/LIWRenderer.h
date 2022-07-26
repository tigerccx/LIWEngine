#pragma once

#include "Framework/LIWStandardSystem.h"
#include "Maths/LIWMaths.h"
#include "Application/Window.h"

namespace LIW {
	class LIWRenderer :
		public LIWStandardSystem
	{
	public:
		friend class App::Window;
		LIWRenderer(App::Window& window);
		virtual ~LIWRenderer();

		virtual void RendererUpdate(float dt) {
			RenderScene();
		}

		inline bool HasInitialised() const { return init; }

	protected:
		virtual void RenderScene() = 0;
		virtual void Resize(int x, int y) = 0;

		Maths::LIWMatrix4 projMatrix;		//Projection matrix
		Maths::LIWMatrix4 modelMatrix;	//Model matrix. NOT MODELVIEW
		Maths::LIWMatrix4 viewMatrix;		//View matrix
		Maths::LIWMatrix4 textureMatrix;	//Texture matrix
		Maths::LIWMatrix4 shadowMatrix;

		int		width = 0;			//Render area width (not quite the same as window width)
		int		height = 0;			//Render area height (not quite the same as window height)
		bool	init = false;			//Did the renderer initialise properly?

	protected:
		App::Window* currentWindow = nullptr;
	};
}