#include "./fast_sky_sun_system.hpp"

#include <mm/services/scene_service_interface.hpp>

#include <entt/entity/registry.hpp>

#include <mm/opengl/render_tasks/fast_sky_render_task.hpp>

#include <glm/trigonometric.hpp>

namespace MM::Systems {

void FastSkySun(Scene& scene, float delta) {
	auto& sky_ctx = scene.ctx<MM::OpenGL::RenderTasks::FastSkyContext>();

	sky_ctx.time += delta * 0.2f;

	sky_ctx.fsun.y = glm::sin(sky_ctx.time * 0.01f);
	sky_ctx.fsun.z = glm::cos(sky_ctx.time * 0.01f);
}

} // MM::Systems

