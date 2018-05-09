// Generate a table using wxDC.
//
// Copyright (C) 2015, 2016, 2017, 2018 Gregory W. Chicares.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
//
// http://savannah.nongnu.org/projects/lmi
// email: <gchicares@sbcglobal.net>
// snail: Chicares, 186 Belle Woods Drive, Glastonbury CT 06033, USA

#ifndef wx_table_generator_hpp
#define wx_table_generator_hpp

#include "config.hpp"

#include "oecumenic_enumerations.hpp"

#include <wx/dc.h>
#include <wx/font.h>

#include <cstddef>                      // size_t
#include <string>
#include <vector>

/// Simplifies outputting tabular data on wxDC.
///
/// To create a table, columns must be initialized first by calling
/// add_column() for each of them once. After this, output_header() and
/// output_row() can be called reusing the same pos_y argument which contains
/// the coordinate of the top of the header or row to output and is updated to
/// correspond to the value for the next row by these functions.

class wx_table_generator
{
    class column_info;

  public:
    // The life time of the specified wxDC must be greater than the life time
    // of this object itself and nothing should be using it while this object
    // does (as it changes its attributes).
    //
    // The table has the given total width and starts at the left margin.
    wx_table_generator(wxDC& dc, int left_margin, int total_width);

    wx_table_generator(wx_table_generator const&);

    ~wx_table_generator();

    // Adds a column to the table. The total number of added columns determines
    // the cardinality of the 'values' argument in output_row() calls.
    //
    // Providing an empty header suppresses the column display, while still
    // taking it into account in output_row(), providing a convenient way to
    // hide a single column without changing the data representation.
    //
    // Each column must either have a fixed width, specified as the width of
    // the longest text that may appear in this column, or be expandable
    // meaning that the rest of the page width is allocated to it which will be
    // the case if widest_text is empty.
    //
    // Notice that column headers may be multiline strings.
    void add_column(std::string const& header, std::string const& widest_text);

    // Render the headers at the given position and update it.
    void output_header
        (int*                         pos_y
        ,oenum_render_or_only_measure output_mode = oe_render
        );

    // Render a super-header, i.e. a header spanning over several columns. The
    // columns range is specified as a close/open interval, as usual in C++.
    // The header string may be multiline, just as with normal headers.
    void output_super_header
        (std::string const&           header
        ,std::size_t                  begin_column
        ,std::size_t                  end_column
        ,int*                         pos_y
        ,oenum_render_or_only_measure output_mode = oe_render
        );

    // Render a row with the given values at the given position and update it.
    // The values here can be single-line only and there must be exactly the
    // same number of them as the number of columns.
    void output_row(int* pos_y, std::vector<std::string> const values);

    // Render a single highlighted (by shading its background) cell with the
    // given string displayed in it (always centered).
    void output_highlighted_cell
        (std::size_t        column
        ,int                y
        ,std::string const& value
        );

    // Return the height of a single table row.
    int row_height() const;

    // Return the rectangle containing the cell area.
    wxRect cell_rect(std::size_t column, int y);

    // Return the rectangle adjusted for the text contents of the cell: it is
    // more narrow than the full cell rectangle to leave margins around the
    // text and its vertical position is adjusted so that it can be directly
    // passed to wxDC::DrawLabel().
    wxRect text_rect(std::size_t column, int y);

    // Output a horizontal separator line across the specified columns,
    // using the usual C++ close/open interval convention.
    void output_horz_separator
        (std::size_t                  begin_column
        ,std::size_t                  end_column
        ,int                          y
        ,oenum_render_or_only_measure output_mode = oe_render
        );

    // Output a vertical separator line before the given column. Notice that
    // the column index here may be equal to the number of columns in order to
    // output a separator after the last column.
    void output_vert_separator(std::size_t before_column, int y);

    // Use condensed style: don't draw separators between rows and make them
    // smaller.
    void use_condensed_style();

    // By default, columns are centered if they have fixed size or left-aligned
    // otherwise. By calling this method, this alignment auto-detection is
    // turned off and all columns are right-aligned.
    void align_right();

  private:
    // Return the font used for the headers.
    wxFont get_header_font() const;

    int do_get_cell_x(std::size_t column);

    void do_output_horz_separator(int x1, int x2, int y );
    void do_output_vert_separator(int x , int y1, int y2);

    void do_compute_column_widths();

    void do_output_values
        (int&                            pos_x
        ,int&                            pos_y
        ,std::vector<std::string> const& values
        );

    // Const private accessors. Used in the implementation to
    // distinguish access from mutation.
    int column_margin() const;
    std::vector<column_info> const& all_columns() const;

    wxDC& dc_;

    int left_margin_;
    int total_width_;

    // These values could be recomputed, but cache them for performance.
    int const char_height_;
    int row_height_;
    int column_margin_;

    std::vector<column_info> all_columns_;

    // Initially false, set to true after do_compute_column_widths()
    // has been called to make all column_info::col_width_ values valid.
    bool column_widths_already_computed_;

    // Maximal number of lines in any column header, initially 1 but can be
    // higher if multiline headers are used.
    std::size_t max_header_lines_;

    // If false, separator lines are not drawn automatically (they can still be
    // drawn by calling output_horz_separator() or output_vert_separator()
    // explicitly).
    bool draw_separators_ = true;

    // If true, headers are drawn in bold.
    bool use_bold_headers_ = true;

    // If true, force right alignment for all columns instead of centering them
    // automatically if they have fixed size.
    bool align_right_ = false;
};

#endif // wx_table_generator_hpp
