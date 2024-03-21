#ifndef FMMAIN_H
#define FMMAIN_H

// C++
#include <filesystem>

// Qt
#include <QMainWindow>
#include <QAbstractTableModel>

QT_BEGIN_NAMESPACE
namespace Ui { class FmMain; }
QT_END_NAMESPACE

enum class DiffReason {
    ADDED, DELETED, RESIZED, CHANGED
};

struct DiffLine
{
    DiffReason reason = DiffReason::ADDED;
    std::wstring name;
    QPixmap pix;
};

class DiffModel : public QAbstractTableModel
{
public:
    DiffModel();

    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    std::filesystem::path dirVersioned, dirWorking;
    QIcon icoAdded, icoDeleted, icoResized, icoChanged;

    /// Adds a file that’s versioned and missing in working (deleted)
    void addVersioned(std::wstring_view name);
    /// Adds a file that’s not versioned and appeared in working (added)
    void addWorking(std::wstring_view name);
    /// Adds a file that’s resized (no matter file size of pix size)
    void addResized(std::wstring_view name);
    void addResized(std::wstring_view name, QPixmap&& pix);
    void checkChanged(std::wstring_view name,
                      unsigned long long versSize,
                      unsigned long long workSize);
    void clear() { lines.clear(); }
    QPixmap diffPix(size_t index) const;
    /// @return [+] copied
    bool copyAsGood(size_t index) const;

    using QAbstractTableModel::beginResetModel;
    using QAbstractTableModel::endResetModel;
private:
    std::vector<DiffLine> lines;
};

class FmMain : public QMainWindow
{
    Q_OBJECT
    using This = FmMain;
public:
    FmMain(QWidget *parent = nullptr);
    ~FmMain() override;

    struct Config {
        QString versioned;
        QString working;

        /// @return [+] can save to file
        bool isPresent() const;

        /// @return [+] can work
        bool isFull() const;
    };

    Config config();

    void saveConfig();
    void loadConfig();

private:
    Ui::FmMain *ui;
    std::filesystem::path configPath;
    DiffModel diffModel;

private slots:
    void run();
    void diffCurrentChanged(const QModelIndex& index);
    void copyAsGood();
};
#endif // FMMAIN_H
