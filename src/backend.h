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

#ifndef __STREDIT_BACKEND_H__
#define __STREDIT_BACKEND_H__

#include <stdint.h>
#include <string>
#include <map>
#include <list>

namespace stredit {
    //Structure for holding string data.
    struct string_data {
        int matchProb;  //For when using Levenstein matching.
        uint32_t id;
        std::string oldString;
        std::string newString;
    };

    //Some global constants.
    const std::string readme_path = "Docs/readme.html"
    const std::string version_string = "0.1.0";
    const std::string prog_filename = "StrEdit.exe";

    //String file reading/writing.
    std::map<uint32_t, std::string> GetStrings(const std::string path);
    void SetStrings(const std::string path, const std::map<uint32_t, std::string>& strings);

    //Matches the strings of map1 and map2 up using their IDs, and outputs the
    //result. The matchProb for all matches is 100, as only exact matching is used.
    std::list<string_data> TwoStringMatching(const std::map<uint32_t, std::string>& map1, const std::map<uint32_t, std::string>& map2);

    //Updates the IDs of stringList elements by matching their oldStrings to the strings of map1.
    //If an exact match cannot be found, then Levenstein matching is used and the matchProb
    //updated to reflect the probability of a correct match.
    void UpdateStringIDs(std::list<string_data>& stringList, const std::map<uint32_t, std::string>& map1);
}

#endif
