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
#include <wx/progdlg.h>


//UI helper functions.
namespace stredit {
    wxString translate(const std::string str);
    wxString FromUTF8(const std::string str);
    wxString FromUTF8(const boost::format f);
}

//wxWidgets element IDs.
enum {
    //Main window.
    SEARCH_Strings = wxID_HIGHEST + 1, // declares an id which will be used to call our button
    LIST_Strings,
    MENU_MachineTranslate,
    MENU_ImportXML,
    MENU_ExportXML
};

class StrEditApp : public wxApp {
public:
    bool OnInit();
};

class VirtualList : public wxListCtrl {
public:
    VirtualList(wxWindow * parent, wxWindowID id);

    void OnClose(wxCloseEvent& event);

    void SetItems(const wxString sourcePath, const int sourceEnc,
                  const wxString transPath = "", const int transEnc = 1252);
    void SetItems(const wxString xmlPath);

    void FuzzyTranslate(const boost::unordered_map<std::string, std::string>& stringMap, wxProgressDialog * pd);

    int GetTotalItemCount() const;
    int GetHiddenCount() const;
    int GetFuzzyCount() const;
    int GetTranslatedCount() const;

    const std::vector<stredit::str_data>& GetItems() const;

    bool IsContentEdited() const;
    void ResetEditedFlags();

    void ApplyFilter(const wxString str);
    bool IsFiltered() const;

    void UpdateSelectedItem(const wxString str);
    void SetSelectedIndex(const int i);
    stredit::str_data GetSelectedItem() const;
protected:
    wxString OnGetItemText(long item, long column) const;
    wxListItemAttr * OnGetItemAttr(long item) const;
    wxListItemAttr * attr;
private:
    std::vector<stredit::str_data> internalData;
    std::vector<int> filter;
    int currentSelectionIndex;

    DECLARE_EVENT_TABLE()
};

//Main window class.
class MainFrame : public wxFrame {
public:
    MainFrame(const wxChar *title);
    void OnOpenFile(wxCommandEvent& event);
    void OnSaveFile(wxCommandEvent& event);
    void OnMachineTranslate(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnViewReadme(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnImportXML(wxCommandEvent& event);
    void OnExportXML(wxCommandEvent& event);

    void OnStringSelect(wxListEvent& event);
    void OnStringDeselect(wxListEvent& event);
    void OnStringFilter(wxCommandEvent& event);
    void OnStringFilterCancel(wxCommandEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    void SaveFile();
    void Reset();
    void UpdateStatus();
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

    int GetSourceFallbackEnc() const;
    int GetTransFallbackEnc() const;
private:
    wxFilePickerCtrl * srcPicker;
    wxFilePickerCtrl * transPicker;
    wxChoice * srcFallbackEncChoice;
    wxChoice * transFallbackEncChoice;
};

namespace stredit {
    struct vocab_pair {
        std::string source;
        std::string trans;
        int sourceFallbackEnc;
        int transFallbackEnc;
    };
}

class VocabDialog : public wxDialog {
public:
    VocabDialog(wxWindow * parent, wxWindowID id, const wxString& title);

    void OnAddPair(wxCommandEvent& event);
    void OnRemovePair(wxCommandEvent& event);

    std::vector<stredit::vocab_pair> GetVocabPairs() const;
private:
    wxButton * addButton;
    wxButton * removeButton;
    wxListCtrl * vocabPairList;

    DECLARE_EVENT_TABLE()
};

#endif
