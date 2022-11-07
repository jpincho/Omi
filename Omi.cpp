#include <glad/glad.h>
#include <glm/ext.hpp>
#include "Omi.hpp"
#include "json_stuff.hpp"
#include "window_manager.hpp"

struct object_data
{
	std::vector <glm::vec2> points;
	float min_x, max_x, min_y, max_y;
	GLuint buffer_GLID;
};

void calculate_object_boundaries(object_data &object)
{
	object.min_x = object.points[0].x;
	object.max_x = object.points[0].x;
	object.min_y = object.points[0].y;
	object.max_x = object.points[0].y;
	for (unsigned index = 1; index < object.points.size(); ++index)
	{
		if (object.min_x < object.points[index].x)
			object.min_x = object.points[index].x;
		if (object.max_x> object.points[index].x)
			object.max_x = object.points[index].x;
		if (object.min_y < object.points[index].y)
			object.min_y= object.points[index].y;
		if (object.max_y > object.points[index].y)
			object.max_y = object.points[index].y;
	}
}

void create_object_buffer(object_data &object)
{
	glm::mat4 projection_matrix = glm::ortho(object.min_x, object.max_x, object.min_y, object.max_y);
	std::vector <glm::vec4> point_buffer;
	point_buffer.resize(object.points.size());
	for (unsigned index = 0; index < point_buffer.size(); ++index)
		point_buffer[index] = glm::vec4(object.points[index].x, object.points[index].y, 0, 1.0f) * projection_matrix;
	glGenBuffers(1, &object.buffer_GLID);
	glBindBuffer(GL_ARRAY_BUFFER, object.buffer_GLID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * point_buffer.size(), point_buffer.data(), GL_STATIC_DRAW);

}

int main() 
{
	object_data object1;
	if (load_json("C:/Users/joaot/Programming/omi/Omi/points1.json", object1.points) == false)
		return -1;
	if (object1.points.empty() == true)
		return -1;
	if (wm_initialize() == false)
		return -1;
	calculate_object_boundaries(object1);
	create_object_buffer(object1);

	while (wm_is_alive())
	{
		wm_new_frame();
		wm_end_frame();
	}
	wm_shutdown();
	return 0;
}
