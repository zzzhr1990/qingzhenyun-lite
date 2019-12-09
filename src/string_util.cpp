//
// Created by herui on 2019/12/3.
//

#include <qingzhen/string_util.h>

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
