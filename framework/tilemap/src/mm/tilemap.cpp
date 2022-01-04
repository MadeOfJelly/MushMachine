#include "./tilemap.hpp"

#include <mm/path_utils.hpp>
#include <mm/opengl/texture_loader.hpp>

#include <mm/opengl/spritesheet.hpp>

#include <mm/services/scene_service_interface.hpp>
#include <entt/entity/registry.hpp>

#include <mm/components/position2d.hpp>
#include <mm/components/position2d_zoffset.hpp>
#include <mm/components/transform4x4.hpp>
#include <mm/components/name.hpp>

#include <nlohmann/json.hpp>

#include <glm/common.hpp>

#include <mm/logger.hpp>
#define LOG_CRIT(...)		__LOG_CRIT(	"Tilemap", __VA_ARGS__)
#define LOG_ERROR(...)		__LOG_ERROR("Tilemap", __VA_ARGS__)
#define LOG_WARN(...)		__LOG_WARN(	"Tilemap", __VA_ARGS__)
#define LOG_INFO(...)		__LOG_INFO(	"Tilemap", __VA_ARGS__)
#define LOG_DEBUG(...)		__LOG_DEBUG("Tilemap", __VA_ARGS__)
#define LOG_TRACE(...)		__LOG_TRACE("Tilemap", __VA_ARGS__)

