#ifndef __CARDBOARD_LAYERS_H_
#define __CARDBOARD_LAYERS_H_

extern "C" {
#include <wayland-server-core.h>
#include <wlr/types/wlr_box.h>
#include <wlr/types/wlr_surface.h>

#include <wlr_cpp_fixes/types/wlr_layer_shell_v1.h>
}

#include <array>
#include <list>

#include "OptionalRef.h"

struct Server;
struct Output;

/**
 * \brief Represents a layer_surface from the layer shell in the compositor.
 */
struct LayerSurface {
    struct wlr_layer_surface_v1* surface;
    struct wlr_box geometry;
    enum zwlr_layer_shell_v1_layer layer;
    OptionalRef<Output> output;

    bool get_surface_under_coords(double lx, double ly, struct wlr_surface*& surface, double& sx, double& sy) const;
    /// Returns true if \a output is the output of this layer surface.
    bool is_on_output(Output* output) const;

    static void commit_handler(struct wl_listener* listener, void* data);
    static void destroy_handler(struct wl_listener* listener, void* data);
    static void map_handler(struct wl_listener* listener, void* data);
    static void unmap_handler(struct wl_listener* listener, void* data);
    static void new_popup_handler(struct wl_listener* listener, void* data);
    static void output_destroy_handler(struct wl_listener* listener, void* data);
};

struct LayerSurfacePopup {
    struct wlr_xdg_popup* wlr_popup;
    LayerSurface* parent;

    void unconstrain(Server* server);

    static void destroy_handler(struct wl_listener* listener, void* data);
    static void new_popup_handler(struct wl_listener* listener, void* data);
    static void map_handler(struct wl_listener* listener, void* data);
};

using LayerArray = std::array<std::list<LayerSurface>, 4>;

/// Registers a LayerSurface.
void create_layer(Server* server, LayerSurface&& layer_surface);

/// Creates and registers a LayerSurfacePopup whose parent is \a layer_surface.
void create_layer_popup(Server* server, struct wlr_xdg_popup* wlr_popup, LayerSurface* layer_surface);

/// Arranges all the layers of an \a output.
void arrange_layers(Server* server, Output* output);

#endif // __CARDBOARD_LAYERS_H_
