/*  StrEdit

    A STRINGS, ILSTRINGS and DLSTRINGS file editor designed for mod translators.

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
#include "backend.h"

#include <boost/locale.hpp>
#include <boost/filesystem.hpp>
#include <wx/aboutdlg.h>

BEGIN_EVENT_TABLE ( MainFrame, wxFrame )
    EVT_CLOSE (MainFrame::OnQuit )

    EVT_MENU ( wxID_OPEN , MainFrame::OnOpenFile )
    EVT_MENU ( wxID_SAVE , MainFrame::OnSaveFile )
    EVT_MENU ( wxID_SAVEAS , MainFrame::OnSaveFileAs )
    EVT_MENU ( wxID_HELP , MainFrame::OnViewReadme )
    EVT_MENU ( wxID_ABOUT , MainFrame::OnAbout )

    EVT_LIST_ITEM_SELECTED ( LIST_Strings , MainFrame::OnStringSelect)

    EVT_SEARCHCTRL_SEARCH_BTN ( SEARCH_Strings , UserRulesEditorFrame::OnStringFilter )
    EVT_SEARCHCTRL_CANCEL_BTN ( SEARCH_Strings , UserRulesEditorFrame::OnStringFilterCancel )
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
    MainFrame *frame = new MainFrame(wxT("StrEdit"));
    frame->SetIcon(wxIconLocation(prog_filename));
    frame->Show(true);
    SetTopWindow(frame);

    return true;
}

MainFrame::MainFrame(const wxChar *title) : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize) {
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
    SetBackgroundColour(*wxWHITE);

    searchBox = new wxSearchCtrl(this, SEARCH_Strings, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);

    stringList = new wxListCtrl(this, LIST_Strings);
    originalTextBox = new wxTextCtrl(this, TEXT_Original, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
    newTextBox = new wxTextCtrl(this, TEXT_New, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);

    //Contents in one big resizing box.
    wxBoxSizer *bigBox = new wxBoxSizer(wxVERTICAL);

    bigBox->Add(searchBox, 0, wxEXPAND);
    bigBox->Add(stringList, 0, wxALL, 5);
    bigBox->Add(originalTextBox, 0, wxALL, 5);
    bigBox->Add(newTextBox, 0, wxALL, 5);

    //Now set the layout and sizes.
    SetSizerAndFit(bigBox);
}

void MainFrame::OnOpenFile(wxCommandEvent& event) {
    //Display the OpenDialog, then
}

void MainFrame::OnSaveFile(wxCommandEvent& event) {

}

void MainFrame::OnSaveFileAs(wxCommandEvent& event) {

}

void MainFrame::OnQuit(wxCloseEvent& event) {

}

void MainFrame::OnViewReadme(wxCommandEvent& event) {
    if (boost::filesystem::exists(readme_path))
        wxLaunchDefaultApplication(readme_path);
    else  //No ReadMe exists, show a pop-up message saying so.
        wxMessageBox(
            FromUTF8(format(loc::translate("Error: \"%1%\" cannot be found.")) % readme_path),
            translate("StrEdit: Error"),
            wxOK | wxICON_ERROR,
            this);
}

void MainFrame::OnAbout(wxCommandEvent& event) {
    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetName("StrEdit");
    aboutInfo.SetVersion(version_string);
    aboutInfo.SetDescription(translate("A STRINGS, ILSTRINGS and DLSTRINGS file editor designed for mod translators."));
    aboutInfo.SetCopyright("Copyright (C) 2012 WrinklyNinja.");
    aboutInfo.SetWebSite("https://github.com/WrinklyNinja/stredit");
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
    aboutInfo.SetIcon(wxIconLocation(prog_filename));
    wxAboutBox(aboutInfo);
}


void MainFrame::OnStringSelect(wxListEvent& event) {

}

void MainFrame::OnStringFilter(wxCommandEvent& event) {

}

void MainFrame::OnStringFilterCancel(wxCommandEvent& event) {

}

