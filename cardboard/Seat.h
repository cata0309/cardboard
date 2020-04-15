#ifndef __CARDBOARD_SEAT_H_
#define __CARDBOARD_SEAT_H_

extern "C" {
#include <wayland-server-core.h>
#include <wlr/types/wlr_input_device.h>
#include <wlr/types/wlr_seat.h>
}

#include <wlr_cpp_fixes/types/wlr_layer_shell_v1.h>

#include <functional>
#include <list>
#include <optional>
#include <vector>

#include "Cursor.h"
#include "Keyboard.h"
#include "View.h"
#include "SafePointer.h"

struct Server;

constexpr const char* DEFAULT_SEAT = "seat0";

struct Seat {
    struct GrabState {
        enum class Mode {
            MOVE,
            RESIZE
        } mode;
        View* view;
        double x, y;
        int width, height;
        uint32_t resize_edges;
    };

    SeatCursor cursor;
    std::optional<GrabState> grab_state;

    struct wlr_seat* wlr_seat;

    std::list<Keyboard> keyboards;
    std::list<View*> focus_stack; ///< Views ordered by the time they were focused, from most recent.

    std::optional<struct wlr_layer_surface_v1*> focused_layer;
    std::optional<struct wl_client*> exclusive_client;

    /// Sets up a newly attached input device.
    void add_input_device(Server* server, struct wlr_input_device* device);
    /// Sets up a new keyboard device, with events and keymap.
    void add_keyboard(Server* server, struct wlr_input_device* device);
    /// Attaches a new pointer device (Server* server, e.g. mouse) to the cursor.
    void add_pointer(Server* server, struct wlr_input_device* device);

    /// Returns the currently focused View. It is defined as the View currently holding keyboard focus.
    View* get_focused_view();
    /// Hides the \a view from the screen without unmapping. Happens when a Workspace is deactivated.
    void hide_view(Server* server, View* view);
    /// Gives keyboard focus to a plain surface (OR xwayland usually)
    void focus_surface(Server* server, struct wlr_surface* surface);
    /**
     * \brief Sets the focus state on \a view. Auto-scrolls the Workspace if it's tiled.
     *
     * If \a view is null, the previously focused view will be unfocused and no other view will be focused.
     */
    void focus_view(Server* server, View* view);
    /// Marks the layer as receiving keyboard focus from this seat.
    void focus_layer(Server* server, struct wlr_layer_surface_v1* layer);
    /**
     * \brief Focus the <em>offset</em>-nth tiled window to the right (or to the left if negative) of the currently
     * focused view.
     */
    void focus_by_offset(Server* server, int offset);
    /// Removes the \a view from the focus stack.
    void remove_from_focus_stack(View* view);

    void begin_interactive(View* view, GrabState::Mode mode, uint32_t edges);
    void process_cursor_motion(Server* server, uint32_t time);
    void process_cursor_move();
    void process_cursor_resize();
    void end_interactive();

    /// Returns the workspace under the cursor.
    SafePointer<Workspace> get_focused_workspace(Server* server);

    /// Considers a \a client as exclusive. Only the surfaces of the \a client will get input events.
    void set_exclusive_client(Server* server, struct wl_client* client);
    /**
     * \brief Returns true if input events can be delivered to \a surface.
     *
     * Input events can be delivered freely when there is no exclusive client. If there is
     * an exclusive client set, input may be allowed only if the surface belongs to that client.
     */
    bool is_input_allowed(struct wlr_surface* surface);

private:
    /// Sets keyboard focus on a \a surface.
    void keyboard_notify_enter(struct wlr_surface* surface);
};

void init_seat(Server* server, Seat* seat, const char* name);

/**
 * \brief Called when a client wants to set its own mouse cursor image.
 */
void seat_request_cursor_handler(struct wl_listener* listener, void* data);

/**
 * \brief Called when a client puts something in the clipboard (Ctrl-C)
 */
void seat_request_selection_handler(struct wl_listener* listener, void* data);

/**
 * \brief Called when a client puts something in the primary selection (selects some text).
 */
void seat_request_primary_selection_handler(struct wl_listener* listener, void* data);

#endif // __CARDBOARD_SEAT_H_
