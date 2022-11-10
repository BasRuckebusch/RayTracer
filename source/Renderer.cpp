//External includes
#include "SDL.h"
#include "SDL_surface.h"
#include <stdio.h>
#include <omp.h>
#include <future>
#include <ppl.h>

//Project includes
#include "Renderer.h"
#include "Math.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"

//Defines
//#define ASYNC
#define PARALLEL_FOR

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
	camera.CalculateCameraToWorld();

	const float fov = tan((camera.fovAngle * TO_RADIANS) / 2.f);

	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	const float aspectRatio{ static_cast<float>(m_Width) / static_cast<float>(m_Height) };

	// const float FOV{ tan((camera.fovAngle * TO_RADIANS) / 2) };

	const uint32_t numPixels = m_Width * m_Height;

	#if defined(ASYNC)
	//async logic
	const uint32_t numCores = std::thread::hardware_concurrency();
	std::vector<std::future<void>> async_futures{};

	const uint32_t numPixelPerTask{ numPixels / numCores };
	uint32_t numAssignedPixels = numPixels % numCores;
	uint32_t currentPixelIndex = 0;

	for (uint32_t coreId{ 0 }; coreId < numCores; ++coreId)
	{
		uint32_t taskSize{ numPixelPerTask };
		if (numAssignedPixels > 0)
		{
			++taskSize;
			--numAssignedPixels;
		}

		async_futures.push_back(
			std::async(std::launch::async, [=, this]
				{
					const uint32_t endPixelIndex{ currentPixelIndex + taskSize };
					for (uint32_t pixelIndex{ currentPixelIndex }; pixelIndex < endPixelIndex; ++pixelIndex)
					{
						RenderPixel(pScene, pixelIndex, fov, aspectRatio, camera, lights, materials);
					}
				})
		);
		currentPixelIndex += taskSize;
	}

	//wait until all tasks are finished
	for (const std::future<void>& f : async_futures)
	{
		f.wait();
	}

	#elif defined(PARALLEL_FOR)
	//parallel for logic

	concurrency::parallel_for(0u, numPixels, [=, this](int pixelIndex)
		{
			RenderPixel(pScene, pixelIndex, fov, aspectRatio, camera, lights, materials);
		});

	#else
	//No Threading
	for (uint32_t i = 0; i < numPixels; ++i)
	{
		RenderPixel(pScene, i, fov, aspectRatio, camera, lights, materials);
	}
	#endif


	//Loop over all the pixels
	//	#pragma omp parallel for
	//	for (int px{}; px < m_Width; ++px)
	//	{
	//		for (int py{}; py < m_Height; ++py)
	//		{
	//	
	//			const float cx = (2 * ((px + 0.5f) / m_Width) - 1) * aspectRatio * camera.FOV;
	//			const float cy = (1 - 2 * ((py + 0.5f) / m_Height)) * camera.FOV;
	//	
	//			Vector3 rayDirection{ cx, cy, 1 };
	//			rayDirection = camera.CalculateCameraToWorld().TransformVector(rayDirection);
	//	
	//			Ray viewRay(camera.origin, rayDirection);
	//			ColorRGB finalColor{};
	//	
	//			HitRecord closestHit{};
	//			pScene->GetClosestHit(viewRay, closestHit);
	//	
	//	
	//			if (closestHit.didHit)
	//			{
	//				for (auto& light : lights)
	//				{
	//					
	//					const ColorRGB E = LightUtils::GetRadiance(light, closestHit.origin);
	//					const float lambertCos = Vector3::Dot(closestHit.normal, LightUtils::GetDirectionToLight(light, closestHit.origin).Normalized());
	//					if (lambertCos < 0)
	//					{
	//						continue;
	//					}
	//	
	//					const Vector3 invLightRay = LightUtils::GetDirectionToLight(light, closestHit.origin);
	//					if (m_ShadowsEnabled)
	//					{
	//						const Ray lray{ closestHit.origin, invLightRay.Normalized(),0.1f, invLightRay.Magnitude() };
	//	
	//						if (pScene->DoesHit(lray))
	//						{
	//							continue;
	//						}
	//					}
	//					
	//					switch (m_CurrentLightingMode)
	//					{
	//					case LightingMode::Combined:
	//						finalColor += E * materials[closestHit.materialIndex]->Shade(closestHit, invLightRay.Normalized(), -viewRay.direction.Normalized()) * lambertCos;
	//						break;
	//					case LightingMode::ObservedArea:
	//						finalColor += E;
	//						break;
	//					case LightingMode::Radiance:
	//						finalColor += E * lambertCos;
	//						break;
	//					case LightingMode::BRDF:
	//						finalColor += materials[closestHit.materialIndex]->Shade(closestHit, invLightRay.Normalized(), -viewRay.direction.Normalized());
	//						break;
	//					
	//					}
	//				}
	//			}
	//	
	//			//Update Color in Buffer
	//			finalColor.MaxToOne();
	//	
	//			m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
	//				static_cast<uint8_t>(finalColor.r * 255),
	//				static_cast<uint8_t>(finalColor.g * 255),
	//				static_cast<uint8_t>(finalColor.b * 255));
	//		}
	//	}

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

void Renderer::RenderPixel(Scene* pScene, uint32_t pixelIndex, float fov, float aspectRatio,
	const Camera& camera, const std::vector<Light>& lights, const std::vector<Material*>& materials) const
{
	const int px = pixelIndex % m_Width;
	const int py = pixelIndex  / m_Width;

	float rx = px + 0.5f;
	float ry = py + 0.5f;

	float cx = (2 * (rx / float(m_Width)) - 1) * aspectRatio * fov;
	float cy = (1 - (2 * (ry / float(m_Height)))) * fov;

	Vector3 rayDirection{ cx, cy, 1 };
	Camera cam = camera;
	rayDirection = cam.CalculateCameraToWorld().TransformVector(rayDirection);

	const Ray viewRay(camera.origin, rayDirection);
	ColorRGB finalColor{};

	HitRecord closestHit{};
	pScene->GetClosestHit(viewRay, closestHit);


	if (closestHit.didHit)
	{
		for (auto& light : lights)
		{

			const ColorRGB E = LightUtils::GetRadiance(light, closestHit.origin);
			const float lambertCos = Vector3::Dot(closestHit.normal, LightUtils::GetDirectionToLight(light, closestHit.origin).Normalized());
			if (lambertCos < 0)
			{
				continue;
			}

			const Vector3 invLightRay = LightUtils::GetDirectionToLight(light, closestHit.origin);
			if (m_ShadowsEnabled)
			{
				const Ray lray{ closestHit.origin, invLightRay.Normalized(),0.1f, invLightRay.Magnitude() };

				if (pScene->DoesHit(lray))
				{
					continue;
				}
			}

			switch (m_CurrentLightingMode)
			{
			case LightingMode::Combined:
				finalColor += E * materials[closestHit.materialIndex]->Shade(closestHit, invLightRay.Normalized(), -viewRay.direction.Normalized()) * lambertCos;
				break;
			case LightingMode::ObservedArea:
				finalColor += ColorRGB(1,1,1) * lambertCos;
				break;
			case LightingMode::Radiance:
				finalColor += E;
				break;
			case LightingMode::BRDF:
				finalColor += materials[closestHit.materialIndex]->Shade(closestHit, invLightRay.Normalized(), -viewRay.direction.Normalized());
				break;
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
