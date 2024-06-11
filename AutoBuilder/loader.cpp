// My header
#include "loader.h"

// C++
#include <cstdlib>
#include <string_view>
#include <string>
#include <filesystem>

// Libs
#include "u_TypedFlags.h"

using namespace std::string_view_literals;

void LocalFile::preload() const
{
    static constexpr int N_UPDIRS = 4;

    std::filesystem::path rqPath(fname);
    auto destPath = rqPath.filename();
    std::string testPath { fname };
    for (int i = 0; i < N_UPDIRS; ++i) {
        std::filesystem::path srcPath(testPath);
        if (std::filesystem::exists(srcPath)) {
            // filesystem::copy does not overwrite, even with options
            if (std::filesystem::exists(destPath))
                std::filesystem::remove(destPath);
            // Will throw if problems, that’s OK
            std::filesystem::copy(srcPath, destPath);
            // Check once again for existence
            if (!std::filesystem::exists(destPath)) {
                throw std::runtime_error("Copied " + rqPath.generic_string() + ", still not found");
            }
            return;
        }
        testPath = "../" + testPath;
    }
    throw std::runtime_error("Cannot find file " + rqPath.generic_string());
}


std::filesystem::path LocalFile::localName() const
{
    return std::filesystem::path{fname}.filename();
}


namespace {

    template <std::string_view const&... Strs>
    struct join
    {
        // Join all strings into a single std::array of chars
        static constexpr auto impl() noexcept
        {
            constexpr std::size_t len = (Strs.size() + ... + 0);
            std::array<char, len + 1> arr{};
            auto append = [i = 0, &arr](auto const& s) mutable {
                for (auto c : s) arr[i++] = c;
            };
            (append(Strs), ...);
            arr[len] = 0;
            return arr;
        }
        // Give the joined string static storage
        static constexpr auto arr = impl();
        // View as a std::string_view
        static constexpr std::string_view value {arr.data(), arr.size() - 1};
    };
    // Helper to get the value out
    template <std::string_view const&... Strs>
    static constexpr auto join_v = join<Strs...>::value;

}   // anon namespace


consteval OldCompInfo::OldCompInfo(std::string_view aFname, uc::OldComp aFlag)
    : LocalFile(aFname), flag(aFlag) {}

template <const std::string_view& aFname>
consteval OldCompInfo OldCompInfo::mk(uc::OldComp aFlag)
{
    return OldCompInfo{join_v<MAPPINGS, aFname>, aFlag};
}

namespace {

    class Action {  // interface
    public:
        virtual void exec() const = 0;
        virtual ~Action() = default;
    };

    class NoAction final : public Action
    {
    public:
        void exec() const override {}
        //static const NoAction INST;
    };

    //const NoAction NoAction::INST;

    // class SingleZip final : public Action
    // {
    // public:
    //     constexpr SingleZip(
    //             std::vector<unsigned char>& aDest,
    //             std::string_view aArc, std::string_view aFile)
    //         : dest(aDest), arc(aArc), file(aFile) {}
    //     void exec() const override;
    // private:
    //     std::vector<unsigned char>& dest;
    //     std::string_view arc, file;
    // };

    // void SingleZip::exec() const
    // {
    //     Zippy::ZipArchive za;
    //     za.Open(std::string{arc});
    //     auto entry = za.GetEntry(std::string{file});
    //     auto data = entry.GetData();
    //     if (data.empty())
    //         throw std::logic_error("File not found in archive");
    //     dest = std::move(data);
    // }


    // Step flags
    enum class Stfg {
        FINAL = 1,
    };

    struct Step {
        std::string_view what;
        const Action& action;
        std::string_view file;
        Flags<Stfg> flags;
    };

    constinit const LocalFile allLocalFiles[] {
        { RAWDATA ENTITIES_JSON },
        { RAWDATA UCD_PVA },
        { RAWDATA UCD_NAMES },
        { RAWDATA UCD_SCRIPTS },
        { RAWDATA UCD_ALIASES },
        { RAWDATA UCD_PROP },
        { RAWDATA UCD_BLOCKS },
        { RAWDATA UCD_MAIN },
        { RAWDATA DER_AGE },
        { RAWDATA DER_CORE },
        { RAWDATA DER_NAME },
        { RAWDATA HAN_DIC },
        { RAWDATA HAN_NUM_VALUE },
        { RAWDATA HAN_IRG },
        { RAWDATA EMOJI_TEST },
        { RAWDATA EGYP_UNICODE },
        { RAWDATA EGYP_DESCRIPTION },
        { MISCFILES LIBRARY_XML },
        { MISCFILES SUTTON_TXT }
    };

    constexpr std::string_view OLD_AMSTRAD_CPC_1 = "AMSCPC.TXT";
    constexpr std::string_view OLD_AMSTRAD_CPC_2 = "AMSCPCI.TXT";
    constexpr std::string_view OLD_AMSTRAD_CPC_3 = "AMSCPCV.TXT";
    constexpr std::string_view OLD_APPLE2_1 = "APL2ALT1.TXT";
    constexpr std::string_view OLD_APPLE2_2 = "APL2ALT2.TXT";
    constexpr std::string_view OLD_APPLE2_3 = "APL2ICHG.TXT";
    constexpr std::string_view OLD_APPLE2_4 = "APL2PRIM.TXT";
    constexpr std::string_view OLD_AQUARIUS = "AQUARIUS.TXT";
    constexpr std::string_view OLD_ATARI_ST_1 = "ATARISTI.TXT";
    constexpr std::string_view OLD_ATARI_ST_2 = "ATARISTV.TXT";

    constinit const OldCompInfo allOldCompsFiles[] {
        // Old computers
        OldCompInfo::mk<OLD_AMSTRAD_CPC_1>(uc::OldComp::AMSTRAD_CPC),
        OldCompInfo::mk<OLD_AMSTRAD_CPC_2>(uc::OldComp::AMSTRAD_CPC),
        OldCompInfo::mk<OLD_AMSTRAD_CPC_3>(uc::OldComp::AMSTRAD_CPC),
        OldCompInfo::mk<OLD_APPLE2_1>(uc::OldComp::APPLE),
        OldCompInfo::mk<OLD_APPLE2_2>(uc::OldComp::APPLE),
        OldCompInfo::mk<OLD_APPLE2_3>(uc::OldComp::APPLE),
        OldCompInfo::mk<OLD_APPLE2_4>(uc::OldComp::APPLE),
        OldCompInfo::mk<OLD_AQUARIUS>(uc::OldComp::AQUARIUS),
        OldCompInfo::mk<OLD_ATARI_ST_1>(uc::OldComp::ATARI_ST),
        OldCompInfo::mk<OLD_ATARI_ST_2>(uc::OldComp::ATARI_ST),
    };

}

void checkLoader()
{
    for (auto& lf : allLocalFiles) {
        lf.preload();
    }
    for (auto& oc: allOldCompsFiles) {
        oc.preload();
    }
}


std::span<const OldCompInfo> allOldComps()
{
    return allOldCompsFiles;
}
