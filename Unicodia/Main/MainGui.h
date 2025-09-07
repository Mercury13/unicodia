#pragma once

#include "MyWiki.h"

class FmMessage;
class FmPopup2;
class FmPopupChar;

template <class T>
class Uptr
{
public:
    using Pointer = T *;

    template <class... UU>
    T& ensure(UU&&... u) {
        if (!*this) {
            v = new T(std::forward<UU>(u)...);
        }
        return *v;
    }

    ~Uptr() { delete v; }

    operator Pointer() const noexcept { return v; }
    Pointer operator->() const noexcept { return v; }
    explicit operator bool() const noexcept { return v; }
private:
    std::atomic<T*> v = nullptr;
};


class MyGui :
        public QObject,
        public mywiki::Gui
{
    Q_OBJECT
public:
    MyGui(QWidget* aWiMain, FontSource& aFontSource, mywiki::InternalLinkWalker& aWalker);

    void popupAtAbs(
            QWidget* widget, const QRect& absRect, const QString& html) override;
    void popupCharAbs(
            QWidget* widget, const QRect& absRect, const uc::Cp& cp) override;
    FontSource& fontSource() override { return fFontSource; }
    void copyTextAbs(
            QWidget* widget, const QRect& absRect, const QString& text,
            LocKey locKey) override;
    void followUrl(const QString& x) override;
    mywiki::InternalLinkWalker& linkWalker() override { return fInternalWalker; }

    // Several ways to blink fmMessage
    void blinkCopied(QWidget* widget, const QRect& absRect,
                     LocKey locKey);
    void blinkAtWidget(const QString& text, QWidget* widget);
    void blinkAtRel(const QString& text, const QWidget* widget, const QRect& relRect);
    void closePopup();

    ~MyGui() override;
signals:
    void linkActivated(QWidget* thing, const QString& link);
private:
    friend class FmPopup2;
    friend class PopupGui;
    QWidget* wiMain;
    FontSource& fFontSource;
    mywiki::InternalLinkWalker& fInternalWalker;
    Uptr<FmPopup2> popup;
    Uptr<FmPopupChar> popupChar;
    Uptr<FmMessage> fmMessage;
private slots:
    void popupLinkActivated(const QString& link);
};


///
/// Special generic GUI for pop-up windows:
/// when we click the link, we pop up around main window’s link
///   rather than run far away from it
///
class PopupGui : public mywiki::Gui
{
public:
    PopupGui(MyGui& aOwner) : owner(aOwner) {}
    void popupAtAbs(
            QWidget* widget, const QRect& absRect, const QString& html) override;
    void popupCharAbs(
            QWidget* widget, const QRect& absRect, const uc::Cp& cp) override;
    FontSource& fontSource() override;
    void copyTextAbs(
            QWidget* widget, const QRect& absRect, const QString& text,
            LocKey locKey) override;
    void followUrl(const QString& x) override;
    mywiki::InternalLinkWalker& linkWalker() override;
private:
    MyGui& owner;
};
