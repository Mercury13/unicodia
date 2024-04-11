#include "unibase.h"

// C++
#include <fstream>

// Unicode bases
#include "ucdcom.h"
#include "loader.h"
#include "data.h"

// Misc
#include "u_Strings.h"

using namespace std::string_view_literals;

void ucd::processMainBase(const BaseSink& sink)
{
    std::ifstream is(UCD_MAIN);
    std::string line;
    CpInfo info;
    char32_t firstCp = 0;
    std::string_view repeatedText = "REPEAT!! #"sv;
    while (std::getline(is, line)) {
        auto trimmed = str::trimSv(line);
        if (trimmed.empty())
            continue;

        auto vals = str::splitSv(trimmed, ';', false);
        if (vals.size() < 13)  // Actually 15, but used just 13
            continue;

        auto sCp = vals.at(0);
        info.cp = fromHex(sCp);
        info.generalCat = vals.at(2);
        info.bidiCat = vals.at(4);
        info.decimalValue = vals.at(6);
        info.digitValue = vals.at(7);
        info.numericValue = vals.at(8);

        // Check these numeric values, need smth
        if (!info.decimalValue.empty() && info.decimalValue != info.digitValue) {
            throw std::logic_error(
                str::cat("Cp ", sCp, " has decValue=", info.decimalValue,
                         " and digValue=", info.digitValue));
        }
        if (!info.digitValue.empty() && info.digitValue != info.numericValue) {
            throw std::logic_error(
                str::cat("Cp ", sCp, " has digValue=", info.digitValue,
                         " and numValue=", info.numericValue));
        }

        info.isMirrored = (vals.at(9) == "Y"sv);
        if (auto sUpper = vals.at(12); !sUpper.empty()) {
            info.upperCase = fromHex(sUpper);
        } else {
            info.upperCase = 0;
        }

        // Name is the last
        info.name = vals.at(1);
        if (info.name.starts_with('<')) { // SPECIAL NAMES
            if (info.name == "<control>"sv) {
                info.name = ""sv;
                sink.act(info);
            } else if (info.name == "<CJK Ideograph Extension A, First>"sv
                    || info.name == "<CJK Ideograph, First>"sv
                    || info.name == "<Hangul Syllable, First>"sv
                    || info.name == "<CJK Ideograph Extension B, First>"sv
                    || info.name == "<CJK Ideograph Extension C, First>"sv
                    || info.name == "<CJK Ideograph Extension D, First>"sv
                    || info.name == "<CJK Ideograph Extension E, First>"sv
                    || info.name == "<CJK Ideograph Extension F, First>"sv
                    || info.name == "<CJK Ideograph Extension G, First>"sv
                    || info.name == "<CJK Ideograph Extension H, First>"sv
                    || info.name == "<CJK Ideograph Extension I, First>"sv
                    || info.name == "<CJK Ideograph Extension J, First>"sv) {
                firstCp = info.cp;
                repeatedText = "CJK unified ideograph-#"sv;  // Let it be for Hangul
            } else if (info.name == "<Tangut Ideograph, First>"sv
                    || info.name == "<Tangut Ideograph Supplement, First>"sv) {
                firstCp = info.cp;
                repeatedText = "Tangut ideograph-#"sv;
            } else if (info.name == "<CJK Ideograph Extension A, Last>"sv
                    || info.name == "<CJK Ideograph, Last>"sv
                    || info.name == "<Tangut Ideograph, Last>"sv
                    || info.name == "<Tangut Ideograph Supplement, Last>"sv
                    || info.name == "<CJK Ideograph Extension B, Last>"sv
                    || info.name == "<CJK Ideograph Extension C, Last>"sv
                    || info.name == "<CJK Ideograph Extension D, Last>"sv
                    || info.name == "<CJK Ideograph Extension E, Last>"sv
                    || info.name == "<CJK Ideograph Extension F, Last>"sv
                    || info.name == "<CJK Ideograph Extension G, Last>"sv
                    || info.name == "<CJK Ideograph Extension H, Last>"sv
                    || info.name == "<CJK Ideograph Extension I, Last>"sv
                    || info.name == "<CJK Ideograph Extension J, Last>"sv) {
                auto lastCp = info.cp;
                info.name = repeatedText;
                for (char32_t i = firstCp; i <= lastCp; ++i) {
                    info.cp = i;
                    sink.act(info);
                }
            } else if (info.name == "<Hangul Syllable, Last>"sv) {
                /// @todo [urgent] What to do with Hangul?
            } else if (info.name == "<Non Private Use High Surrogate, First>"sv
                    || info.name == "<Non Private Use High Surrogate, Last>"sv
                    || info.name == "<Private Use High Surrogate, First>"sv
                    || info.name == "<Private Use High Surrogate, Last>"sv
                    || info.name == "<Low Surrogate, First>"sv
                    || info.name == "<Low Surrogate, Last>"sv
                    || info.name == "<Private Use, First>"sv
                    || info.name == "<Private Use, Last>"sv
                    || info.name == "<Plane 15 Private Use, First>"sv
                    || info.name == "<Plane 15 Private Use, Last>"sv
                    || info.name == "<Plane 16 Private Use, First>"sv
                    || info.name == "<Plane 16 Private Use, Last>"sv) {
                // do nothing
            } else {
                throw std::logic_error(
                    str::cat("Unknown special name ", info.name));
            }
        } else {    // NORMAL NAME
            // Decapitalise it here!
            std::string decapped = decapitalize(info.name, info.cp);
            info.name = decapped;
            sink.act(info);
        }
    }
}
