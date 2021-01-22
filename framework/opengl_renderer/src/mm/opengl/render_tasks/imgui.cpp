#include <mm/opengl/render_tasks/imgui.hpp> // bc imgui.hpp is overused

#include <mm/engine.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <tracy/Tracy.hpp>
#ifndef MM_OPENGL_3_GLES
	#include <tracy/TracyOpenGL.hpp>
#else
	#define TracyGpuContext
	#define TracyGpuCollect
	#define TracyGpuZone(...)
#endif

namespace MM::OpenGL::RenderTasks {

ImGuiRT::ImGuiRT(Engine&) {
	assert(ImGui::GetCurrentContext());
}

ImGuiRT::~ImGuiRT(void) {
}

void ImGuiRT::render(Services::OpenGLRenderer& rs, Engine&) {
	ZoneScopedN("MM::OpenGL::RenderTasks::ImGuiRT::render");
	TracyGpuZone("MM::OpenGL::RenderTasks::ImGuiRT::render");

	rs.targets["display"]->bind(FrameBufferObject::W);

	// render
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

} // MM::OpenGL::RenderTasks

