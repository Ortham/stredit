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

#include "ui.h"

#include <stdexcept>
#include <algorithm>
#include <boost/locale.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <wx/aboutdlg.h>
#include <wx/msgdlg.h>
#include <wx/progdlg.h>
#include <wx/splitter.h>

BEGIN_EVENT_TABLE ( MainFrame, wxFrame )
    EVT_CLOSE ( MainFrame::OnClose )

    EVT_MENU ( wxID_OPEN , MainFrame::OnOpenFile )
    EVT_MENU ( wxID_SAVE , MainFrame::OnSaveFile )
    EVT_MENU ( wxID_SAVEAS , MainFrame::OnSaveFile )
    EVT_MENU ( wxID_EXIT , MainFrame::OnQuit )
    EVT_MENU ( wxID_HELP , MainFrame::OnViewReadme )
    EVT_MENU ( wxID_ABOUT , MainFrame::OnAbout )

    EVT_LIST_ITEM_SELECTED ( LIST_Strings , MainFrame::OnStringSelect )

    EVT_TEXT_ENTER ( SEARCH_Strings, MainFrame::OnStringFilter )
    EVT_SEARCHCTRL_SEARCH_BTN ( SEARCH_Strings , MainFrame::OnStringFilter )
    EVT_SEARCHCTRL_CANCEL_BTN ( SEARCH_Strings , MainFrame::OnStringFilterCancel )

    EVT_CHAR_HOOK ( MainFrame::OnKeyDown )
END_EVENT_TABLE()

BEGIN_EVENT_TABLE ( VirtualList, wxListCtrl )
    EVT_CLOSE (VirtualList::OnClose )
END_EVENT_TABLE()

IMPLEMENT_APP(StrEditApp)

using namespace std;
using namespace stredit;

namespace stredit {
    //UI helper functions.
    wxString translate(char * cstr) {
        return wxString(boost::locale::translate(cstr).str().c_str(), wxConvUTF8);
    }

    wxString translate(string str) {
        return wxString(boost::locale::translate(str).str().c_str(), wxConvUTF8);
    }

    wxString FromUTF8(string str) {
        return wxString(str.c_str(), wxConvUTF8);
    }

    wxString FromUTF8(boost::format f) {
        return FromUTF8(f.str());
    }
}

//Draws the main window when program starts.
bool StrEditApp::OnInit() {
    //Initialise main window.
    MainFrame *frame = new MainFrame(wxT("StrEdit"));
    frame->Show(true);
    SetTopWindow(frame);

    //Display OpenDialog.
    wxCommandEvent event;
    frame->OnOpenFile(event);

    return true;
}

VirtualList::VirtualList(wxWindow * parent, wxWindowID id) : wxListCtrl(parent, id, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_VIRTUAL), currentSelectionIndex(-1) {
    attr = new wxListItemAttr();
}

wxString VirtualList::OnGetItemText(long item, long column) const {
    if (!filter.empty())
        item = filter[item];

    if (column == 0) {
        if (internalData[item].fuzzy)
            return FromUTF8("\u2713");
        else
            return "";
    } else if (column == 1)
        return wxString::Format(wxT("%i"), internalData[item].id);
    else if (column == 2)
        return FromUTF8(internalData[item].oldString);
    else
        return FromUTF8(internalData[item].newString);
}

wxListItemAttr * VirtualList::OnGetItemAttr(long item) const {
    if (!filter.empty())
        item = filter[item];

    if (internalData[item].edited)
        attr->SetTextColour(*wxBLUE);
    else
        attr->SetTextColour(*wxBLACK);
    return attr;
}

void VirtualList::OnClose(wxCloseEvent& event) {
    delete attr;
    Destroy();
}

