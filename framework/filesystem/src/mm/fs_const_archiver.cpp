#include "./fs_const_archiver.hpp"

#include <cstring>

#include <mm/logger.hpp>
#define LOG_CRIT(...)		__LOG_CRIT(	"Filesystem", __VA_ARGS__)
#define LOG_ERROR(...)		__LOG_ERROR("Filesystem", __VA_ARGS__)
#define LOG_WARN(...)		__LOG_WARN(	"Filesystem", __VA_ARGS__)
#define LOG_INFO(...)		__LOG_INFO(	"Filesystem", __VA_ARGS__)
#define LOG_DEBUG(...)		__LOG_DEBUG("Filesystem", __VA_ARGS__)
#define LOG_TRACE(...)		__LOG_TRACE("Filesystem", __VA_ARGS__)

namespace MM {

std::unordered_map<std::string, std::pair<uint8_t*, size_t>> FSConstArchiver::_storage {};

void* FSConstArchiver::PH_openArchive(PHYSFS_Io* io, const char* name, int forWrite, int* claimed) {
	(void)io;
	(void)name;
	// TODO: test name

	if (forWrite) {
		//PHYSFS_setErrorCode(PHYSFS_ERR_READ_ONLY);
		*claimed = 0;
		return nullptr;
	}

	*claimed = 1;

	return (void*)1; // non zero
}

PHYSFS_EnumerateCallbackResult FSConstArchiver::PH_enumerate(void* opaque, const char* dirname, PHYSFS_EnumerateCallback cb, const char* origdir, void* callbackdata) {
	(void)opaque;
	(void)dirname;
	(void)cb;
	(void)origdir;
	(void)callbackdata;

	// "files" just not enumeratable
	//return PHYSFS_ENUM_STOP;
	return PHYSFS_ENUM_OK;
}

PHYSFS_Io* FSConstArchiver::PH_openRead(void* opaque, const char* fnm) {
	(void)opaque;

	if (!_storage.count(fnm)) {
		PHYSFS_setErrorCode(PHYSFS_ERR_NOT_FOUND);
		return nullptr;
	}

	return createIO(fnm);
}

PHYSFS_Io* FSConstArchiver::PH_openWrite(void* opaque, const char* filename) {
	(void)opaque;
	(void)filename;
	return nullptr;
}

PHYSFS_Io* FSConstArchiver::PH_openAppend(void* opaque, const char* filename) {
	(void)opaque;
	(void)filename;
	return nullptr;
}

int FSConstArchiver::PH_remove(void* opaque, const char* filename) {
	(void)opaque;
	(void)filename;

	// TODO: support this?

	PHYSFS_setErrorCode(PHYSFS_ERR_READ_ONLY);

	return 0;
}

int FSConstArchiver::PH_mkdir(void* opaque, const char* filename) {
	(void)opaque;
	(void)filename;

	PHYSFS_setErrorCode(PHYSFS_ERR_READ_ONLY);

	return 0;
}

int FSConstArchiver::PH_stat(void* opaque, const char* fn, PHYSFS_Stat* stat) {
	(void)opaque;

	if (pathIsDir(fn)) {
		stat->filesize = 0;
		stat->filetype = PHYSFS_FILETYPE_DIRECTORY;
	} else if (_storage.count(fn)) {
		stat->filesize = _storage[fn].second;
		stat->filetype = PHYSFS_FILETYPE_REGULAR;
	} else {
		return 0;
	}

	stat->accesstime = -1;
	stat->createtime = -1;
	stat->modtime = -1;
	stat->readonly = 1;

	return 1;
}

void FSConstArchiver::PH_closeArchive(void* opaque) {
	(void)opaque;
}

PHYSFS_Io* FSConstArchiver::createIO(const char* filename, PHYSFS_uint64 pos) {
	if (!_storage.count(filename)) {
		//LOGCA(std::string("error: path '") + filename + "' alredy used!");
		LOG_ERROR("path '{}' already used!", filename);
		return nullptr;
	}

	//LOGCA(std::string("###creating io for ") + filename);
	LOG_TRACE("creating io for {}", filename);

	struct io_internal {
		std::string filename;
		PHYSFS_uint64 pos = 0;
		uint8_t* data = nullptr;
		size_t data_size = 0;
	};

	auto io = new PHYSFS_Io;


	io->version = 0;
	io->write = nullptr;
	io->flush = nullptr;

	io->opaque = new io_internal{
		filename,
		pos,
		_storage[filename].first,
		_storage[filename].second
	};
	io->destroy = [](PHYSFS_Io* io) {
		delete (io_internal*)io->opaque;
		delete io;
	};

	io->tell = [](PHYSFS_Io* io) -> PHYSFS_sint64 { return ((io_internal*)io->opaque)->pos; };
	io->seek = [](PHYSFS_Io* io, PHYSFS_uint64 offset) -> int {
		auto* inter = (io_internal*)io->opaque;
		if (offset > inter->data_size)
			return 0; // error, past end
		inter->pos = offset;
		return 1;
	};
	io->length = [](PHYSFS_Io* io) -> PHYSFS_sint64 { return ((io_internal*)io->opaque)->data_size; };
	io->duplicate = [](PHYSFS_Io* io) -> PHYSFS_Io* {
		if (!io)
			return nullptr;

		auto* inter = (io_internal*)io->opaque;
		auto* dup = createIO(inter->filename.c_str(), inter->pos);
		return dup;
	};
	io->read = [](PHYSFS_Io* io, void* buf, PHYSFS_uint64 len) -> PHYSFS_sint64 {
		if (!io)
			return -1;

		auto* inter = (io_internal*)io->opaque;
		if (inter->data_size == inter->pos)
			return 0; // EOF

		PHYSFS_sint64 bytes_to_read = 0;
		if (inter->pos + len >= inter->data_size) {
			bytes_to_read = inter->data_size - inter->pos; // remaining data
		} else {
			bytes_to_read = len;
		}

		memcpy(buf, inter->data + inter->pos, bytes_to_read);
		inter->pos += bytes_to_read;
		return bytes_to_read;
	};

	return io;
}

bool FSConstArchiver::pathIsDir(const char* path) {
	std::string_view pstr {path};
	//for (auto&[str, data] : _storage) {
	for (auto& it : _storage) {
		// stats_with c++20 <.<

		if (it.first.compare(0, pstr.size(), pstr) == 0) {
			return true;
		}
	}

	return false;
}

const PHYSFS_Archiver* FSConstArchiver::getArchiverStruct(void) {
	static const PHYSFS_Archiver a {
		0, // version
		{ // info
			"MEM", // ext
			"in const memory 'archiver'", // desc
			"Green", // author
			"", // url
			0 // sym
		},
		&PH_openArchive,
		&PH_enumerate,
		&PH_openRead,
		&PH_openWrite,
		&PH_openAppend,
		&PH_remove,
		&PH_mkdir,
		&PH_stat,
		&PH_closeArchive
	};

	return &a;
}

void FSConstArchiver::addFile(const char* path, uint8_t* data, size_t data_size) {
	if (!path)
		return;

	// remove leading / es
	while (path[0] == '/') {
		path++;

		if (path[0] == '\0')
			return;
	}

	if (_storage.count(path)) {
		//LOGCA(std::string("error: '") + path + "' already in path");
		LOG_TRACE("'{}' already in path, overriding...", path);
	}

	_storage[path] = {data, data_size};
}

} // MM

