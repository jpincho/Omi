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
	static GLuint shader_handle;
	static GLint vertex_attribute_location, color_location;
	glm::vec2 min, max;

public:
	object(void);
	~object(void);
	bool load_from_json_file(const char *filename);
	void destroy(void);
	void render(void);
};