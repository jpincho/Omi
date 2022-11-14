#include <vector>
#include <glad/glad.h>

bool load_text_file(const char *filename, std::vector<char> &contents);
bool check_opengl_error(void);
unsigned create_shader(const char *vertex_shader_filename, const char *geometry_shader_filename, const char *fragment_shader_filename);