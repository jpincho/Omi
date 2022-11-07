#include "json_stuff.hpp"
#include "libjson/json.h"
#include <fstream>


bool load_json(const char *filename, std::vector <glm::vec2> &points)
{
	std::vector<char> contents;
	// Load file into memory
	{
		std::ifstream json_file;
		json_file.open(filename, std::ios::binary | std::ios::ate);
		if (json_file.is_open() == false)
			return false;

		std::streamsize file_size = json_file.tellg();
		json_file.seekg(0, std::ios::beg);

		contents.resize(file_size);
		json_file.read(contents.data(), file_size);
		json_file.close();
		if (!json_file)
			return false;
	}

	// parse these contents
	json_parser parser;
	json_config config;
	struct parser_callback_user_data
	{
		glm::vec2 point;
		int last_position = 0;
		std::vector <glm::vec2> *vector_pointer;
		bool inside_point = false;
	} userdata;
	userdata.vector_pointer = &points;
	auto json_callback = [](void *userdata, int type, const char *data, uint32_t length)->int
	{
		parser_callback_user_data *cast_userdata = (parser_callback_user_data *)userdata;
		switch (type)
		{
		case JSON_FLOAT:
		{
			// TODO error checking, etc...
			cast_userdata->point[cast_userdata->last_position] = (float)atof(data);
			++cast_userdata->last_position;
			break;
		}
		case JSON_ARRAY_BEGIN:
		{
			cast_userdata->last_position = 0;
			cast_userdata->point = glm::vec2(0.0f, 0.0f);
			cast_userdata->inside_point = true;
			break;
		}
		case JSON_ARRAY_END:
		{
			if (cast_userdata->inside_point)
			{
				cast_userdata->vector_pointer->push_back(cast_userdata->point);
				cast_userdata->inside_point = false;
			}
			break;
		}
		default:// ignore everything other than int's and float's
			break;
		}
		printf("processed data %s, type %d\n", data, type);
		return 0;
	};
	memset(&config, 0, sizeof(json_config));
	config.max_nesting = 0;
	config.max_data = 0;
	config.allow_c_comments = 1;
	config.allow_yaml_comments = 1;
	if (json_parser_init(&parser, &config, json_callback, &userdata) != 0)
		return false;

	if (json_parser_string(&parser, contents.data(), (uint32_t)contents.size(), nullptr) != 0)
		return false;

	if (json_parser_is_done(&parser) == 0)
		return false;

	json_parser_free(&parser);
	return true;
}