//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : cl_aui_dock_art.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "cl_aui_dock_art.h"
#include "imanager.h"
#include <wx/dcmemory.h>
#include <wx/settings.h>
#include <editor_config.h>
#include "globals.h"
#include "cl_command_event.h"
#include "event_notifier.h"
#include "drawingutils.h"

// --------------------------------------------

static wxString wxAuiChopText(wxDC& dc, const wxString& text, int max_size)
{
    wxCoord x, y;

    // first check if the text fits with no problems
    dc.GetTextExtent(text, &x, &y);
    if(x <= max_size) return text;

    size_t i, len = text.Length();
    size_t last_good_length = 0;
    for(i = 0; i < len; ++i) {
        wxString s = text.Left(i);
        s += wxT("...");

        dc.GetTextExtent(s, &x, &y);
        if(x > max_size) break;

        last_good_length = i;
    }

    wxString ret = text.Left(last_good_length);
    ret += wxT("...");
    return ret;
}

// ------------------------------------------------------------

clAuiDockArt::clAuiDockArt(IManager* manager)
    : m_manager(manager)
{
}

clAuiDockArt::~clAuiDockArt() {}

void clAuiDockArt::DrawPaneButton(wxDC& dc,
                                  wxWindow* window,
                                  int button,
                                  int button_state,
                                  const wxRect& _rect,
                                  wxAuiPaneInfo& pane)
{
    wxAuiDefaultDockArt::DrawPaneButton(dc, window, button, button_state, _rect, pane);
}

void
clAuiDockArt::DrawCaption(wxDC& dc, wxWindow* window, const wxString& text, const wxRect& rect, wxAuiPaneInfo& pane)
{
    wxRect tmpRect(wxPoint(0, 0), rect.GetSize());

    // Hackishly prevent assertions on linux
    if(tmpRect.GetHeight() == 0) tmpRect.SetHeight(1);
    if(tmpRect.GetWidth() == 0) tmpRect.SetWidth(1);

    wxBitmap bmp(tmpRect.GetSize());
    {
        wxMemoryDC memDc;
        memDc.SelectObject(bmp);
        

        wxGCDC gdc;
        wxDC* pDC = NULL;
        if(!DrawingUtils::GetGCDC(memDc, gdc)) {
            pDC = &memDc;
        } else {
            pDC = &gdc;
        }
        
        // Prepare the colours
        wxColour bgColour, penColour, textColour;
        textColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
        bgColour = "#dcdcdc"; // Same as the notebook background colour
        penColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
        penColour = bgColour;
        
        wxFont f = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        pDC->SetFont(f);
        pDC->SetPen(penColour);
        pDC->SetBrush(bgColour);
        pDC->DrawRectangle(tmpRect);

        pDC->SetPen(penColour);
        pDC->SetBrush(*wxTRANSPARENT_BRUSH);
        pDC->DrawRectangle(tmpRect);

        int caption_offset = 0;
        if(pane.icon.IsOk()) {
            DrawIcon(gdc, tmpRect, pane);
            caption_offset += pane.icon.GetWidth() + 3;
        } else {
            caption_offset = 3;
        }
        pDC->SetTextForeground(textColour);
        wxCoord w, h;
        pDC->GetTextExtent(wxT("ABCDEFHXfgkj"), &w, &h);

        wxRect clip_rect = tmpRect;
        clip_rect.width -= 3; // text offset
        clip_rect.width -= 2; // button padding
        if(pane.HasCloseButton()) clip_rect.width -= m_buttonSize;
        if(pane.HasPinButton()) clip_rect.width -= m_buttonSize;
        if(pane.HasMaximizeButton()) clip_rect.width -= m_buttonSize;

        wxString draw_text = wxAuiChopText(gdc, text, clip_rect.width);

        wxSize textSize = pDC->GetTextExtent(draw_text);
        pDC->SetTextForeground(textColour);
        pDC->DrawText(draw_text, tmpRect.x + 3 + caption_offset, tmpRect.y + ((tmpRect.height - textSize.y) / 2));
        memDc.SelectObject(wxNullBitmap);
    }
    
    dc.DrawBitmap(bmp, rect.x, rect.y, true);
}

void clAuiDockArt::DrawBackground(wxDC& dc, wxWindow* window, int orientation, const wxRect& rect)
{
    wxUnusedVar(window);
    wxUnusedVar(orientation);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(DrawingUtils::GetAUIPaneBGColour());
    dc.DrawRectangle(rect);
}

void clAuiDockArt::DrawBorder(wxDC& dc, wxWindow* window, const wxRect& rect, wxAuiPaneInfo& pane)
{
    wxColour penColour;
#ifdef __WXMAC__
    penColour = DrawingUtils::GetAUIPaneBGColour();
#else
    penColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
#endif
    dc.SetPen(wxColour("#dcdcdc"));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(rect);
}

void clAuiDockArt::DrawSash(wxDC& dc, wxWindow* window, int orientation, const wxRect& rect)
{
    wxUnusedVar(window);
    wxUnusedVar(orientation);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    dc.DrawRectangle(rect);
}
