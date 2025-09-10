#pragma once

#include "MyWiki.h"
#include "u_Popup.h"

class FmMessage;
class FmPopup2;
class FmPopupChar2;

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
    pop::ClickMemory memory;
    MyGui(QWidget* aWiMain, FontSource& aFontSource, mywiki::InternalLinkWalker& aWalker);

    void popupAtAbs(
            QWidget* widget, const QRect& absRect,
            const mywiki::PLink& that, const QString& html) override;
    void popupCharAbs(
            QWidget* widget, const QRect& absRect,
            const mywiki::PLink& that, const uc::Cp& cp) override;
    void goToHistory(QWidget* widget, unsigned index) override;
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

    void goToHistoryAbs(QWidget* widget, const QRect& rect, unsigned index);
    void goToHistoryRel(QWidget* widget, const QRect& rect, unsigned index);

    /// @brief
    ///   Simple pointer with a little bit error-proofing
    struct RestrictedPtr {
        const void* const v = nullptr;

        /// …can point to nowhere
        RestrictedPtr(std::nullptr_t) noexcept {}
        RestrictedPtr() noexcept {}

        /// …can point to Uptr, that’s all
        template <class Target>
        RestrictedPtr(Uptr<Target>& x) noexcept : v(&x) {}

        operator const void* () { return v; }
        bool operator == (const void* x) const noexcept { return (v == x); }
    };
    void closePopup(RestrictedPtr remainingThing = {});

    ~MyGui() override;
signals:
    void linkActivated(QWidget* thing, const QString& link);
private:
    friend class FmPopup2;
    friend class FmPopupChar2;
    friend class PopupGui;
    QWidget* wiMain;
    FontSource& fFontSource;
    mywiki::InternalLinkWalker& fInternalWalker;
    Uptr<FmPopup2> popup;
    Uptr<FmPopupChar2> popupChar;
    Uptr<FmMessage> fmMessage;
    mywiki::History history;
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
            QWidget* widget, const QRect& absRect,
            const mywiki::PLink& that, const QString& html) override;
    void popupCharAbs(
            QWidget* widget, const QRect& absRect,
            const mywiki::PLink& that, const uc::Cp& cp) override;
    void goToHistory(QWidget* widget, unsigned index) override;
    FontSource& fontSource() override;
    void copyTextAbs(
            QWidget* widget, const QRect& absRect, const QString& text,
            LocKey locKey) override;
    void followUrl(const QString& x) override;
    mywiki::InternalLinkWalker& linkWalker() override;
private:
    MyGui& owner;
};
