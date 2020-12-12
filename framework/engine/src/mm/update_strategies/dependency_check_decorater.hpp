#pragma once

#include "./update_strategy.hpp"

#include <memory>
#include <type_traits>

namespace MM::UpdateStrategies {

// checks for cyles
template<class T>
class DependencyCheckDecorator : public T {
	static_assert(std::is_base_of_v<UpdateStrategy, T>);

	public:
		//DependencyCheckDecorator(void) = default;

		//template<typename ...Args>
		//DependencyCheckDecorator(Args... args) : T(args) {
		//}

		virtual ~DependencyCheckDecorator(void) {}

	private:
		void doUpdate(MM::Engine& engine) override {
			T::doUpdate(engine);
		}

		bool registerService(const entt::id_type s_id, std::vector<UpdateCreationInfo>&& info_array) override {
			return T::registerService(s_id, std::move(info_array));
		}

		bool enableService(const entt::id_type s_id) override {
			return T::enableService(s_id);
		}

		bool disableService(const entt::id_type s_id) override {
			return T::disableService(s_id);
		}

	public:
		bool enable(const update_key_t key) override {
			return T::enable(key);
		}

		bool disable(const update_key_t key) override {
			return T::disable(key);
		}

		bool depend(const update_key_t A, const update_key_t B) override {
			return T::depend(A, B);
		}
};

} // MM::UpdateStrategies

