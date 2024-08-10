#include "i_OpenSave.h"

//qt
#include <QString>
#include <QLineEdit>

// Uncomment to test Qt version under W32
//#define TEST_QT

#if defined(_WIN32) && !defined (TEST_QT)
    //win
    #include <windows.h>

    namespace {
        constexpr auto MAXFILE = 1024 * 16u;
    } // anon ns

    std::wstring filedlg::open(
            QWidget* aOwner,
            Zsv<wchar_t> aCaption,
            const Filters& aFilters,
            Zsv<wchar_t> aExtension,
            AddToRecent aAddToRecent,
            CheckForAccess aCheckForAccess)
    {
        std::wstring filter = filedlg::filterToW32(aFilters);

        Array1d<wchar_t> fname(MAXFILE);
        fname[0] = 0;

        OPENFILENAME ofn;
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = reinterpret_cast<HWND>(aOwner->winId());
        ofn.lpstrFilter = filter.c_str();
        ofn.lpstrFile = fname.buffer();
        ofn.nMaxFile = fname.size();
        ofn.lpstrTitle = aCaption.szOrNull();
        ofn.lpstrDefExt = aExtension.szOrNull();
        ofn.Flags = OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST
                 | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
        if (aAddToRecent == AddToRecent::NO)
            ofn.Flags |= OFN_DONTADDTORECENT;
        if (aCheckForAccess == CheckForAccess::NO)
            ofn.Flags |= OFN_NOTESTFILECREATE;

        if (::GetOpenFileName(&ofn))
            return ofn.lpstrFile;
        return {};
    }


    std::wstring filedlg::save(
            QWidget* aOwner,
            Zsv<wchar_t> aCaption,
            const Filters& aFilters,
            Zsv<wchar_t> aExtension,
            std::wstring_view aDefaultFname,
            AddToRecent aAddToRecent)
    {
        std::wstring filter = filedlg::filterToW32(aFilters);

        size_t sz = MAXFILE;
        constexpr auto Q_BIGGER = 2;
        if (!aDefaultFname.empty())
            sz = std::max(sz, aDefaultFname.length() * Q_BIGGER);
        std::wstring fname;
        fname.resize(sz);
        fname[0] = 0;
        if (!aDefaultFname.empty()) {
            // fname is always bigger than aDefaultFname, we did this
            auto nullPlace = std::copy(aDefaultFname.begin(), aDefaultFname.end(), fname.begin());
            // Do not forget to null-terminate
            *nullPlace = 0;
        }

        OPENFILENAME ofn;
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = reinterpret_cast<HWND>(aOwner->winId());
        ofn.lpstrFilter = filter.c_str();
        ofn.lpstrFile = fname.data();
        ofn.nMaxFile = fname.size();
        ofn.lpstrTitle = aCaption.szOrNull();
        ofn.lpstrDefExt = aExtension.szOrNull();
        ofn.Flags = OFN_ENABLESIZING | OFN_EXPLORER | OFN_OVERWRITEPROMPT
                 | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
        if (aAddToRecent == AddToRecent::NO)
            ofn.Flags |= OFN_DONTADDTORECENT;

        if (::GetSaveFileName(&ofn))
            return ofn.lpstrFile;
        return {};
    }
#else
    namespace {
        QString fileDir = QDir::homePath(); //todo [macOS] - fileDir save in project

        std::wstring getFile(QFileDialog& dialog)
        {
            if (dialog.exec() == QDialog::DialogCode::Accepted) {
                auto selectedFiles = dialog.selectedFiles();
                if (!selectedFiles.isEmpty()) {
                    fileDir = dialog.directory().absolutePath();
                    return selectedFiles.first().toStdWString();
                }
            }
            return L"";
        }
    }

    std::wstring filedlg::open(
        QWidget* aOwner,
        Zsv<wchar_t> aCaption,
        const Filters& aFilters,
        Zsv<wchar_t> aExtension,
        [[maybe_unused]] AddToRecent aAddToRecent, ///@todo [macos] add to recent files issue
        CheckForAccess aCheckForAccess)
    {
        QFileDialog dialog(
            aOwner,
            aCaption,
            fileDir,
            QString::fromStdWString(filterToQt(aFilters))
        );

        dialog.setAcceptMode(QFileDialog::AcceptOpen);
        dialog.setFileMode(QFileDialog::ExistingFile);

        auto fileFilter = QDir::Filter::Files | QDir::Filter::Dirs;
        if (aCheckForAccess == CheckForAccess::YES) {
            fileFilter |= QDir::Filter::Readable | QDir::Filter::Writable;
        }
        dialog.setFilter(fileFilter);
        dialog.setDefaultSuffix(aExtension);

        return getFile(dialog);
    }

    std::wstring filedlg::save(
        QWidget* aOwner,
        Zsv<wchar_t> aCaption,
        const Filters& aFilters,
        Zsv<wchar_t> aExtension,
        std::wstring_view aDefaultFname,
        [[maybe_unused]] AddToRecent aAddToRecent) ///@todo [macos] add to recent files issue
    {
        QString defFileName = (aDefaultFname.empty())
                    ? fileDir
                    : QString::fromWCharArray(aDefaultFname.data(), aDefaultFname.length());

        QFileDialog dialog(
            aOwner,
            aCaption,
            QDir{defFileName}.absolutePath(),
            QString::fromStdWString(filterToQt(aFilters))
        );

        dialog.setAcceptMode(QFileDialog::AcceptSave);
        dialog.setFilter(QDir::Files | QDir::Dirs | QDir::Writable);
        dialog.setDefaultSuffix(aExtension);

        return getFile(dialog);
    }

#endif


bool filedlg::browseLineEdit(
        QWidget* owner,
        Zsv<wchar_t> caption,
        const Filter& filter,
        Zsv<wchar_t> extension,
        QLineEdit* editor)
{
    /// @todo [L10n] string hardcoded here
    filedlg::Filters filters {
        filter, filedlg::ALL_FILES,
    };
    auto fname = filedlg::open(
                owner, caption, filters, extension,
                filedlg::AddToRecent::NO);
    if (!fname.empty()) {
        editor->setText(QString::fromStdWString(fname));
        return true;
    } else {
        return false;
    }
}
