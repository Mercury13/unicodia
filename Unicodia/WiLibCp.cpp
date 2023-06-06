#include "WiLibCp.h"
#include "ui_WiLibCp.h"

// Qt
#include <QPainter>

// Unicode
#include "UcData.h"

// Char painting
#include "CharPaint/routines.h"

///// WiCpImage ////////////////////////////////////////////////////////////////

WiCpImage::WiCpImage(QWidget *parent, Qt::WindowFlags f)
    : Super(parent, f), glyphSets(&uc::GlyphStyleSets::EMPTY) {}


void WiCpImage::paintEvent(QPaintEvent *event)
{
    Super::paintEvent(event);
    if (cp) {
        auto winColor = palette().text().color();
        QPainter painter(this);
        drawChar(&painter, rect(), 100, *cp, winColor,
                 TableDraw::CUSTOM, uc::EmojiDraw::CONSERVATIVE, *glyphSets);
    }
}

void WiCpImage::setCp(const uc::Cp* x, const uc::GlyphStyleSets& y)
{
    if (cp != x || glyphSets != &y) {
        cp = x;
        glyphSets = &y;
        update();
    }
}

void WiCpImage::setCp(char32_t x, const uc::GlyphStyleSets& y)
{
    if (x < uc::CAPACITY) {
        setCp(uc::cpsByCode[x], y);
    } else {
        setCp(nullptr, uc::GlyphStyleSets::EMPTY);
    }
}


///// WiLibCp //////////////////////////////////////////////////////////////////

WiLibCp::WiLibCp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WiLibCp)
{
    ui->setupUi(this);
}

WiLibCp::~WiLibCp()
{
    delete ui;
}

void WiLibCp::setCp(char32_t cp, const uc::GlyphStyleSets& glyphSets)
{
    ui->wiImage->setCp(cp, glyphSets);
}
