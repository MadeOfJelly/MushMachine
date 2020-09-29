#include "soloud_filesystem_file_impl.hpp"

#include <mm/logger.hpp>
#define LOGSLF(x) LOG("SoLoudFilesystemFile", x)

namespace MM::SoLoud {

FilesystemFile::FilesystemFile(Services::FilesystemService::fs_file_t file, Engine& engine)
	: _file_handle(file),
	_fs_ptr(engine.tryService<Services::FilesystemService>()) {

	if (!_fs_ptr) {
		LOGSLF("error: engine has no FilesystemService!");
	}

	if (!_file_handle) {
		LOGSLF("error: created with invalid file handle!");
		return;
	}
}

FilesystemFile::~FilesystemFile(void) {
}

int FilesystemFile::eof(void) {
	if (!_file_handle) {
		LOGSLF("error: invalid file handle!");
		return 0;
	}

	return _fs_ptr->eof(_file_handle);
}

unsigned int FilesystemFile::read(unsigned char* aDst, unsigned int aBytes) {
	if (!_file_handle) {
		LOGSLF("error: invalid file handle!");
		return 0;
	}

	return _fs_ptr->read(_file_handle, aDst, aBytes);
}

unsigned int FilesystemFile::length(void) {
	if (!_file_handle) {
		LOGSLF("error: invalid file handle!");
		return 0;
	}

	return _fs_ptr->length(_file_handle);
}

void FilesystemFile::seek(int aOffset) {
	if (!_file_handle) {
		LOGSLF("error: invalid file handle!");
		return;
	}

	_fs_ptr->seek(_file_handle, aOffset);
}

unsigned int FilesystemFile::pos(void) {
	if (!_file_handle) {
		LOGSLF("error: invalid file handle!");
		return 0;
	}

	return _fs_ptr->tell(_file_handle);
}

} // MM::SoLoud

