#ifndef __CARDBOARD_VIEW_H_
#define __CARDBOARD_VIEW_H_

extern "C" {
#include <wayland-server.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_xdg_shell.h>
}

#include <list>

#include "Workspace.h"

struct Server;

/**
 * \brief Represents a normal window on the screen.
 *
 * Each View has its base in a shell (xdg-shell, layer-shell, XWayland). Currently, only
 * xdg-shell is supported. The View represents an xdg-shell toplevel surface.
 *
 * These windows may be associated with a Workspace, if its tiled. You can read about the tiled behaviour
 * in the documentation of the Workspace class.
 *
 * Views can also be mapped or unmapped, shown or hidden. Mapped and unmapped are states defined by the shell.
 * Usually, a mapped View is a window shown on the screen ready for user action, while an unmapped View is
 * deactivated and hidden (think minimized). On the other hand, shown and hidden are specific to the compositor.
 * A shown View is currently displayed on an output (screen), in an active workspace
 * (a workspace must be shown on an output to be active). It is visible. A hidden View is a View that is mapped,
 * but not visible on the screen. This happens when the View is mapped in a deactivated Workspace.
 *
 * So a shown View is always mapped, but a mapped View is not always shown. An unmapped View is always hidden,
 * but a hidden View is not always unmapped.
 *
 * There is only a flag for the mapped state (View::mapped), which is set by the unmap event handler of the
 * corresponding shell.
 *
 * \sa <a href="https://drewdevault.com/2018/07/29/Wayland-shells.html">Writing a Wayland compositor with wlroots: shells</a>
 * by Drew DeVault
 */
class View {
public:
    virtual ~View() = default;
    /**
     * \brief The size and offset of the usable working area.
     *
     * Unlike X11, Wayland only has the concept of a \c wl_surface, which is a rectangular region
     * that displays pixels and receives input. Layers give a meaning to these surfaces.
     *
     * These surfaces also don't know anything about the screen they are drawn on, not even their position
     * on the screen. As these surfaces are just rectangles with pixels, they are also used to contain
     * shadows and decorations that the user can't interact with, near the border of the surface. This is why
     * inside this rectangle there is an inner rectangle, positioned at an <tt>(x, y)</tt> offset from the
     * top-left corner of the surface with a \c width and a \c height that is the usable region of the surface.
     * This is what the user perceives as a window.
     *
     * This inner rectangle is called a \a geometry.
     *
     * \attention This property should not be modified directy, but by the resize() function.
     * Geometry modifications must be communicated to the client and the client must acknowledge
     * how much it will change. The property will change according to the desire of the client.
     * The change happens in xdg_surface_commit_handler().
     */
    struct wlr_box geometry;

    /// The id of the workspace this View is assigned to. Set to -1 if none.
    Workspace::IndexType workspace_id;

    int x, y; ///< Coordinates of the surface, relative to the output layout (root coordinates).
    bool mapped;

    /// Get the top level surface of this view.
    virtual struct wlr_surface* get_surface() = 0;

    /**
     * \brief Gets the child sub-surface of this view's toplevel xdg surface sitting under a point, if exists.
     *
     * \a lx and \a ly are the given point, in output layout relative coordinates.
     *
     * \param[out] surface the surface under the cursor, if found.
     * \param[out] sx the x coordinate of the given point, relative to the surface.
     * \param[out] sy the y coordinate of the given point, relative to the surface
     *
     * \returns \c true if there is a surface underneath the point, \c false if there isn't.
     */
    virtual bool get_surface_under_coords(double lx, double ly, struct wlr_surface*& surface, double& sx, double& sy) = 0;

    /// Requests the resize to the client. Do not assume that the client is resized afterwards.
    virtual void resize(int width, int height) = 0;

    /// Prepares the view before registering to the server by attaching some handlers and doing shell-specific stuff.
    virtual void prepare(Server* server) = 0;

    /// Set activated (focused) status to \a activated.
    virtual void set_activated(bool activated) = 0;

    virtual void for_each_surface(wlr_surface_iterator_func_t iterator, void* data) = 0;

    SafePointer<Output> get_views_output(Server* server);
    void change_output(SafePointer<Output> old_output, SafePointer<Output> new_output);

protected:
    View()
        : geometry { 0, 0, 0, 0 }
        , workspace_id(-1)
        , x(0)
        , y(0)
        , mapped(false)
    {
    }
};

/// Registers a view to the server and attaches the event handlers.
void create_view(Server* server, View* view);

#endif // __CARDBOARD_VIEW_H_
