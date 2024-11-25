// My header
#include "stringlib.h"

// Libs
#include "mojibake.h"

// Unicode
#include "ucdcom.h"

// C++
#include <iostream>


void StringLib::forceRemember(
        StringData*& r,
        char32_t subj,
        uc::TextRole role,
        std::string_view s)
{
    auto r1 = rememberInner(subj, role, s);
    if (!r)
        r = r1.data;
}


RememberResult StringLib::rememberInner(
        char32_t subj, uc::TextRole role, std::string_view aS)
{
    auto func = [this](char32_t c) {
        if (c >= 128)
            fNonAscii.insert(c);
    };
    mojibake::copyS(aS.begin(), aS.end(),
            mojibake::Utf32CallIterator(func));
    auto& v = fInOrder.emplace_back(StringData{
                    .s = std::string { aS },
                    .subj = subj,
                    .offset = static_cast<int>(fLength),
                    .role = role,
                    .isLast = false });
    fLength += (aS.length() + 2);
    // One byte or two?
    if (aS.length() >= uc::detail::MIN_2BYTE_STRING)
        ++fLength;
    return { .data =&v, .wasIns = true };
}

RememberResult StringLib::remember(
        char32_t subj, uc::TextRole role, std::string_view s)
{
    if (role == uc::TextRole::MAIN_NAME) {
        if (s.find('#') != std::string_view::npos) {
            return {
                .data = nullptr,
                .wasIns = false
            };
        }
    }
    return rememberInner(subj, role, s);
}


void StringLib::finishCp()
{
    if (!fInOrder.empty()) {
        auto& bk = fInOrder.back();
        if (!bk.isLast) {
            bk.isLast = true;
            // +1: command CMD_END
            ++fLength;
        }
    }
}


void StringLib::write(std::ostream& os)
{
    char text[40];
    os << R"(u8"\0")" "\n";    // for CPs that have no strings
    for (auto& v : fInOrder) {
        if (v.s.length() < uc::detail::MIN_2BYTE_STRING) {
            // 1-byte string
            snprintf(text, std::size(text), R"(u8"\x%02X\x%02X" ")",
                     static_cast<unsigned>(v.role),
                     static_cast<unsigned>(v.s.length()));
        } else {
            // 2-byte string
            snprintf(text, std::size(text), R"(u8"\x%02X\x%02X\x%02X" ")",
                     static_cast<unsigned>(v.role),
                     static_cast<unsigned>(v.s.length() >> 8) + uc::detail::MIN_2BYTE_STRING,
                     static_cast<unsigned char>(v.s.length()));
        }
        os << text << encodeC(v.s) << R"("  )";
        if (v.isLast)
            os << R"("\0")";
        os << "  // " << std::hex << static_cast<int>(v.subj) << '\n';
    }
}
