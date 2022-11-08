#pragma once
#include <cassert>
#include <fstream>
#include "Math.h"
#include "DataTypes.h"

namespace dae
{
	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			hitRecord.didHit = false;

			const Vector3 rayToSphere{ray.origin - sphere.origin};
			const float A{ Vector3::Dot(ray.direction, ray.direction) };
			const float B{ Vector3::Dot((ray.direction*2), rayToSphere)};
			const float C{ Vector3::Dot(rayToSphere, rayToSphere) - sphere.radius * sphere.radius };
			const float discriminant{ (B * B) - (4 * A * C) };

			if (discriminant < 0.f)
			{
				return false;
			}
			else
			{
				const float sqrt_d = sqrt(discriminant);
				float t = (-B - sqrt_d) / (2 * A);
				if(t < ray.min)
				{
					t = (-B + sqrt_d) / (2 * A);
				}

				if (t <= ray.max && t >= ray.min)
				{
					hitRecord.origin = ray.origin + t * ray.direction;
					hitRecord.t = t;
					hitRecord.materialIndex = sphere.materialIndex;
					hitRecord.normal = Vector3{ hitRecord.origin - sphere.origin }.Normalized();
					hitRecord.didHit = true;
					return true;
				}
			}
			return false;
		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
		}
#pragma endregion
#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			hitRecord.didHit = false;

			const float t = Vector3::Dot((plane.origin - ray.origin), plane.normal) / Vector3::Dot(ray.direction, plane.normal);
			if (t > ray.min && t < ray.max && t < hitRecord.t)
			{
				Vector3 normal = plane.normal;
				normal.Normalize();
				hitRecord.origin = ray.origin + t * ray.direction;
				hitRecord.t = t;
				hitRecord.materialIndex = plane.materialIndex;
				hitRecord.normal = normal;
				hitRecord.didHit = true;
				return true;
			}
			return false;
		}

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
		}
#pragma endregion
#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			const Vector3 a{ triangle.v1 - triangle.v0 };
			const Vector3 b{ triangle.v2 - triangle.v0 };
			
			const Vector3 normal = Vector3::Cross(a, b).Normalized();
			const float dot{ Vector3::Dot(normal, ray.direction) };
			if (dot == 0) return false;
			
			const Vector3 center = Vector3{ (Vector3{triangle.v0} + Vector3{triangle.v1} + Vector3{triangle.v2}) * 0.333f };
			const Vector3 L = center - ray.origin;
			
			const float t{ Vector3::Dot(L, normal) / Vector3::Dot(ray.direction, normal) };
			
			if (t < ray.min || t > ray.max)
			{
				return false;
			}
			
			const Vector3 p = ray.origin + t * ray.direction;
			const Vector3 edgeA = triangle.v1 - triangle.v0;
			Vector3 pointToSide = p - triangle.v0;
			if (Vector3::Dot(normal, Vector3::Cross(edgeA, pointToSide)) < 0.f) 
			{
				return false;
			}
			
			const Vector3 edgeB = triangle.v2 - triangle.v1 ;
			pointToSide = p - triangle.v1;
			if (Vector3::Dot(normal, Vector3::Cross(edgeB, pointToSide)) < 0.0f) {
				return false;
			}
			
			const Vector3 edgeC = triangle.v0 - triangle.v2;
			pointToSide = p - triangle.v2;
			if (Vector3::Dot(normal, Vector3::Cross(edgeC, pointToSide)) < 0.0f) {
				return false;
			}
			
			switch (triangle.cullMode) {
			case TriangleCullMode::NoCulling:
				break;
			case TriangleCullMode::BackFaceCulling:
				if (dot > 0.f) return false;
				break;
			case TriangleCullMode::FrontFaceCulling:
				if (dot < 0.f) return false;
				break;
			}
			
			hitRecord.origin = ray.origin + t * ray.direction;
			hitRecord.t = t;
			hitRecord.materialIndex = triangle.materialIndex;
			hitRecord.normal = normal;
			hitRecord.didHit = true;
			return true;
		}

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
		}
#pragma endregion
#pragma region TriangeMesh HitTest
		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//	//todo W5
			//	assert(false && "No Implemented Yet!");

			for (int i = 0; i < mesh.indices.size(); i += 3)
			{
				Triangle triangle{ mesh.transformedPositions[mesh.indices[i]], mesh.transformedPositions[mesh.indices[i + 1]], mesh.transformedPositions[mesh.indices[i + 2]], mesh.transformedNormals[i/3]};
				triangle.cullMode = mesh.cullMode;
				triangle.materialIndex = mesh.materialIndex;

				if (HitTest_Triangle(triangle, ray, hitRecord))
				{
					return true;
				}
			}

			return false;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
		}
#pragma endregion
	}

	namespace LightUtils
	{
		//Direction from target to light
		inline Vector3 GetDirectionToLight(const Light& light, const Vector3 origin)
		{
			return Vector3{ light.origin - origin };
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			return {light.color * (light.intensity / (light.origin - target).SqrMagnitude())};
		}
	}

	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vector3>& positions, std::vector<Vector3>& normals, std::vector<int>& indices)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				file >> sCommand;
				//use conditional statements to process the different commands	
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					//Vertex
					float x, y, z;
					file >> x >> y >> z;
					positions.push_back({ x, y, z });
				}
				else if (sCommand == "f")
				{
					float i0, i1, i2;
					file >> i0 >> i1 >> i2;

					indices.push_back((int)i0 - 1);
					indices.push_back((int)i1 - 1);
					indices.push_back((int)i2 - 1);
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');

				if (file.eof()) 
					break;
			}

			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				Vector3 edgeV0V1 = positions[i1] - positions[i0];
				Vector3 edgeV0V2 = positions[i2] - positions[i0];
				Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

				if(isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (isnan(normal.x))
				{
					int k = 0;
				}

				normals.push_back(normal);
			}

			return true;
		}
#pragma warning(pop)
	}
}