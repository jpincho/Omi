#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

class object
{
private:
	std::vector <glm::vec2> points;
	std::vector <uint16_t> indices;
	GLuint vertex_buffer_glid, index_buffer_glid;

public:
	glm::vec2 min, max;
	object(void);
	~object(void);
	bool load_from_json_file(const char *filename);
	GLuint get_vertex_buffer_glid(void) const;
	GLuint get_index_buffer_glid(void) const;
	unsigned get_vertex_count(void) const;
	unsigned get_index_count(void) const;
	void destroy(void);
};