#include "misc.hpp"
#include <fstream>

bool load_text_file(const char *filename, std::vector<char> &contents)
{
	std::ifstream file;
	file.open(filename, std::ios::binary | std::ios::ate);
	if (file.is_open() == false)
		return false;

	std::streamsize file_size = file.tellg();
	file.seekg(0, std::ios::beg);

	contents.resize(file_size);
	//contents[file_size] = 0;
	file.read(contents.data(), file_size);
	file.close();
	if (!file)
		return false;
	return true;
}

const char *stringify_opengl_error(GLenum Value)
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

bool check_opengl_error(void)
{
	GLenum error;
	bool result = true;
	do
	{
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			result = false;
			std::string error_string("OpenGL error - ");
			error_string.append(stringify_opengl_error(error));
			printf("%s\n", error_string.c_str());
		}
	}
	while (error != GL_NO_ERROR);
	return result;
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
	if (compile_status != GL_TRUE)
		goto on_error;

	{
		std::string Infolog;
		int InfoLogLength;
		glGetShaderiv(vshader_glid, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength)
		{
			Infolog.resize(InfoLogLength);
			glGetShaderInfoLog(vshader_glid, InfoLogLength, nullptr, (GLchar *)Infolog.c_str());
			printf("GL ERROR - %s\n", Infolog.c_str());
		}
	}
	if (geometry_shader_filename != nullptr)
	{
		if (load_text_file(geometry_shader_filename, gshader_code) == false)
			goto on_error;
		sources[0] = gshader_code.data();
		gshader_glid = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(gshader_glid, (GLsizei)1, sources, nullptr);
		glCompileShader(gshader_glid);
		glGetShaderiv(gshader_glid, GL_COMPILE_STATUS, &compile_status);
		if (glGetError() != GL_NO_ERROR)
			goto on_error;
		{
			std::string Infolog;
			int InfoLogLength;
			glGetShaderiv(gshader_glid, GL_INFO_LOG_LENGTH, &InfoLogLength);
			if (InfoLogLength)
			{
				Infolog.resize(InfoLogLength);
				glGetShaderInfoLog(gshader_glid, InfoLogLength, nullptr, (GLchar *)Infolog.c_str());
				printf("GL ERROR - %s\n", Infolog.c_str());
			}
		}
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
	{
		std::string Infolog;
		int InfoLogLength;
		glGetProgramiv(shader_glid, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength)
		{
			Infolog.resize(InfoLogLength);
			glGetProgramInfoLog(shader_glid, InfoLogLength, nullptr, (GLchar *)Infolog.c_str());
			printf("ERROR - %s\n", Infolog.c_str());
			goto on_error;
		}
	}

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