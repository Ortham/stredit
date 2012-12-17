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

#include "progress.h"

#include <wx/progdlg.h>
#include <wx/msgdlg.h>

namespace stredit {
    void update_progress(void * ptr, const std::string message, int percentage) {
        wxProgressDialog * pd = (wxProgressDialog*)ptr;
        if (percentage == 100)
            percentage = 99;

        pd->Update(percentage, message);
    }
}
