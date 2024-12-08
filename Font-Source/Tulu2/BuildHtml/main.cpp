#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

// Tulu consonant
struct Cons {
    char32_t code;
    std::string_view ascName;
    std::u8string_view niceName;
    struct Issues {
        std::string_view techVisible {};    ///< technical issues visible → really bad
        std::string_view bad {};            ///< bad rendering, better fix
        std::string_view neglect {};        ///< bad rendering, just neglect
        std::string_view nice {};           ///< other but nice rendering
    } issues {};
    /// KK, KKh, KN, KNn etc are always written.
    /// If you need to write e.g. KKhV and KTV:
    ///   "KhV,TV"
    std::string_view attestedBigTails;
};

struct Vowel {
    char32_t code;
    /// Vocalic names: Qr, Qrr
    std::string_view ascName;
    std::u8string_view niceName;
};


#define FNAME_IN "TuluTemplate.html"

std::string readAll(std::istream& is)
{
    std::stringstream ss;
    ss << is.rdbuf();
    return std::move(ss).str();
}

struct FindInd {
    size_t pos = std::string_view::npos;
    unsigned nSpaces = 0;
};

FindInd findIndented(std::string_view s, std::string_view text)
{
    FindInd r;
    r.pos = s.find(text);
    if (r.pos != std::string_view::npos) {
        while (r.pos > 0 && s[r.pos] == ' ') {
            --r.pos;
            ++r.nSpaces;
        }
    }
    return r;
}

int main()
{
    try {
        // Read file’s content
        std::ifstream is(FNAME_IN);
        if (!is.is_open())
            throw std::logic_error("File " FNAME_IN " not open");
        auto sContent = readAll(is);
        std::string_view content = sContent;

        // Find position of </table>
        auto [pos, nSpaces] = findIndented(content, "</table>");
        if (pos == std::string_view::npos)
            throw std::logic_error("Cannot find </table> in " FNAME_IN);

        is.close();
    } catch (const std::exception& e) {
        std::cout << "ERR: " << e.what() << '\n';
    }
}
