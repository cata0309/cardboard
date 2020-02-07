#include <wayland-server.h>
#include <wlr_cpp/types/wlr_input_device.h>

#include "Keyboard.h"
#include "Server.h"

void modifiers_handler(struct wl_listener* listener, [[maybe_unused]] void* data)
{
    Server* server = get_server(listener);
    auto device = get_listener_data<wlr_input_device*>(listener);

    wlr_seat_set_keyboard(server->seat, device);
    // send modifiers to the client
    wlr_seat_keyboard_notify_modifiers(server->seat, &device->keyboard->modifiers);
}

void key_handler(struct wl_listener* listener, void* data)
{
    Server* server = get_server(listener);
    auto device = get_listener_data<wlr_input_device*>(listener);

    auto* event = static_cast<struct wlr_event_keyboard_key*>(data);

    // TODO: handle compositor keybinds

    wlr_seat_set_keyboard(server->seat, device);
    wlr_seat_keyboard_notify_key(server->seat, event->time_msec, event->keycode, event->state);
}
