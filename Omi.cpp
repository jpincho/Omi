#include "object.hpp"
#include "misc.hpp"
#include "render_window.hpp"
#include "config.hpp"
#include <glm/ext.hpp>

int main()
{
	if (render_window_initialize() == false)
		return -1;

	object object1;
	if (object1.load_from_json_file(CURRENT_DIR "/points1.json") == false)
		return -1;

	GLuint shader1 = create_shader(CURRENT_DIR "/shader1.vert",
		nullptr,
		CURRENT_DIR "/shader1.frag");
	if (!shader1)
		return -1;
	GLint proj_matrix_location = glGetUniformLocation(shader1, "u_ProjectionMatrix");
	GLint vertex_attribute_location = glGetAttribLocation(shader1, "a_Position");
	GLint color_location = glGetUniformLocation(shader1, "u_Color");

	glm::mat4 projection_matrix = glm::scale(glm::ortho((float)object1.min.x, (float)object1.max.x, (float)object1.min.y, (float)object1.max.y), glm::vec3(0.9f));

	glUseProgram(shader1);
	assert(check_opengl_error());
	glEnableVertexAttribArray(vertex_attribute_location);
	assert(check_opengl_error());
	glBindBuffer(GL_ARRAY_BUFFER, object1.get_vertex_buffer_glid());
	assert(check_opengl_error());
	glVertexAttribPointer(vertex_attribute_location, 2, GL_FLOAT, false, 0, nullptr);
	assert(check_opengl_error());
	glUniformMatrix4fv(proj_matrix_location, 1, false, glm::value_ptr(projection_matrix));
	assert(check_opengl_error());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object1.get_index_buffer_glid());
	render_window_end_frame();
	while (render_window_is_alive())
	{
		render_window_new_frame();
		assert(check_opengl_error());
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glUniform4f(color_location, 0.5f, 0.5f, 0.5f, 1.0f);
		glDrawElements(GL_TRIANGLES, object1.get_index_count(), GL_UNSIGNED_SHORT, 0);
		assert(check_opengl_error());

		assert(check_opengl_error());
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glUniform4f(color_location, 0.0f, 0.0f, 1.0f, 1.0f);
		glDrawElements(GL_LINE_LOOP, object1.get_index_count(), GL_UNSIGNED_SHORT, 0);
		assert(check_opengl_error());
		render_window_end_frame();
	}
	render_window_shutdown();
	return 0;
}
