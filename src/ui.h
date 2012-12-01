/*  StrEdit

    A minimalist TES V: Skyrim string table editor designed for mod translators.

    Copyright (C) 2012    WrinklyNinja

    This file is part of StrEdit.

    StrEdit is free software: you can redistribute
    it and/or modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version.

    StrEdit is distributed in the hope that it will
    be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with StrEdit.  If not, see
    <http://www.gnu.org/licenses/>.
*/

#ifndef __STREDIT_UI_H__
#define __STREDIT_UI_H__

#include "backend.h"

#include <string>
#include <boost/format.hpp>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#       include <wx/wx.h>
#endif
#include <wx/listctrl.h>
#include <wx/filepicker.h>
#include <wx/srchctrl.h>


//UI helper functions.
namespace stredit {
    wxString translate(char * cstr);
    wxString translate(std::string str);
    wxString FromUTF8(std::string str);
    wxString FromUTF8(boost::format f);
}

//wxWidgets element IDs.
enum {
    //Main window.
    SEARCH_Strings = wxID_HIGHEST + 1, // declares an id which will be used to call our button
    LIST_Strings,
    TEXT_Original,
    TEXT_New
};

class StrEditApp : public wxApp {
public:
    bool OnInit();
};

class VirtualList : public wxListCtrl {
public:
    VirtualList(wxWindow * parent, wxWindowID id);

    void OnClose(wxCloseEvent& event);

    std::vector<stredit::str_data> internalData;
    std::vector<int> filter;
    int currentSelectionIndex;
protected:
    wxString OnGetItemText(long item, long column) const;
    wxListItemAttr * OnGetItemAttr(long item) const;
    wxListItemAttr * attr;

    DECLARE_EVENT_TABLE()
};

//Main window class.
class MainFrame : public wxFrame {
public:
    MainFrame(const wxChar *title);
    void OnOpenFile(wxCommandEvent& event);
    void OnSaveFile(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnViewReadme(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnStringSelect(wxListEvent& event);
    void OnStringDeselect(wxListEvent& event);
    void OnStringFilter(wxCommandEvent& event);
    void OnStringFilterCancel(wxCommandEvent& event);

    void SaveFile();
private:
    VirtualList * stringList;
    wxSearchCtrl * searchBox;  //Could be used for filtering the string list.
    wxTextCtrl * originalTextBox;
    wxTextCtrl * newTextBox;

    std::string filePath;
    bool stringsEdited;

    DECLARE_EVENT_TABLE()
};

class OpenDialog : public wxDialog {
public:
    OpenDialog(wxWindow * parent, wxWindowID id, const wxString& title);

    wxString GetSourcePath() const;
    wxString GetTransPath() const;
    wxString GetNewSourcePath() const;

    int GetSourceFallbackEnc() const;
    int GetTransFallbackEnc() const;
    int GetNewSourceFallbackEnc() const;
private:
    wxFilePickerCtrl * orgPicker;
    wxFilePickerCtrl * refPicker;
    wxFilePickerCtrl * tarPicker;
    wxChoice * orgFallbackEncChoice;
    wxChoice * refFallbackEncChoice;
    wxChoice * tarFallbackEncChoice;
};

#endif
