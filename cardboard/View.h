#ifndef __CARDBOARD_VIEW_H_
#define __CARDBOARD_VIEW_H_

#include <wayland-server.h>
#include <wlr_cpp/types/wlr_output.h>
#include <wlr_cpp/types/wlr_output_layout.h>
#include <wlr_cpp/types/wlr_xdg_shell.h>

#include <list>

#include "Workspace.h"

struct Server;

struct View {
    struct wlr_xdg_surface* xdg_surface;
    bool mapped;
    struct wlr_box geometry;
    int x, y; // coordinates of the surface, relative to the screen layout

    Workspace::IndexType workspace_id;

    View(struct wlr_xdg_surface* xdg_surface);
    // Gets the child xdg surface of this view's toplevel xdg surface sitting under the cursor, if exists.
    // sx and sy and the given output layout relative coordinates (lx and ly), relative to that surface
    bool get_surface_under_coords(double lx, double ly, struct wlr_surface*& surface, double& sx, double& sy);

    wlr_output* get_closest_output(wlr_output_layout* layout);

    void resize(int width, int height);
};

void xdg_surface_map_handler(struct wl_listener* listener, void* data);
void xdg_surface_unmap_handler(struct wl_listener* listener, void* data);
void xdg_surface_destroy_handler(struct wl_listener* listener, void* data);
void xdg_surface_commit_handler(struct wl_listener* listener, void* data);
void xdg_toplevel_request_move_handler(struct wl_listener* listener, void* data);
void xdg_toplevel_request_resize_handler(struct wl_listener* listener, void* data);

void create_view(Server* server, View&& view);

#endif // __CARDBOARD_VIEW_H_
