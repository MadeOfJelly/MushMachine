#pragma once

#include <mm/services/filesystem.hpp>

#include <physfs.h>

#include <unordered_map>

namespace MM {
	class FSConstArchiver {
		private:
			static std::unordered_map<std::string, std::pair<uint8_t*, size_t>> _storage;

		private:
			// archiver interface
			static void* PH_openArchive(PHYSFS_Io* io, const char* name, int forWrite, int* claimed);
			static PHYSFS_EnumerateCallbackResult PH_enumerate(void* opaque, const char* dirname, PHYSFS_EnumerateCallback cb, const char* origdir, void* callbackdata);
			static PHYSFS_Io* PH_openRead(void* opaque, const char* fnm);
			static PHYSFS_Io* PH_openWrite(void* opaque, const char* filename);
			static PHYSFS_Io* PH_openAppend(void* opaque, const char* filename);
			static int PH_remove(void* opaque, const char* filename);
			static int PH_mkdir(void* opaque, const char* filename);
			static int PH_stat(void* opaque, const char* fn, PHYSFS_Stat* stat);
			static void PH_closeArchive(void* opaque);

			static PHYSFS_Io* createIO(const char* filename, PHYSFS_uint64 pos = 0);

			static bool pathIsDir(const char* path);

		public:
			static const PHYSFS_Archiver* getArchiverStruct(void);

			// the archiver is not responsible for memory, you need to keep it around.
			static void addFile(const char* path, uint8_t* data, size_t data_size);
	};
} // MM

#define FS_CONST_MOUNT_FILE(path, x) MM::FSConstArchiver::addFile(path, (uint8_t*)x, sizeof x);
#define FS_CONST_MOUNT_FILE_S(path, x, size) MM::FSConstArchiver::addFile(path, x, size);

//#define FS_CONST_MOUNT_FILE_STATIC(path, x) struct __internal_fs_const_struct_t { __internal_fs_const_struct_t(void) { FS_CONST_MOUNT_FILE(path, x) } }; static __internal_fs_const_struct_t __internal_fs_const_struct {};

