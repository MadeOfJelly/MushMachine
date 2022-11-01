#include <gtest/gtest.h>

#include <mm/services/filesystem.hpp>
//#include <physfs.h>
#include <nlohmann/json.hpp>

#include <SDL.h> // SDL_main

#include <mm/fs_const_archiver.hpp>

#include <mm/logger.hpp>

#include "res/test.zip.h" // zip in memory

char* argv0; // HACK

TEST(fs_service_system, general) {
	MM::Engine engine;

	engine.addService<MM::Services::FilesystemService>(argv0, "filesystem_Test");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	auto* fs_ss_ptr = engine.tryService<MM::Services::FilesystemService>();
	ASSERT_NE(fs_ss_ptr, nullptr);

	ASSERT_FALSE(fs_ss_ptr->open("i_do_not_exist.txt"));

	ASSERT_FALSE(fs_ss_ptr->exists("test_file.txt"));

	ASSERT_TRUE(PHYSFS_mountMemory(test_zip, test_zip_len, NULL, "", NULL, 0));

	ASSERT_TRUE(fs_ss_ptr->exists("test_file.txt"));

	{
		MM::Services::FilesystemService::fs_file_t file = fs_ss_ptr->open("test_file.txt", MM::Services::FilesystemService::FOPEN_t::READ);
		ASSERT_TRUE(file) << "File opened";

		ASSERT_FALSE(fs_ss_ptr->eof(file));

		ASSERT_EQ(fs_ss_ptr->tell(file), 0) << "position at start of file";


		char buffer[9] = {0};
		ASSERT_EQ(fs_ss_ptr->read(file, buffer, 8), 8) << "file length == 8";
		ASSERT_STREQ(buffer, "test :D\n");

		ASSERT_TRUE(fs_ss_ptr->eof(file));

		ASSERT_EQ(fs_ss_ptr->tell(file), 8) << "position at end of file, in this case 8";


		ASSERT_TRUE(fs_ss_ptr->close(file));
	}

	engine.disableService<MM::Services::FilesystemService>();
}

TEST(fs_service_system, fopen_eof_tell) {
	MM::Engine engine;

	engine.addService<MM::Services::FilesystemService>(argv0, "filesystem_Test");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	auto* fs_ss_ptr = engine.tryService<MM::Services::FilesystemService>();
	ASSERT_NE(fs_ss_ptr, nullptr);

	ASSERT_TRUE(PHYSFS_mountMemory(test_zip, test_zip_len, NULL, "", NULL, 0));

	MM::Services::FilesystemService::fs_file_t file = fs_ss_ptr->open("test_file.txt", MM::Services::FilesystemService::FOPEN_t::READ);
	ASSERT_TRUE(file) << "File opened";

	auto& fs = *fs_ss_ptr;

	ASSERT_FALSE(fs.eof(file));
	ASSERT_EQ(fs.tell(file), 0) << "position at start of file";

	ASSERT_TRUE(fs.seek(file, 3));

	ASSERT_FALSE(fs.eof(file));
	ASSERT_EQ(fs.tell(file), 3);

	ASSERT_FALSE(fs.seek(file, 10)) << "seek past end";

	ASSERT_FALSE(fs.eof(file));
	ASSERT_EQ(fs.tell(file), 3);

	ASSERT_TRUE(fs.close(file));

	engine.disableService<MM::Services::FilesystemService>();
}

TEST(fs_service_system, fopen_w_a_write) {
	MM::Engine engine;

	engine.addService<MM::Services::FilesystemService>(argv0, "filesystem_Test");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	auto* fs_ss_ptr = engine.tryService<MM::Services::FilesystemService>();
	ASSERT_NE(fs_ss_ptr, nullptr);

	auto& fs = *fs_ss_ptr;

	const char* filename = "test2_write_file.txt";

	// write
	{
		if (fs.exists(filename)) {
			PHYSFS_delete(filename); // TODO: add wrapper
			std::cout << "######deleted existing file" << std::endl;
		}

		auto file = fs.open(filename, MM::Services::FilesystemService::FOPEN_t::WRITE);
		ASSERT_TRUE(file) << "File opened";

		//ASSERT_TRUE(MM::fs_service_system::eof(file)); actually false
		ASSERT_EQ(fs.tell(file), 0) << "position at start of file";

		ASSERT_TRUE(fs.seek(file, 10)) << "seek past end";
		ASSERT_EQ(fs.tell(file), 10);

		const char data[4] {'a', 's', 'd', 'f'};

		ASSERT_EQ(fs.write(file, data, 4), 4);
		ASSERT_EQ(fs.tell(file), 14);
		fs.flush(file);

		ASSERT_TRUE(fs.close(file));
	}

	// append
	{
		auto file = fs.open(filename, MM::Services::FilesystemService::FOPEN_t::APPEND);
		ASSERT_TRUE(file) << "File opened";

		//ASSERT_TRUE(MM::fs_service_system::eof(file)); actually false
		ASSERT_EQ(fs.tell(file), 14) << "position at start of file";

		const char data[4] {'j', 'k', 'l', 'o'};

		ASSERT_EQ(fs.write(file, data, 4), 4);
		ASSERT_EQ(fs.tell(file), 18);
		fs.flush(file);

		ASSERT_TRUE(fs.close(file));
	}

	// read
	{
		auto file = fs.open(filename, MM::Services::FilesystemService::FOPEN_t::READ);
		ASSERT_TRUE(file) << "File opened";

		ASSERT_FALSE(fs.eof(file));
		ASSERT_EQ(fs.tell(file), 0) << "position at start of file";

		// fail
		{
			const char data[4] {'j', 'k', 'l', 'o'};

			ASSERT_FALSE(fs.write(file, data, 4) == 4);
		}

		ASSERT_TRUE(fs.seek(file, 10));

		char buffer[9] = {0};
		ASSERT_EQ(fs.read(file, buffer, 8), 8);
		ASSERT_STREQ(buffer, "asdfjklo");

		ASSERT_TRUE(fs.eof(file));

		ASSERT_TRUE(fs.close(file));
	}

	engine.disableService<MM::Services::FilesystemService>();
}

