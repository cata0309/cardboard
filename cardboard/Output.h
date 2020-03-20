#ifndef __CARDBOARD_OUTPUT_H_
#define __CARDBOARD_OUTPUT_H_

#include "Server.h"
#include <wayland-server.h>
#include <wlr_cpp/types/wlr_output.h>

void output_frame_handler(struct wl_listener* listener, void* data);
void output_destroy_handler(struct wl_listener* listener, void* data);

void register_output(Server* server, wlr_output*);

#endif // __CARDBOARD_OUTPUT_H_
