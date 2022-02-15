#include "./camera_3d.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <tracy/Tracy.hpp>

namespace MM::OpenGL {

Camera3D::Camera3D(void) {
	updateView();
}

void Camera3D::setOrthographic(void) {
	ortho = true;
	_projection = glm::ortho(
		-horizontalViewPortSize / 2,
		horizontalViewPortSize / 2,
		-horizontalViewPortSize / (screenRatio * 2),
		horizontalViewPortSize / (screenRatio * 2),
		nearPlane,
		farPlane
	);
}

void Camera3D::setPerspective(void) {
	ortho = false;
	_projection = glm::perspective(fov, screenRatio, nearPlane, farPlane);
}

void Camera3D::updateView(void) {
	roll = glm::mod(roll, 2*glm::pi<float>());

	if (ortho) {
		_view = glm::mat4(1);

		_view = glm::rotate(_view, roll, glm::vec3{0.f, 0.f, 1.f});

		auto tmp_trans  = glm::mat4(1);
		tmp_trans[3] = glm::vec4(-translation.x, -translation.y, 0.f, 1.f);
		_view = _view * tmp_trans;
		//_view[3] = glm::vec4(-translation.x, -translation.y, 0.f, 1.f);
		////_view[3] = glm::vec4(-translation.x, -translation.y, 1.0f, 1.f);
		//_view = glm::translate(_view, glm::vec3{-translation.x, -translation.y, -1.f});

		//_view = glm::inverse(_view);

		_view[2][2] = -1.f;

		// TODO: please fix me
	} else {
		pitch = glm::clamp(pitch, -(glm::pi<float>()/2 - 0.00001f), glm::pi<float>()/2 - 0.00001f);
		yaw = glm::mod(yaw, 2*glm::pi<float>());

		glm::vec3 front = getViewDir();

		_view = glm::lookAt(translation, translation + front, up);
	}
}

glm::mat4 Camera3D::getViewProjection() const {
	return _projection * _view;
}

glm::mat4 Camera3D::getView() const {
	return _view;
}

glm::mat4 Camera3D::getProjection() const {
	return _projection;
}

glm::vec3 Camera3D::getViewDir(void) const {
	glm::vec3 front {0,0,0};
	{ // TODO: optimize
#if 0
		// if y up/down
		front.x += up.y * glm::cos(pitch) * glm::cos(-yaw); // TODO: y is yaw inverted??
		front.y += up.y * glm::sin(pitch);
		front.z += up.y * glm::cos(pitch) * glm::sin(-yaw);
#else
		// if z up/down
		front.x += up.z * glm::cos(pitch) * glm::cos(yaw);
		front.y += up.z * glm::cos(pitch) * glm::sin(yaw);
		front.z += up.z * glm::sin(pitch);
#endif
	}

	front = glm::normalize(front);

	return front;
}

std::array<glm::vec4, 6> Camera3D::getFrustumPlanes(void) const {
	ZoneScopedN("Camera3D::getFrustumPlanes")

	std::array<glm::vec4, 6> planes;

	glm::mat4 wvp = getViewProjection();

	// see:
	// https://stackoverflow.com/questions/11770262/glm-calculating-the-frustum-from-the-projection-matrix
	// http://web.archive.org/web/20120531231005/http://crazyjoke.free.fr/doc/3D/plane%20extraction.pdf

#ifdef LEFT_HANDED
	// left handed
	{
		// Left clipping plane
		planes[0].x = wvp[0][3] + wvp[0][0];
		planes[0].y = wvp[1][3] + wvp[1][0];
		planes[0].z = wvp[2][3] + wvp[2][0];
		planes[0].w = wvp[3][3] + wvp[3][0];

		// Right clipping plane
		planes[1].x = wvp[0][3] - wvp[0][0];
		planes[1].y = wvp[1][3] - wvp[1][0];
		planes[1].z = wvp[2][3] - wvp[2][0];
		planes[1].w = wvp[3][3] - wvp[3][0];

		// Top clipping
		planes[2].x = wvp[0][3] - wvp[0][1];
		planes[2].y = wvp[1][3] - wvp[1][1];
		planes[2].z = wvp[2][3] - wvp[2][1];
		planes[2].w = wvp[3][3] - wvp[3][1];

		// Bottom clipping plane
		planes[3].x = wvp[0][3] + wvp[0][1];
		planes[3].y = wvp[1][3] + wvp[1][1];
		planes[3].z = wvp[2][3] + wvp[2][1];
		planes[3].w = wvp[3][3] + wvp[3][1];

		// Near clipping plane
		planes[4].x = wvp[0][2];
		planes[4].y = wvp[1][2];
		planes[4].z = wvp[2][2];
		planes[4].w = wvp[3][2];

		// Far clipping
		planes[5].x = wvp[0][3] - wvp[0][2];
		planes[5].y = wvp[1][3] - wvp[1][2];
		planes[5].z = wvp[2][3] - wvp[2][2];
		planes[5].w = wvp[3][3] - wvp[3][2];
	}
#else
	// right handed
	{
		// Left clipping plane
		planes[0].x = wvp[0][3] + wvp[0][0];
		planes[0].y = wvp[1][3] + wvp[1][0];
		planes[0].z = wvp[2][3] + wvp[2][0];
		planes[0].w = wvp[3][3] + wvp[3][0];

		// Right clipping plane
		planes[1].x = wvp[0][3] - wvp[0][0];
		planes[1].y = wvp[1][3] - wvp[1][0];
		planes[1].z = wvp[2][3] - wvp[2][0];
		planes[1].w = wvp[3][3] - wvp[3][0];

		// Top clipping plane
		planes[2].x = wvp[0][3] - wvp[0][1];
		planes[2].y = wvp[1][3] - wvp[1][1];
		planes[2].z = wvp[2][3] - wvp[2][1];
		planes[2].w = wvp[3][3] - wvp[3][1];

		// Bottom clipping plane
		planes[3].x = wvp[0][3] + wvp[0][1];
		planes[3].y = wvp[1][3] + wvp[1][1];
		planes[3].z = wvp[2][3] + wvp[2][1];
		planes[3].w = wvp[3][3] + wvp[3][1];

		// Near clipping plane
		planes[4].x = wvp[0][3] + wvp[0][2];
		planes[4].y = wvp[1][3] + wvp[1][2];
		planes[4].z = wvp[2][3] + wvp[2][2];
		planes[4].w = wvp[3][3] + wvp[3][2];

		// Far clipping plane
		planes[5].x = wvp[0][3] - wvp[0][2];
		planes[5].y = wvp[1][3] - wvp[1][2];
		planes[5].z = wvp[2][3] - wvp[2][2];
		planes[5].w = wvp[3][3] - wvp[3][2];
	}
#endif

	// normalize
	for (size_t i = 0; i < 6; i++) {
		planes[i] = glm::normalize(planes[i]);
	}

	return planes;
}

// example use:
// const glm::vec4 screen {
	// (
		// glm::vec2{mouse_pos.x, window_size.y - mouse_pos.y}
		// / (glm::vec2{window_size.x, window_size.y} * 0.5f)
	// )
	// - glm::vec2{1.f, 1.f}
	// , 1.f, 1.f
// };
glm::vec4 Camera3D::screenToWorld(const glm::vec4& screen) const {
	return glm::inverse(getViewProjection()) * screen;
}

template<typename T>
static T dot34(glm::vec<3, T> lhs, glm::vec<4, T> rhs) {
	return glm::dot(lhs, glm::vec<3, T>(rhs)) + rhs.w;
}

bool in_frustum_aabb(
	const std::array<glm::vec4, 6>& frustum,
	const std::array<glm::vec3, 8>& aabb
) {
	// test each plane
	for (int i = 0; i < 6; i++) {
		bool inside = false; // inside current plane

		for (int j = 0; j < 8; j++) {
			if (dot34(aabb[j], frustum[i]) >= 0) { // == for on plane
				inside = true;
				break;
			}
		}

		if (!inside) {
			return false;
		}
	}

	return true;
}

} // MM::Rendering

