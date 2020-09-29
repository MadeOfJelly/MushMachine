#pragma once

#include <entt/core/family.hpp>

#include <functional>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
#include <string>
#include <cassert>

#include <mm/services/service.hpp>

namespace MM {

class Engine {
	private:
		using service_family = entt::family<struct internal_service_family>;

	public:
		using service_family_type = service_family::family_type;

	// the services "internal" interface
	//private:
	public:
		struct FunctionPriorityDataStructure {
			std::function<void(Engine&)> f;
			int16_t priority = 0; // 0 is equal to scene update, the higher the prio the earlier
			std::string name;

			explicit FunctionPriorityDataStructure(std::function<void(Engine&)> fun) : f(fun) {}
			FunctionPriorityDataStructure(const FunctionPriorityDataStructure& rhs)
				: f(rhs.f), priority(rhs.priority), name(rhs.name) {}
		};

		using FunctionDataType = FunctionPriorityDataStructure;
		//using FunctionDataHandle = FunctionDataType*; // important: its a pointer
		using FunctionDataHandle = std::weak_ptr<FunctionDataType>; // important: its a pointer

		// return nullptr on error
		[[nodiscard]] FunctionDataHandle addUpdate(std::function<void(Engine&)> function);
		[[nodiscard]] FunctionDataHandle addFixedUpdate(std::function<void(Engine&)> function);

		void removeUpdate(FunctionDataHandle handle);
		void removeFixedUpdate(FunctionDataHandle handle);

		// dont use, if you are not using it to modify the engine.
		// you usualy dont need to use this, if you think you need to use this, you probably dont.
		void addFixedDefer(std::function<void(Engine&)> function);

	private:
		std::vector<std::shared_ptr<FunctionDataType>> _update_functions;
		bool _update_functions_modified;

		std::vector<std::shared_ptr<FunctionDataType>> _fixed_update_functions;
		bool _fixed_update_functions_modified;

		std::vector<std::function<void(Engine&)>> _fixed_defered;

	private:
		void traverseUpdateFunctions(std::vector<std::shared_ptr<FunctionDataType>>& list); // traverses an update list, gets called by update()/fixedUpdate()

	private:
		volatile bool _is_running = false;
		const float _fixed_delta_time;

	public:
		explicit Engine(float f_delta_time = 1.f/60.f);
		~Engine(void);

		// called from destructor or explicitly
		void cleanup(void);

		[[nodiscard]] float getFixedDeltaTime(void) const { return _fixed_delta_time; };

		void update(void);
		void fixedUpdate(void);

		void run(void); // calls update()/fixedUpdate() until stopped
		void stop(void);

	private:
		std::vector<service_family::family_type> _service_add_order; // ?
		std::vector<service_family::family_type> _service_enable_order; // ?

		//std::unordered_map<service_family::family_type, std::pair<bool, std::unique_ptr<Service>>> _services;
		std::unordered_map<
			service_family::family_type,
			std::shared_ptr<std::pair<
				bool,
				std::unique_ptr<Services::Service>
			>>
		> _services;

		// maps I to T
		//std::unordered_map<service_family::family_type, service_family::family_type> _implementation_provider;

	public:
		template<typename T>
		constexpr auto type(void) {
			return service_family::type<T>;
		}

		template<typename T, typename... Args>
		T& addService(Args&& ... args) {
			assert(!tryService<T>());

			auto& ss_entry = _services[service_family::type<T>] =
				std::make_shared<std::pair<bool, std::unique_ptr<Services::Service>>>(
					std::make_pair<bool, std::unique_ptr<Services::Service>>(
						false,
						std::make_unique<T>(std::forward<Args>(args)...)
					)
				);

			_service_add_order.emplace_back(service_family::type<T>);

			return (T&)*ss_entry.get()->second.get();
		}

		template<typename T>
		[[nodiscard]] T* tryService(void) const {
			if (_services.count(service_family::type<T>)) {
				return static_cast<T*>(_services.at(service_family::type<T>).get()->second.get());
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

		bool enableService(service_family::family_type s_t);
		void disableService(service_family::family_type s_t);

		template<typename T>
		bool enableService(void) {
			return enableService(service_family::type<T>);
		}

		template<typename T>
		void disableService(void) {
			disableService(service_family::type<T>);
		}

		// provide T as I implementation
		// T needs to be an added Service
		bool provide(service_family::family_type I, service_family::family_type T);

		// provide T as I implementation
		// T needs to be an added Service
		template<typename I, typename T>
		bool provide(void) {
			static_assert(std::is_base_of_v<I, T>, "T is not derived from I!");
			static_assert(!std::is_same_v<I, T>, "I and T are the same, makes no sense!");

			return provide(service_family::type<I>, service_family::type<T>);
		}

		// TODO: reimplement???
		//template<typename I>
		//void removeProvider(void) {
			//if (auto it = _implementation_provider.find(service_family::type<I>); it != _implementation_provider.end()) {
				//_implementation_provider.erase(it);
			//}
		//}
};

} // MM

