#include "object.hpp"
#include <glad/glad.h>
#include "libjson/json.h"
#include <fstream>

object::object(void)
{
	vertex_buffer_glid = index_buffer_glid = 0;
}

object::~object(void)
{
	destroy();
}

bool object::load_from_json_file(const char *filename)
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
	calculate_boundaries();
	return true;
}

void object::calculate_boundaries(void)
{
	min = max = glm::vec2(points[0].x, points[0].y);
	for (unsigned index = 1; index < points.size(); ++index)
	{
		if (min.x < points[index].x)
			min.x = points[index].x;
		else if (max.x > points[index].x)
			max.x = points[index].x;
		if (min.y < points[index].y)
			min.y = points[index].y;
		else if (max.y > points[index].y)
			max.y = points[index].y;
	}
}

void object::upload_to_opengl(void)
{
	std::vector <glm::vec4> point_buffer;
	point_buffer.resize(points.size());
	for (unsigned index = 0; index < point_buffer.size(); ++index)
		point_buffer[index] = glm::vec4(points[index].x, points[index].y, 0.0f, 1.0f);
	if (vertex_buffer_glid == 0)
		glGenBuffers(1, &vertex_buffer_glid);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_glid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * point_buffer.size(), point_buffer.data(), GL_STATIC_DRAW);

	indices.clear();
	for (unsigned index = 1; index < point_buffer.size(); ++index)
	{
		indices.push_back(index - 1);
		indices.push_back(index);
	}
	indices.push_back(point_buffer.size() - 1); // Closing segment
	indices.push_back(0);
	if (index_buffer_glid == 0)
		glGenBuffers(1, &index_buffer_glid);
	glBindBuffer(GL_ARRAY_BUFFER, index_buffer_glid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint16_t) * indices.size(), indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLuint object::get_vertex_buffer_glid(void) const
{
	return vertex_buffer_glid;
}

GLuint object::get_index_buffer_glid(void) const
{
	return index_buffer_glid;
}

unsigned object::get_vertex_count(void) const
{
	return (unsigned)points.size();
}

unsigned object::get_index_count(void) const
{
	return (unsigned)indices.size();
}

void object::destroy(void)
{
	if (vertex_buffer_glid != 0)
		glDeleteBuffers(1, &vertex_buffer_glid);
	if (index_buffer_glid != 0)
		glDeleteBuffers(1, &index_buffer_glid);
	index_buffer_glid = vertex_buffer_glid = 0;
	points.clear();
	points.shrink_to_fit();
}
