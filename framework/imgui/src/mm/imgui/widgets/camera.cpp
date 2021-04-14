#include "./camera.hpp"

#include <imgui/imgui.h>

#include "./entity.hpp"

#include <mm/components/transform2d.hpp>

#include <entt/entity/registry.hpp>

namespace MM::ImGuiWidgets {

void Camera3D(MM::Scene& scene) {
	ImGui::TextUnformatted("Camera:");

	ImGui::Indent();

	auto* camera = scene.try_ctx<MM::OpenGL::Camera3D>();
	if (!camera) {
		ImGui::TextUnformatted("NO CAMERA!");
		return;
	}

	static bool follow_entity = false;
	static MM::Entity tracking = entt::null;

	ImGui::InputFloat("screenRatio", &camera->screenRatio);
	ImGui::InputFloat("nearPlane", &camera->nearPlane);
	ImGui::InputFloat("farPlane", &camera->farPlane);

	if (camera->ortho) {
		ImGui::TextUnformatted("orthographic mode");
		ImGui::Checkbox("follow entity", &follow_entity);

		if (follow_entity) {
			ImGui::SameLine();

			MM::ImGuiWidgets::Entity(tracking, scene);
			if (scene.valid(tracking)) {
				if (scene.all_of<MM::Components::Transform2D>(tracking)) {
					camera->translation = {scene.get<MM::Components::Transform2D>(tracking).position, 0.f};
				} else {
					ImGui::TextUnformatted("error: Entity has no Transform");
				}
			}
		} else {
			ImGui::DragFloat2("translation", &camera->translation.x, 0.1f);
		}


		ImGui::DragFloat("h_vp_size", &camera->horizontalViewPortSize, 0.1f);

		// TODO: aspect ratio

		// TODO: check for change
		camera->setOrthographic();
	} else { // perspective
		ImGui::TextUnformatted("perspective mode");

		ImGui::DragFloat3("translation", &camera->translation.x, 0.1f);
		ImGui::SliderFloat("fov", &camera->fov, 0.1f, glm::pi<float>());

		ImGui::SliderFloat("yaw", &camera->yaw, 0.0f, 2*glm::pi<float>());
		ImGui::SliderFloat("pitch", &camera->pitch, -glm::pi<float>()/2, glm::pi<float>()/2);

		ImGui::InputFloat3("up", &camera->up.x);

		camera->setPerspective();
	}

	camera->updateView();

	ImGui::Unindent();
}

}