// TODO: readString

// TODO: fwrite ?

TEST(fs_service_system, read_json) {
	MM::Engine engine;

	engine.addService<MM::Services::FilesystemService>(argv0, "filesystem_Test");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	auto* fs_ss_ptr = engine.tryService<MM::Services::FilesystemService>();
	ASSERT_NE(fs_ss_ptr, nullptr);

	auto& fs = *fs_ss_ptr;

	{
		auto j = fs.readJson("does_not_exist.json");
		ASSERT_TRUE(j.empty());
	}

	ASSERT_TRUE(PHYSFS_mountMemory(test_zip, test_zip_len, NULL, "", NULL, 0));

	{
		auto j1 = fs.readJson("wall_concrete-1_se_0.2.json");
		//SPDLOG_INFO("dump: \n{}", j1.dump(4));
		ASSERT_FALSE(j1.empty());
		ASSERT_EQ(j1["type"], "tileset");
	}

	engine.disableService<MM::Services::FilesystemService>();
}

TEST(fs_service_system, read_const_json) {
	MM::Engine engine;

	engine.addService<MM::Services::FilesystemService>(argv0, "filesystem_Test");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	auto* fs_ss_ptr = engine.tryService<MM::Services::FilesystemService>();
	ASSERT_NE(fs_ss_ptr, nullptr);

	auto& fs = *fs_ss_ptr;

	{
		auto j = fs.readJson("does_not_exist.json");
		ASSERT_TRUE(j.empty());
	}

	FS_CONST_MOUNT_FILE("/json/file1.json",
R"({
	"answer": {
		"everything": 42
	},
	"happy": true,
	"list": [
		1,
		0,
		2
	],
	"name": "Niels",
	"nothing": null,
	"object": {
		"currency": "USD",
		"value": 42.99
	},
	"pi": 3.141
})")

	FS_CONST_MOUNT_FILE("/json/file2.json",
R"({
	"answer": 42
})")

	{
		auto j1 = fs.readJson("/json/file1.json");
		//std::cout << "dump: \n" << std::setw(4) << j1 << std::endl;
		ASSERT_FALSE(j1.empty());
		ASSERT_EQ(j1["happy"], true);
	}

	{
		auto j2 = fs.readJson("json/file2.json");
		//std::cout << "dump: \n" << std::setw(4) << j2 << std::endl;
		ASSERT_FALSE(j2.empty());
		ASSERT_EQ(j2["answer"], 42);
	}

	engine.disableService<MM::Services::FilesystemService>();
}

TEST(fs_service_system, write_json) {
	MM::Engine engine;

	engine.addService<MM::Services::FilesystemService>(argv0, "filesystem_Test");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	auto* fs_ss_ptr = engine.tryService<MM::Services::FilesystemService>();
	ASSERT_NE(fs_ss_ptr, nullptr);

	auto& fs = *fs_ss_ptr;

	//ASSERT_TRUE(PHYSFS_mountMemory(test_zip, test_zip_len, NULL, "", NULL, 0));

	// taken from json README.md
	nlohmann::json j = {
		{"pi", 3.141},
		{"happy", true},
		{"name", "Niels"},
		{"nothing", nullptr},
		{"answer", {
			{"everything", 42}
		}},
		{"list", {1, 0, 2}},
		{"object", {
			{"currency", "USD"},
			{"value", 42.99}
		}}
	};

	ASSERT_TRUE(fs.writeJson("write_test.json", j, 4));

	engine.disableService<MM::Services::FilesystemService>();
}

//FS_CONST_MOUNT_FILE_STATIC("static_test_file.txt",
//R"(test text.)")

int main(int argc, char** argv) {
	argv0 = argv[0];

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

