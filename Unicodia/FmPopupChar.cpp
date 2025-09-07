#include "FmPopupChar.h"
#include "ui_FmPopupChar.h"

// Project-local
#include "WiSample.h"
#include "Skin.h"
#include "MyWiki.h"

// Unicode
#include "UcAutoDefines.h"

FmPopupChar::FmPopupChar(QWidget *parent, pop::ClickMemory& memory) :
    Super(parent, memory),
    ui(new Ui::FmPopupChar)
{
    ui->setupUi(this);
    ui->wiSample->hideHead();

    ui->wiBg->setStyleSheet(
        "#wiBg { background-color: " CNAME_BG_POPUP "; border: 1px solid black; }");
}

FmPopupChar::~FmPopupChar()
{
    delete ui;
}

auto FmPopupChar::setCp(const uc::Cp& cp, const uc::GlyphStyleSets& glyphSets) -> This&
{
    /// @todo [urgent] setCp: what to do?
    char buf[30];
    snprintf(buf, std::size(buf), "U+%04X", cp.subj.uval());
    ui->lbCode->setText(buf);

    ui->wiSample->showCp(cp, uc::EmojiDraw::CONSERVATIVE, glyphSets, false);

    auto text = mywiki::buildHtml(cp, mywiki::HtmlVariant::POPUP);
    ui->lbText->setText(text);

    return *this;
}
