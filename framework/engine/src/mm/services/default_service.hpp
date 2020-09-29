#pragma once

#include "./service.hpp"
#include <mm/engine.hpp>

namespace MM::Services {

class DefaultService : public Service {
	private:
		Engine::FunctionDataHandle _func_handles[4];

	public:
		virtual const char* name(void) override { return "DefaultService"; }

		virtual bool enable(Engine& engine) override;
		virtual void disable(Engine& engine) override;

		virtual void preSceneUpdate(Engine&) {}
		virtual void postSceneUpdate(Engine&) {}
		virtual void preSceneFixedUpdate(Engine&) {}
		virtual void postSceneFixedUpdate(Engine&) {}
};

} //MM::Services


