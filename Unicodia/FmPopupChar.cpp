#include "FmPopupChar.h"
#include "ui_FmPopupChar.h"

// Project-local
#include "WiSample.h"
#include "Skin.h"
#include "MyWiki.h"

// Unicode
#include "UcData.h"

// L10n
#include "LocFmt.h"
#include "LocDic.h"


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
    charCode = cp.subj;

    // Sample
    ui->wiSample->showCp(cp, uc::EmojiDraw::CONSERVATIVE, glyphSets, false);

    /// @todo [urgent] setCp: what to do?
    // Code
    char buf[40];
    QString s = "U+";
    uc::sprint(buf, cp.subj);
    mywiki::appendCopyable(s, buf, "' style='" STYLE_BIGCOPY);
    ui->lbCode->setText(s);

    // Find in Blocks: let it be this way, change every time
    s = loc::Fmt("<a href='gi:xxx' style='{1}'>{2}</a>")
                (STYLE_INET)
                (str::toSv(loc::get("Main.Local.FindBlk"))).q();
    ui->lbToBlocks->setText(s);

    // Copy
    s = loc::Fmt("<a href='gi:xxx' style='{1}'>{2}</a>")
                (STYLE_INET)
                (str::toSv(loc::get("Main.Local.Copy"))).q();
    ui->lbCopy->setText(s);

    // Copy + VS16
    bool hasVs16 = cp.isVs16Emoji();
    if (hasVs16) {
        s = loc::Fmt("<a href='gi:xxx' style='{1}'>{2}</a>")
                    (STYLE_INET)
                    (str::toSv(loc::get("Main.Popup.CopyVs16"))).q();
        ui->lbCopyVs16->setText(s);
    }
    //ui->lbCopyVs16->setVisible(hasVs16);

    // Text
    auto text = mywiki::buildHtml(cp, mywiki::HtmlVariant::POPUP);
    ui->lbText->setText(text);

    return *this;
}
