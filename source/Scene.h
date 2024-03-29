#pragma once
#include <string>
#include <vector>

#include "Math.h"
#include "DataTypes.h"
#include "Camera.h"

namespace dae
{
	//Forward Declarations
	class Timer;
	class Material;
	struct Plane;
	struct Sphere;
	struct Light;

	//Scene Base Class
	class Scene
	{
	public:
		Scene();
		virtual ~Scene();

		Scene(const Scene&) = delete;
		Scene(Scene&&) noexcept = delete;
		Scene& operator=(const Scene&) = delete;
		Scene& operator=(Scene&&) noexcept = delete;

		virtual void Initialize() = 0;
		virtual void Update(dae::Timer* pTimer)
		{
			m_Camera.Update(pTimer);
		}

		Camera& GetCamera() { return m_Camera; }
		void GetClosestHit(const Ray& ray, HitRecord& closestHit) const;
		bool DoesHit(const Ray& ray) const;

		const std::vector<Plane>& GetPlaneGeometries() const { return m_PlaneGeometries; }
		const std::vector<Sphere>& GetSphereGeometries() const { return m_SphereGeometries; }
		const std::vector<Light>& GetLights() const { return m_Lights; }
		const std::vector<Material*> GetMaterials() const { return m_Materials; }

	protected:
		std::string	sceneName;

		std::vector<Plane> m_PlaneGeometries{};
		std::vector<Sphere> m_SphereGeometries{};
		std::vector<TriangleMesh> m_TriangleMeshGeometries{};
		std::vector<Light> m_Lights{};
		std::vector<Material*> m_Materials{};

		//Temp individual triangle
		//std::vector<Triangle> m_Triangles{};

		Camera m_Camera{};

		Sphere* AddSphere(const Vector3& origin, float radius, unsigned char materialIndex = 0);
		Plane* AddPlane(const Vector3& origin, const Vector3& normal, unsigned char materialIndex = 0);
		TriangleMesh* AddTriangleMesh(TriangleCullMode cullMode, unsigned char materialIndex = 0);

		Light* AddPointLight(const Vector3& origin, float intensity, const ColorRGB& color);
		Light* AddDirectionalLight(const Vector3& direction, float intensity, const ColorRGB& color);
		Light* AddRectAreaLight(const Vector3& origin, float intensity, const Vector3& normal,
			const Vector3& up, float width, float height, const ColorRGB& color);
		Light* AddCircleAreaLight(const Vector3& origin, float intensity, const Vector3& normal,
			const Vector3& up, float radius, const ColorRGB& color);
		dae::Light* AddSphereAreaLight(const Vector3& origin, float intensity, const Vector3& normal, const Vector3& up, float radius, const ColorRGB& color);
		unsigned char AddMaterial(Material* pMaterial);
	};

	//+++++++++++++++++++++++++++++++++++++++++
	//WEEK 1 Test Scene
	class Scene_W1 final : public Scene
	{
	public:
		Scene_W1() = default;
		~Scene_W1() override = default;

		Scene_W1(const Scene_W1&) = delete;
		Scene_W1(Scene_W1&&) noexcept = delete;
		Scene_W1& operator=(const Scene_W1&) = delete;
		Scene_W1& operator=(Scene_W1&&) noexcept = delete;

		void Initialize() override;
	};
	//+++++++++++++++++++++++++++++++++++++++++
	//WEEK 2 Test Scene
	class Scene_W2 final : public Scene
	{
	public:
		Scene_W2() = default;
		~Scene_W2() override = default;

		Scene_W2(const Scene_W2&) = delete;
		Scene_W2(Scene_W2&&) noexcept = delete;
		Scene_W2& operator=(const Scene_W2&) = delete;
		Scene_W2& operator=(Scene_W2&&) noexcept = delete;

		void Initialize() override;
	};
	//+++++++++++++++++++++++++++++++++++++++++
	//WEEK 3 Test Scene
	class Scene_W3 final : public Scene
	{
	public:
		Scene_W3() = default;
		~Scene_W3() override = default;

		Scene_W3(const Scene_W3&) = delete;
		Scene_W3(Scene_W3&&) noexcept = delete;
		Scene_W3& operator=(const Scene_W3&) = delete;
		Scene_W3& operator=(Scene_W3&&) noexcept = delete;

