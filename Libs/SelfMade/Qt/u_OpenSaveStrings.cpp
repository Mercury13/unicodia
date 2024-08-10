#include "u_OpenSaveStrings.h"

#include "u_Strings.h"

const filedlg::Filter filedlg::ALL_FILES { L"All files", L"*" };

///// Filter ///////////////////////////////////////////////////////////////////


const wchar_t* filedlg::Filter::extension() const
{
    // Should start with star-dot
    if (!fileMask.starts_with(L"*."))
        return L"";
    // Should not have other stars, questions, spaces
    if (fileMask.find_first_of(L"*? ", 2) != std::wstring::npos)
        return L"";
    // return ".ext"
    return fileMask.c_str() + 1;
}


///// Misc functions ///////////////////////////////////////////////////////////

std::wstring filedlg::filterToW32(const Filters& aFilter)
{
    if (aFilter.empty()) // very edge case: return \0 + auto. \0
        return L"\0";
    std::wstring r;
    for (const Filter& f : aFilter) {
        std::wstring fileMaskCode = f.fileMask;
        str::replace(fileMaskCode, L' ', L';');
        r += f.description + L" (" + f.fileMask + L")" + L'\0' + fileMaskCode + L'\0';
    }
    // W32 filter should end with twin 0, one is here, the other is automatic
    return r;
}

std::wstring filedlg::filterToQt(const Filters& aFilter)
{
    if (aFilter.empty()) // very edge case: return nothing
        return {};
    std::wstring r;
    for (const Filter& f : aFilter)
        r += f.description + L" (" + f.fileMask + L");;";
    return r.erase(r.size() - 2); //remove last ";;" for default All files (*)
}
