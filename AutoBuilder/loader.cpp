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


void LocalFile::cleanUp() const
{
    std::filesystem::remove(localName());
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
    constexpr std::string_view OLD_ATARI_8BIT_1 = "ATARI8IG.TXT";
    constexpr std::string_view OLD_ATARI_8BIT_2 = "ATARI8II.TXT";
    constexpr std::string_view OLD_ATARI_8BIT_3 = "ATARI8VG.TXT";
    constexpr std::string_view OLD_ATARI_8BIT_4 = "ATARI8VI.TXT";
    constexpr std::string_view OLD_MINITEL_1 = "MINITLG0.TXT";
    constexpr std::string_view OLD_MINITEL_2 = "MINITLG1.TXT";
    constexpr std::string_view OLD_MINITEL_STUB = "Minitel_stub.txt";
    constexpr std::string_view OLD_MSX_01 = "MSXICH.TXT";
    constexpr std::string_view OLD_MSX_02 = "MSXICHAE.TXT";
    constexpr std::string_view OLD_MSX_03 = "MSXICHAR.TXT";
    constexpr std::string_view OLD_MSX_04 = "MSXICHBG.TXT";
    constexpr std::string_view OLD_MSX_05 = "MSXICHBH.TXT";
    constexpr std::string_view OLD_MSX_06 = "MSXICHBR.TXT";
    constexpr std::string_view OLD_MSX_07 = "MSXICHJP.TXT";
    constexpr std::string_view OLD_MSX_08 = "MSXICHKR.TXT";
    constexpr std::string_view OLD_MSX_09 = "MSXICHRU.TXT";
    constexpr std::string_view OLD_MSX_10 = "MSXVID.TXT";
    constexpr std::string_view OLD_MSX_11 = "MSXVIDAE.TXT";
    constexpr std::string_view OLD_MSX_12 = "MSXVIDAR.TXT";
    constexpr std::string_view OLD_MSX_13 = "MSXVIDBG.TXT";
    constexpr std::string_view OLD_MSX_14 = "MSXVIDBH.TXT";
    constexpr std::string_view OLD_MSX_15 = "MSXVIDBR.TXT";
    constexpr std::string_view OLD_MSX_16 = "MSXVIDJP.TXT";
    constexpr std::string_view OLD_MSX_17 = "MSXVIDKR.TXT";
    constexpr std::string_view OLD_MSX_18 = "MSXVIDRU.TXT";
    constexpr std::string_view OLD_ORIC_1 = "ORICG0.TXT";
    constexpr std::string_view OLD_ORIC_2 = "ORICG1.TXT";
    constexpr std::string_view OLD_C64_1 = "C64IALT.TXT";
    constexpr std::string_view OLD_C64_2 = "C64IPRI.TXT";
    constexpr std::string_view OLD_C64_3 = "C64VALT.TXT";
    constexpr std::string_view OLD_C64_4 = "C64VPRI.TXT";
    constexpr std::string_view OLD_PET_1 = "CPETIALT.TXT";
    constexpr std::string_view OLD_PET_2 = "CPETIPRI.TXT";
    constexpr std::string_view OLD_PET_3 = "CPETVALT.TXT";
    constexpr std::string_view OLD_PET_4 = "CPETVPRI.TXT";
    constexpr std::string_view OLD_VIC_1 = "CVICIALT.TXT";
    constexpr std::string_view OLD_VIC_2 = "CVICIPRI.TXT";
    constexpr std::string_view OLD_VIC_3 = "CVICVALT.TXT";
    constexpr std::string_view OLD_VIC_4 = "CVICVPRI.TXT";
    constexpr std::string_view OLD_COMMOD_STUB = "Commodore_stub.txt";
    constexpr std::string_view OLD_RISC_1 = "RISCEFF.TXT";
    constexpr std::string_view OLD_RISC_2 = "RISCOSB.TXT";
    constexpr std::string_view OLD_RISC_3 = "RISCOSI.TXT";
    constexpr std::string_view OLD_RISC_4 = "RISCOSV.TXT";
    constexpr std::string_view OLD_SINC_1 = "ZX80.TXT";
    constexpr std::string_view OLD_SINC_2 = "ZX81.TXT";
    constexpr std::string_view OLD_TELETEXT_1 = "TELTXTG0.TXT";
    constexpr std::string_view OLD_TELETEXT_2 = "TELTXTG1.TXT";
    constexpr std::string_view OLD_TELETEXT_3 = "TELTXTG2.TXT";
    constexpr std::string_view OLD_TELETEXT_4 = "TELTXTG3.TXT";
    constexpr std::string_view OLD_TRS_01 = "TRSM1ICH.TXT";
    constexpr std::string_view OLD_TRS_02 = "TRSM1ORG.TXT";
    constexpr std::string_view OLD_TRS_03 = "TRSM1REV.TXT";
    constexpr std::string_view OLD_TRS_04 = "TRSM3IIN.TXT";
    constexpr std::string_view OLD_TRS_05 = "TRSM3IJP.TXT";
    constexpr std::string_view OLD_TRS_06 = "TRSM3IRV.TXT";
    constexpr std::string_view OLD_TRS_07 = "TRSM3VIN.TXT";
    constexpr std::string_view OLD_TRS_08 = "TRSM3VJP.TXT";
    constexpr std::string_view OLD_TRS_09 = "TRSM3VRV.TXT";
    constexpr std::string_view OLD_TRS_10 = "TRSM4AIA.TXT";
    constexpr std::string_view OLD_TRS_11 = "TRSM4AIP.TXT";
    constexpr std::string_view OLD_TRS_12 = "TRSM4AIR.TXT";
    constexpr std::string_view OLD_TRS_13 = "TRSM4AVA.TXT";
    constexpr std::string_view OLD_TRS_14 = "TRSM4AVP.TXT";
    constexpr std::string_view OLD_TRS_15 = "TRSM4AVR.TXT";
    constexpr std::string_view OLD_COCO_1 = "COCOICHG.TXT";
    constexpr std::string_view OLD_COCO_2 = "COCOSGR4.TXT";
    constexpr std::string_view OLD_COCO_3 = "COCOSGR6.TXT";
    constexpr std::string_view OLD_KORVET = "Korvet-Koi.txt";

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
        OldCompInfo::mk<OLD_ATARI_8BIT_1>(uc::OldComp::ATARI_8BIT),
        OldCompInfo::mk<OLD_ATARI_8BIT_2>(uc::OldComp::ATARI_8BIT),
        OldCompInfo::mk<OLD_ATARI_8BIT_3>(uc::OldComp::ATARI_8BIT),
        OldCompInfo::mk<OLD_ATARI_8BIT_4>(uc::OldComp::ATARI_8BIT),
        OldCompInfo::mk<OLD_MINITEL_1>(uc::OldComp::MINITEL),
        OldCompInfo::mk<OLD_MINITEL_2>(uc::OldComp::MINITEL),
        OldCompInfo::mk<OLD_MINITEL_STUB>(uc::OldComp::MINITEL),
        OldCompInfo::mk<OLD_MSX_01>(uc::OldComp::MSX),
        OldCompInfo::mk<OLD_MSX_02>(uc::OldComp::MSX),
        OldCompInfo::mk<OLD_MSX_03>(uc::OldComp::MSX),
        OldCompInfo::mk<OLD_MSX_04>(uc::OldComp::MSX),
        OldCompInfo::mk<OLD_MSX_05>(uc::OldComp::MSX),
        OldCompInfo::mk<OLD_MSX_06>(uc::OldComp::MSX),
        OldCompInfo::mk<OLD_MSX_07>(uc::OldComp::MSX),
        OldCompInfo::mk<OLD_MSX_08>(uc::OldComp::MSX),
        OldCompInfo::mk<OLD_MSX_09>(uc::OldComp::MSX),
        OldCompInfo::mk<OLD_MSX_10>(uc::OldComp::MSX),
        OldCompInfo::mk<OLD_MSX_11>(uc::OldComp::MSX),
        OldCompInfo::mk<OLD_MSX_12>(uc::OldComp::MSX),
        OldCompInfo::mk<OLD_MSX_13>(uc::OldComp::MSX),
        OldCompInfo::mk<OLD_MSX_14>(uc::OldComp::MSX),
        OldCompInfo::mk<OLD_MSX_15>(uc::OldComp::MSX),
        OldCompInfo::mk<OLD_MSX_16>(uc::OldComp::MSX),
        OldCompInfo::mk<OLD_MSX_17>(uc::OldComp::MSX),
        OldCompInfo::mk<OLD_MSX_18>(uc::OldComp::MSX),
        OldCompInfo::mk<OLD_ORIC_1>(uc::OldComp::ORIC),
        OldCompInfo::mk<OLD_ORIC_2>(uc::OldComp::ORIC),
        OldCompInfo::mk<OLD_C64_1>(uc::OldComp::COMMODORE),
        OldCompInfo::mk<OLD_C64_2>(uc::OldComp::COMMODORE),
        OldCompInfo::mk<OLD_C64_3>(uc::OldComp::COMMODORE),
        OldCompInfo::mk<OLD_C64_4>(uc::OldComp::COMMODORE),
        OldCompInfo::mk<OLD_PET_1>(uc::OldComp::COMMODORE),
        OldCompInfo::mk<OLD_PET_2>(uc::OldComp::COMMODORE),
        OldCompInfo::mk<OLD_PET_3>(uc::OldComp::COMMODORE),
        OldCompInfo::mk<OLD_PET_4>(uc::OldComp::COMMODORE),
        OldCompInfo::mk<OLD_VIC_1>(uc::OldComp::COMMODORE),
        OldCompInfo::mk<OLD_VIC_2>(uc::OldComp::COMMODORE),
        OldCompInfo::mk<OLD_VIC_3>(uc::OldComp::COMMODORE),
        OldCompInfo::mk<OLD_VIC_4>(uc::OldComp::COMMODORE),
        OldCompInfo::mk<OLD_COMMOD_STUB>(uc::OldComp::COMMODORE),
        OldCompInfo::mk<OLD_RISC_1>(uc::OldComp::RISC_OS),
        OldCompInfo::mk<OLD_RISC_2>(uc::OldComp::RISC_OS),
        OldCompInfo::mk<OLD_RISC_3>(uc::OldComp::RISC_OS),
        OldCompInfo::mk<OLD_RISC_4>(uc::OldComp::RISC_OS),
        OldCompInfo::mk<OLD_SINC_1>(uc::OldComp::SINCLAIR),
        OldCompInfo::mk<OLD_SINC_2>(uc::OldComp::SINCLAIR),
        OldCompInfo::mk<OLD_TELETEXT_1>(uc::OldComp::TELETEXT),
        OldCompInfo::mk<OLD_TELETEXT_2>(uc::OldComp::TELETEXT),
        OldCompInfo::mk<OLD_TELETEXT_3>(uc::OldComp::TELETEXT),
        OldCompInfo::mk<OLD_TELETEXT_4>(uc::OldComp::TELETEXT),
        OldCompInfo::mk<OLD_TRS_01>(uc::OldComp::TANDY_TRS80),
        OldCompInfo::mk<OLD_TRS_02>(uc::OldComp::TANDY_TRS80),
        OldCompInfo::mk<OLD_TRS_03>(uc::OldComp::TANDY_TRS80),
        OldCompInfo::mk<OLD_TRS_04>(uc::OldComp::TANDY_TRS80),
        OldCompInfo::mk<OLD_TRS_05>(uc::OldComp::TANDY_TRS80),
        OldCompInfo::mk<OLD_TRS_06>(uc::OldComp::TANDY_TRS80),
        OldCompInfo::mk<OLD_TRS_07>(uc::OldComp::TANDY_TRS80),
        OldCompInfo::mk<OLD_TRS_08>(uc::OldComp::TANDY_TRS80),
        OldCompInfo::mk<OLD_TRS_09>(uc::OldComp::TANDY_TRS80),
        OldCompInfo::mk<OLD_TRS_10>(uc::OldComp::TANDY_TRS80),
        OldCompInfo::mk<OLD_TRS_11>(uc::OldComp::TANDY_TRS80),
        OldCompInfo::mk<OLD_TRS_12>(uc::OldComp::TANDY_TRS80),
        OldCompInfo::mk<OLD_TRS_13>(uc::OldComp::TANDY_TRS80),
        OldCompInfo::mk<OLD_TRS_14>(uc::OldComp::TANDY_TRS80),
        OldCompInfo::mk<OLD_TRS_15>(uc::OldComp::TANDY_TRS80),
        OldCompInfo::mk<OLD_COCO_1>(uc::OldComp::TANDY_COCO),
        OldCompInfo::mk<OLD_COCO_2>(uc::OldComp::TANDY_COCO),
        OldCompInfo::mk<OLD_COCO_3>(uc::OldComp::TANDY_COCO),
        OldCompInfo::mk<OLD_KORVET>(uc::OldComp::KORVET),
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


void cleanUpLoader()
{
    for (auto& lf : allLocalFiles) {
        lf.cleanUp();
    }
    for (auto& oc: allOldCompsFiles) {
        oc.cleanUp();
    }
}


std::span<const OldCompInfo> allOldComps()
{
    return allOldCompsFiles;
}
