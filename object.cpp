#include "object.hpp"
#include <glad/glad.h>
#include "libjson/json.h"
#include "misc.hpp"
#include "geompack.h"

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
	// Clear everything
	destroy();

	std::vector<char> contents;
	if (load_text_file(filename, contents) == false)
		return false;

	// parse these contents using libjson. This is using doubles instead of floats because of the triangulation algorithm below
	struct parser_callback_user_data
	{
		glm::dvec2 point;
		int last_position = 0;
		std::vector <glm::dvec2> temp_double_vector;
		bool inside_point = false;
	} userdata;
	{
		json_parser parser;
		json_config config;
		auto json_callback = [](void *userdata, int type, const char *data, uint32_t length)->int
		{
			parser_callback_user_data *cast_userdata = (parser_callback_user_data *)userdata;
			switch (type)
			{
			case JSON_FLOAT:
			{
				// TODO error checking, etc...
				cast_userdata->point[cast_userdata->last_position] = atof(data);
				++cast_userdata->last_position;
				break;
			}
			case JSON_ARRAY_BEGIN:
			{
				cast_userdata->last_position = 0;
				cast_userdata->point = glm::dvec2(0.0f, 0.0f);
				cast_userdata->inside_point = true;
				break;
			}
			case JSON_ARRAY_END:
			{
				if (cast_userdata->inside_point)
				{
					cast_userdata->temp_double_vector.push_back(cast_userdata->point);
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
	}
	// Triangulate using an implementation of DELAUNAY algorithm found online at https://people.math.sc.edu/Burkardt/cpp_src/geompack/geompack.html
	std::vector <int> nod_tri, triangle_neighbour;
	nod_tri.resize(2 * userdata.temp_double_vector.size() * 3);
	triangle_neighbour.resize(2 * userdata.temp_double_vector.size() * 3);
	int tri_num;
	if (r8tris2((int)userdata.temp_double_vector.size(), (double *)userdata.temp_double_vector.data(), &tri_num, nod_tri.data(), triangle_neighbour.data()) != 0)
		return false;

	// Copy the indices to my own vector
	for (int index = 0; index < tri_num * 3; ++index)
		indices.push_back(nod_tri[index]);
	// Copy the positions to my own vector ( double to float )
	for (int index = 0; index < userdata.temp_double_vector.size(); ++index)
		points.push_back(glm::vec2(userdata.temp_double_vector[index].x, userdata.temp_double_vector[index].y));

	// Calculate boundaries ( basically for being able to know how to resize the viewport )
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

	if (vertex_buffer_glid == 0)
		glGenBuffers(1, &vertex_buffer_glid);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_glid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * points.size(), points.data(), GL_STATIC_DRAW);

	if (index_buffer_glid == 0)
		glGenBuffers(1, &index_buffer_glid);
	glBindBuffer(GL_ARRAY_BUFFER, index_buffer_glid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint16_t) * indices.size(), indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return true;
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
	indices.clear();
}
