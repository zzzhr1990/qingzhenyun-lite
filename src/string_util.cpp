//
// Created by herui on 2019/12/3.
//

#include <qingzhen/string_util.h>
#include <cpprest/details/basic_types.h>
#include <cpprest/asyncrt_utils.h>
#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32


utility::string_t qingzhen::string_util::pretty_bytes(const int64_t &bytes) {
    using ppy = decltype(_XPLATSTR('a'));
    const ppy *suffixes[7];
    suffixes[0] = _XPLATSTR("B");
    suffixes[1] = _XPLATSTR("KB");
    suffixes[2] = _XPLATSTR("MB");
    suffixes[3] = _XPLATSTR("GB");
    suffixes[4] = _XPLATSTR("TB");
    suffixes[5] = _XPLATSTR("PB");
    suffixes[6] = _XPLATSTR("EB");
    int s = 0; // which suffix to use
    auto count = static_cast<double>(bytes);
    while (count >= 1024 && s < 7) {
        s++;
        count /= 1024;
    }
    utility::ostringstream_t out;
    if (count - floor(count) == 0.0) {
        out << count << _XPLATSTR(" ") << suffixes[s];
    } else {
        out.precision(2);
        out << std::fixed << count << _XPLATSTR(" ") << suffixes[s];
    }
    return out.str();
}


utility::string_t qingzhen::string_util::ansi_to_string_t(const char* chars)
{

	return utility::conversions::to_string_t(chars);
}

utility::string_t qingzhen::string_util::ansi_to_string_t(const std::string& string)
{
	return utility::conversions::to_string_t(string);
}

std::string qingzhen::string_util::string_t_to_ansi(const utility::string_t& str)
{
	// utility::conversions::to
#ifdef _WIN32
	return qingzhen::string_util::string_t_to_ansi(str.c_str());
#endif
	return utility::conversions::to_utf8string(str);
}

std::string qingzhen::string_util::string_t_to_ansi(const wchar_t* str)
{
#ifdef _WIN32
	auto wstr_len = wcslen(str);
	if (wstr_len< 0) {
		return std::string();
	}
	int num_chars = WideCharToMultiByte(CP_ACP, 0, str, wstr_len, NULL, 0, NULL, NULL);
	CHAR* strTo = (CHAR*)malloc((num_chars + 1) * sizeof(CHAR));
	if (strTo)
	{
		WideCharToMultiByte(CP_ACP, 0, str, wstr_len, strTo, num_chars, NULL, NULL);
		strTo[num_chars] = '\0';
	}
	auto res = std::string(strTo);
	free(strTo);
	return res;
#else
	return utility::conversions::to_utf8string(str);
#endif // _WIN32

}

std::string qingzhen::string_util::string_t_to_ansi(const char* str)
{
	return utility::conversions::to_utf8string(str);
}