// My header
#include "loader.h"

// C++
#include <cstdlib>
#include <string_view>
#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>

// Libs
#include "u_TypedFlags.h"
#include "Zippy.hpp"


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

    class CurlAction final : public Action
    {
    public:
        constexpr CurlAction(std::string_view aUrl, std::string_view aFname)
            : url(aUrl), fname(aFname) {}
        void exec() const override;
    private:
        std::string_view url, fname;
    };

    class SingleZip final : public Action
    {
    public:
        constexpr SingleZip(std::string_view aArc, std::string_view aFile)
            : arc(aArc), file(aFile) {}
        void exec() const override;
    private:
        std::string_view arc, file;
    };

    void SingleZip::exec() const
    {
        Zippy::ZipArchive za;
        za.Open(std::string{arc});
        auto entry = za.GetEntry(std::string{file});
        auto data = entry.GetData();
        if (data.empty())
            throw std::logic_error("File not found in archive");
        std::ofstream os(std::filesystem::path(file), std::ios::binary);
        static_assert(sizeof(decltype(data)::value_type) == sizeof(char));
        os.write(reinterpret_cast<const char*>(data.data()), data.size());
    }

    void CurlAction::exec() const
    {
        std::string s = "curl --output ";
        s.append(fname);
        s.append(" ");
        s.append(url);
        system(s.c_str());
    }

    constexpr int N_UPDIRS = 4;

    class LocalFile
    {
    public:
        constexpr LocalFile(std::string_view aFname) : fname(aFname) {}
        void preload() const;
    private:
        std::string_view fname;
    };

    void LocalFile::preload() const
    {
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

    constinit const CurlAction AC_UCD_ZIP { "https://www.unicode.org/Public/15.0.0/ucdxml/ucd.all.flat.zip", UCD_ZIP };
    constinit const SingleZip AC_UCD_XML { UCD_ZIP, UCD_XML };
    constinit const CurlAction AC_UCD_NAMES { "https://www.unicode.org/Public/15.0.0/ucd/NamesList.txt", UCD_NAMES };
    constinit const CurlAction AC_EMOJI { "https://unicode.org/Public/emoji/15.0/emoji-test.txt", EMOJI_TEST };
    constinit const CurlAction AC_EGYP_UNICODE { "https://mjn.host.cs.st-andrews.ac.uk/egyptian/unicode/signunicode.xml", EGYP_UNICODE };
    constinit const CurlAction AC_EGYP_DESC { "https://mjn.host.cs.st-andrews.ac.uk/egyptian/unicode/signdescriptioneng.xml", EGYP_DESCRIPTION };

    constinit const LocalFile allLocalFiles[] {
        { MISCFILES ENTITIES_HTML },
        { MISCFILES LIBRARY_XML },
        { MISCFILES SUTTON_TXT },
        { "NotoEmoji/" SINGLEEMOJI_TXT },
    };

    constinit const Step allSteps[] {
        { "Load UCD XML database", AC_UCD_ZIP, UCD_ZIP, NO_FLAGS },
        { "Unzip UCD XML database", AC_UCD_XML, UCD_XML, Stfg::FINAL },
        { "Load UCD names list", AC_UCD_NAMES, UCD_NAMES, Stfg::FINAL },
        { "Load emoji test", AC_EMOJI, EMOJI_TEST, Stfg::FINAL },
        { "Load Egyptian Unicode", AC_EGYP_UNICODE, EGYP_UNICODE, Stfg::FINAL },
        { "Load Egyptian descriptions", AC_EGYP_DESC, EGYP_DESCRIPTION, Stfg::FINAL },
    };

}


bool checkLoader()
{
    for (auto& lf : allLocalFiles) {
        lf.preload();
    }
    for (auto& st : allSteps) {
        if (st.flags.have(Stfg::FINAL)) {
            if (!std::filesystem::exists(st.file))
                return false;
        }
    }
    return true;
}


void runLoader()
{
    for (auto& st : allSteps) {
        // Make backup file: if e.g. no Internet, the file will be intact
        std::string bakName { st.file };
            bakName.append(".bak");
        if (std::filesystem::exists(st.file)) {
            std::filesystem::remove(bakName);
            std::filesystem::rename(st.file, bakName);
        }
        std::cout << st.what << "..." << std::endl;
        st.action.exec();
        if (!std::filesystem::exists(st.file))
            throw std::logic_error("Step failed");
        // Remove backup file
        std::filesystem::remove(bakName);
    }
    std::cout << "ALL STEPS DONE" << std::endl << std::endl;
}
