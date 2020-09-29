#pragma once

#include <mm/engine_fwd.hpp>

#include <mm/services/filesystem.hpp>
#include <mm/opengl/spritesheet.hpp>

#include <mm/opengl/render_tasks/tilemap_renderable.hpp>

namespace MM {

// TODO: test, clear asserts
class Tilemap {
	protected:
		struct SpriteSheetEntry {
			uint32_t first_id;
			uint32_t last_id;
			MM::OpenGL::SpriteSheet sp;
		};
		std::vector<SpriteSheetEntry> _sprite_sheet_map; // used to map tileids to the spritesheet ids

		// tiles
		uint32_t _width = 0;
		uint32_t _height = 0;

	public:
		std::string map_file_path;

	public:
		inline uint32_t getWidth(void) {
			return _width;
		}

		inline uint32_t getHeight(void) {
			return _height;
		}

	public:
		Tilemap(void);
		Tilemap(const Tilemap&) = delete;
		virtual ~Tilemap(void) = default;

		bool parseTiled_Map(MM::Engine& engine, MM::Scene& scene, MM::Services::FilesystemService::fs_file_t file);
		bool parseTiled_Map(MM::Engine& engine, MM::Scene& scene, std::string_view path);

		virtual bool parseTiled_LayerCollision(MM::Engine& engine, MM::Scene& scene, nlohmann::json& jlayer, entt::entity e);
		virtual bool parseTiled_LayerRenderable(MM::Engine& engine, MM::Scene& scene, nlohmann::json& jlayer, entt::entity e);
		virtual bool parseTiled_ObjectLayer(MM::Engine& engine, MM::Scene& scene, nlohmann::json& jlayer);

		bool parseTiled_Layer(MM::Engine& engine, MM::Scene& scene, nlohmann::json& jlayer);
		bool parseTiled_Layers(MM::Engine& engine, MM::Scene& scene, nlohmann::json& jlayers);

		bool parseTiled_Tileset(MM::Engine& engine, std::string_view src_path, nlohmann::json& jts, uint32_t first_g_id);
		// populate sprite_sheet_map and load textures
		bool parseTiled_Tilesets(MM::Engine& engine, nlohmann::json& jtss);

	protected:
		std::pair<uint32_t, OpenGL::TilemapRenderable::SpriteLayer&> _get_sid_and_layer(uint32_t gid, OpenGL::TilemapRenderable& tilemap_rend);

		nlohmann::json Tiled_get_custom_prop(nlohmann::json& j, std::string_view name, std::string_view type);
		bool Tiled_is_collision_layer(nlohmann::json& jlayer);
		float Tiled_get_z_prop(nlohmann::json& jlayer);
};

} // MM

