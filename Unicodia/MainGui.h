#pragma once

#include "MyWiki.h"

class FmMessage;
class FmPopup2;

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
    MyGui(QWidget* aWiMain, FontSource& aFontSource);

    void popupAtAbs(
            QWidget* widget, const QRect& absRect, const QString& html) override;
    virtual FontSource& fontSource() override { return fFontSource; }
    virtual void copyTextAbs(
            QWidget* widget, const QRect& absRect, const QString& text) override;
    virtual void followUrl(const QString& x) override;

    // Several ways to blink fmMessage
    void blinkCopied(QWidget* widget, const QRect& absRect);
    void blinkAtWidget(const QString& text, QWidget* widget);
    void blinkAtRel(const QString& text, const QWidget* widget, const QRect& relRect);

    ~MyGui() override;
signals:
    void linkActivated(QWidget* thing, const QString& link);
private:
    friend class FmPopup2;
    friend class PopupGui;
    QWidget* wiMain;
    FontSource& fFontSource;
    Uptr<FmPopup2> popup;
    Uptr<FmMessage> fmMessage;
private slots:
    void popupLinkActivated(const QString& link);
};


class PopupGui : public mywiki::Gui
{
public:
    PopupGui(MyGui& aOwner) : owner(aOwner) {}
    void popupAtAbs(
            QWidget* widget, const QRect& absRect, const QString& html) override;
    virtual FontSource& fontSource() override;
    virtual void copyTextAbs(
            QWidget* widget, const QRect& absRect, const QString& text) override;
    virtual void followUrl(const QString& x) override;
private:
    MyGui& owner;
};