		void Initialize() override;
	};
	//+++++++++++++++++++++++++++++++++++++++++
	// Test Scene
	class Scene_TEST final : public Scene
	{
	public:
		Scene_TEST() = default;
		~Scene_TEST() override = default;

		Scene_TEST(const Scene_W3&) = delete;
		Scene_TEST(Scene_W3&&) noexcept = delete;
		Scene_TEST& operator=(const Scene_W3&) = delete;
		Scene_TEST& operator=(Scene_W3&&) noexcept = delete;

		void Initialize() override;
	};
	//+++++++++++++++++++++++++++++++++++++++++
	//WEEK 4 Test Scene
	class Scene_W4 final : public Scene
	{
	public:
		Scene_W4() = default;
		~Scene_W4() override = default;

		Scene_W4(const Scene_W4&) = delete;
		Scene_W4(Scene_W4&&) noexcept = delete;
		Scene_W4& operator=(const Scene_W4&) = delete;
		Scene_W4& operator=(Scene_W4&&) noexcept = delete;

		void Initialize() override;
		void Update(Timer* pTimer) override;
	private:
		TriangleMesh* pMesh{ nullptr };
	};
	//+++++++++++++++++++++++++++++++++++++++++
	//WEEK 4 Refrence Scene
	class SceneW4_ReferenceScene final : public Scene
	{
	public:
		SceneW4_ReferenceScene() = default;
		~SceneW4_ReferenceScene() override = default;

		SceneW4_ReferenceScene(const SceneW4_ReferenceScene&) = delete;
		SceneW4_ReferenceScene(SceneW4_ReferenceScene&&) noexcept = delete;
		SceneW4_ReferenceScene& operator=(const SceneW4_ReferenceScene&) = delete;
		SceneW4_ReferenceScene& operator=(SceneW4_ReferenceScene&&) noexcept = delete;

		void Initialize() override;
		void Update(Timer* pTimer) override;
	private:
		TriangleMesh* m_Meshes[3] = {};
	};
	//+++++++++++++++++++++++++++++++++++++++++
	//WEEK 4 Bunny Scene
	class Scene_W4_BunnyScene final : public Scene
	{
	public:
		Scene_W4_BunnyScene() = default;
		~Scene_W4_BunnyScene() override = default;

		Scene_W4_BunnyScene(const Scene_W4_BunnyScene&) = delete;
		Scene_W4_BunnyScene(Scene_W4_BunnyScene&&) noexcept = delete;
		Scene_W4_BunnyScene& operator=(const Scene_W4_BunnyScene&) = delete;
		Scene_W4_BunnyScene& operator=(Scene_W4_BunnyScene&&) noexcept = delete;

		void Initialize() override;
		void Update(Timer* pTimer) override;
	private:
		TriangleMesh* pMesh{ nullptr };
	};
	//+++++++++++++++++++++++++++++++++++++++++
	//EXTRA Random Scene
	//Generates sphere with a random color, size and location
	class Scene_Extra_RandomScene final : public Scene
	{
	public:
		Scene_Extra_RandomScene() = default;
		~Scene_Extra_RandomScene() override = default;

		Scene_Extra_RandomScene(const Scene_Extra_RandomScene&) = delete;
		Scene_Extra_RandomScene(Scene_Extra_RandomScene&&) noexcept = delete;
		Scene_Extra_RandomScene& operator=(const Scene_Extra_RandomScene&) = delete;
		Scene_Extra_RandomScene& operator=(Scene_Extra_RandomScene&&) noexcept = delete;

		void Initialize() override;
	};
	class Scene_Extra_AreaLight final : public Scene
	{
	public:
		Scene_Extra_AreaLight() = default;
		~Scene_Extra_AreaLight() override = default;

		Scene_Extra_AreaLight(const Scene_Extra_AreaLight&) = delete;
		Scene_Extra_AreaLight(Scene_Extra_AreaLight&&) noexcept = delete;
		Scene_Extra_AreaLight& operator=(const Scene_Extra_AreaLight&) = delete;
		Scene_Extra_AreaLight& operator=(Scene_Extra_AreaLight&&) noexcept = delete;

		void Initialize() override;
		void Update(Timer* pTimer) override;
	private:
		TriangleMesh* m_pMeshes[3] = {};
	};
}
