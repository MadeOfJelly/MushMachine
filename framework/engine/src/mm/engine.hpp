#pragma once

#include "./engine_fwd.hpp"

#include <entt/core/type_info.hpp>

#include <functional>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
#include <string>
#include <cassert>

#include <mm/services/service.hpp>
#include <mm/update_strategies/update_strategy.hpp>

namespace MM {

// fwd
namespace Services {
	class ImGuiEngineTools;
}

class Engine {
	friend Services::ImGuiEngineTools;

	public:
		using service_id_type = entt::id_type; // alias, for future proof

	// UpdateStrategy
	protected:
		std::unique_ptr<UpdateStrategies::UpdateStrategy> _update_strategy;

	public:
		UpdateStrategies::UpdateStrategy& getUpdateStrategy(void) { return *_update_strategy; }

	// state
	private:
		volatile bool _is_running = false;
		// ... just realisied: i never needed a getter ...

		std::vector<entt::id_type> _service_add_order; // ?
		std::vector<entt::id_type> _service_enable_order; // ?

		std::unordered_map<
			entt::id_type,
			std::shared_ptr<std::pair<
				bool,
				std::unique_ptr<Services::Service>
			>>
		> _services;


	// private state helper
	private:
		void setup(void);

	// ctr dtr ...
	public:
		Engine(void);
		~Engine(void);

		explicit Engine(std::unique_ptr<UpdateStrategies::UpdateStrategy> us) {
			setup();
			_update_strategy = std::move(us);
		}

		Engine(const Engine&) = delete;
		Engine(Engine&&) = delete;

		// called from destructor or explicitly (if eg "global", u need dis)
		void cleanup(void);

		void update(void);

		void run(void); // calls update() until stopped
		void stop(void);

	public:
		template<typename T>
		constexpr static auto type(void) {
			//return entt::type_id<T>.hash();
			return entt::type_hash<T>::value();
		}

		template<typename T, typename... Args>
		T& addService(Args&& ... args) {
			assert(!tryService<T>());

			auto& ss_entry = _services[type<T>()] =
				std::make_shared<std::pair<bool, std::unique_ptr<Services::Service>>>(
					std::make_pair<bool, std::unique_ptr<Services::Service>>(
						false,
						std::make_unique<T>(std::forward<Args>(args)...)
					)
				);

			_service_add_order.emplace_back(type<T>());

			return (T&)*ss_entry.get()->second.get();
		}

		template<typename T>
		[[nodiscard]] T* tryService(void) const {
			if (_services.count(type<T>())) {
				return static_cast<T*>(_services.at(type<T>()).get()->second.get());
			}

			return nullptr;
		}

		template<typename T>
		[[deprecated("use tryService() instead")]]
		[[nodiscard]] T* tryGetService(void) const {
			return tryService<T>();
		}

		template<typename T>
		[[nodiscard]] T& getService(void) const {
			T* tmp = tryService<T>();
			assert(tmp);
			return *tmp;
		}

		bool enableService(entt::id_type s_t);
		void disableService(entt::id_type s_t);

		template<typename T>
		bool enableService(void) {
			return enableService(type<T>());
		}

		template<typename T>
		void disableService(void) {
			disableService(type<T>());
		}

		// provide T as I implementation
		// T needs to be an added Service
		bool provide(entt::id_type I, entt::id_type T);

		// provide T as I implementation
		// T needs to be an added Service
		template<typename I, typename T>
		bool provide(void) {
			static_assert(std::is_base_of_v<I, T>, "T is not derived from I!");
			static_assert(!std::is_same_v<I, T>, "I and T are the same, makes no sense!");

			return provide(type<I>(), type<T>());
		}

		// TODO: remove service
		//void removeProvider(void) {
};

} // MM

