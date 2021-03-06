#ifndef CARDBOARD_TILING_H_INCLUDED
#define CARDBOARD_TILING_H_INCLUDED

extern "C" {
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_output_layout.h>
}

#include <algorithm>
#include <list>
#include <optional>
#include <unordered_set>
#include <vector>

#include "NotNull.h"
#include "OptionalRef.h"

class View;
struct Server;
struct Output;
struct OutputManager;
struct Seat;

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
 * scrolling of the viewport is controlled by the #scroll_x variable.
 */
struct Workspace {
    using IndexType = ssize_t;
    struct Column {
        struct Tile {
            NotNullPointer<View> view;
            NotNullPointer<Column> column;
            /// This is initially 1. It represents the number of "parts" (as in "two parts water, one part sugar") when calculating the height of the tile relative to the column.
            float vertical_scale = 1.0f;
        };

        /**
         * \brief This abomination is meant to be a singleton in Column to let us write
         * loops that iterate over tiles whose views are mapped (on screen) and in normal state
         * (i.e. not fullscreened or recovering from fullscreen). Example:
         *
         * \code{.cpp}
         * for (auto& tile : columns.mapped_and_normal_tiles()) {
         *     // do stuff
         * }
         * \endcode
         */
        class MappedAndNormal {
        private:
            NotNullPointer<std::list<Tile>> tile_list;

            MappedAndNormal(NotNullPointer<std::list<Tile>> tile_list)
                : tile_list(tile_list)
            {
            }

        public:
            /// This quasi-iterator iterates over mapped and normal tiles.
            struct IteratorWrapper {
                NotNullPointer<std::list<Tile>> tile_list;
                std::list<Tile>::iterator it;

                IteratorWrapper& operator++();
                Tile& operator*();
                bool operator!=(const IteratorWrapper& other);
            };

            IteratorWrapper begin();
            IteratorWrapper end();

            friend struct Column;
        };

        std::list<Tile> tiles;

        MappedAndNormal mapped_and_normal_tiles();
        std::unordered_set<NotNullPointer<View>> get_mapped_and_normal_set();
    };

    std::list<Column> columns;
    std::list<NotNullPointer<View>> floating_views;

    /**
     * \brief The output assigned to this workspace (or the output to which this workspace is assigned).
     *
     * It's equal to \c std::nullopt if this workspace isn't assigned.
     */
    OptionalRef<Output> output;
    /// The currently full screened view, if any.
    OptionalRef<View> fullscreen_view;
    std::optional<std::pair<int, int>> fullscreen_original_size;
    Server* server;

    IndexType index;

    /**
     * \brief The offset of the viewport.
     */
    int scroll_x = 0;

    /// If set to true, arrange_workspace will not use animations.
    bool suspend_animations = false;

    /**
     * \brief Returns an iterator to the column containing \a view.
     *
     * \param view - can be null!
     */
    std::list<Column>::iterator find_column(View* view);

    /**
     * \brief Returns an iterator to the a floating view.
     *
     * \param view - can be null!
     */
    std::list<NotNullPointer<View>>::iterator find_floating(View* view);

    /**
    * \brief Adds the \a view to the right of the \a next_to view and tiles it accordingly.
    *
    * Even if \a next_to is not the only tile in a column, \a view is still going to be
    * added to its very own column on the right.
    *
    * \param transfering - set to true if we toggle the floating state
    * \param next_to - can be null!
    */
    void add_view(OutputManager& output_manager, View& view, View* next_to, bool floating = false, bool transferring = false);

    /**
    * \brief Removes \a view from the workspace and tiles the others accordingly.
    *
    * If the \a view is the sole view of a column, the column is destroyed too.
    */
    void remove_view(OutputManager& output_manager, View& view, bool transferring = false);

    /**
     * \brief Takes \a view from its column and attaches it at the end of \a column.
     *
     * \param view - must be tiled in this workspace
     * \param column - must be from this workspace
     */
    void insert_into_column(OutputManager& output_manager, View& view, Column& column);

    /**
     * \brief Takes the least (bottom-most) view from the \a column and puts it in its own column on the right.
     *
     * \param column - must be from this workspace
     */
    void pop_from_column(OutputManager& output_manager, Column& column);

    /**
    * \brief Puts windows in tiled position and takes care of fullscreen views.
    */
    void arrange_workspace(OutputManager& output_manager, bool animate = true);

    /**
     * \brief Scrolls the viewport of the workspace just enough to make the
     * entirety of \a view visible, i.e. there are no off-screen parts of it.
     *
     * \param condense - if true, if \a view is the first or last in the sequence, align it to the border
     */
    void fit_view_on_screen(OutputManager& output_manager, View& view, bool condense = false);

    /**
     * \brief From the currently visible view (those that are inside the viewport), return the one that has
     * most coverage as a ratio of its width. There may be more views having the most coverage.
     * If \a focused_view is one of them, return it directly. Can return nullptr.
     */
    OptionalRef<View> find_dominant_view(OutputManager& output_manager, Seat& seat, OptionalRef<View> focused_view);

    /**
    * \brief Returns the x coordinate of \a view in workspace coordinates.
    * The origin of the workspace plane is the top-left corner of the first window,
    * be it off-screen or not.
    */
    int get_view_wx(View&);

    /// Sets \a view as the currently fullscreen view. If null, the fullscreen view will be cleared, if any.
    void set_fullscreen_view(OutputManager& output_manager, OptionalRef<View> view);

    /// Returns true if the view is floating in this Workspace.
    bool is_view_floating(View& view);

    /**
     * \brief Assigns the workspace to an \a output.
     */
    void activate(Output& output);

    /**
     * \brief Marks the workspace as inactive: it is not assigned to any output.
     */
    void deactivate();
};

#endif //  CARDBOARD_TILING_H_INCLUDED
