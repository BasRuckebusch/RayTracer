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

		Vector3 forward{Vector3::UnitZ};
		//Vector3 forward{ 0.266f, -0.453f, 0.86f };
		Vector3 up{Vector3::UnitY};
		Vector3 right{Vector3::UnitX};

		float totalPitch{0.f};
		float totalYaw{0.f};

		Matrix cameraToWorld{};

		void ChangeFOV(const float& _fovAngle)
		{
			fovAngle = _fovAngle;
			FOV = tan((_fovAngle * TO_RADIANS) / 2);
		}

		Matrix CalculateCameraToWorld()
		{
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
			const float defVelocity = 10.f;
			float velocity = 10.f;
			const Matrix ONB = CalculateCameraToWorld();


			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			// LShift movement increase
			if (pKeyboardState[SDL_SCANCODE_LSHIFT])
			{
				velocity = defVelocity * 4;
			}
			else
			{
				velocity = defVelocity;
			}

			// Forward Backward
			if (pKeyboardState[SDL_SCANCODE_W])
			{
				origin += forward * velocity * deltaTime;
			}
			else if (pKeyboardState[SDL_SCANCODE_S])
			{
				origin -= forward * velocity * deltaTime;
			}

			// Left Right
			if (pKeyboardState[SDL_SCANCODE_A])
			{
				origin -= Vector3{ONB[0]} * velocity * deltaTime;
			}
			else if (pKeyboardState[SDL_SCANCODE_D])
			{
				origin += Vector3{ ONB[0] } *velocity * deltaTime;
			}

			// FOV change
			if (pKeyboardState[SDL_SCANCODE_LEFT] && fovAngle > 1)
			{
				ChangeFOV(fovAngle - 1);
			}
			else if (pKeyboardState[SDL_SCANCODE_RIGHT] && fovAngle < 179)
			{
				ChangeFOV(fovAngle + 1);
			}

			// Up down keys
			if (pKeyboardState[SDL_SCANCODE_Q])
			{
				origin.y -= velocity * deltaTime;
			}
			else if (pKeyboardState[SDL_SCANCODE_E])
			{
				origin.y += velocity * deltaTime;
			}

			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			const float MouseSensitivity = 0.2f;
			const float lookConstraint = 0.95f;
			float YawAngle{};
			float PitchAngle{};

			Matrix finalRotation{};

			// Both buttons up down
			if (mouseState & SDL_BUTTON_RMASK && mouseState & SDL_BUTTON_LMASK)
			{
				if (mouseY != 0)
				{
					origin.y -= mouseY * velocity * deltaTime;
				}
			}
			else
			{
				// RMB 
				if (mouseState & SDL_BUTTON_RMASK)
				{
					if (mouseX != 0)
					{
						YawAngle -= mouseX * MouseSensitivity * deltaTime;
					}

					if (mouseY != 0)
					{
						PitchAngle -= mouseY * MouseSensitivity * deltaTime;
					}

					finalRotation = Matrix::CreateRotation(PitchAngle, YawAngle, 0);
					
					forward = finalRotation.TransformVector(forward);
					forward.Normalize();

					// stop camera from upside down
					if (forward.y > lookConstraint)
					{
						forward.y = lookConstraint;
					}
					if (forward.y < -lookConstraint)
					{
						forward.y = -lookConstraint;
					}
				}
				// LMB
				if (mouseState & SDL_BUTTON_LMASK)
				{
					if (mouseX != 0)
					{
						YawAngle -= mouseX * MouseSensitivity * deltaTime;
					}
					if (mouseY != 0)
					{
						origin -= mouseY * velocity * forward * deltaTime;
					}

					finalRotation = Matrix::CreateRotation(PitchAngle, YawAngle, 0);

					forward = finalRotation.TransformVector(forward);
					forward.Normalize();
				}
			}
		}
	};
}
