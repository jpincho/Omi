#include "Omi.hpp"
#include <glad/glad.h>
#include "object.hpp"
#include "render_window.hpp"
#include <glm/ext.hpp>
#include <fstream>

bool load_text_file(const char *filename, std::vector<char> &contents)
{
	std::ifstream file;
	file.open(filename, std::ios::binary | std::ios::ate);
	if (file.is_open() == false)
		return false;

	std::streamsize file_size = file.tellg();
	file.seekg(0, std::ios::beg);

	contents.resize(file_size+1);
	contents[file_size] = 0;
	file.read(contents.data(), file_size);
	file.close();
	if (!file)
		return false;
	return true;
}


const char *StringifyOpenGL(GLenum Value)
{
	switch (Value)
	{
#define STRINGIFY(X) case X: return #X;
		STRINGIFY(GL_FRAMEBUFFER_UNDEFINED);
		STRINGIFY(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT);
		STRINGIFY(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT);
		STRINGIFY(GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER);
		STRINGIFY(GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER);
		STRINGIFY(GL_FRAMEBUFFER_UNSUPPORTED);
		STRINGIFY(GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE);
		STRINGIFY(GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS);
		STRINGIFY(GL_FRAMEBUFFER_COMPLETE);

		STRINGIFY(GL_NO_ERROR);
		STRINGIFY(GL_INVALID_ENUM);
		STRINGIFY(GL_INVALID_VALUE);
		STRINGIFY(GL_INVALID_OPERATION);
		STRINGIFY(GL_STACK_OVERFLOW);
		STRINGIFY(GL_STACK_UNDERFLOW);
		STRINGIFY(GL_OUT_OF_MEMORY);
		STRINGIFY(GL_TABLE_TOO_LARGE);
	default:
		throw "Unhandled OpenGL enum";
}
#undef STRINGIFY
	}


bool CheckError(void)
{
#if defined ( ILLUSION_OPENGL_DISABLE_CHECK_ERROR )
	return true;
#else
	GLenum Error;
	bool Result = true;
	do
	{
		Error = glGetError();
		if (Error != GL_NO_ERROR)
		{
			Result = false;
			std::string ErrorString("OpenGL error - ");
			ErrorString.append(StringifyOpenGL(Error));
			printf("%s\n", ErrorString.c_str());
		}
	}
	while (Error != GL_NO_ERROR);
	return Result;
#endif
}

unsigned create_shader(const char *vertex_shader_filename, const char *geometry_shader_filename, const char *fragment_shader_filename)
{
	std::vector <char> vshader_code, gshader_code, fshader_code;
	GLuint shader_glid = 0, vshader_glid = 0, fshader_glid = 0, gshader_glid = 0;
	int compile_status = 0;
	char *sources[1];

	if (load_text_file(vertex_shader_filename, vshader_code) == false)
		goto on_error;
	sources[0] = vshader_code.data();
	vshader_glid = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vshader_glid, (GLsizei)1, sources, nullptr);
	glCompileShader(vshader_glid);
	glGetShaderiv(vshader_glid, GL_COMPILE_STATUS, &compile_status);
	if (glGetError() != GL_NO_ERROR)
		goto on_error;
	{
		std::string Infolog;
		int InfoLogLength;
		glGetShaderiv(vshader_glid, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength)
		{
			Infolog.resize(InfoLogLength);
			glGetShaderInfoLog(vshader_glid, InfoLogLength, nullptr, (GLchar *)Infolog.c_str());
			printf("%s\n", Infolog.c_str());
		}
	}
	if (compile_status != GL_TRUE)
		goto on_error;

	if (geometry_shader_filename != nullptr)
	{
		if (load_text_file(vertex_shader_filename, gshader_code) == false)
			goto on_error;
		sources[0] = gshader_code.data();
		gshader_glid = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(gshader_glid, (GLsizei)1, sources, nullptr);
		glCompileShader(gshader_glid);
		glGetShaderiv(gshader_glid, GL_COMPILE_STATUS, &compile_status);
		if (glGetError() != GL_NO_ERROR)
			goto on_error;
		if (compile_status != GL_TRUE)
			goto on_error;
	}

	if (load_text_file(fragment_shader_filename, fshader_code) == false)
		goto on_error;
	sources[0] = fshader_code.data();
	fshader_glid = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshader_glid, (GLsizei)1, sources, nullptr);
	glCompileShader(fshader_glid);
	glGetShaderiv(fshader_glid, GL_COMPILE_STATUS, &compile_status);
	if (glGetError() != GL_NO_ERROR)
		goto on_error;
	{
		std::string Infolog;
		int InfoLogLength;
		glGetShaderiv(fshader_glid, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength)
		{
			Infolog.resize(InfoLogLength);
			glGetShaderInfoLog(fshader_glid, InfoLogLength, nullptr, (GLchar *)Infolog.c_str());
			printf("%s\n", Infolog.c_str());
		}
	}
	if (compile_status != GL_TRUE)
		goto on_error;

	shader_glid = glCreateProgram();
	glAttachShader(shader_glid, vshader_glid);
	glAttachShader(shader_glid, fshader_glid);
	if (gshader_glid)
		glAttachShader(shader_glid, gshader_glid);
	glLinkProgram(shader_glid);
	GLint link_status;
	glGetProgramiv(shader_glid, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE)
		goto on_error;

	glDeleteShader(vshader_glid);
	glDeleteShader(fshader_glid);
	if (gshader_glid)
		glDeleteShader(gshader_glid);

	return shader_glid;

