#pragma once

#include <cstdint>
#include <functional>

//#include <physfs.h>
//fwd
typedef struct PHYSFS_File PHYSFS_File;
typedef struct PHYSFS_Stat PHYSFS_Stat;

#include <nlohmann/json_fwd.hpp>

#include <mm/engine.hpp>

namespace MM::Services {

class FilesystemService : public Service {
	public:
		using fs_file_t = PHYSFS_File*;


	public:
		bool enable(Engine&) override;
		void disable(Engine&) override;

		const char* name(void) override { return "Filesystem"; }

	private:
		// setup data
		const char* _argv0 = nullptr;
		const char* _app_name = "default_app";
		const bool _try_mount_self = true;
		const bool _try_mount_base = true;
		// TODO: write dir
		std::vector<std::tuple<std::string, std::string, bool>> _try_mount_list;

	public:
		FilesystemService(void);
		FilesystemService(
			const char* argv0 = nullptr,
			const char* app_name = "default_app",
			const bool try_mount_self = true,
			const bool try_mount_base = true,
			const std::vector<std::tuple<std::string, std::string, bool>>& try_mount_list = {}
		);

		~FilesystemService(void);

		bool exists(const char* filepath) const;
		bool isFile(const char* filepath) const;
		bool isDir(const char* filepath) const;

		void forEachIn(const char* filepath, std::function<bool(const char*)> fn) const;

		enum FOPEN_t { READ, WRITE, APPEND };
		// opens file, expects path and mode
		fs_file_t open(const char* filepath, FOPEN_t = READ) const;

		bool close(fs_file_t file) const;
		bool eof(fs_file_t file) const;
		int64_t tell(fs_file_t file) const;
		bool seek(fs_file_t file, uint64_t pos) const;

		// read from file into buffer max size bytes, returns read bytes
		int64_t read(fs_file_t file, void* buffer, uint64_t size) const;

		// writes from buffer to file, returns written bytes
		uint64_t write(fs_file_t file, const void* buffer, uint64_t size) const;

		void flush(fs_file_t file) const;

		int64_t length(fs_file_t file) const;

		// reads in (remaining) file to string, and returns read bytes
		int64_t readString(fs_file_t file, std::string& string) const;

		// json
		nlohmann::json readJson(fs_file_t file) const;
		nlohmann::json readJson(const char* filepath) const;

		bool writeJson(fs_file_t file, nlohmann::json& j, const int indent = -1, const char indent_char = ' ') const;
		bool writeJson(const char* filepath, nlohmann::json& j, const int indent = -1, const char indent_char = ' ') const;

		bool writeJson(fs_file_t file, nlohmann::ordered_json& j, const int indent = -1, const char indent_char = ' ') const;
		bool writeJson(const char* filepath, nlohmann::ordered_json& j, const int indent = -1, const char indent_char = ' ') const;
};

} // namespace MM::Services

