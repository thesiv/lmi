// PDF generation helpers.
//
// Copyright (C) 2017 Gregory W. Chicares.
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

#include "pchfile_wx.hpp"

#include "pdf_writer_wx.hpp"

#include "alert.hpp"
#include "html.hpp"

#include <wx/html/htmlcell.h>

namespace
{

// These margins are arbitrary and can be changed to conform to subjective
// preferences.
constexpr int horz_margin = 24;
constexpr int vert_margin = 36;

wxPrintData make_print_data
    (wxString const&    output_filename
    ,wxPrintOrientation orientation
    )
{
    wxPrintData print_data;
    print_data.SetPaperId(wxPAPER_LETTER);
    print_data.SetFilename(output_filename);
    print_data.SetOrientation(orientation);
    return print_data;
}

} // Unnamed namespace.

pdf_writer_wx::pdf_writer_wx
    (wxString const&    output_filename
    ,wxPrintOrientation orientation
    ,std::array<int, 7> const* html_font_sizes
    )
    :print_data_        {make_print_data(output_filename, orientation)}
    ,pdf_dc_            {print_data_}
    ,html_parser_       {nullptr}
    ,total_page_size_   {pdf_dc_.GetSize()}
{
    // Ensure that the output is independent of the current display resolution:
    // it seems that this is only the case with the PDF map mode and wxDC mode
    // different from wxMM_TEXT.
    pdf_dc_.SetMapModeStyle(wxPDF_MAPMODESTYLE_PDF);

    // For simplicity, use points for everything: font sizers are expressed in
    // them anyhow, so it's convenient to use them for everything else too.
    pdf_dc_.SetMapMode(wxMM_POINTS);

    pdf_dc_.StartDoc(wxString()); // Argument is not used.
    pdf_dc_.StartPage();

    // Use a standard PDF Helvetica font (without embedding any custom fonts in
    // the generated file, the only other realistic choice is Times New Roman).
    pdf_dc_.SetFont
        (wxFontInfo
            (html_font_sizes
                ? html_font_sizes->at(2)
                : 8
            )
            .Family(wxFONTFAMILY_SWISS)
            .FaceName("Helvetica")
        );

    // Create an HTML parser to allow easily adding HTML contents to the output.
    html_parser_.SetDC(&pdf_dc_);
    if(html_font_sizes)
        {
        html_parser_.SetFonts
            ("Helvetica"
            ,"Courier"
            ,html_font_sizes->data()
            );
        }
    else
        {
        html_parser_.SetStandardFonts
            (pdf_dc_.GetFont().GetPointSize()
            ,"Helvetica"
            ,"Courier"
            );
        }
}

/// Output an image at the given scale into the PDF.
///
/// The scale specifies how many times the image should be shrunk:
/// scale > 1 makes the image smaller, while scale < 1 makes it larger.
///
/// Updates pos_y by increasing it by the height of the specified
/// image at the given scale.

void pdf_writer_wx::output_image
    (wxImage const&   image
    ,char const*      image_name
    ,double           scale
    ,int              x
    ,int*             pos_y
    ,enum_output_mode output_mode
    )
{
    int const y = wxRound(image.GetHeight() / scale);

    switch(output_mode)
        {
        case e_output_normal:
            {
            // Use wxPdfDocument API directly as wxDC doesn't provide a way to
            // set the image scale at PDF level and also because passing via
            // wxDC wastefully converts wxImage to wxBitmap only to convert it
            // back to wxImage when embedding it into the PDF.
            wxPdfDocument* const pdf_doc = pdf_dc_.GetPdfDocument();
            LMI_ASSERT(pdf_doc);

            pdf_doc->SetImageScale(scale);
            pdf_doc->Image(image_name, image, x, *pos_y);
            pdf_doc->SetImageScale(1);
            }
            break;
        case e_output_measure_only:
            // Do nothing.
            break;
        default:
            {
            alarum() << "Case " << output_mode << " not found." << LMI_FLUSH;
            }
        }

    *pos_y += y;
}

/// Render, or just pretend rendering in order to measure it, the given HTML
/// contents at the specified position wrapping it at the given width.
/// Return the height of the output (using this width).

int pdf_writer_wx::output_html
    (int x
    ,int y
    ,int width
    ,html::text const& html
    ,enum_output_mode output_mode
    )
{
    auto const html_str = wxString::FromUTF8(html.as_html());
    std::unique_ptr<wxHtmlContainerCell> const cell
        (static_cast<wxHtmlContainerCell*>(html_parser_.Parse(html_str))
        );
    LMI_ASSERT(cell);

    cell->Layout(width);
    switch(output_mode)
        {
        case e_output_normal:
            {
            wxHtmlRenderingInfo rendering_info;
            cell->Draw
                (pdf_dc_
                ,x
                ,y
                ,0
                ,std::numeric_limits<int>::max()
                ,rendering_info
                );
            }
            break;
        case e_output_measure_only:
            // Do nothing.
            break;
        default:
            {
            alarum() << "Case " << output_mode << " not found." << LMI_FLUSH;
            }
        }

    return cell->GetHeight();
}

int pdf_writer_wx::get_horz_margin() const
{
    return horz_margin;
}

int pdf_writer_wx::get_vert_margin() const
{
    return vert_margin;
}

int pdf_writer_wx::get_page_width()  const
{
    return total_page_size_.x - 2 * horz_margin;
}

int pdf_writer_wx::get_total_width() const
{
    return total_page_size_.x;
}

int pdf_writer_wx::get_page_height() const
{
    return total_page_size_.y - 2 * vert_margin;
}

int pdf_writer_wx::get_page_bottom() const
{
    return total_page_size_.y - vert_margin;
}

pdf_writer_wx::~pdf_writer_wx()
{
    // This will finally generate the PDF file.
    pdf_dc_.EndDoc();
}