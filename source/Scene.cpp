#include "Scene.h"

#include <algorithm>

#include "Utils.h"
#include "Material.h"

namespace dae {

#pragma region Base Scene
	//Initialize Scene with Default Solid Color Material (RED)
	Scene::Scene() :
		m_Materials({ new Material_SolidColor({1,0,0}) })
	{
		m_SphereGeometries.reserve(32);
		m_PlaneGeometries.reserve(32);
		m_TriangleMeshGeometries.reserve(32);
		m_Lights.reserve(32);
	}

	Scene::~Scene()
	{
		for (auto& pMaterial : m_Materials)
		{
			delete pMaterial;
			pMaterial = nullptr;
		}

		m_Materials.clear();
	}

	void dae::Scene::GetClosestHit(const Ray& ray, HitRecord& closestHit) const
	{
		HitRecord hitTest;

		for (auto& sphere : m_SphereGeometries)
		{
			if (GeometryUtils::HitTest_Sphere(sphere, ray, hitTest))
			{
				if (hitTest.t < closestHit.t)
				{
					closestHit = hitTest;
				}
			}
		}
		for (auto& plane : m_PlaneGeometries)
		{
			if (GeometryUtils::HitTest_Plane(plane, ray, hitTest))
			{
				if (hitTest.t < closestHit.t)
				{
					closestHit = hitTest;
				}
			}
		}
		//TEMP
		//	for (auto& triangle : m_Triangles)
		//	{
		//		if (GeometryUtils::HitTest_Triangle(triangle, ray, hitTest))
		//		{
		//			if (hitTest.t < closestHit.t)
		//			{
		//				closestHit = hitTest;
		//			}
		//		}
		//	}

		for (auto& triangleMesh : m_TriangleMeshGeometries)
		{
			if (GeometryUtils::HitTest_TriangleMesh(triangleMesh, ray, hitTest))
			{
				if (hitTest.t < closestHit.t)
				{
					closestHit = hitTest;
				}
			}
		}
	}

	bool Scene::DoesHit(const Ray& ray) const
	{
		for (auto& sphere : m_SphereGeometries)
		{
			if (GeometryUtils::HitTest_Sphere(sphere, ray))
			{
				return true;
			}
		}
		for (auto& plane : m_PlaneGeometries)
		{
			if (GeometryUtils::HitTest_Plane(plane, ray))
			{
				return true;
			}
		}
		//TEMP
		//	for (auto& triangle : m_Triangles)
		//	{
		//		if (GeometryUtils::HitTest_Triangle(triangle, ray))
		//		{
		//			return true;
		//		}
		//	}
		for (auto& triangleMesh : m_TriangleMeshGeometries)
		{
			if (GeometryUtils::HitTest_TriangleMesh(triangleMesh, ray))
			{
				return true;
			}
		}
		
		return false;
	}

#pragma region Scene Helpers
	Sphere* Scene::AddSphere(const Vector3& origin, float radius, unsigned char materialIndex)
	{
		Sphere s;
		s.origin = origin;
		s.radius = radius;
		s.materialIndex = materialIndex;

		m_SphereGeometries.emplace_back(s);
		return &m_SphereGeometries.back();
	}

	Plane* Scene::AddPlane(const Vector3& origin, const Vector3& normal, unsigned char materialIndex)
	{
		Plane p;
		p.origin = origin;
		p.normal = normal;
		p.materialIndex = materialIndex;

		m_PlaneGeometries.emplace_back(p);
		return &m_PlaneGeometries.back();
	}

	TriangleMesh* Scene::AddTriangleMesh(TriangleCullMode cullMode, unsigned char materialIndex)
	{
		TriangleMesh m{};
		m.cullMode = cullMode;
		m.materialIndex = materialIndex;

		m_TriangleMeshGeometries.emplace_back(m);
		return &m_TriangleMeshGeometries.back();
	}

