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

#include "backend.h"
#include "progress.h"

#include <libstrings.h>
#include <stdexcept>
#include <boost/locale.hpp>
#include <pugixml.hpp>

using namespace std;
using boost::locale::translate;

namespace stredit {
    //String file reading.
    void GetStrings(const std::string path, const int fallbackEnc, boost::unordered_map<uint32_t, std::string>& stringMap) {
        uint32_t ret;
        strings_handle sh;
        string_data * strings;
        size_t numStrings = 0;

        ret = OpenStringsFile(&sh, reinterpret_cast<const uint8_t *>(path.c_str()), fallbackEnc);

        if (ret != LIBSTRINGS_OK)
            throw runtime_error(translate("Could not open strings file."));

        ret = GetStrings(sh, &strings, &numStrings);

        if (ret != LIBSTRINGS_OK) {
            CloseStringsFile(sh);
            throw runtime_error(translate("Could not read strings file."));
        }

        stringMap.clear();
        for (size_t i=0; i < numStrings; ++i) {
            stringMap.insert(pair<uint32_t, string>(strings[i].id, string(reinterpret_cast<const char *>(strings[i].data))));
        }

        CloseStringsFile(sh);
    }

    void GetStrings(const std::string path, const int fallbackEnc, std::vector<str_data>& stringList) {
        uint32_t ret;
        strings_handle sh;
        string_data * strings;
        size_t numStrings = 0;

        ret = OpenStringsFile(&sh, reinterpret_cast<const uint8_t *>(path.c_str()), fallbackEnc);

        if (ret != LIBSTRINGS_OK)
            throw runtime_error(translate("Could not open strings file."));

        ret = GetStrings(sh, &strings, &numStrings);

        if (ret != LIBSTRINGS_OK) {
            CloseStringsFile(sh);
            throw runtime_error(translate("Could not read strings file."));
        }

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

        ret = OpenStringsFile(&sh, reinterpret_cast<const uint8_t *>(path.c_str()), 1252);

        if (ret != LIBSTRINGS_OK)
            throw runtime_error(translate("Could not open strings file."));

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

        if (ret != LIBSTRINGS_OK) {
            CloseStringsFile(sh);
            throw runtime_error(translate("Could not set new strings."));
        }

        ret = SaveStringsFile(sh, reinterpret_cast<const uint8_t *>(path.c_str()));

        if (ret != LIBSTRINGS_OK) {
            CloseStringsFile(sh);
            throw runtime_error(translate("Could not write strings file."));
        }

        CloseStringsFile(sh);
    }

    //Import/Export strings as XML data.
    void ImportAsXML(const std::string path,       std::vector<str_data>& stringList) {

        using namespace pugi;

        xml_document doc;

        xml_parse_result result = doc.load_file(path.c_str(), (parse_default | parse_ws_pcdata_single) & ~parse_eol, encoding_utf8 );

        if (!result)
            throw runtime_error(translate("Could not read XML file."));

        xml_node strings = doc.child("strings");

        stringList.clear();
        for (xml_node string = strings.child("string"); string; string = string.next_sibling("string")) {
            str_data data;
            data.id = string.attribute("id").as_int();
            data.oldString = string.text().get();
            stringList.push_back(data);
        }
    }

    void ExportAsXML(const std::string path, const std::vector<str_data>& stringList) {

        using namespace pugi;

        xml_document doc;

        xml_node topNode = doc.append_child();

        topNode.set_name("strings");

        for (int i=0, max=stringList.size(); i < max; ++i) {
            xml_node strNode = topNode.append_child();

            strNode.set_name("string");

            xml_attribute id = strNode.append_attribute("id");
            id.set_value(stringList[i].id);

            string str;
            if (stringList[i].newString.empty())
                str = stringList[i].oldString;
            else
                str = stringList[i].newString;
            strNode.text().set(str.c_str());
        }

        if (!doc.save_file(path.c_str()))
            throw runtime_error(translate("Could not write XML file."));
    }

    //Matches the strings in the maps by their IDs. Any IDs which are not present in both maps
    //are not included in the output. The passed vector has its contents appended to, not replaced.
    void BuildStringPairs(const boost::unordered_map<uint32_t, std::string>& originalStrMap,
                           const boost::unordered_map<uint32_t, std::string>& targetStrMap,
                           boost::unordered_map<std::string, std::string>& stringMap) {
        boost::unordered_map<uint32_t, std::string>::const_iterator itr;
        for (boost::unordered_map<uint32_t, std::string>::const_iterator it=originalStrMap.begin(), endIt=originalStrMap.end(); it != endIt; ++it) {
            itr = targetStrMap.find(it->first);
            if (itr != targetStrMap.end())
                stringMap.insert(pair<string, string>(it->second, itr->second));
        }
    }