on_error:
	if (vshader_glid)
		glDeleteShader(vshader_glid);
	if (fshader_glid)
		glDeleteShader(fshader_glid);
	if (gshader_glid)
		glDeleteShader(gshader_glid);
	if (shader_glid)
		glDeleteProgram(shader_glid);
	return 0;
}

int main()
{
	if (render_window_initialize() == false)
		return -1;

	object object1;
	if (object1.load_from_json_file("C:/Users/joaot/Programming/omi/Omi/points1.json") == false)
		return -1;
	object1.calculate_boundaries();
	object1.upload_to_opengl();

	GLuint shader1 = create_shader("C:/Users/joaot/Programming/omi/Omi/shader1.vert", nullptr, "C:/Users/joaot/Programming/omi/Omi/shader1.frag");
	if (!shader1)
		return -1;
	GLint proj_matrix_location = glGetUniformLocation(shader1, "u_ProjectionMatrix");
	GLint vertex_attribute_location = glGetAttribLocation(shader1, "a_Position");
	glm::ivec2 extra_window_space = glm::ivec2((object1.max.x - object1.min.x) * 0.2f, (object1.max.y - object1.min.y) * 0.2f);// Add 20% padding space around the object
	glm::mat4 projection_matrix = glm::ortho(object1.min.x - extra_window_space.x, object1.max.x + extra_window_space.x, object1.min.y - extra_window_space.y, object1.max.y + extra_window_space.y );

	glUseProgram(shader1);
	assert(CheckError());
	glEnableVertexAttribArray(vertex_attribute_location);
	assert(CheckError());
	glBindBuffer(GL_ARRAY_BUFFER, object1.get_vertex_buffer_glid());
	assert(CheckError());
	glVertexAttribPointer(vertex_attribute_location, 4, GL_FLOAT, false, 0, nullptr);
	assert(CheckError());
	glUniformMatrix4fv(proj_matrix_location, 1, false, glm::value_ptr(projection_matrix));
	assert(CheckError());
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object1.get_index_buffer_glid());
	render_window_end_frame();

	while (render_window_is_alive())
	{
		render_window_new_frame();
		assert(CheckError());
		//glDrawElements(GL_LINES, object1.get_index_count(), GL_UNSIGNED_SHORT, 0);
		glDrawArrays(GL_LINE_LOOP, 0, object1.get_vertex_count());
		assert(CheckError());
		render_window_end_frame();
	}
	render_window_shutdown();
	return 0;
}
