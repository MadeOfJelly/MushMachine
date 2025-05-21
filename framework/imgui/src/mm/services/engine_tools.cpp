#include "./engine_tools.hpp"

#include <entt/config/version.h>

#include <mm/engine.hpp>

#include <mm/update_strategies/update_strategy.hpp>

#include <list>

#include <imgui/imgui.h>

namespace MM::Services {

using UpdateStrategies::update_key_t;

template<typename NColFn, typename LColFn, typename TTFn>
static void renderUpdateStratGraph(
	const std::unordered_map<update_key_t, std::set<update_key_t>>& g,
	std::set<update_key_t>& a,
	NColFn&& node_color_fn,
	LColFn&& line_color_fn,
	TTFn&& tooltip_fn
) {
	using UpdateStrategies::update_key_t;

	std::set<update_key_t> work_queue{a.begin(), a.end()};

	std::vector<std::vector<update_key_t>> node_table{};
	node_table.push_back({});

	std::unordered_map<update_key_t, std::pair<size_t, size_t>> node_table_pos;

	// THX to the schwammal

	{ // phase one, extract dep less nodes
		for (auto it = work_queue.begin(); it != work_queue.end();) {
			bool no_deps = g.at(*it).empty();

			if (!no_deps) {
				for (auto dep : g.at(*it)) {
					if (a.count(dep)) {
						no_deps = false;
						break;
					}
				}
			}

			if (no_deps) {
				node_table_pos[*it] = {0, node_table[0].size()};
				node_table[0].emplace_back(*it);
				it = work_queue.erase(it);
			} else {
				it++;
			}
		}
	}

	{ // phase two, do rest
		size_t curr_col = 0;
		while (!work_queue.empty()) {
			curr_col++;
			node_table.push_back({});

			std::vector<update_key_t> nodes_this_round{};
			for (auto it = work_queue.begin(); it != work_queue.end();) {
				bool deps_resolved = true;
				const auto& it_deps = g.at(*it);

				for (const update_key_t dep : it_deps) {
					if (a.count(dep) && work_queue.count(dep)) {
						deps_resolved = false;
						break;
					}
				}

				if (deps_resolved) {
					//node_table_pos[*it] = {curr_col, node_table[curr_col].size()};
					//node_table[curr_col].emplace_back(*it);
					//it = work_queue.erase(it);
					nodes_this_round.push_back(*it);
				} else {
					//it++;
				}
				it++;
			}

			// apply nodes
			for (auto it : nodes_this_round) {
				node_table_pos[it] = {curr_col, node_table[curr_col].size()};
				node_table[curr_col].emplace_back(it);
				work_queue.erase(it);
			}
		}
	}

	{ // display
		//ImGui::BeginChild("canvas", {0, 0}, true);

		const ImVec2 cp = ImGui::GetCursorScreenPos();
		const auto max_cont = ImGui::GetContentRegionAvail();
		ImGui::Dummy(max_cont);

		auto* dl = ImGui::GetWindowDrawList();
		const float circle_radius = 20.f;
		const float circle_padding = 40.f;

		// lines
		for (const update_key_t node : a) {
			for (const update_key_t dep : g.at(node)) {
				if (!a.count(dep)) {
					continue;
				}

				ImVec2 p_start{
					cp.x + node_table_pos[node].first*(2*circle_radius+circle_padding) + circle_radius,
					cp.y + node_table_pos[node].second*(2*circle_radius+circle_padding) + circle_radius,
				};

				ImVec2 p_end{
					cp.x + node_table_pos[dep].first*(2*circle_radius+circle_padding) + circle_radius,
					cp.y + node_table_pos[dep].second*(2*circle_radius+circle_padding) + circle_radius,
				};

				dl->AddLine(p_start, p_end, ImGui::ColorConvertFloat4ToU32(line_color_fn(node, dep)));
			}
		}

		// nodes
		for (size_t curr_col = 0; curr_col < node_table.size(); curr_col++) {
			const auto& curr_table_col = node_table[curr_col];
			for (size_t curr_row = 0; curr_row < curr_table_col.size(); curr_row++) {
				ImVec2 p{
					cp.x + curr_col*(2*circle_radius+circle_padding) + circle_radius,
					cp.y + curr_row*(2*circle_radius+circle_padding) + circle_radius,
				};

				//dl->AddCircleFilled(p, circle_radius, IM_COL32(100, 100, 100, 100));
				auto col_vec = node_color_fn(curr_table_col.at(curr_row));
				ImVec4 col_vec_bg = col_vec;
				col_vec_bg.w *= 0.2f;

				dl->AddCircleFilled(p, circle_radius, ImGui::ColorConvertFloat4ToU32(col_vec_bg));
				dl->AddCircle(p, circle_radius, ImGui::ColorConvertFloat4ToU32(col_vec));

				if (ImGui::IsMouseHoveringRect(
					{p.x - circle_radius, p.y - circle_radius},
					{p.x + circle_radius, p.y + circle_radius}
				)) {
					//ImGui::SetTooltip("'%s'\n[%u]", , curr_table_col.at(curr_row));
					tooltip_fn(curr_table_col.at(curr_row));
				}
			}
		}

		//ImGui::EndChild();
	}
}

bool ImGuiEngineTools::enable(Engine& engine, std::vector<UpdateStrategies::TaskInfo>& task_array) {
	auto& menu_bar = engine.getService<MM::Services::ImGuiMenuBar>();

	// use underscore hack to make it last
	menu_bar.menu_tree["Engine"]["_Stop Engine"] = [](Engine& e) {
		ImGui::Separator();
		if (ImGui::MenuItem("Stop Engine")) {
			e.stop();
		}
	};

	menu_bar.menu_tree["Engine"]["About"] = [this](Engine&) {
		ImGui::MenuItem("About", NULL, &_show_about);
	};

	menu_bar.menu_tree["Engine"]["Services"] = [this](Engine&) {
		ImGui::MenuItem("Services", NULL, &_show_services);
	};

	menu_bar.menu_tree["Engine"]["UpdateStrategy"] = [this](Engine&) {
		ImGui::MenuItem("UpdateStrategy", NULL, &_show_update_stategy);
	};

	// add task
	task_array.push_back(
		UpdateStrategies::TaskInfo{"ImGuiEngineTools::render"}
		.fn([this](Engine& e){ renderImGui(e); })
		.phase(UpdateStrategies::update_phase_t::PRE)
		.succeed("ImGuiMenuBar::render")
	);

	return true;
}

void ImGuiEngineTools::disable(Engine& engine) {
	auto& menu_bar = engine.getService<MM::Services::ImGuiMenuBar>();

	menu_bar.menu_tree["Engine"].erase("_Stop Engine");
	menu_bar.menu_tree["Engine"].erase("About");
	menu_bar.menu_tree["Engine"].erase("Services");
	menu_bar.menu_tree["Engine"].erase("UpdateStrategy");
}

void ImGuiEngineTools::renderImGui(Engine& engine) {
	if (_show_about) {
		renderAbout(engine);
	}

	if (_show_services) {
		renderServices(engine);
	}

	if (_show_update_stategy) {
		renderUpdateStrategy(engine);
	}
}

void ImGuiEngineTools::renderAbout(Engine& engine) {
	if (ImGui::Begin("About##EngineTools", &_show_about)) {
		ImGui::Text("TODO");
		ImGui::Text("MushMachine");
		ImGui::Text("UpdateStrategy: %s", engine.getUpdateStrategy().name());
		ImGui::Text("EnTT v%d.%d.%d", ENTT_VERSION_MAJOR, ENTT_VERSION_MINOR, ENTT_VERSION_PATCH);
	}
	ImGui::End();
}

void ImGuiEngineTools::renderServices(Engine& engine) {
	if (ImGui::Begin("Services##EngineTools", &_show_services)) {
		ImGui::Text("TODO: make sortable");
		ImGui::Checkbox("edit mode", &_services_edit_mode);

		if (ImGui::BeginTable(
			"services_table",
			3,
			ImGuiTableFlags_RowBg //|
		)) {
			ImGui::TableSetupColumn("id", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("name");
			ImGui::TableSetupColumn("status");
			ImGui::TableHeadersRow();

			for (auto& it : engine._services) {
				ImGui::TableNextRow();

				ImGui::TableNextColumn();
				ImGui::Text("%u", it.first);

				ImGui::TableNextColumn();
				ImGui::Text("%s", it.second->second->name());

				ImGui::TableNextColumn();
				if (_services_edit_mode) {
					ImGui::PushID(it.first);
					if (ImGui::SmallButton(it.second->first ? "enabled" : "disabled")) {
						if (it.second->first) {
							engine.disableService(it.first);
						} else {
							engine.enableService(it.first);
						}
					}
					//ImGui::SetTooltip("click to toggle!"); the heck?
					ImGui::PopID();
				} else {
					ImGui::Text("%s", it.second->first ? "enabled" : "disabled");
				}
			}

		ImGui::EndTable();
		}
	}
	ImGui::End();
}

void ImGuiEngineTools::renderUpdateStrategy(Engine& engine) {
	if (ImGui::Begin("UpdateStrategy##EngineTools", &_show_update_stategy, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
		auto& us = engine.getUpdateStrategy();
		auto us_name = us.name();

		ImGui::Text("UpdateStrategy: '%s'", us_name);

		// TODO: tabs? dropdown(combo)?
		static const char* const phase_str[4] = {
			"All",
			"Pre",
			"Main",
			"Post",
		};
		static int curr_phase = 2;

		ImGui::Combo("Phase", &curr_phase, phase_str, 4);

		if (ImGui::BeginTabBar("tabs")) {
			if (curr_phase != 0 && ImGui::BeginTabItem("Graph")) {
				// BRUHHH this is needs to be cached
				std::unordered_map<update_key_t, std::set<update_key_t>> graph;
				std::unordered_map<update_key_t, UpdateStrategies::TaskInfo*> task_lut;
				std::set<update_key_t> nodes;

				// build lut
				us.forEachTask([&graph, &nodes, &task_lut](UpdateStrategies::TaskInfo& task) -> bool {
					if (task._phase != UpdateStrategies::update_phase_t(curr_phase-1)) {
						return true; // skip, not our phase
					}

					graph[task._key] = task._dependencies;
					task_lut[task._key] = &task;
					nodes.emplace(task._key);

					return true;
				});

				// also do dependents // ugh, 2*N
				us.forEachTask([&graph](UpdateStrategies::TaskInfo& task) -> bool {
					if (task._phase != UpdateStrategies::update_phase_t(curr_phase-1)) {
						return true; // skip, not our phase
					}

					for (const auto it : task._dependents) {
						graph[it].emplace(task._key);
					}

					return true;
				});

				// TODO: make sense of the colloring
				renderUpdateStratGraph(graph, nodes,
					[](const update_key_t key) -> ImVec4 {
						return {0.9f, 1.f, 0.9f, 1.f};
					},
					[](const update_key_t from, const update_key_t to) -> ImVec4 {
						return {0.9f, 1.f, 0.9f, 1.f};
					},
					[&task_lut](update_key_t key) {
						ImGui::SetTooltip("%s\n[%u]", task_lut.at(key)->_name.c_str(), key);
					}
				);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("List")) {
				if (ImGui::BeginTable("table", curr_phase == 0 ? 3 : 2)) {
					us.forEachTask([](UpdateStrategies::TaskInfo& task) -> bool {
						if (
							curr_phase == 0 ||
							task._phase == UpdateStrategies::update_phase_t(curr_phase-1)
						) {

							ImGui::TableNextRow();

							ImGui::TableNextColumn();
							ImGui::Text("%u", task._key);

							if (curr_phase == 0) {
								ImGui::TableNextColumn();
								ImGui::Text("%s", phase_str[task._phase + 1]);
							}

							ImGui::TableNextColumn();
							ImGui::Text("%s", task._name.c_str());
						}

						return true;
					});
					ImGui::EndTable();
				}
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
#if 0
			auto& g = us_default->getGraph(UpdateStrategies::update_phase_t::MAIN);
			auto& a = us_default->getActiveSet(UpdateStrategies::update_phase_t::MAIN);
			std::set<update_key_t> nodes;
			for (const auto& it : g) {
				nodes.emplace(it.first);
			}

			if (ImGui::BeginTabBar("tabs")) {
				if (ImGui::BeginTabItem("table")) {
					if (ImGui::BeginTable("table", 2)) {
						for (const auto it : nodes) {
							ImGui::TableNextRow();

							ImGui::TableNextColumn();
							ImGui::Text("%u", it);

							ImGui::TableNextColumn();
							ImGui::Text("%s", us_default->_tasks.at(it)._name.c_str());
						}
						ImGui::EndTable();
					}
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("graph")) {
					renderUpdateStratGraph(g, nodes,
					[&a](const update_key_t key) -> ImVec4 {
						if (a.count(key)) {
							return {0.9f, 1.f, 0.9f, 1.f};
						} else {
							return {0.4f, 0.2f, 0.2f, 1.f};
						}
					},
					[&a](const update_key_t from, const update_key_t to) -> ImVec4 {
						if (a.count(from) && a.count(to)) {
							return {0.9f, 1.f, 0.9f, 1.f};
						} else {
							return {0.4f, 0.2f, 0.2f, 1.f};
						}
					},
					[us_default](update_key_t key) {
						ImGui::SetTooltip("'%s'\n[%u]", us_default->_tasks.at(key)._name.c_str(), key);
					});
					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}
		}
#endif
	}
	ImGui::End();
}

} // namespace MM::Services

