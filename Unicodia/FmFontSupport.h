#ifndef FMFONTSUPPORT_H
#define FMFONTSUPPORT_H

#include <QDialog>
#include <QAbstractListModel>

#include "FontDef.h"

namespace Ui {
class FmFontSupport;
}


class FontSupportModel final : public QAbstractListModel
{
public:
    FontSource& fontSource;
    QString sample;

    struct Entry {
        QString name;
    };
    std::vector<Entry> entries;

    FontSupportModel(FontSource& aFontSource);

    size_t size() const noexcept { return entries.size(); }
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void loadFrom(char32_t c);
};

class FmFontSupport : public QDialog
{
    Q_OBJECT
    using This = FmFontSupport;
    using Super = QDialog;
public:
    explicit FmFontSupport(
            QWidget *parent,
            FontSource& aFontSource);
    ~FmFontSupport() override;

    void exec(char32_t c);

private:
    Ui::FmFontSupport *ui;
    FontSupportModel model;

    using Super::exec;

    void loadFrom(char32_t c);
    void selectFirstFontIf();

private slots:
    void selectionChanged(const QModelIndex& curr, const QModelIndex& prev);
};

#endif // FMFONTSUPPORT_H
