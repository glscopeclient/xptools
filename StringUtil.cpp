/***********************************************************************************************************************
*                                                                                                                      *
* ANTIKERNEL                                                                                                           *
*                                                                                                                      *
* Copyright (c) 2012-2026 Andrew D. Zonenberg and contributors                                                         *
* All rights reserved.                                                                                                 *
*                                                                                                                      *
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the     *
* following conditions are met:                                                                                        *
*                                                                                                                      *
*    * Redistributions of source code must retain the above copyright notice, this list of conditions, and the         *
*      following disclaimer.                                                                                           *
*                                                                                                                      *
*    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the       *
*      following disclaimer in the documentation and/or other materials provided with the distribution.                *
*                                                                                                                      *
*    * Neither the name of the author nor the names of any contributors may be used to endorse or promote products     *
*      derived from this software without specific prior written permission.                                           *
*                                                                                                                      *
* THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED   *
* TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL *
* THE AUTHORS BE HELD LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES        *
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR       *
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
* POSSIBILITY OF SUCH DAMAGE.                                                                                          *
*                                                                                                                      *
***********************************************************************************************************************/

/**
	@file
	@author Frederic BORRY
	@brief Implementation of string utility functions
 */
#include "StringUtil.h"

#include <string>
#include <cwchar>
#include <clocale>
#include <cstdlib>

#ifdef _WIN32
#include <locale.h>
#endif

static void EnsureUtf8LocaleForCurrentThread()
{
#ifdef _WIN32
    static thread_local bool initialized = false;

    if (!initialized)
    {
        _configthreadlocale(_ENABLE_PER_THREAD_LOCALE);
        std::setlocale(LC_CTYPE, ".UTF-8");
        initialized = true;
    }
#else
    static bool initialized = false;

    if (!initialized)
    {
        std::setlocale(LC_CTYPE, "");
        initialized = true;
    }
#endif
}

/**
 * @brief Convert a std::wstring into an std::string
 * @param wstr the wstring to convert
 * @return the converted string
 */
std::string WstringToString(const std::wstring &wstr)
{
    EnsureUtf8LocaleForCurrentThread();

	std::mbstate_t state{};
	const wchar_t* src = wstr.c_str();
	std::size_t len = std::wcsrtombs(nullptr, &src, 0, &state);

	if (len == static_cast<std::size_t>(-1))
	return "Invalid wide-character string or invalid locale";

	std::string result(len, '\0');
	state = std::mbstate_t{};
	src = wstr.c_str();
	std::size_t converted = std::wcsrtombs(result.data(), &src, result.size(), &state);

	if (converted == static_cast<std::size_t>(-1))
	return "Wstring to string conversion failed";

	return result;
}

/**
 * @brief Convert a std::string into an std::wstring
 * @param wstr the string to convert
 * @return the converted wstring
 */
std::wstring StringToWstring(const std::string &str)
{
    EnsureUtf8LocaleForCurrentThread();

    std::mbstate_t state{};
    const char* src = str.c_str();
    std::size_t len = std::mbsrtowcs(nullptr, &src, 0, &state);

	if (len == static_cast<std::size_t>(-1))
        return L"Invalid UTF-8 string or invalid locale";

    std::wstring result(len, L'\0');
    state = std::mbstate_t{};
    src = str.c_str();
    std::size_t converted = std::mbsrtowcs(result.data(), &src, result.size(), &state);

    if (converted == static_cast<std::size_t>(-1))
        return L"String to wstring conversion failed";

    return result;
}
