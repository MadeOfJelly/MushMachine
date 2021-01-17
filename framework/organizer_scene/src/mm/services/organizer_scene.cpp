#include "./organizer_scene.hpp"

#include <mm/components/time_delta.hpp>

#include <entt/entity/registry.hpp>
#include <entt/entity/organizer.hpp>

#include <tracy/Tracy.hpp>

#include <mm/logger.hpp>
#include <spdlog/fmt/ostr.h>
#define LOG_OSS(x) LOG("OrganizerSceneService", x)

namespace entt {

// graphviz dot export
static std::ostream& operator<<(std::ostream& out, const std::vector<entt::organizer::vertex>& nodes) {
	out << "digraph EnTT_organizer {\nrankdir=RL;\n";


	for (size_t i = 0; i < nodes.size(); i++) {
		out << "n" << i << "[shape=" << (nodes[i].top_level() ? "doublecircle" : "circle") << " label=\"" << (nodes[i].name() == nullptr ? "NoName" : nodes[i].name()) << "\"];\n";
	}

	for (size_t i = 0; i < nodes.size(); i++) {
		for (const size_t child : nodes[i].children()) {
			out << "n" << child << " -> " << "n" << i << ";\n";
		}
	}

	out << "}";
	return out;
}

} // entt

namespace MM::Services {

bool OrganizerSceneService::enable(Engine& engine) {
	// default scene
	if (!_scene) {
		_scene = std::make_unique<Scene>();
		_scene->set<MM::Engine*>(&engine);
		updateOrganizerVertices(*_scene);
	}

	resetTime();

	return true;
}

void OrganizerSceneService::disable(Engine&) {
}

std::vector<UpdateStrategies::UpdateCreationInfo> OrganizerSceneService::registerUpdates(void) {
	using namespace entt::literals;
	return {
		{
			"OrganizerSceneService::scene_update"_hs,
			"OrganizerSceneService::scene_update",
			[this](Engine& e) { sceneUpdate(e); },
			// depends on imgui
		},
		{
			"OrganizerSceneService::scene_tick"_hs,
			"OrganizerSceneService::scene_tick",
			[this](Engine& e) { sceneFixedUpdate(e); },
			UpdateStrategies::update_phase_t::MAIN,
			true,
			{
				"OrganizerSceneService::scene_update"_hs, // bc it renders imgui, but this is not "hard"
			}
		},
		{
			"OrganizerSceneService::scene_changer"_hs,
			"OrganizerSceneService::scene_changer",
			[this](Engine& e) { changeSceneFixedUpdate(e); },
			UpdateStrategies::update_phase_t::MAIN,
			true,
			{
				"OrganizerSceneService::scene_tick"_hs,
			}
		},
	};
}

void OrganizerSceneService::sceneFixedUpdate(Engine&) {
	ZoneScoped;

	auto newNow = clock::now();
	auto deltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(newNow - _last_time);
	_last_time = newNow;
	_accumulator += deltaTime.count();
	const double dt = f_delta * 1'000'000'000.0;

	size_t continuous_counter = 0;

	auto& time_ctx = _scene->ctx_or_set<MM::Components::TimeDelta>(f_delta, delta_factor);
	time_ctx.tickDelta = f_delta * delta_factor;
	time_ctx.deltaFactor = delta_factor;

	// TODO: this while is just cancer
	while (_accumulator >= static_cast<decltype(_accumulator)>(dt)){
		_accumulator -= static_cast<decltype(_accumulator)>(dt);
		continuous_counter++;

		for (auto&& v : _scene->ctx<std::vector<entt::organizer::vertex>>()) {
			v.callback()(v.data(), *_scene);
		}

		TracyPlot("MM::Services::OrganizerSceneService::_scene.alive", (int64_t)_scene->alive());
	}

	if (continuous_counter > 2) {
		LOG_OSS(fmt::format("had {} contiguous scene ticks!", continuous_counter));
	}
}

void OrganizerSceneService::changeSceneFixedUpdate(Engine& engine) {
	if (_next_scene) {
		LOG_OSS("changing scene...");
		_scene = std::move(_next_scene);
		_scene->set<MM::Engine*>(&engine); // make engine accessible from scene
		updateOrganizerVertices(*_scene);
	}
}

void OrganizerSceneService::sceneUpdate(Engine&) {
	ZoneScoped;

	//if (ImGui::Begin("OrganizerScene")) {
		//ImGui::SliderFloat("time_factor", &delta_factor, 0.f, 10.f, "%.5f", ImGuiSliderFlags_Logarithmic);
	//}
	//ImGui::End();

	//for (auto&& v : _scene->ctx<std::vector<entt::organizer::vertex>>()) {
		//v.callback()(v.data(), *_scene);
	//}

	//TracyPlot("MM::Services::OrganizerSceneService::_scene.alive", (int64_t)_scene->alive());
}

void OrganizerSceneService::changeScene(std::unique_ptr<Scene>&& new_scene) {
	if (_next_scene) {
		LOG_OSS("warn: already next scene enqueued! overwriting...");
	}

	_next_scene = std::move(new_scene);
}

void OrganizerSceneService::changeSceneNow(std::unique_ptr<Scene>&& new_scene) {
	_scene = std::move(new_scene);
	//_scene->set<MM::Engine*>(&engine); // make engine accessible from scene
	updateOrganizerVertices(*_scene);
}

void OrganizerSceneService::updateOrganizerVertices(Scene& scene) {
	scene.ctx_or_set<std::vector<entt::organizer::vertex>>() =
		scene.ctx_or_set<entt::organizer>().graph();

	if (!scene.try_ctx<MM::Components::TimeDelta>()) {
		scene.set<MM::Components::TimeDelta>();
	}

	SPDLOG_DEBUG("graph:\n{}", scene.ctx<std::vector<entt::organizer::vertex>>());
}

void OrganizerSceneService::resetTime(void) {
	_last_time = clock::now();
	_accumulator = 0;
}

} // MM::Services

