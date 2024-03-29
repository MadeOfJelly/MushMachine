#pragma once

#include <glm/mat4x4.hpp>
#include <glm/gtc/constants.hpp>

#include <array>

namespace MM::OpenGL {

	class Camera3D {
		private:
			glm::mat4 _view{1}, _projection{1};

		public:
			glm::vec3	translation				{0.f, 0.f, 0.f};
			float		screenRatio				{16.f / 9.f};
			float		nearPlane				{0.1f};
			float		farPlane				{1000.f};

			// for orth and pers
			float		roll					{0.f};					// in rad

			bool		ortho					{true};

			// for orthogonal
			float		horizontalViewPortSize	{100.f}; // 100 meters

			// for perspective (right handed, z up)
			float 		fov						{glm::half_pi<float>()};	// in rad
			float		yaw						{0.f};					// in rad
			float		pitch					{0.f};					// in rad
			glm::vec3	up						{0.f, 0.f, 1.f};		// normalize!

		public:
			Camera3D(void);

			// call this if ortho and horizontalViewPortSize or screenRatio changes
			void setOrthographic(void);

			// call this if not ortho and fov or screenRation changes
			void setPerspective(void);

			// call this after changing translation
			void updateView(void);

			glm::mat4 getViewProjection(void) const;
			glm::mat4 getView(void) const;
			glm::mat4 getProjection(void) const;

			glm::vec3 getViewDir(void) const;

			// call updateView beforehand, does not cache
			std::array<glm::vec4, 6> getFrustumPlanes(void) const;

			// uses getViewProjection(), does not cache
			// screen has to be in NDC, xy -> [-1, 1]
			glm::vec4 screenToWorld(const glm::vec4& screen) const;
	};

	// test if aabb in frustum
	// aabb is in world space
	bool in_frustum_aabb(
		const std::array<glm::vec4, 6>& frustum,
		const std::array<glm::vec3, 8>& aabb
	);

	// TODO: implement
	//// test if sphere in frustum
	//bool in_frustum_sphere(
		//const std::array<glm::vec4, 6>& frustum,
		//const glm::vec3 position, const float radius
	//);

} // MM::OpenGL

