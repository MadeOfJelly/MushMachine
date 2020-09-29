#pragma once

#include <soloud_file.h>

#include <mm/services/filesystem.hpp>

namespace MM::SoLoud {

class FilesystemFile : public ::SoLoud::File {
	private:
		Services::FilesystemService::fs_file_t _file_handle;
		Services::FilesystemService* _fs_ptr; // convinience

	public:
		FilesystemFile(void) = delete;
		FilesystemFile(Services::FilesystemService::fs_file_t file, Engine& engine);
		FilesystemFile(const char* file_path, Engine& engine);

		~FilesystemFile(void);

	// soloud api
	public:
		int eof(void) override;
		unsigned int read(unsigned char* aDst, unsigned int aBytes) override;
		unsigned int length(void) override;
		void seek(int aOffset) override;
		unsigned int pos(void) override;

};

} // MM

