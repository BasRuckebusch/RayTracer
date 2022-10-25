//External includes
#include "SDL.h"
#include "SDL_surface.h"
#include <stdio.h>
#include <omp.h>

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

	// const float FOV{ tan((camera.fovAngle * TO_RADIANS) / 2) };

	//Loop over all the pixels
#pragma omp parallel for
	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{

			const float cx = (2 * ((px + 0.5f) / m_Width) - 1) * aspectRatio * camera.FOV;
			const float cy = (1 - 2 * ((py + 0.5f) / m_Height)) * camera.FOV;

			Vector3 rayDirection{ cx, cy, 1 };
			rayDirection = camera.CalculateCameraToWorld().TransformVector(rayDirection);

			Ray viewRay(camera.origin, rayDirection);
			ColorRGB finalColor{};

			HitRecord closestHit{};
			pScene->GetClosestHit(viewRay, closestHit);


			if (closestHit.didHit)
			{
				bool shade{ false };
				
				// Lighting

				// Hard Shadows
				//	if (m_ShadowsEnabled)
				//	{
				//		for (auto& light : lights)
				//		{
				//			Vector3 invLightRay = LightUtils::GetDirectionToLight(light, closestHit.origin);
				//			Ray lray{ closestHit.origin, invLightRay.Normalized(),0.1f, invLightRay.Magnitude() };
				//			shade = pScene->DoesHit(lray);
				//		}
				//	}

				// Set final color to material if hit
				//	finalColor = materials[closestHit.materialIndex]->Shade();
				//	if (shade)
				//	{
				//		finalColor * 0.5;
				//	}

				
				for (auto& light : lights)
				{
					const ColorRGB E = LightUtils::GetRadiance(light, closestHit.origin);
					const float lambertCos = Vector3::Dot(closestHit.normal, LightUtils::GetDirectionToLight(light, closestHit.origin).Normalized());
					if (lambertCos > 0)
					{
						finalColor += E * materials[closestHit.materialIndex]->Shade() * lambertCos;
					}
					
				}
			}

			//Update Color in Buffer
			finalColor.MaxToOne();

			m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));
		}
	}

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}

void dae::Renderer::CycleLightingMode()
{
	switch (m_CurrentLightingMode)
	{
	case LightingMode::ObservedArea:
		m_CurrentLightingMode = LightingMode::Radiance;
		break;
	case LightingMode::Radiance:
		m_CurrentLightingMode = LightingMode::BRDF;
		break;
	case LightingMode::BRDF:
		m_CurrentLightingMode = LightingMode::Combined;
		break;
	case LightingMode::Combined:
		m_CurrentLightingMode = LightingMode::ObservedArea;
		break;
	}
}
