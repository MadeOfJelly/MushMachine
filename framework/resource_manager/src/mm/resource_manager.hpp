#pragma once

#include <memory>
#include <utility>
#include <algorithm>
#include <cassert>
#include <functional>
#include <optional>
#include <unordered_map>

#include <entt/config/config.h>
#include <entt/core/hashed_string.hpp>

#include <mm/logger.hpp>
#define LOGRM(x) LOG("ResourceManager", x)

namespace MM {

// modeled heavily after entt::resource_cache
// https://github.com/skypjack/entt/blob/master/src/entt/resource/cache.hpp
template<typename Resource>
class ResourceManager {
	public:
		using res_id_type = typename entt::hashed_string::hash_type;
		using handle_type = std::shared_ptr<Resource>;
		using container_type = std::unordered_map<res_id_type, handle_type>;
		using size_type = typename container_type::size_type;

	private:
		ResourceManager(void) = default;

		container_type _storage;

	public:
		ResourceManager(ResourceManager const&) = delete;
		void operator=(ResourceManager const&) = delete;

		static ResourceManager& ref(void) {
			static ResourceManager<Resource> instance{};
			return instance;
		}

		// Loader s must implement a load method
		template<typename Loader, typename... Args>
		bool load(const res_id_type id, Args &&... args) {
			//static_assert(std::is_base_of<entt::resource_loader<Loader, Resource>, Loader>::value);

			if (!contains(id)) {
				handle_type r = Loader{}.load(std::forward<Args>(args)...);
				if (static_cast<bool>(r)) {
					_storage[id] = std::move(r);
				} else {
					return false;
				}
			}

			return true;
		}

		// string overload
		template<typename Loader, typename... Args>
		bool load(const char* id, Args &&... args) {
			return load<Loader>(entt::hashed_string{id}.value(), std::forward<Args>(args)...);
		}

		template<typename Loader, typename... Args>
		bool reload(const res_id_type id, Args &&... args) {
			discard(id);
			return load<Loader>(id, std::forward<Args>(args)...);
		}

		// string overload
		template<typename Loader, typename... Args>
		bool reload(const char* id, Args &&... args) {
			return reload<Loader>(entt::hashed_string{id}.value(), std::forward<Args>(args)...);
		}

		void discard(const res_id_type id) ENTT_NOEXCEPT {
			if (auto it = _storage.find(id); it != _storage.end()) {
				_storage.erase(it);
			}
		}

		void discard(const char* id) ENTT_NOEXCEPT {
			discard(entt::hashed_string{id}.value());
		}

		template<typename FN>
		void each(FN&& fn) {
			for (auto& it : _storage) {
				fn(it.first, it.second);
			}
		}

		size_type size(void) const {
			return _storage.size();
		}

		bool empty(void) const {
			return _storage.empty();
		}

		void clear(void) ENTT_NOEXCEPT {
			_storage.clear();
		}

		bool contains(const res_id_type id) const ENTT_NOEXCEPT {
			return (_storage.find(id) != _storage.cend());
		}

		handle_type get(const res_id_type id) const {
			using namespace entt::literals;

			auto it = _storage.find(id);
			if (it == _storage.end()) {
				if (id != "default"_hs) {
					LOGRM("not in cache, loading default...");
					return get("default"_hs);
				} else {
					return nullptr;
				}
			} else {
				return it->second;
			}
		}

		// TODO: fix
		//handle_type get(const char* id) const {
			//return get(entt::hashed_string{id}.value());
		//}

		// reverse lookup
		std::optional<res_id_type> id_from_handle(const std::shared_ptr<Resource>& handle) {
			if (static_cast<bool>(handle)) {
				auto it = std::find_if(_storage.cbegin(), _storage.cend(), [&](auto& v) { return handle == v.second; });
				if (it != _storage.cend()) {
					return it->first;
				}
			}

			return std::nullopt;
		}
};

} // namespace MM

#undef LOGRM

