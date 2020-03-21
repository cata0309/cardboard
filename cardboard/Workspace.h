#ifndef __CARDBOARD_TILING_H_
#define __CARDBOARD_TILING_H_

#include <algorithm>
#include <list>

#include <wlr_cpp/types/wlr_output_layout.h>

#include "View.h"

/**
 * \brief A Workspace is a group of tiled windows.
 *
 * Workspaces are assigned to at most one output.
 *
 * The workspace can be imagined as a plane with vertically maxed
 * views placed side-by-side. The plane has its origin in the origin of its first view.
 * Parts of this plane are visible on the output, regarded as the viewport.
 *
 * This viewport can be moved on the horizontal axis of the plane, like a sliding window.
 * As a result, only a segment of these horizontally aligned views is shown on the screen. The
 * scrolling of the viewport is controlled by the \c scroll_x variable.
 */
struct Workspace {
    struct Tile {
        View* view;
    };

    std::list<Tile> tiles;
    struct wlr_output_layout* output_layout;

    /**
     * The offset of the viewport.
     */
    int scroll_x = 0;

    void set_output_layout(struct wlr_output_layout* ol)
    {
        output_layout = ol;
    }

    auto find_tile(View* view)
    {
        return std::find_if(tiles.begin(), tiles.end(), [view](const auto& t) {
            return t.view == view;
        });
    }

    /**
    * Adds the \a view to the right of the \a next_to view and tiles it accordingly.
    */
    void add_view(View* view, View* next_to);

    /**
    * Removes \a view from the workspace and tiles the others accordingly.
    */
    void remove_view(View* view);

    /**
    * Puts the windows in tiled position.
    */
    void arrange_tiles();

    /**
    * Returns \c true if the views of the workspace overflow the output \a output.
    */
    bool is_spanning(wlr_output* output);

    /**
    * Scrolls the viewport of the workspace just enough to make the
    * entirety of \a view visible, i.e. there are no off-screen parts of it.
    */
    void fit_view_on_screen(View* view);

    /**
    * Returns the x coordinate of \a view in workspace coordinates.
    * The origin of the workspace plane is the top-left corner of the first window,
    * be it off-screen or not.
    */
    int get_view_wx(View*);
};

#endif //  __CARDBOARD_TILING_H_