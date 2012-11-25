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

#include "backend.h"

#include <libstrings.h>
#include <stdexcept>

using namespace std;

namespace stredit {
    //String file reading.
    void GetStrings(const std::string path, boost::unordered_map<uint32_t, std::string>& stringMap) {
        uint32_t ret;
        strings_handle sh;
        string_data * strings;
        size_t numStrings = 0;

        ret = OpenStringsFile(&sh, reinterpret_cast<const uint8_t *>(path.c_str()));

        if (ret != LIBSTRINGS_OK)
            throw runtime_error("Could not open strings file.");

        ret = GetStrings(sh, &strings, &numStrings);

        if (ret != LIBSTRINGS_OK)
            throw runtime_error("Could not read strings file.");

        stringMap.clear();
        for (size_t i=0; i < numStrings; ++i) {
            stringMap.insert(pair<uint32_t, string>(strings[i].id, string(reinterpret_cast<const char *>(strings[i].data))));
        }

        CloseStringsFile(sh);
    }

    void GetStrings(const std::string path, std::vector<str_data>& stringList) {
        uint32_t ret;
        strings_handle sh;
        string_data * strings;
        size_t numStrings = 0;

        ret = OpenStringsFile(&sh, reinterpret_cast<const uint8_t *>(path.c_str()));

        if (ret != LIBSTRINGS_OK)
            throw runtime_error("Could not open strings file.");

        ret = GetStrings(sh, &strings, &numStrings);

        if (ret != LIBSTRINGS_OK)
            throw runtime_error("Could not read strings file.");

        stringList.clear();
        for (size_t i=0; i < numStrings; ++i) {
            str_data data;
            data.id = strings[i].id;
            data.oldString = string(reinterpret_cast<const char *>(strings[i].data));
            stringList.push_back(data);
        }

        CloseStringsFile(sh);
    }

    //String file writing.
    void SetStrings(const std::string path, const std::vector<str_data>& stringList) {
        uint32_t ret;
        strings_handle sh;
        string_data * strings;
        size_t numStrings;

        ret = OpenStringsFile(&sh, reinterpret_cast<const uint8_t *>(path.c_str()));

        if (ret != LIBSTRINGS_OK)
            throw runtime_error("Could not open strings file.");

        //Create string_data array.
        numStrings = stringList.size();
        strings = new string_data[numStrings];
        size_t i = 0;
        for (std::vector<str_data>::const_iterator it=stringList.begin(), endIt=stringList.end(); it != endIt; ++it) {
            string_data data;
            data.id = it->id;
            if (it->newString.empty())
                data.data = ToUint8_tString(it->oldString);
            else
                data.data = ToUint8_tString(it->newString);
            strings[i] = data;
            ++i;
        }

        ret = SetStrings(sh, strings, numStrings);

        //Free memory allocated for string_data array.
        for (size_t i=0; i < numStrings; i++)
            delete [] strings[i].data;
        delete [] strings;

        if (ret != LIBSTRINGS_OK)
            throw runtime_error("Could not set new strings.");

        ret = SaveStringsFile(sh, reinterpret_cast<const uint8_t *>(path.c_str()));

        if (ret != LIBSTRINGS_OK)
            throw runtime_error("Could not write strings file.");

        CloseStringsFile(sh);
    }

    //Matches the strings of map1 and map2 up using their IDs, and outputs the
    //result. The lDist for all matches is 0, as only exact matching is used.
    void TwoStringMatching(const boost::unordered_map<uint32_t, std::string>& originalStrMap,
                           const boost::unordered_map<uint32_t, std::string>& targetStrMap,
                           std::vector<str_data>& stringList) {
        stringList.clear();
        for (boost::unordered_map<uint32_t, std::string>::const_iterator it=originalStrMap.begin(), endIt=originalStrMap.end(); it != endIt; ++it) {
            str_data data;
            data.id = it->first;
            data.oldString = it->second;
            boost::unordered_map<uint32_t, std::string>::const_iterator itr = targetStrMap.find(it->first);
            if (itr != targetStrMap.end()) {
                data.newString = itr->second;
            }
            stringList.push_back(data);
        }
    }

    //Updates the IDs of stringList elements by matching their oldStrings to the strings of map1.
    //If an exact match cannot be found, then Levenstein matching is used and the lDist
    //updated to reflect the distance of the chosen match.
    void UpdateStringIDs(const boost::unordered_map<uint32_t, std::string>& oldOrigStrMap,
                         std::vector<str_data>& stringList) {
        for (std::vector<str_data>::iterator it=stringList.begin(), endIt=stringList.end(); it != endIt; ++it) {
            int leastLDist = -1;
            for (boost::unordered_map<uint32_t, std::string>::const_iterator itr=oldOrigStrMap.begin(), endItr=oldOrigStrMap.end(); itr != endItr; ++itr) {
                if (itr->second == it->oldString) {
                    it->id = itr->first;
                    break;
                } else {
                    //Levenshtein match.
                    //Get the Levenshtein distance of the two strings, compare
                    //to the previously-obtained distance (if exists). If smaller,
                    //store the current string.
                    int lDist = Levenshtein(it->oldString, itr->second);
                    if (leastLDist == -1 || leastLDist > lDist) {
                        it->id = itr->first;
                        it->lDist = lDist;
                        leastLDist = lDist;
                    }
                }
            }
        }
    }

    //Explicit memory management, need to call delete on the output when finished with it.
    uint8_t * ToUint8_tString(const std::string str) {
        size_t length = str.length() + 1;
        uint8_t * p = new uint8_t[length];

        for (size_t j=0; j < str.length(); j++) {  //UTF-8, so this is code-point by code-point rather than char by char, but same result here.
            p[j] = str[j];
        }
        p[length - 1] = '\0';
        return p;
    }

    int Levenshtein(const std::string first, const std::string second) {
        int fLen = first.length();
        int sLen = second.length();
        int cost = 0;

        if (first[0] != second[0])
            cost = 1;

        if (fLen == 0)
            return sLen;
        else if (sLen == 0)
            return fLen;

        //It gets complicated - look for a library to use.
        return 0;
    }

    bool compare_old_new(const str_data first, const str_data second) {
        //Untranslated strings first, followed by fuzzy matches, followed by all
        //other strings. Within each group, sort alphabetically by the oldString.
        if (first.newString.empty() && !second.newString.empty())
            return true;
        else if (!first.newString.empty() && second.newString.empty())
            return false;
        else if (first.lDist > 0 && second.lDist == 0)
            return true;
        else if (first.lDist == 0 && second.lDist > 0)
            return false;
        else
            return first.oldString < second.oldString;
    }
}