namespace MM {

Tilemap::Tilemap(void) {
	MM::Logger::initSectionLogger("Tilemap");
}

bool Tilemap::parseTiled_Map(MM::Engine& engine, MM::Scene& scene, MM::Services::FilesystemService::fs_file_t file) {
	auto& fs = engine.getService<MM::Services::FilesystemService>();
	auto jmap = fs.readJson(file);

	{
		if (!jmap.contains("tilesets")) {
			LOG_ERROR("tilemap without tilesets");
			return false;
		}

		bool r = parseTiled_Tilesets(engine, jmap["tilesets"]);
		assert(r);
	}


	{
		if (!jmap.contains("layers")) {
			LOG_ERROR("tilemap without tile-layers");
			return false;
		}

		bool r = parseTiled_Layers(engine, scene, jmap["layers"]);
		assert(r);
	}

	return true;
}

bool Tilemap::parseTiled_Map(MM::Engine& engine, MM::Scene& scene, std::string_view path) {
	auto& fs = engine.getService<MM::Services::FilesystemService>();
	map_file_path = path;

	if (!fs.isFile(map_file_path.c_str())) {
		LOG_ERROR("Tiled map '{}' not found!", map_file_path);
		return false;
	}

	auto file = fs.open(map_file_path.c_str());
	if (!file) {
		LOG_ERROR("opening file '{}'!", map_file_path);
		return false;
	}

	//return parseTiled_Map(engine, scene, file);
	auto r = parseTiled_Map(engine, scene, file);
	fs.close(file);
	return r;
}

bool Tilemap::parseTiled_LayerCollision(MM::Engine&, MM::Scene&, nlohmann::json&, entt::entity) {
	LOG_WARN("no collision implementation!");
	return true;
}

bool Tilemap::parseTiled_ObjectLayer(MM::Engine&, MM::Scene&, nlohmann::json&) {
	LOG_WARN("no object layer implementation!");
	return true;
}

bool Tilemap::parseTiled_LayerRenderable(MM::Engine&, MM::Scene& scene, nlohmann::json& jlayer, entt::entity e) {
	assert(jlayer.contains("width"));
	assert(jlayer.contains("height"));

	uint32_t layer_width = jlayer["width"];
	uint32_t layer_height = jlayer["height"];

	scene.get_or_emplace<Components::Position2D_ZOffset>(e).z_offset = Tiled_get_z_prop(jlayer) + 500.f; // TODO: magic

	auto& tilemap_rend = scene.emplace<OpenGL::TilemapRenderable>(e);

	// TODO: inefficient
	for (auto& it : _sprite_sheet_map) {
		auto& layer = tilemap_rend.sprite_layer.emplace_back();
		layer.sprite_sheet = it.sp;
	}

	assert(jlayer.contains("data"));

	// fill sprite layers
	for (uint32_t curr_y = 0; curr_y < layer_height; curr_y++) {
		for (uint32_t curr_x = 0; curr_x < layer_width; curr_x++) {
			uint32_t curr_tile_id = jlayer["data"][curr_x+curr_y*layer_width];
			if (curr_tile_id != 0) {
				auto [id, l] = _get_sid_and_layer(curr_tile_id, tilemap_rend);

				auto& tile = l.map.emplace_back();

				tile.pos[0] = curr_x;
				tile.pos[1] = curr_y;

				tile.sprite_sheet_index = id;
			}
		}
	}


	// remove unused sprite layer
	for (auto it = tilemap_rend.sprite_layer.begin(); it != tilemap_rend.sprite_layer.end();) {
		if (it->map.empty()) {
			it = tilemap_rend.sprite_layer.erase(it);
			continue;
		}

		it->syncMapBuffer();
		assert(it->map_buffer->getSize());
		it++;
	}

	return true;
}

bool Tilemap::parseTiled_Layer(MM::Engine& engine, MM::Scene& scene, nlohmann::json& jlayer) {
	assert(jlayer.contains("width"));
	assert(jlayer.contains("height"));

	//uint32_t layer_width = jlayer["width"];
	//uint32_t layer_height = jlayer["height"];

	{
		uint32_t layer_width;
		jlayer["width"].get_to(layer_width);
		_width = glm::max(_width, layer_width);
	}
	{
		uint32_t layer_height;
		jlayer["height"].get_to(layer_height);
		_height = glm::max(_height, layer_height);
	}

	auto e = scene.create();

	if (jlayer.contains("name")) {
		auto& name = scene.emplace<MM::Components::Name>(e);
		name.str = jlayer["name"];
	}

	scene.emplace<MM::Components::Position2D>(e);

	// in case there is no auto system
	scene.emplace_or_replace<MM::Components::Transform4x4>(e);
	scene.emplace_or_replace<MM::Components::DirtyTransformTag>(e);

	if (Tiled_is_collision_layer(jlayer)) {
		return parseTiled_LayerCollision(engine, scene, jlayer, e);
	} else { // assume renderable
		return parseTiled_LayerRenderable(engine, scene, jlayer, e);
	}

	return false;
}

bool Tilemap::parseTiled_Layers(MM::Engine& engine, MM::Scene& scene, nlohmann::json& jlayers) {
	assert(jlayers.is_array());

	bool succ = true;

	for (auto& e : jlayers) {
		if (!e.contains("type")) {
			LOG_ERROR("layer without type?");
			//return false;
			continue;
		}

		std::string type = e["type"];
		if (type == "group") {
			assert(e.contains("layers"));
			succ &= parseTiled_Layers(engine, scene, e["layers"]);
		} else if (type == "objectgroup") {
			succ &= parseTiled_ObjectLayer(engine, scene, e);
		} else if (type == "tilelayer") {
			succ &= parseTiled_Layer(engine, scene, e);
		} else if (type == "imagelayer") {
			LOG_DEBUG("TODO: implement image layers");
		} else {
			LOG_ERROR("unknown layer type '{}'", type);
		}
	}

	return succ;
}

bool Tilemap::parseTiled_Tileset(MM::Engine& engine,  std::string_view src_path, nlohmann::json& jts, uint32_t first_g_id) {
	using namespace entt::literals;
	auto& fs = engine.getService<MM::Services::FilesystemService>();
	auto& rm_t = MM::ResourceManager<MM::OpenGL::Texture>::ref();


	if (jts.contains("source")) { // external file
		std::string path = MM::base_path(src_path);
		path += jts["source"];
		bool r = MM::path_shorten(path); assert(r);

		assert(fs.isFile(path.c_str()));

		auto external_jts = fs.readJson(path.c_str());

		assert(jts.contains("firstgid"));

		// call self
		parseTiled_Tileset(engine, path, external_jts, jts["firstgid"]);
	} else if (jts.contains("grid")) {
		assert(jts.contains("type"));
		assert(jts["type"].get<std::string>() == "tileset");
		LOG_DEBUG("grid Tiled tilesets are not yet supported!");
		return true; // TODO: implement
	} else {
		assert(jts.contains("type"));
		assert(jts["type"].get<std::string>() == "tileset");
		if (!jts.contains("image")) {
			LOG_ERROR("missing image path from tileset?!");
			return false;
		}

		std::string img_path = MM::base_path(src_path);
		img_path += jts["image"];

		//std::cerr << "got tileset image: '" << img_path << "'\n";

		assert(!img_path.empty());
		MM::path_shorten(img_path);
		assert(MM::std_utils::split(img_path, "/")[0] == "textures");

		// TODO: use somthing different than the full path of the image file
		if (!rm_t.load<MM::OpenGL::TextureLoaderFile>(img_path.c_str(), engine, img_path)) {
			LOG_ERROR("could not load texture '{}'!", img_path);
		}

		MM::OpenGL::SpriteSheet sprite_sheet;

		sprite_sheet.tex = rm_t.get(entt::hashed_string{img_path.c_str()}.value());
		if (!sprite_sheet.tex) {
			sprite_sheet.tex = rm_t.get("default"_hs); // defaulting
		}

		// x
		if (jts.contains("columns")) {
			sprite_sheet.tile_count.x = jts["columns"];
		} else if (jts.contains("imagewidth") && jts.contains("tilewidth")) {
			uint32_t img_width = jts["imagewidth"];
			uint32_t img_tile_width = jts["tilewidth"];
			sprite_sheet.tile_count.x = img_width / img_tile_width;
		} else {
			LOG_ERROR("no way to determain horizontal tile count! ({})", src_path);
			return false;
		}

		// y
		if (jts.contains("imageheight") && jts.contains("tileheight")) {
			uint32_t img_height = jts["imageheight"];
			uint32_t img_tile_height = jts["tileheight"];
			sprite_sheet.tile_count.y = img_height / img_tile_height;
		} else {
			LOG_ERROR("no way to determain vertical tile count! ({})", src_path);
			return false;
		}

		if (first_g_id == 0) {
			first_g_id = jts["firstgid"];
			assert(jts.contains("firstgid"));
		}

		uint32_t last_g_id = 0;
		if (jts.contains("tilecount")) {
			last_g_id = jts["tilecount"];
		} else {
			last_g_id = sprite_sheet.tile_count.x * sprite_sheet.tile_count.y;
		}
		last_g_id += first_g_id-1;

		// create entry for tileid mapper
		{
			auto& ref = _sprite_sheet_map.emplace_back();
			ref.first_id = first_g_id;
			ref.last_id = last_g_id;
			ref.sp = sprite_sheet;
		}
	}

	return true;
}

bool Tilemap::parseTiled_Tilesets(MM::Engine& engine, nlohmann::json& jtss) {
	assert(jtss.is_array());

	bool succ = true;

	for (auto& jts : jtss) {
		succ &= parseTiled_Tileset(engine, map_file_path, jts, 0);
	}

	std::sort(_sprite_sheet_map.begin(), _sprite_sheet_map.end(),
		[](auto& a, auto& b) {
			return a.first_id < b.first_id;
		}
	);

	return succ;
}

std::pair<uint32_t, OpenGL::TilemapRenderable::SpriteLayer&> Tilemap::_get_sid_and_layer(uint32_t gid, OpenGL::TilemapRenderable& tilemap_rend) {
	assert(gid);

	for (auto& it : _sprite_sheet_map) {
		if (it.first_id <= gid && it.last_id >= gid) {
			auto a_tile_id = gid - it.first_id;

			for (auto& l : tilemap_rend.sprite_layer) {
				if (l.sprite_sheet.tex == it.sp.tex) {
					return {a_tile_id, l};
				}
			}
		}
	}

	assert(!"not found");
}

nlohmann::json Tilemap::Tiled_get_custom_prop(nlohmann::json& j, std::string_view name, std::string_view type) {
	if (!j.contains("properties")) {
		return {};
	}

	auto& props = j["properties"];

	if (!props.is_array()) {
		return {};
	}

	for (auto& entry : props) {
		if (!entry.contains("name")) {
			LOG_ERROR("property without a name !!({})", map_file_path);
			continue;
		}

		if (entry["name"].get<std::string>() != name)
			continue;

		if (!entry.contains("type")) {
			LOG_ERROR("property without a type !!(name: {})({})", name, map_file_path);
			continue;
		}

		if (entry["type"].get<std::string>() != type) {
			LOG_WARN("property type mismatch, expected '{}', got '{}' !!(name: {})({})", type, entry["type"], name, map_file_path);
			continue;
		}

		if (!entry.contains("value")) {
			LOG_ERROR("property without a value !!(name: {})({})", name, map_file_path);
			continue;
		}

		return entry["value"];
	}

	return {};
}

bool Tilemap::Tiled_is_collision_layer(nlohmann::json& jlayer) {
	auto prop = Tiled_get_custom_prop(jlayer, "collision", "bool");

	return prop.is_boolean() && prop;
}

float Tilemap::Tiled_get_z_prop(nlohmann::json& jlayer) {
	auto prop = Tiled_get_custom_prop(jlayer, "z", "float");

	if (prop.is_number())
		return prop;

	return 0.f; // default
}

} // MM

