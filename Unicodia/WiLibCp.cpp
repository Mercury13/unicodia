#include "WiLibCp.h"
#include "ui_WiLibCp.h"

// Qt
#include <QPainter>

// Libs
#include "u_Qstrings.h"

// Unicode
#include "UcData.h"

// Char painting
#include "Skin.h"
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
                 TableDraw::CUSTOM, emojiDraw, *glyphSets);
    }
}

void WiCpImage::setCp(const uc::Cp* x, uc::EmojiDraw em, const uc::GlyphStyleSets& y)
{
    if (cp != x || glyphSets != &y || emojiDraw != em) {
        cp = x;
        glyphSets = &y;
        emojiDraw = em;
        if (cp) {
            setCursor(Qt::WhatsThisCursor);
            setToolTip(cp->viewableName());
        } else {
            setCursor(Qt::ArrowCursor);
            setToolTip({});
        }
        update();
    }
}

void WiCpImage::setCp(char32_t x, uc::EmojiDraw em, const uc::GlyphStyleSets& y)
{
    if (x < uc::CAPACITY) {
        if (auto q = uc::cpsByCode[x]) {
            setCp(q, em, y);
            return;
        }
    }
    setCp(nullptr, uc::EmojiDraw::CONSERVATIVE, uc::GlyphStyleSets::EMPTY);
}


///// WiLibCp //////////////////////////////////////////////////////////////////

WiLibCp::WiLibCp(QWidget *parent) :
    Super(parent),
    ui(new Ui::WiLibCp)
{
    ui->setupUi(this);
    connect(ui->lbCode, &QLabel::linkActivated, this, &This::linkActivated);
}

WiLibCp::~WiLibCp()
{
    delete ui;
}

void WiLibCp::setCp(char32_t cp, uc::EmojiDraw em, const uc::GlyphStyleSets& glyphSets)
{
    ui->wiImage->setCp(cp, em, glyphSets);
    char q[200];
    snprintf(q, std::size(q),
             "<a href='g' style='" STYLE_CODE "'>" "%04X" "</a>",
             static_cast<int>(cp));
    ui->lbCode->setText(q);
    currentCp = cp;
}

void WiLibCp::removeCp()
{
    ui->wiImage->setCp(nullptr, uc::EmojiDraw::CONSERVATIVE, uc::GlyphStyleSets::EMPTY);
    ui->lbCode->setText("---");
    currentCp = NO_CP;
}


void WiLibCp::linkActivated()
{
    if (currentCp != NO_CP) {
        emit goToCp(currentCp);
    }
}