	Light* Scene::AddPointLight(const Vector3& origin, float intensity, const ColorRGB& color)
	{
		Light l;
		l.origin = origin;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Point;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	Light* Scene::AddDirectionalLight(const Vector3& direction, float intensity, const ColorRGB& color)
	{
		Light l;
		l.direction = direction;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Directional;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	unsigned char Scene::AddMaterial(Material* pMaterial)
	{
		m_Materials.push_back(pMaterial);
		return static_cast<unsigned char>(m_Materials.size() - 1);
	}
#pragma endregion
#pragma endregion

#pragma region SCENE W1
	void Scene_W1::Initialize()
	{
		//default: Material id0 >> SolidColor Material (RED)
		constexpr unsigned char matId_Solid_Red = 0;
		const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });

		const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });
		const unsigned char matId_Solid_Green = AddMaterial(new Material_SolidColor{ colors::Green });
		const unsigned char matId_Solid_Magenta = AddMaterial(new Material_SolidColor{ colors::Magenta });

		//Spheres
		AddSphere({ -25.f, 0.f, 100.f }, 50.f, matId_Solid_Red);
		AddSphere({ 25.f, 0.f, 100.f }, 50.f, matId_Solid_Blue);

		//Plane
		AddPlane({ -75.f, 0.f, 0.f }, { 1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 75.f, 0.f, 0.f }, { -1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 0.f, -75.f, 0.f }, { 0.f, 1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 75.f, 0.f }, { 0.f, -1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 0.f, 125.f }, { 0.f, 0.f,-1.f }, matId_Solid_Magenta);
	}
#pragma endregion
#pragma region SCENE W2
	void Scene_W2::Initialize()
	{

		m_Camera.origin = { 0.f, 3.f, -9.f };
		// m_Camera.fovAngle = 45.f;
		m_Camera.ChangeFOV(45.f);

		//default: Material id0 >> SolidColor Material (RED)
		constexpr unsigned char matId_Solid_Red = 0;
		const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });

		const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });
		const unsigned char matId_Solid_Green = AddMaterial(new Material_SolidColor{ colors::Green });
		const unsigned char matId_Solid_Magenta = AddMaterial(new Material_SolidColor{ colors::Magenta });

		//Plane
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f,-1.f }, matId_Solid_Magenta);


		//Spheres
		AddSphere({ -1.75f, 1.f, 0.f }, .75f, matId_Solid_Red);
		AddSphere({ 0.f, 1.f, 0.f }, .75f, matId_Solid_Blue);
		AddSphere({ 1.75f, 1.f, 0.f }, .75f, matId_Solid_Red);
		AddSphere({ -1.75f, 3.f, 0.f }, .75f, matId_Solid_Blue);
		AddSphere({ 0.f, 3.f, 0.f }, .75f, matId_Solid_Red);
		AddSphere({ 1.75f, 3.f, 0.f }, .75f, matId_Solid_Blue);

		//Lights
		AddPointLight({ 0.f, 5.f, -5.f }, 70.f, colors::White);
	}
#pragma endregion
#pragma region SCENE W3
	void Scene_W3::Initialize()
	{
		m_Camera.origin = { 0.f, 3.f, -9.f };
		m_Camera.ChangeFOV(45.f);

		const auto matCT_GrayRoughMetal = AddMaterial(new Material_CookTorrence({ .95f, .93f, .88f}, 1, 1.f));
		const auto matCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence({ .95f, .93f, .88f }, 1, .6f));
		const auto matCT_GraySmoothMetal = AddMaterial(new Material_CookTorrence({ .95f, .93f, .88f }, 1, .1f));
		const auto matCT_GrayRoughPlastic = AddMaterial(new Material_CookTorrence({ .8f, .8f, .8f }, 0, 1.f));
		const auto matCT_GrayMediumPlastic = AddMaterial(new Material_CookTorrence({ .8f, .8f, .8f }, 0, .6f));
		const auto matCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ .8f, .8f, .8f }, 0, .1f));

		const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ .49f, .57f, .57f }, 1.f));

		//Spheres
		AddSphere({ -1.75f, 1.f, 0.f }, .75f, matCT_GrayRoughMetal);
		AddSphere({ 0.f, 1.f, 0.f }, .75f, matCT_GrayMediumMetal);
		AddSphere({ 1.75f, 1.f, 0.f }, .75f, matCT_GraySmoothMetal);
		AddSphere({ -1.75f, 3.f, 0.f }, .75f, matCT_GrayRoughPlastic);
		AddSphere({ 0.f, 3.f, 0.f }, .75f, matCT_GrayMediumPlastic);
		AddSphere({ 1.75f, 3.f, 0.f }, .75f, matCT_GraySmoothPlastic);

		//Plane
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f, -1.f }, matLambert_GrayBlue); //BACK
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matLambert_GrayBlue); //BOTTOM
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f, 0.f }, matLambert_GrayBlue); //TOP
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, matLambert_GrayBlue); //RIGHT
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, matLambert_GrayBlue); //LEFT

		//Lights
		AddPointLight({ 0.f, 5.f, 5.f }, 50.f, ColorRGB{ 1.f, .61f, .45f }); //BACKLIGHT
		AddPointLight({ -2.5f, 5.f, -5.f }, 70.f, ColorRGB{ 1.f, .8f, .45f }); //FRONT LEFT
		AddPointLight({ 2.5f, 2.5f, -5.f }, 50.f, ColorRGB{ .34f, .47f, .68f }); //BACK LEFT
	}