    //Matches the strings of map1 and map2 up using their IDs, and outputs the
    //result. The lDist for all matches is 0, as only exact matching is used.
    void BuildStringData(const boost::unordered_map<uint32_t, std::string>& originalStrMap,
                           const boost::unordered_map<uint32_t, std::string>& targetStrMap,
                           std::vector<str_data>& stringList) {
        stringList.clear();
        boost::unordered_map<uint32_t, std::string>::const_iterator itr;
        for (boost::unordered_map<uint32_t, std::string>::const_iterator it=originalStrMap.begin(), endIt=originalStrMap.end(); it != endIt; ++it) {
            str_data data;
            data.id = it->first;
            data.oldString = it->second;
            itr = targetStrMap.find(it->first);
            if (itr != targetStrMap.end() && it->second != itr->second)
                data.newString = itr->second;
            stringList.push_back(data);
        }
    }

    //Fills in the empty newStrings in stringList by finding the closest Levenshtein match between
    //their corresponding oldStrings and the keys of stringMap, then using the corresponding
    //mapped string.
    void FuzzyMatchStrings(const boost::unordered_map<std::string, std::string>& stringMap,
                                 std::vector<str_data>& stringList,
                                 void * progDiaPtr) {
        const int num = stringList.size();
        int i = 1;
        boost::unordered_map<std::string, std::string>::const_iterator bestMatch;
        for (std::vector<str_data>::iterator it=stringList.begin(), endIt=stringList.end(); it != endIt; ++it) {
            if (it->newString.empty()) {
                bestMatch = stringMap.find(it->oldString);
                int leastDist = 0;
                if (bestMatch == stringMap.end()) {
                    leastDist = -1;
                    for (boost::unordered_map<std::string, std::string>::const_iterator itr=stringMap.begin(), endItr=stringMap.end(); itr != endItr; ++itr) {
                        int dist = Levenshtein(it->oldString, itr->first);
                        if (leastDist == -1 || leastDist > dist) {
                            bestMatch = itr;
                            leastDist = dist;
                            if (dist == 1)  //Closest non-exact match possible.
                                break;
                        }
                    }
                }
                if (bestMatch != stringMap.end()) {
                    //bestMatch now points to the best matching element of stringMap.
                    it->newString = bestMatch->second;
                    it->fuzzy = (leastDist != 0);
                }
            }
            update_progress(progDiaPtr, "", ((float)i / num) * 100);
            ++i;
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

    //Calculate the Levenshtein distance between two strings.
    //Code from <https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C.2B.2B>
    //Used under the CC-BY-SA 3.0 license: <http://creativecommons.org/licenses/by-sa/3.0/>
    int Levenshtein(const std::string s1, const std::string s2) {
        const size_t len1 = s1.size(), len2 = s2.size();

        typedef unsigned int t;

        unsigned int * col = new t[ len2 + 1 ];
        unsigned int * prevCol = new t[ len2 + 1];

        for (unsigned int i = 0; i < len2 + 1; i++) {
            prevCol[i] = i;
        }
        for (unsigned int i = 0; i < len1; i++) {
            col[0] = i+1;
            for (unsigned int j = 0; j < len2; j++) {
                col[j+1] = min( min( 1 + col[j], 1 + prevCol[1 + j]), prevCol[j] + (s1[i]==s2[j] ? 0 : 1) );
            }
            unsigned int * ptr = col;
            col = prevCol;
            prevCol = ptr;
        }

        delete [] col;
        delete [] prevCol;
        return prevCol[len2];
    }

    bool compare_old_new(const str_data first, const str_data second) {
        //Untranslated strings first, followed by fuzzy matches, followed by all
        //other strings. Within each group, sort alphabetically by the oldString.
        if (first.newString.empty() && !second.newString.empty())
            return true;
        else if (!first.newString.empty() && second.newString.empty())
            return false;
        else if (first.fuzzy && !second.fuzzy)
            return true;
        else if (!first.fuzzy && second.fuzzy)
            return false;
        else if (first.oldString != second.oldString)
            return first.oldString < second.oldString;
        else
            return first.id < second.id;
    }
}
