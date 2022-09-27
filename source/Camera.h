#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle):
			origin{_origin},
			fovAngle{_fovAngle},
			FOV{ tan((_fovAngle * TO_RADIANS) / 2) }
		{
		}


		Vector3 origin{};
		float fovAngle{90.f};
		float FOV{tan((fovAngle * TO_RADIANS) / 2)};

		// Vector3 forward{Vector3::UnitZ};
		Vector3 forward{ 0.266f, -0.453f, 0.86f };
		Vector3 up{Vector3::UnitY};
		Vector3 right{Vector3::UnitX};

		float totalPitch{0.f};
		float totalYaw{0.f};

		Matrix cameraToWorld{};

		void ChangeFOV(const float& _fovAngle)
		{
			fovAngle = fovAngle;
			FOV = tan((_fovAngle * TO_RADIANS) / 2);
		}

		Matrix CalculateCameraToWorld()
		{
			//todo: W2
			// assert(false && "Not Implemented Yet");
			// return {};

			const Vector3 worldUp{ 0.f,1.f,0.f };
			right = Vector3::Cross(worldUp, forward).Normalized();
			up = Vector3::Cross(forward, right).Normalized();
			
			Matrix ONB{};

			ONB[0] = Vector4{ right.x, right.y, right.z, 0.f };
			ONB[1] = Vector4{ up.x, up.y, up.z, 0.f };
			ONB[2] = Vector4{ forward.x, forward.y, forward.z, 0.f };
			ONB[3] = Vector4{ origin.x, origin.y, origin.z, 1.f };

			return ONB;
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);


			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			//todo: W2
			//assert(false && "Not Implemented Yet");
		}
	};
}