#pragma endregion
#pragma region SCENE TEST
	void Scene_TEST::Initialize()
	{
		m_Camera.origin = { 0.f, 3.f, -9.f };
		m_Camera.ChangeFOV(45.f);

		const auto matLambert_Red = AddMaterial(new Material_Lambert{ colors::Red, 1.f });
		const auto matLambert_Blue = AddMaterial(new Material_Lambert{ colors::Blue, 1.f });
		const auto matLambert_Yellow = AddMaterial(new Material_Lambert{ colors::Yellow, 1.f });
		const auto matLambertPhong_Blue = AddMaterial(new Material_LambertPhong{ colors::Blue, 1.f, 1.f, 60.f });
		
		const auto matCT = AddMaterial(new Material_CookTorrence({ colors::Blue }, 0.f, 0.6f));

		//Spheres
		AddSphere({ -.75f, 1.f, 0.f }, 1.f, matLambert_Red);
		AddSphere({ .75f, 1.f, 0.f }, 1.f, matCT);

		//Planes
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f,0.f }, matLambert_Yellow);

		//Lights
		AddPointLight({ 0.f, 5.f, 5.f }, 25.f, colors::White);
		AddPointLight({ 0.f, 2.5f, -5.f }, 25.f, colors::White);
	}
#pragma endregion
#pragma region SCENE W4
	void Scene_W4::Initialize()
	{
		m_Camera.origin = { 0.f, 3.f, -9.f };
		m_Camera.ChangeFOV(45.f);

		const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ .49f, .57f, .57f }, 1.f));
		const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

		//Plane
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f, -1.f }, matLambert_GrayBlue); //BACK
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matLambert_GrayBlue); //BOTTOM
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f, 0.f }, matLambert_GrayBlue); //TOP
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, matLambert_GrayBlue); //RIGHT
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, matLambert_GrayBlue); //LEFT

		//Triangle (temp)
		// auto triangle = Triangle{ {-.75f, .5f, .0f}, {-.75f, 2.f, .0f}, {.75f, .5f, .0f}};
		// triangle.cullMode = TriangleCullMode::NoCulling;
		// triangle.materialIndex = matLambert_White;
		// 
		// m_Triangles.emplace_back(triangle);

		//Triangle Mesh
		// pMesh = AddTriangleMesh(TriangleCullMode::NoCulling, matLambert_White);
		//	pMesh->positions = {
		//		{-.75f, -1.f, .0f},
		//		{-.75f, 1.f, .0f},
		//		{.75f, 1.f, 1.f},
		//		{.75f, -1.f, 0.f}
		//	};
		//	pMesh->indices = {
		//		0,1,2, //Tri 1
		//		0,2,3  //Tri 2
		//	};
		//	
		//	pMesh->CalculateNormals();
		//	
		//	pMesh->Translate({0.f, 1.5f, 0.f});
		//	pMesh->UpdateTransforms();

		pMesh = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
		Utils::ParseOBJ("Resources/simple_cube.obj",
			pMesh->positions,
			pMesh->normals,
			pMesh->indices);

		pMesh->Scale({ .7f, .7f, .7f });
		pMesh->Translate({ 0.f, 1.f, 0.f });

		pMesh->UpdateTransforms();

		//Lights
		AddPointLight({ 0.f, 5.f, 5.f }, 50.f, ColorRGB{ 1.f, .61f, .45f }); //BACKLIGHT
		AddPointLight({ -2.5f, 5.f, -5.f }, 70.f, ColorRGB{ 1.f, .8f, .45f }); //FRONT LEFT
		AddPointLight({ 2.5f, 2.5f, -5.f }, 50.f, ColorRGB{ .34f, .47f, .68f }); //BACK LEFT
	}
	void Scene_W4::Update(Timer* pTimer)
	{
		Scene::Update(pTimer);

		pMesh->RotateY(PI_DIV_2 *pTimer->GetTotal());
		pMesh->UpdateTransforms();
	}

	void SceneW4_ReferenceScene::Initialize()
	{
		m_Camera.origin = { 0.f, 3.f, -9.f };
		m_Camera.ChangeFOV(45.f);

		const auto matCT_GrayRoughMetal = AddMaterial(new Material_CookTorrence({ .95f, .93f, .88f }, 1, 1.f));
		const auto matCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence({ .95f, .93f, .88f }, 1, .6f));
		const auto matCT_GraySmoothMetal = AddMaterial(new Material_CookTorrence({ .95f, .93f, .88f }, 1, .1f));
		const auto matCT_GrayRoughPlastic = AddMaterial(new Material_CookTorrence({ .8f, .8f, .8f }, 0, 1.f));
		const auto matCT_GrayMediumPlastic = AddMaterial(new Material_CookTorrence({ .8f, .8f, .8f }, 0, .6f));
		const auto matCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ .8f, .8f, .8f }, 0, .1f));

		const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ .49f, .57f, .57f }, 1.f));
		const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

		//Spheres
		AddSphere({ -1.75f, 1.f, 0.f }, .75f, matCT_GrayRoughMetal);
		AddSphere({ 0.f, 1.f, 0.f }, .75f, matCT_GrayMediumMetal);
		AddSphere({ 1.75f, 1.f, 0.f }, .75f, matCT_GraySmoothMetal);
		AddSphere({ -1.75f, 3.f, 0.f }, .75f, matCT_GrayRoughPlastic);
		AddSphere({ 0.f, 3.f, 0.f }, .75f, matCT_GrayMediumPlastic);
		AddSphere({ 1.75f, 3.f, 0.f }, .75f, matCT_GraySmoothPlastic);

		//Plane
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f, -1.f }, matLambert_GrayBlue); //BACK
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matLambert_GrayBlue); //BOTTOM
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f, 0.f }, matLambert_GrayBlue); //TOP
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, matLambert_GrayBlue); //RIGHT
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, matLambert_GrayBlue); //LEFT

		//Triangles
		const Triangle baseTriangle = { Vector3(-.75f,1.5f,0.f), Vector3(.75f, 0.f,0.f), Vector3(-.75f,0.f,0.f) };

		m_Meshes[0] = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
		m_Meshes[0]->AppendTriangle(baseTriangle, true);
		m_Meshes[0]->Translate({ -1.75f, 4.5f, 0.f });
		m_Meshes[0]->UpdateAABB();
		m_Meshes[0]->UpdateTransforms();

		m_Meshes[1] = AddTriangleMesh(TriangleCullMode::FrontFaceCulling, matLambert_White);
		m_Meshes[1]->AppendTriangle(baseTriangle, true);
		m_Meshes[1]->Translate({ 0.f, 4.5f, 0.f });
		m_Meshes[1]->UpdateAABB();
		m_Meshes[1]->UpdateTransforms();

		m_Meshes[2] = AddTriangleMesh(TriangleCullMode::NoCulling, matLambert_White);
		m_Meshes[2]->AppendTriangle(baseTriangle, true);
		m_Meshes[2]->Translate({ 1.75f, 4.5f, 0.f });
		m_Meshes[2]->UpdateAABB();
		m_Meshes[2]->UpdateTransforms();

		//Lights
		AddPointLight({ 0.f, 5.f, 5.f }, 50.f, ColorRGB{ 1.f, .61f, .45f }); //BACKLIGHT
		AddPointLight({ -2.5f, 5.f, -5.f }, 70.f, ColorRGB{ 1.f, .8f, .45f }); //FRONT LEFT
		AddPointLight({ 2.5f, 2.5f, -5.f }, 50.f, ColorRGB{ .34f, .47f, .68f }); //BACK LEFT
	}
	void dae::SceneW4_ReferenceScene::Update(Timer* pTimer)
	{
		Scene::Update(pTimer);

		const auto yawAngle = (cos(pTimer->GetTotal()) + 1.f) / 2.f * PI_2;
		for (const auto m : m_Meshes)
		{
			m->RotateY(yawAngle);
			m->UpdateTransforms();
		}

	}

	void Scene_W4_BunnyScene::Initialize()
	{
		m_Camera.origin = { 0.f, 3.f, -9.f };
		m_Camera.ChangeFOV(45.f);

		const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ .49f, .57f, .57f }, 1.f));
		const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

		//Plane
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f, -1.f }, matLambert_GrayBlue); //BACK
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matLambert_GrayBlue); //BOTTOM
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f, 0.f }, matLambert_GrayBlue); //TOP
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, matLambert_GrayBlue); //RIGHT
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, matLambert_GrayBlue); //LEFT

		pMesh = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
		Utils::ParseOBJ("Resources/lowpoly_bunny.obj",
			pMesh->positions,
			pMesh->normals,
			pMesh->indices);

		pMesh->Scale({ 2.f, 2.f, 2.f });

		pMesh->UpdateAABB();
		pMesh->UpdateTransforms();

		//Lights
		AddPointLight({ 0.f, 5.f, 5.f }, 50.f, ColorRGB{ 1.f, .61f, .45f }); //BACKLIGHT
		AddPointLight({ -2.5f, 5.f, -5.f }, 70.f, ColorRGB{ 1.f, .8f, .45f }); //FRONT LEFT
		AddPointLight({ 2.5f, 2.5f, -5.f }, 50.f, ColorRGB{ .34f, .47f, .68f }); //BACK LEFT
	}

	void Scene_W4_BunnyScene::Update(Timer* pTimer)
	{
		Scene::Update(pTimer);

		pMesh->RotateY(PI_DIV_2 * pTimer->GetTotal());
		pMesh->UpdateTransforms();
	}
