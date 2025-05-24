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
		for (const size_t child : nodes[i].in_edges()) {
			out << "n" << child << " -> " << "n" << i << ";\n";
		}
	}

	out << "}";
	return out;
}

} // entt

template <> struct fmt::formatter<std::vector<entt::organizer::vertex>> : fmt::ostream_formatter {};

namespace MM::Services {

bool OrganizerSceneService::enable(Engine& engine, std::vector<UpdateStrategies::TaskInfo>& task_array) {
	// add tasks
	task_array.push_back(
		UpdateStrategies::TaskInfo{"OrganizerSceneService::scene_tick"}
		.fn([this](Engine& e) { sceneFixedUpdate(e); })
	);
	task_array.push_back(
		UpdateStrategies::TaskInfo{"OrganizerSceneService::scene_changer"}
		.fn([this](Engine& e) { changeSceneFixedUpdate(e); })
		.succeed("OrganizerSceneService::scene_tick")
	);

	// default scene
	if (!_scene) {
		_scene = std::make_unique<Scene>();
		_scene->ctx().emplace<MM::Engine&>(engine);
		updateOrganizerVertices(*_scene);
	}

	resetTime();

	return true;
}

void OrganizerSceneService::disable(Engine&) {
}

void OrganizerSceneService::sceneFixedUpdate(Engine&) {
	ZoneScoped;

	auto newNow = clock::now();
	std::chrono::duration<double, std::ratio<1, 1>> deltaTime = newNow - _last_time;
	_last_time = newNow;
	_accumulator += deltaTime.count();

	size_t continuous_counter = 0;

	auto& time_ctx = _scene->ctx().emplace<MM::Components::TimeDelta>(f_delta, initial_delta_factor);
	time_ctx.tickDelta = f_delta * time_ctx.deltaFactor;

	while (_accumulator >= f_delta){
		_accumulator -= f_delta;
		continuous_counter++;

		for (auto&& v : _scene->ctx().get<std::vector<entt::organizer::vertex>>()) {
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
		if (!_scene->ctx().contains<MM::Engine>()) {
			_scene->ctx().emplace<MM::Engine&>(engine); // make engine accessible from scene
		}
		updateOrganizerVertices(*_scene);
	}
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
	const auto& graph = scene.ctx().emplace<std::vector<entt::organizer::vertex>>() =
		scene.ctx().emplace<entt::organizer>().graph();

	if (!scene.ctx().contains<MM::Components::TimeDelta>()) {
		scene.ctx().emplace<MM::Components::TimeDelta>();
	}

	// TODO: use entt::dot instead
	SPDLOG_DEBUG("graph:\n{}", graph);
}

void OrganizerSceneService::resetTime(void) {
	_last_time = clock::now();
	_accumulator = 0.0;
}

} // MM::Services

