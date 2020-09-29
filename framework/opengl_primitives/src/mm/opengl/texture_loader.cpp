#include "./texture_loader.hpp"

#include <stb/stb_image.h>

#ifdef MM_OPENGL_3_GLES
	#include <GLES3/gl3.h>
#else
	#include <glad/glad.h>
#endif


#include <mm/services/filesystem.hpp>
#include <mm/engine.hpp>

#include <mm/logger.hpp>
#define LOG_CRIT(...)		__LOG_CRIT(	"OpenGL", __VA_ARGS__)
#define LOG_ERROR(...)		__LOG_ERROR("OpenGL", __VA_ARGS__)
#define LOG_WARN(...)		__LOG_WARN(	"OpenGL", __VA_ARGS__)
#define LOG_INFO(...)		__LOG_INFO(	"OpenGL", __VA_ARGS__)
#define LOG_DEBUG(...)		__LOG_DEBUG("OpenGL", __VA_ARGS__)
#define LOG_TRACE(...)		__LOG_TRACE("OpenGL", __VA_ARGS__)

namespace MM::OpenGL {

std::shared_ptr<Texture> TextureLoaderFile::load(Engine& engine, const std::string& path) const {
	stbi_set_flip_vertically_on_load(1);

	int width, height, bpp;
	uint32_t handle;

	//auto buffer = stbi_load(path.c_str(), &width, &height, &bpp, 4);

	auto* fs = engine.tryService<MM::Services::FilesystemService>();

	struct cb_data_t {
		MM::Services::FilesystemService* fs;
		MM::Services::FilesystemService::fs_file_t file;
	};

	stbi_io_callbacks io_cb {
		[](void* user, char* data, int size) -> int {
			cb_data_t* fs_d = (cb_data_t*)user;

			return (int)fs_d->fs->read(fs_d->file, data, size);
		},

		[](void* user, int n) -> void {
			cb_data_t* fs_d = (cb_data_t*)user;

			fs_d->fs->seek(fs_d->file, fs_d->fs->tell(fs_d->file) + n);
		},

		[](void* user) -> int {
			cb_data_t* fs_d = (cb_data_t*)user;

			return (int)fs_d->fs->eof(fs_d->file);
		}
	};

	auto file = fs->open(path.c_str());
	if (!file) {
		LOG_ERROR("opening texture '{}'", path);
		return nullptr;
	}

	//auto* buffer = stbi_load_from_callbacks(&io_cb, file, &width, &height, &bpp, 4);
	cb_data_t data {fs, file};
	auto* buffer = stbi_load_from_callbacks(&io_cb, &data, &width, &height, &bpp, 4);

	fs->close(file);

	if (buffer == nullptr) {
		LOG_ERROR("stbi error opening texture '{}'", path);
		return nullptr;
	}

	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(buffer);

	return std::shared_ptr<Texture>(new Texture(handle, width, height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE));
}

std::shared_ptr<Texture> TextureLoaderConstBuffer::load(const uint8_t* data, size_t size) const {
	stbi_set_flip_vertically_on_load(1);

	int width, height, bpp;
	uint32_t handle;

	auto* buffer = stbi_load_from_memory(data, size, &width, &height, &bpp, 4);

	if (buffer == nullptr) {
		LOG_ERROR("stbi error opening memory texture");
		return nullptr;
	}

	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(buffer);

	return std::shared_ptr<Texture>(new Texture(handle, width, height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE));
}

} // MM::OpenGL