#pragma endregion
	void Scene_Extra_RandomScene::Initialize()
	{
		m_Camera.origin = { 0.f, 3.f, -20.f };
		m_Camera.ChangeFOV(70.f);
		//Spheres
		for (int i = 0; i < 20; ++i)
		{
			const auto RandMat = AddMaterial(new Material_CookTorrence({ (rand() % 255 + 1) / 255.f, (rand() % 255 + 1) / 255.f, (rand() % 255 + 1) / 255.f }, (rand() % 1), (rand() % 1) / 10.f));

			const float rX{ (static_cast<float>(rand() % 20 + 1)) - 10 };
			const float rZ{ (static_cast<float>(rand() % 20 + 1)) - 10 };
			const float rRadius{ (rand() % 150 + 1) / 100.f};
			AddSphere({ rX, 2.f, rZ }, rRadius, RandMat);
		}

		//Plane
		const auto RandMat = AddMaterial(new Material_CookTorrence({ (rand() % 255 + 1) / 255.f, (rand() % 255 + 1) / 255.f, (rand() % 255 + 1) / 255.f }, (rand() % 1), (rand() % 1) / 10.f));
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, RandMat); //BOTTOM

		//Lights
		AddPointLight({ 0.f, 15.f, 0.f }, 215.f, colors::White);

	}
}