MainFrame::MainFrame(const wxChar *title) : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize), stringsEdited(false) {
    //Set up menu bar first.
    wxMenuBar * MenuBar = new wxMenuBar();
    // File Menu
    wxMenu * FileMenu = new wxMenu();
    FileMenu->Append(wxID_OPEN);
    FileMenu->Append(wxID_SAVE);
    FileMenu->Append(wxID_SAVEAS);
    FileMenu->Append(wxID_EXIT);
    MenuBar->Append(FileMenu, translate("&File"));
    //Help Menu
    wxMenu * HelpMenu = new wxMenu();
    HelpMenu->Append(wxID_HELP);
    HelpMenu->Append(wxID_ABOUT);
    MenuBar->Append(HelpMenu, translate("&Help"));
    SetMenuBar(MenuBar);

    //Set up stuff in the frame.
    wxSplitterWindow * splitter = new wxSplitterWindow(this);

    wxPanel * topPanel = new wxPanel(splitter);
    wxPanel * bottomPanel = new wxPanel(splitter);

    searchBox = new wxSearchCtrl(topPanel, SEARCH_Strings, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);

    stringList = new VirtualList(topPanel, LIST_Strings);
    stringList->InsertColumn(0, translate("Fuzzy"));
    stringList->InsertColumn(1, translate("ID"));
    stringList->InsertColumn(2, translate("Original String"));
    stringList->InsertColumn(3, translate("New String"));

    originalTextBox = new wxTextCtrl(bottomPanel, TEXT_Original, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
    newTextBox = new wxTextCtrl(bottomPanel, TEXT_New, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);

    //Set up the sizers.
    wxBoxSizer * bigBox = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer * topSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer * bottomSizer = new wxBoxSizer(wxVERTICAL);

    topSizer->Add(searchBox, 0, wxEXPAND);
    topSizer->Add(stringList, 1, wxEXPAND);
    bottomSizer->Add(originalTextBox, 1, wxEXPAND|wxBOTTOM, 5);
    bottomSizer->Add(newTextBox, 1, wxEXPAND);
    bigBox->Add(splitter, 1, wxEXPAND|wxALL, 5);

    //Now set the layout and misc. elements.
    splitter->SplitHorizontally(topPanel, bottomPanel);
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    SetIcon(wxICON(MAINICON));
    CreateStatusBar();

    topPanel->SetSizerAndFit(topSizer);
    bottomPanel->SetSizerAndFit(bottomSizer);
    SetSizerAndFit(bigBox);
}

void MainFrame::OnOpenFile(wxCommandEvent& event) {
    //Display the OpenDialog, then get the strings from the picked files and
    //fill the string list with them.

    OpenDialog * od = new OpenDialog(this, wxID_ANY, translate("Open File(s)..."));

    if (od->ShowModal() != wxID_OK) {
        od->Destroy();
        return;
    }

    string sourcePath = od->GetSourcePath().ToUTF8().data();
    string transPath = od->GetTransPath().ToUTF8().data();
    int sourceFallbackEnc = od->GetSourceFallbackEnc();
    int transFallbackEnc = od->GetTransFallbackEnc();
    od->Destroy();

    if (sourcePath.empty()) {
        wxMessageBox(
            FromUTF8("Invalid file combination selected."),
            translate("StrEdit: Error"),
            wxOK | wxICON_ERROR,
            this);
        return;
    }

    wxProgressDialog progDia(translate("StrEdit: Working..."), translate("Opening file..."), 100, this, wxPD_APP_MODAL);
    progDia.SetIcon(wxICON(MAINICON));
    progDia.Pulse();
    try {
        if (transPath.empty()) {
            //Only one file.
            GetStrings(sourcePath, sourceFallbackEnc, stringList->internalData);
        } else {
            //Two files.
            boost::unordered_map<uint32_t, std::string> sourceMap;
            boost::unordered_map<uint32_t, std::string> transMap;
            GetStrings(sourcePath, sourceFallbackEnc, sourceMap);
            GetStrings(transPath, transFallbackEnc, transMap);
            BuildStringData(sourceMap, transMap, stringList->internalData);
        }
   } catch (runtime_error& e) {
        wxMessageBox(
            FromUTF8(e.what()),
            translate("StrEdit: Error"),
            wxOK | wxICON_ERROR,
            this);
        return;
    }
    progDia.Pulse();
    sort(stringList->internalData.begin(), stringList->internalData.end(), compare_old_new);
    progDia.Pulse();
    size_t listSize = stringList->internalData.size();
    stringList->SetItemCount(listSize);
    stringList->RefreshItems(0, listSize - 1);
    //Reset everything.
    stringList->filter.clear();
    stringList->currentSelectionIndex = -1;
    searchBox->Clear();
    originalTextBox->Clear();
    newTextBox->Clear();
    stringsEdited = false;
    if (!transPath.empty())
        filePath = transPath;
    else
        filePath.clear();
    SetStatusText(wxString::Format(wxT("%i strings"), listSize));
    SetTitle("StrEdit : " + sourcePath);
}

void MainFrame::OnSaveFile(wxCommandEvent& event) {
    if (event.GetId() == wxID_SAVEAS)
        filePath.clear();

    SaveFile();
}

void MainFrame::SaveFile() {
    if (filePath.empty()) {
        //Display file picker dialog.
        wxFileDialog saveFileDialog(this, translate("Save As..."), wxEmptyString, wxEmptyString,
            "Strings files (*.STRINGS;*.DLSTRINGS;*.ILSTRINGS)|*.STRINGS;*.DLSTRINGS;*.ILSTRINGS",
            wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

        saveFileDialog.SetIcon(wxICON(MAINICON));

        if (saveFileDialog.ShowModal() == wxID_CANCEL)
            return;

        filePath = saveFileDialog.GetPath().ToUTF8();
    }

    wxProgressDialog * progDia = new wxProgressDialog(translate("StrEdit: Working..."), translate("Saving file..."), 100, this);
    progDia->SetIcon(wxICON(MAINICON));
    progDia->Pulse();
    try {
        SetStrings(filePath, stringList->internalData);
    } catch (runtime_error& e) {
        progDia->Destroy();
        wxMessageBox(
            FromUTF8(e.what()),
            translate("StrEdit: Error"),
            wxOK | wxICON_ERROR,
            this);
        return;
    }
    progDia->Destroy();

    stringsEdited = false;
}

void MainFrame::OnQuit(wxCommandEvent& event) {
    Close();
}

void MainFrame::OnClose(wxCloseEvent& event) {
    //Need to prompt save if strings have been edited.
    if (stringsEdited) {
        wxMessageDialog messDia(this, translate("Your changes have not been saved. Do you want to save them before exiting?"), translate("Save changes?"), wxCANCEL|wxYES_NO);
        messDia.SetIcon(wxICON(MAINICON));

        int ret = messDia.ShowModal();

        if (ret == wxID_CANCEL)
            return;
        else if (ret == wxID_YES)
            SaveFile();
    }

    Destroy();
}

void MainFrame::OnViewReadme(wxCommandEvent& event) {
    if (boost::filesystem::exists(readme_path))
        wxLaunchDefaultApplication(readme_path);
    else  //No ReadMe exists, show a pop-up message saying so.
        wxMessageBox(
            FromUTF8(boost::format(boost::locale::translate("Error: \"%1%\" cannot be found.")) % readme_path),
            translate("StrEdit: Error"),
            wxOK | wxICON_ERROR,
            this);
}

void MainFrame::OnAbout(wxCommandEvent& event) {
    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetName("StrEdit");
    aboutInfo.SetVersion(version_string);
    aboutInfo.SetDescription(translate("A minimalist TES V: Skyrim string table editor designed for mod translators."));
    aboutInfo.SetCopyright("Copyright (C) 2012 WrinklyNinja.");
    aboutInfo.SetWebSite("http://github.com/WrinklyNinja/stredit");
    aboutInfo.SetLicence("This program is free software: you can redistribute it and/or modify\n"
    "it under the terms of the GNU General Public License as published by\n"
    "the Free Software Foundation, either version 3 of the License, or\n"
    "(at your option) any later version.\n"
    "\n"
    "This program is distributed in the hope that it will be useful,\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    "GNU General Public License for more details.\n"
    "\n"
    "You should have received a copy of the GNU General Public License\n"
    "along with this program.  If not, see <http://www.gnu.org/licenses/>.");
    aboutInfo.SetIcon(wxICON(MAINICON));
    wxAboutBox(aboutInfo);
}


void MainFrame::OnStringSelect(wxListEvent& event) {

    if (stringList->currentSelectionIndex != -1) {

        wxString currStr = stringList->internalData[stringList->currentSelectionIndex].newString;
        wxString newStr = newTextBox->GetValue().ToUTF8();

        if (currStr != newStr) {
            stringsEdited = true;
            stringList->internalData[stringList->currentSelectionIndex].newString = newStr;
            stringList->internalData[stringList->currentSelectionIndex].edited = true;
            stringList->RefreshItems(0, stringList->GetItemCount() - 1);
        }
    }
    if (stringList->filter.empty())
        stringList->currentSelectionIndex = event.GetIndex();
    else
        stringList->currentSelectionIndex = stringList->filter[event.GetIndex()];

    //Load the selected strings.
    str_data data = stringList->internalData[stringList->currentSelectionIndex];

    originalTextBox->SetValue(FromUTF8(data.oldString));
    newTextBox->SetValue(FromUTF8(data.newString));
}

void MainFrame::OnStringFilter(wxCommandEvent& event) {
    string filterStr = event.GetString().ToUTF8().data(); //For some reason it doesn't like not having data()...
    if (filterStr.empty()) {
        OnStringFilterCancel(event);
        return;
    }
    searchBox->ShowCancelButton(true);
    size_t length = filterStr.length();
    stringList->filter.clear();
    for (size_t i=0, max=stringList->internalData.size(); i < max; ++i) {
        if (boost::iequals(stringList->internalData[i].oldString.substr(0, length), filterStr))
            stringList->filter.push_back(i);
    }
    size_t itemCount = stringList->filter.size();
    stringList->SetItemCount(itemCount);
    stringList->RefreshItems(0, itemCount - 1);
    SetStatusText(wxString::Format(wxT("%i strings (%i hidden)"), stringList->internalData.size(), stringList->internalData.size() - itemCount));
}

void MainFrame::OnStringFilterCancel(wxCommandEvent& event) {
    searchBox->ShowCancelButton(false);
    stringList->filter.clear();
    size_t itemCount = stringList->internalData.size();
    stringList->SetItemCount(itemCount);
    stringList->RefreshItems(0, itemCount - 1);
    SetStatusText(wxString::Format(wxT("%i strings"), stringList->internalData.size()));
}

void MainFrame::OnKeyDown(wxKeyEvent& event) {
    if (event.AltDown() && event.GetKeyCode() == 'C') {
        //Fast copy/paste.
        newTextBox->SetValue(originalTextBox->GetValue());
        newTextBox->SetFocus();
    }
    event.Skip();
}

OpenDialog::OpenDialog(wxWindow * parent, wxWindowID id, const wxString& title) : wxDialog(parent, id, title) {

    wxString encs[] = {
      "Windows-1251",
      "Windows-1252"
    };

    //Set up stuff in the frame.
    SetIcon(wxICON(MAINICON));

    wxSizer * buttons = CreateSeparatedButtonSizer(wxOK|wxCANCEL);

    wxBoxSizer * bigBox = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer * srcBox = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer * transBox = new wxBoxSizer(wxHORIZONTAL);

    srcPicker = new wxFilePickerCtrl(this, wxID_ANY, wxEmptyString, wxFileSelectorPromptStr, "Strings files (*.STRINGS;*.DLSTRINGS;*.ILSTRINGS)|*.STRINGS;*.DLSTRINGS;*.ILSTRINGS");
    transPicker = new wxFilePickerCtrl(this, wxID_ANY, wxEmptyString, wxFileSelectorPromptStr, "Strings files (*.STRINGS;*.DLSTRINGS;*.ILSTRINGS)|*.STRINGS;*.DLSTRINGS;*.ILSTRINGS");

    srcFallbackEncChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, encs);
    transFallbackEncChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, encs);

    srcBox->Add(new wxStaticText(this, wxID_ANY, translate("Source file")), 1, wxEXPAND|wxLEFT|wxALL, 5);
    srcBox->Add(srcPicker, 0, wxCENTER|wxALL, 5);
    srcBox->Add(srcFallbackEncChoice, 0, wxRIGHT|wxALL, 5);

    transBox->Add(new wxStaticText(this, wxID_ANY, translate("Translation file")), 1, wxEXPAND|wxLEFT|wxALL, 5);
    transBox->Add(transPicker, 0, wxCENTER|wxALL, 5);
    transBox->Add(transFallbackEncChoice, 0, wxRIGHT|wxALL, 5);

    bigBox->Add(srcBox, 1, wxEXPAND|wxALL, 5);
    bigBox->Add(transBox, 1, wxEXPAND|wxALL, 5);
    bigBox->Add(buttons, 0, wxEXPAND|wxALL, 5);

    //Now set the layout and sizes.
    SetSizerAndFit(bigBox);

    //Set default fallback encodings.
    srcFallbackEncChoice->SetSelection(1);
    transFallbackEncChoice->SetSelection(1);
}

wxString OpenDialog::GetSourcePath() const {
    return srcPicker->GetPath();
}

wxString OpenDialog::GetTransPath() const {
    return transPicker->GetPath();
}

int OpenDialog::GetSourceFallbackEnc() const {
    int ret = srcFallbackEncChoice->GetSelection();
    if (ret == wxNOT_FOUND)
        return ret;
    else
        return 1251 + ret;
}

int OpenDialog::GetTransFallbackEnc() const {
    int ret = transFallbackEncChoice->GetSelection();
    if (ret == wxNOT_FOUND)
        return ret;
    else
        return 1251 + ret;
}
