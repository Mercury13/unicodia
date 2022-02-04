// My header
#include "loader.h"

// C++
#include <cstdlib>
#include <string_view>
#include <string>
#include <filesystem>
#include <iostream>

// Libs
#include "u_TypedFlags.h"


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

    constexpr const char* UCD_ALL = "ucd.all.flat.zip";

    constinit const CurlAction AC_UCD_ALL { "https://www.unicode.org/Public/14.0.0/ucdxml/ucd.all.flat.zip", UCD_ALL };

    constinit const Step allSteps[] {
        { "Load UCD XML database", AC_UCD_ALL, UCD_ALL, Stfg::FINAL },
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
