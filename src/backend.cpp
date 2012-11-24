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
        size_t numStrings;

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

    //String file writing.
    void SetStrings(const std::string path, const boost::unordered_map<uint32_t, std::string>& stringMap) {
        uint32_t ret;
        strings_handle sh;
        string_data * strings;
        size_t numStrings;

        ret = OpenStringsFile(&sh, reinterpret_cast<const uint8_t *>(path.c_str()));

        if (ret != LIBSTRINGS_OK)
            throw runtime_error("Could not open strings file.");

        //Create string_data array.
        numStrings = stringMap.size();
        strings = new string_data[numStrings];
        size_t i = 0;
        for (boost::unordered_map<uint32_t, std::string>::const_iterator it=stringMap.begin(), endIt=stringMap.end(); it != endIt; ++it) {
            string_data data;
            data.id = it->first;
            data.data = ToUint8_tString(it->second);
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
                           std::list<str_data>& stringList) {

        stringList.clear();
        for (boost::unordered_map<uint32_t, std::string>::iterator it=originalStrMap.begin(), endIt=originalStrMap.end(); it != endIt; ++it) {
            str_data data;
            data.id = it->first;
            data.oldString = it->second;
            boost::unordered_map<uint32_t, std::string>::iterator itr = targetStrMap.find(it->first);
            if (itr != targetStrMap.end()) {
                data.newString = itr->second;
            }
            stringList.insert(data);
        }

        return stringList;
    }

    //Updates the IDs of stringList elements by matching their oldStrings to the strings of map1.
    //If an exact match cannot be found, then Levenstein matching is used and the lDist
    //updated to reflect the distance of the chosen match.
    void UpdateStringIDs(const boost::unordered_map<uint32_t, std::string>& oldOrigStrMap,
                         std::list<str_data>& stringList) {
        for (std::list<str_data>::iterator it=stringList.begin(), endIt=stringList.end(); it != endIt; ++it) {
            int leastLDist = -1;
            for (boost::unordered_map<uint32_t, std::string>::const_iterator itr=originalStrMap.begin(), endItr=originalStrMap.end(); itr != endItr; ++itr) {
                if (itr->second == it->oldString) {
                    it->id = itr->first;
                    break;
                } else {
                    //Levenshtein match.
                    //Get the Levenshtein distance of the two strings, compare
                    //to the previously-obtained distance (if exists). If smaller,
                    //store the current string.
                    int lDist = Levenshtein(it->oldString, it->second);
                    if (leastLDist == -1 || leastLDist > lDist)
                        it->id = itr->first;
                        it->lDist = lDist;
                        leastLDist = lDist;
                    }
                }
            }
        }
    }

    //Explicit memory management, need to call delete on the output when finished with it.
    ToUint8_tString(std::string str) {
        size_t length = str.length() + 1;
        uint8_t * p = new uint8_t[length];

        for (size_t j=0; j < str.length(); j++) {  //UTF-8, so this is code-point by code-point rather than char by char, but same result here.
            p[j] = str[j];
        }
        p[length - 1] = '\0';
        return p;
    }

    void ToStringList(const boost::unordered_map<uint32_t, std::string>& stringMap, std::list<str_data>& stringList) {
        stringList.clear();
        for (boost::unordered_map<uint32_t, std::string>::const_iterator it=stringMap.begin(), endIt=stringMap.end(); it != endIt; ++it) {
            str_data data;
            data.id = it->first;
            data.oldString = it->second;
            stringList.insert(data);
        }
    }

    void ToStringMap(const std::list<str_data>& stringList, boost::unordered_map<uint32_t, std::string>& stringMap) {
        stringMap.clear();
        for (std::list<str_data>::const_iterator it=stringList.begin(), endIt=stringList.end(); it != endIt; ++it) {
            string str;
            if (it->newString.empty())
                str = it->oldString;
            else
                str = it->newString;
            stringMap.insert(pair<uint32_t, string>(it->id, str));
        }
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
}
