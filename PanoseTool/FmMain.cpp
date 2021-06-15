#include "FmMain.h"
#include "ui_FmMain.h"

// Qt
#include <QFileDialog>
#include <QFile>

// Libs
#include "u_Qstrings.h"

// Project-local
#include "Ttf.h"

FmMain::FmMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FmMain)
{
    ui->setupUi(this);
}

FmMain::~FmMain()
{
    delete ui;
}


void FmMain::on_btOpen_clicked()
{
    try {
        auto fname = QFileDialog::getOpenFileName(
                    this,           // parent
                    "Open font",    // caption
                    {},             // dir
                    "TTF fonts (*.ttf)");   // filter
        if (!fname.isEmpty()) {
            QFile f(fname);
            if (!f.open(QIODevice::ReadOnly))
                throw std::logic_error("Cannot open file for reading");
            Ttf ttf(f);
            auto panose = ttf.readPanoseData();
            QString s;
            char buf[20];
            for (auto v : panose.fit) {
                auto r = std::to_chars(std::begin(buf), std::end(buf), v);
                if (!s.isEmpty())
                    s += ' ';
                str::append(s, std::string_view(buf, r.ptr));
            }
            str::append(s, " /");
            for (auto v : panose.scripts) {
                s += ' ';
                snprintf(buf, sizeof(buf), " %02x", (unsigned int)v);
                s += buf;
            }
            ui->lbPanose->setText(s);
            lastName = fname;
            ui->btErase->setEnabled(true);
        }
    } catch (const std::exception& e) {
        ui->lbPanose->setText(QString("ERROR: ") + e.what());
    }
}


void FmMain::on_btErase_clicked()
{
    try {
        if (lastName.isEmpty())
            return;
        QFile f(lastName);
        if (!f.open(QIODevice::ReadWrite))
            throw std::logic_error("Cannot open file for modifying");
        Ttf ttf(f);
        PanoseData pd { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
        ttf.writePanoseData(pd);
        ui->lbPanose->setText("Successfully erased PANOSE");
    } catch (const std::exception& e) {
        ui->lbPanose->setText(QString("ERROR: ") + e.what());
    }
}

