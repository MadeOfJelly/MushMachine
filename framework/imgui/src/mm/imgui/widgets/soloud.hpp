#pragma once

// fwd
namespace SoLoud {
	class Soloud;
	class Sfxr;
	struct SfxrParams;
	class Filter;
	class FilterInstance;

	typedef unsigned int handle;
}

namespace MM::ImGuiWidgets {

bool SoLoudSfxrPlain(const char* label, SoLoud::SfxrParams* sfxr_params);
// uses knobs and childs
bool SoLoudSfxrFancy(const char* label, SoLoud::SfxrParams* sfxr_params);

// calls getParam*() methods
//bool SoLoudFilterLiveParams(const char* label, SoLoud::Filter* filter, SoLoud::FilterInstance* filter_instance); // no access to instance
bool SoLoudFilterLiveParams(const char* label, SoLoud::Soloud* soloud, SoLoud::Filter* filter, unsigned int filter_id, SoLoud::handle voice_handle = 0);
bool SoLoudFilterLiveParamsFancy(const char* label, SoLoud::Soloud* soloud, SoLoud::Filter* filter, unsigned int filter_id, SoLoud::handle voice_handle = 0, bool same_line = false);

} // MM::ImGuiWidgets

