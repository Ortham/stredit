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
#include <list>
#include <boost/unordered_map.hpp>

namespace stredit {
    //Structure for holding string data.
    struct str_data {
        str_data() : lDist(0), id(0) {}

        unsigned int lDist;  //For when using Levenstein matching.
        uint32_t id;
        std::string oldString;
        std::string newString;
    };

    //Some global constants.
    const std::string readme_path = "Docs/readme.html";
    const std::string version_string = "0.1.0";
    const std::string prog_filename = "StrEdit.exe";

    //String file reading/writing. These could be replaced by a more optimised
    //per-string editing system once everything is working.
    void GetStrings(const std::string path,       boost::unordered_map<uint32_t, std::string>& stringMap);
    void SetStrings(const std::string path, const boost::unordered_map<uint32_t, std::string>& stringMap);

    //Matches the strings of map1 and map2 up using their IDs, and outputs the
    //result. The lDist for all matches is 0, as only exact matching is used.
    void TwoStringMatching(const boost::unordered_map<uint32_t, std::string>& originalStrMap,
                           const boost::unordered_map<uint32_t, std::string>& targetStrMap,
                           std::list<str_data>& stringList);

    //Updates the IDs of stringList elements by matching their oldStrings to the strings of map1.
    //If an exact match cannot be found, then Levenshtein matching is used and the lDist
    //updated to reflect the distance of the chosen match.
    void UpdateStringIDs(const boost::unordered_map<uint32_t, std::string>& oldOrigStrMap,
                         std::list<str_data>& stringList);

    //Some helper conversion functions.
    uint8_t * ToUint8_tString(std::string str);
    void ToStringList(const boost::unordered_map<uint32_t, std::string>& stringMap, std::list<str_data>& stringList);
    void ToStringMap(const std::list<str_data>& stringList, boost::unordered_map<uint32_t, std::string>& stringMap);

    int Levenshtein(const std::string first, const std::string second);
}

#endif
