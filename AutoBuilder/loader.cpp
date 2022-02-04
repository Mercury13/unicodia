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

    constexpr const char* UCD_ZIP = "ucd.all.flat.zip";
    constexpr const char* UCD_XML = "ucd.all.flat.xml";

    constinit const CurlAction AC_UCD_ZIP { "https://www.unicode.org/Public/14.0.0/ucdxml/ucd.all.flat.zip", UCD_ZIP };
    constinit const SingleZip AC_UCD_XML { UCD_ZIP, UCD_XML };

    constinit const Step allSteps[] {
        { "Load UCD XML database", AC_UCD_ZIP, UCD_ZIP, NO_FLAGS },
        { "Unzip UCD XML database", AC_UCD_XML, UCD_XML, Stfg::FINAL },
    };

}


bool checkLoader()
{
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
