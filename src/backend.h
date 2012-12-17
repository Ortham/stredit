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

#ifndef __STREDIT_BACKEND_H__
#define __STREDIT_BACKEND_H__

#include <stdint.h>
#include <string>
#include <vector>
#include <boost/unordered_map.hpp>

namespace stredit {
    //Structure for holding string data.
    struct str_data {
        str_data() : fuzzy(false), id(0), edited(false) {}

        bool fuzzy;  //For when using Levenstein matching.
        uint32_t id;
        std::string oldString;
        std::string newString;
        bool edited;
    };

    //Some global constants.
    const std::string readme_path = "StrEdit Readme.html";
    const std::string version_string = "0.3.1";

    //String file reading/writing. These could be replaced by a more optimised
    //per-string editing system once everything is working.
    void GetStrings(const std::string path, const int fallbackEnc, boost::unordered_map<uint32_t, std::string>& stringMap);
    void GetStrings(const std::string path, const int fallbackEnc, std::vector<str_data>& stringList);
    void SetStrings(const std::string path, const std::vector<str_data>& stringList);

    //Import/Export strings as XML data.
    void ImportAsXML(const std::string path,       std::vector<str_data>& stringList);
    void ExportAsXML(const std::string path, const std::vector<str_data>& stringList);

    //Matches the strings in the maps by their IDs. Any IDs which are not present in both maps
    //are not included in the output. The passed vector has its contents appended to, not replaced.
    void BuildStringPairs(const boost::unordered_map<uint32_t, std::string>& originalStrMap,
                          const boost::unordered_map<uint32_t, std::string>& targetStrMap,
                                boost::unordered_map<std::string, std::string>& stringMap);

    //Matches the strings of map1 and map2 up using their IDs, and outputs the
    //result. The lDist for all matches is 0, as only exact matching is used.
    void BuildStringData(const boost::unordered_map<uint32_t, std::string>& originalStrMap,
                         const boost::unordered_map<uint32_t, std::string>& targetStrMap,
                               std::vector<str_data>& stringList);

    //Fills in the empty newStrings in stringList by finding the closest Levenshtein match between
    //their corresponding oldStrings and the keys of stringMap, then using the corresponding
    //mapped string. It also updates the fuzzy data member as necessary.
    void FuzzyMatchStrings(const boost::unordered_map<std::string, std::string>& stringMap,
                                 std::vector<str_data>& stringList,
                                 void * progDiaPtr);

    //Some helper functions.
    uint8_t * ToUint8_tString(const std::string str);

    int Levenshtein(const std::string first, const std::string second);

    bool compare_old_new(const str_data first, const str_data second);
}

#endif
