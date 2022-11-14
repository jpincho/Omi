#include "object.hpp"
#include "misc.hpp"
#include "render_window.hpp"
#include "config.hpp"

int main()
{
	if (render_window_initialize() == false)
		return -1;

	object object1;
	if (object1.load_from_json_file(CURRENT_DIR "/points1.json") == false)
		return -1;

	render_window_end_frame();
	while (render_window_is_alive())
	{
		render_window_new_frame();
		object1.render();
		render_window_end_frame();
	}
	render_window_shutdown();
	return 0;
}
