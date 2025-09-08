#include "WiLibCp.h"
#include "ui_WiLibCp.h"

// Qt
#include <QPainter>
#include <QMouseEvent>

// Unicode
#include "UcData.h"

// Char painting
#include "Skin.h"
#include "CharPaint/routines.h"

///// WiCpImage ////////////////////////////////////////////////////////////////

WiCpImage::WiCpImage(QWidget *parent, Qt::WindowFlags f)
    : Super(parent, f), glyphSets(&uc::GlyphStyleSets::EMPTY) {}


void WiCpImage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    auto bgColor = palette().window().color();
    painter.fillRect(rect(), bgColor);
    if (cp) {
        auto winColor = palette().windowText().color();
        drawChar(&painter, rect(), Fsz::TABLE, 100, *cp, winColor,
                 TableDraw::CUSTOM, uc::EmojiDraw::CONSERVATIVE, *glyphSets);
    }
}

void WiCpImage::setCp(const uc::Cp* x, const uc::GlyphStyleSets& y)
{
    if (cp != x || glyphSets != &y) {
        cp = x;
        glyphSets = &y;
        if (cp) {
            setCursor(Qt::PointingHandCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }
        update();
    }
}

void WiCpImage::setCp(char32_t x, const uc::GlyphStyleSets& y)
{
    if (x < uc::CAPACITY) {
        if (auto q = uc::cpsByCode[x]) {
            setCp(q, y);
            return;
        }
    }
    setCp(nullptr, uc::GlyphStyleSets::EMPTY);
}


void WiCpImage::mousePressEvent(QMouseEvent *event)
{
    isLeftButton = (event->button() == Qt::LeftButton);
}


void WiCpImage::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (isLeftButton        // really left button
                && rect().contains(event->pos())) {   // released within
            // Let it be this way: if some button has erroneously stuck → OK
            emit clicked();
        }
    }
    isLeftButton = false;
}


///// WiLibCp //////////////////////////////////////////////////////////////////

WiLibCp::WiLibCp(QWidget *parent) :
    Super(parent),
    ui(new Ui::WiLibCp)
{
    ui->setupUi(this);
    connect(ui->lbCode, &QLabel::linkActivated, this, &This::linkActivated);
    connect(ui->wiImage, &WiCpImage::clicked, this, &This::linkActivated);
}

WiLibCp::~WiLibCp()
{
    delete ui;
}

void WiLibCp::setCp(char32_t cp, const uc::GlyphStyleSets& glyphSets)
{
    ui->wiImage->setCp(cp, glyphSets);
    char q[200];
    snprintf(q, std::size(q),
             "<a href='g' style='" STYLE_POPUP_DARK "'>" "%04X" "</a>",
             static_cast<int>(cp));
    ui->lbCode->setText(q);
    currentCp = cp;
}

void WiLibCp::removeCp()
{
    ui->wiImage->setCp(nullptr, uc::GlyphStyleSets::EMPTY);
    ui->lbCode->setText("---");
    currentCp = NO_CP;
}


void WiLibCp::linkActivated()
{
    if (currentCp != NO_CP) {
        emit goToCp(qobject_cast<QWidget*>(sender()), currentCp);
    }
}
