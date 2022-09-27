//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Math.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"

using namespace dae;

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	const float aspectRatio{ static_cast<float>(m_Width) / static_cast<float>(m_Height) };

	//Loop over all the pixels

	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			const float cx = (2 * ((px + 0.5f) / m_Width) - 1) * aspectRatio;
			const float cy = (1 - 2 * ((py + 0.5f) / m_Height));

			const Vector3 rayDirection{ cx, cy, 1};

			Ray viewRay({ 0,0,0 }, rayDirection);
			ColorRGB finalColor{};

			HitRecord closestHit{};
			pScene->GetClosestHit(viewRay, closestHit);
			
			if (closestHit.didHit)
			{
				// Set final color to material if hit
				finalColor = materials[closestHit.materialIndex]->Shade();
			}

			//Update Color in Buffer
			finalColor.MaxToOne();
		
			m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));
		}
	}

	//	Calculate �viewRay�
	//	Scene::GetClosestHit(from Ray)
	//		Iterate Sphere GeometriesHit ? Closest Hit ?
	//		Iterate Plane GeometriesHit ? Closest Hit ?
	//	Did Hit ?
	//		NO > Pixel = BLACK
	//		YES > Pixel = Material Colo

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}
