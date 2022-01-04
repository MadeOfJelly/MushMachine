#include "./transform.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <tracy/Tracy.hpp>

namespace MM::Systems {

void position3d_from_2d(entt::view<entt::get_t<const Components::DirtyTransformTag, const Components::Position2D, const Components::Position2D_ZOffset, Components::Position3D>> view) {
	ZoneScoped;
	view.each([](const Components::Position2D& pos2, const Components::Position2D_ZOffset& zoff, Components::Position3D& pos3) {
		pos3.pos = glm::vec3(pos2.pos, zoff.z_offset + pos2.pos.y/10.f);
	});
}

void transform3d_translate(entt::view<entt::get_t<const Components::DirtyTransformTag, const Components::Position3D, Components::Transform4x4>> view) {
	ZoneScoped;
	view.each([](const Components::Position3D& pos, Components::Transform4x4& trans) {
		trans.trans = glm::translate(glm::mat4x4{1.f}, pos.pos);
	});
}

void transform3d_rotate2d(entt::view<entt::get_t<const Components::DirtyTransformTag, const Components::Rotation2D, Components::Transform4x4>> view) {
	ZoneScoped;
	view.each([](const Components::Rotation2D& rot, Components::Transform4x4& trans) {
		trans.trans = glm::rotate(trans.trans, rot.rot, glm::vec3(0.f, 0.f, 1.f));
	});
}

void transform3d_scale2d(entt::view<entt::get_t<const Components::DirtyTransformTag, const Components::Scale2D, Components::Transform4x4>> view) {
	ZoneScoped;
	view.each([](const Components::Scale2D& scale, Components::Transform4x4& trans) {
		trans.trans = glm::scale(trans.trans, glm::vec3(scale.scale, 1.f));
	});
}

void transform_clear_dirty(entt::registry& scene, entt::view<entt::get_t<Components::DirtyTransformTag>> view) {
	ZoneScoped;
	scene.remove<Components::DirtyTransformTag>(view.begin(), view.end());
}

} // MM::Systems

