#include "./filesystem.hpp"

#include "../fs_const_archiver.hpp"

#include <physfs.h>

#include <mm/path_utils.hpp>

#ifndef MM_HEADLESS
	#include <SDL.h>
#endif

#include <nlohmann/json.hpp>

#include <mm/logger.hpp>
#define LOG_CRIT(...)		__LOG_CRIT(	"Filesystem", __VA_ARGS__)
#define LOG_ERROR(...)		__LOG_ERROR("Filesystem", __VA_ARGS__)
#define LOG_WARN(...)		__LOG_WARN(	"Filesystem", __VA_ARGS__)
#define LOG_INFO(...)		__LOG_INFO(	"Filesystem", __VA_ARGS__)
#define LOG_DEBUG(...)		__LOG_DEBUG("Filesystem", __VA_ARGS__)
#define LOG_TRACE(...)		__LOG_TRACE("Filesystem", __VA_ARGS__)

namespace MM::Services {

bool FilesystemService::enable(Engine&, std::vector<UpdateStrategies::TaskInfo>&) {
	if (PHYSFS_isInit()) {
		LOG_ERROR("physfs already initialized!!");
		return false;
	}

	if (!PHYSFS_init(_argv0)) {
		LOG_ERROR("error initializing physfs: {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		return false;
	}

#if !defined(MM_HEADLESS) && !defined(EMSCRIPTEN)
	char* pref_path = SDL_GetPrefPath("made_of_jelly", _app_name);
#else
	// org only respected on windows
	const char* pref_path = PHYSFS_getPrefDir("made_of_jelly", _app_name); // TODO: make this point to the right dir
#endif
	if (!pref_path) {
		LOG_ERROR("error getting pref path");
		return false; // TODO: defaulting to base path?
	} else {
		if (!PHYSFS_setWriteDir(pref_path)) {
			LOG_ERROR("error setting physfs write dir: {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
			//return false;
		}

		//LOGFS(std::string("write dir set to: ") + PHYSFS_getWriteDir());
		LOG_INFO("write dir set to: {}", PHYSFS_getWriteDir());

		if (!PHYSFS_mount(pref_path, NULL, 0)) {
			LOG_ERROR("mounting physfs write dir: {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
			//return false;
		}

#if !defined(MM_HEADLESS) && !defined(EMSCRIPTEN)
		SDL_free(pref_path);
#endif
	}

	// add base path to search tree
	if (_try_mount_base) {
		LOG_TRACE("mounting physfs base dir: {}", PHYSFS_getBaseDir());
		if (!PHYSFS_mount(PHYSFS_getBaseDir(), NULL, 0)) {
			LOG_ERROR("mounting physfs base dir: {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		}
	}

	// mount self (exec) :P
	if (_try_mount_self) {
		LOG_TRACE("mounting self");
		if (PHYSFS_mount(_argv0, "/", 1)) {
			LOG_INFO("mounted self!!");
		}
	}

	// const archiver
	if (!PHYSFS_registerArchiver(FSConstArchiver::getArchiverStruct())) {
		LOG_ERROR("error registering const archiver: {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
	}

	// fake mount, to add mount const archiver
	if (!PHYSFS_mountMemory((void*)1, 1, nullptr, "main.mem", "/", 1)) { // should trigger
		LOG_ERROR("error mounting const archiver: {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
	}

	for (const auto&[archive, mount_point, append] : _try_mount_list) {
		LOG_TRACE("mounting physfs userdefined archive: '{}' '{}' {}", archive, mount_point, append);
		if (!PHYSFS_mount(archive.c_str(), mount_point.c_str(), append?1:0)) {
			LOG_ERROR("mounting physfs userdefined archive: {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		}
	}

	return true;
}

void FilesystemService::disable(Engine&) {
	if (!PHYSFS_deinit()) {
		LOG_ERROR("error deinitializing physfs: {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
	}
}

FilesystemService::FilesystemService(void) {
	MM::Logger::initSectionLogger("Filesystem");
}

FilesystemService::FilesystemService(
	const char* argv0,
	const char* app_name,
	const bool try_mount_self,
	const bool try_mount_base,
	const std::vector<std::tuple<std::string, std::string, bool>>& try_mount_list
) : _argv0(argv0), _app_name(app_name), _try_mount_self(try_mount_self), _try_mount_base(try_mount_base) {
	MM::Logger::initSectionLogger("Filesystem");

	//_try_mount_list = try_mount_list;
	for (const auto& it : try_mount_list) {
		_try_mount_list.emplace_back(it);
	}
}

FilesystemService::~FilesystemService(void) {
}

bool FilesystemService::exists(const char* filepath) const {
	return PHYSFS_exists(filepath);
}

bool FilesystemService::isFile(const char* filepath) const {
	if (!PHYSFS_exists(filepath)) {
		return false;
	}

	PHYSFS_Stat stat;

	if (PHYSFS_stat(filepath, &stat) == 0) {
		return false;
	}

	// TODO: y other tho
	return stat.filetype == PHYSFS_FileType::PHYSFS_FILETYPE_REGULAR || stat.filetype == PHYSFS_FileType::PHYSFS_FILETYPE_OTHER;
}

bool FilesystemService::isDir(const char* filepath) const {
	if (!PHYSFS_exists(filepath)) {
		return false;
	}

	PHYSFS_Stat stat;

	if (PHYSFS_stat(filepath, &stat) == 0) {
		return false;
	}

	return stat.filetype == PHYSFS_FileType::PHYSFS_FILETYPE_DIRECTORY;
}

void FilesystemService::forEachIn(const char* filepath, std::function<bool(const char*)> fn) const {
	char** rc = PHYSFS_enumerateFiles(filepath);
	for (char** i = rc; *i != NULL; i++) {
		if (!fn(*i)) {
			break;
		}
	}
	PHYSFS_freeList(rc);
}

FilesystemService::fs_file_t FilesystemService::open(const char* filepath, FilesystemService::FOPEN_t t) const {
	if (!filepath) {
		return nullptr;
	}

	PHYSFS_File* phys_file = nullptr;

	switch (t) {
		case FOPEN_t::READ:
			LOG_TRACE("opening '{}' in read mode", filepath);
			phys_file = PHYSFS_openRead(filepath);
			break;
		case FOPEN_t::WRITE:
			LOG_TRACE("opening '{}' in write mode", filepath);
			if (std::string_view(filepath).find('/') != std::string_view::npos) {// bc dirs might not exist
				auto tmp_dir_path = MM::base_path(filepath);
				PHYSFS_mkdir(tmp_dir_path.c_str());
			}
			phys_file = PHYSFS_openWrite(filepath);
			break;
		case FOPEN_t::APPEND:
			LOG_TRACE("opening '{}' in append mode", filepath);
			if (std::string_view(filepath).find('/') != std::string_view::npos) {// bc dirs might not exist
				auto tmp_dir_path = MM::base_path(filepath);
				PHYSFS_mkdir(tmp_dir_path.c_str());
			}
			phys_file = PHYSFS_openAppend(filepath);
			break;
		default:
			LOG_ERROR("invalid fopen mode {} for '{}'", t, filepath);
			return nullptr;
	}

	if (!phys_file) {
		LOG_ERROR("error while opening '{}' : {}", filepath, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
	}

	return phys_file;
}

bool FilesystemService::close(fs_file_t file) const {
	if (!PHYSFS_close(file)) {
		//LOGFS_physfs("error while closing file ");
		// ignore. windoof for some reason allways fails with "permission denied" (for closing read handles)
		LOG_TRACE("error while closing file : {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));

		return false;
	}

	return true;
}

bool FilesystemService::eof(fs_file_t file) const {
	return PHYSFS_eof(file);
}

int64_t FilesystemService::tell(fs_file_t file) const {
	int64_t ret = PHYSFS_tell(file);
	if (ret < 0) {
		LOG_ERROR("error while determining file position (tell()) {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
	}

	return ret;
}

bool FilesystemService::seek(fs_file_t file, uint64_t pos) const {
	if (!PHYSFS_seek(file, pos)) {
		LOG_ERROR("error while seeking in file: {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		return false;
	}

	return true;
}

int64_t FilesystemService::read(fs_file_t file, void* buffer, uint64_t size) const {
	auto r = PHYSFS_readBytes(file, buffer, size);

	if (r < 0) {
		LOG_ERROR("error while reading file: {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
	}

	return r;
}

uint64_t FilesystemService::write(fs_file_t file, const void* buffer, uint64_t size) const {
	uint64_t r = static_cast<uint64_t>(PHYSFS_writeBytes(file, buffer, size));

	// TODO: handle partial writes differently
	if (r != size) {
		LOG_ERROR("error while writing file: {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
	}

	return r;
}

void FilesystemService::flush(fs_file_t file) const {
	if (!PHYSFS_flush(file)) {
		LOG_ERROR("error flushing ..?: {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
	}
}

int64_t FilesystemService::length(fs_file_t file) const {
	return PHYSFS_fileLength(file);
}

int64_t FilesystemService::readString(fs_file_t file, std::string& string) const {
	char buffer[256] = {};

	int64_t read_bytes = 0;

	while (int64_t r = read(file, buffer, 256)) {
		if (r < 0) {
			return r;
		}

		string.append(buffer, static_cast<size_t>(r));
		read_bytes += r;
	}

	return read_bytes;
}

nlohmann::json FilesystemService::readJson(fs_file_t file) const {
	if (!file)
		return {};

	seek(file, 0);

	std::string buffer;
	readString(file, buffer);

	// disable exeptions
	// TODO: use callback instead of readString()
	return nlohmann::json::parse(buffer, nullptr, false);
}

nlohmann::json FilesystemService::readJson(const char* filepath) const {
	if (!isFile(filepath)) {
		return {};
	}

	auto h = open(filepath, READ);
	auto r = readJson(h);
	close(h);
	return r;
}

bool FilesystemService::writeJson(fs_file_t file, nlohmann::json& j, const int indent, const char indent_char) const {
	if (!file) {
		LOG_ERROR("writing json to invalid file");
		return false;
	}

	auto s = j.dump(indent, indent_char);

	auto r = write(file, s.c_str(), s.size());

	return r == s.size();
}

bool FilesystemService::writeJson(const char* filepath, nlohmann::json& j, const int indent, const char indent_char) const {
	auto h = open(filepath, WRITE);
	auto r = writeJson(h, j, indent, indent_char);
	close(h);
	return r;
}

bool FilesystemService::writeJson(fs_file_t file, nlohmann::ordered_json& j, const int indent, const char indent_char) const {
	if (!file) {
		LOG_ERROR("writing json to invalid file");
		return false;
	}

	auto s = j.dump(indent, indent_char);

	auto r = write(file, s.c_str(), s.size());

	return r == s.size();
}

bool FilesystemService::writeJson(const char* filepath, nlohmann::ordered_json& j, const int indent, const char indent_char) const {
	auto h = open(filepath, WRITE);
	auto r = writeJson(h, j, indent, indent_char);
	close(h);
	return r;
}

} // MM::Services

