#pragma once

bool render_window_initialize(void);
bool render_window_shutdown(void);
bool render_window_is_alive(void);
void render_window_new_frame(void);
void render_window_end_frame(void);