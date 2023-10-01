/*
 * This file was written by Gozzim (https://github.com/Gozzim)
 * LZW algorithm: Rochet2 (https://github.com/Rochet2/lualzw)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "LZW.h"

LZW* LZW::instance()
{
    static LZW instance;

    if (!DictionariesInitialized)
    {
        instance.InitializeDictionaries();
        DictionariesInitialized = true;
    }

    return &instance;
}

std::string LZW::CharToString(char c)
{
    return {c};
}

std::string LZW::GetWordFromDict(const std::string& word, const LZWDictionary& baseDict, const LZWDictionary& altDict)
{
    if (baseDict.find(word) != baseDict.end())
    {
        return baseDict.at(word);
    }
    else if (altDict.find(word) != altDict.end())
    {
        return altDict.at(word);
    }
    else
    {
        return "";
    }
}

void LZW::InitializeDictionaries()
{
    for (uint16_t i = 0; i < 256; ++i)
    {
        std::string ic = CharToString(static_cast<char>(i));
        std::string iic = CharToString(static_cast<char>(i)) + CharToString(static_cast<char>(0));
        basedictcompress[ic] = iic;
        basedictdecompress[iic] = ic;
    }
}

void LZW::DictAddA(const std::string& str, LZWDictionary& dict, uint32_t& a, uint32_t& b)
{
    if (a >= 256)
    {
        a = 0;
        b += 1;
        if (b >= 256)
        {
            b = 1;
            dict.clear();
        }
    }

    dict[str] = CharToString(static_cast<char>(a)) + CharToString(static_cast<char>(b));
    a += 1;
}

void LZW::DictAddB(const std::string& str, LZWDictionary& dict, uint32_t& a, uint32_t& b)
{
    if (a >= 256)
    {
        a = 0;
        b += 1;
        if (b >= 256)
        {
            b = 1;
            dict.clear();
        }
    }

    dict[CharToString(static_cast<char>(a)) + CharToString(static_cast<char>(b))] = str;
    a += 1;
}

LZWResult LZW::Compress(const std::string& input)
{
    size_t len = input.size();
    if (len <= 1)
    {
        return {"u" + input, LZW_OK};
    }

    LZWDictionary dict;
    uint32_t a = 0;
    uint32_t b = 1;

    std::string result = "c";
    size_t resultLen = 1;
    std::string word = "";

    for (size_t i = 0; i < len; ++i)
    {
        char c = input[i];
        std::string wc = word + CharToString(c);

        if (basedictcompress.find(wc) == basedictcompress.end() && dict.find(wc) == dict.end())
        {
            std::string write = GetWordFromDict(word, basedictcompress, dict);

            if (write.empty())
            {
                return {nullptr, LZW_ERR_EMPTY_DICT_RESULT};
            }

            result += write;
            resultLen += write.size();

            if (len <= resultLen)
            {
                return {"u" + input, LZW_OK};
            }

            DictAddA(wc, dict, a, b);
            word = c;
        }
        else
        {
            word = wc;
        }
    }

    std::string finalWrite = GetWordFromDict(word, basedictcompress, dict);
    if (finalWrite.empty()) {
        return {nullptr, LZW_ERR_EMPTY_DICT_RESULT};
    }

    result += finalWrite;
    resultLen += finalWrite.size();

    if (len <= resultLen)
    {
        return {"u" + input, LZW_OK};
    }

    return {result, LZW_OK};
}

LZWResult LZW::Decompress(const std::string& input)
{
    if (input.empty())
    {
        return {nullptr, LZW_ERR_UNCOMPRESSED_STRING};
    }

    char control = input[0];
    if (control == 'u')
    {
        return {input.substr(1), LZW_OK};
    }
    else if (control != 'c')
    {
        return {nullptr, LZW_ERR_UNCOMPRESSED_STRING};
    }

    std::string data = input.substr(1);
    size_t len = data.size();
    if (len < 2)
    {
        return {nullptr, LZW_ERR_UNCOMPRESSED_STRING};
    }

    LZWDictionary dict;
    uint32_t a = 0;
    uint32_t b = 1;
    std::string result;
    std::string last = data.substr(0, 2);

    result = GetWordFromDict(last, basedictdecompress, dict);

    for (size_t i = 2; i < len; i += 2)
    {
        std::string code = data.substr(i, 2);
        std::string lastStr = GetWordFromDict(last, basedictdecompress, dict);

        if (lastStr.empty())
        {
            return {nullptr, LZW_ERR_EMPTY_DICT_RESULT};
        }

        std::string toAdd = GetWordFromDict(code, basedictdecompress, dict);

        if (!toAdd.empty())
        {
            result += toAdd;
            DictAddB(lastStr + toAdd[0], dict, a, b);
        }
        else
        {
            std::string tmp = lastStr + lastStr[0];
            result += tmp;
            DictAddB(tmp, dict, a, b);
        }

        last = code;
    }

    return {result, LZW_OK};
}
