#include "../include/QHexView.h"
#include <QScrollBar>
#include <QPainter>
#include <QSize>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QClipboard>
#include <QApplication>

#include <QDebug>

#include <stdexcept>

const int HEXCHARS_IN_LINE = 47;
const int GAP_ADR_HEX = 10;
const int GAP_HEX_ASCII = 16;
const int BYTES_PER_LINE = 16;


QHexView::QHexView(QWidget *parent):
QAbstractScrollArea(parent),
m_pdata(NULL)
{
    setFont(QFont("Courier", 10));

    m_charWidth = fontMetrics().horizontalAdvance(QLatin1Char('9'));
    m_charHeight = fontMetrics().height();

    m_posAddr = 0;
    m_posHex = 10 * m_charWidth + GAP_ADR_HEX;
    m_posAscii = m_posHex + HEXCHARS_IN_LINE * m_charWidth + GAP_HEX_ASCII;

    setMinimumWidth(m_posAscii + (BYTES_PER_LINE * m_charWidth));

    setFocusPolicy(Qt::StrongFocus);
}


QHexView::~QHexView()
{
    if(m_pdata)
        delete m_pdata;
}

void QHexView::setData(QHexView::DataStorage *pData)
{
    verticalScrollBar()->setValue(0);
    if(m_pdata)
        delete m_pdata;
    m_pdata = pData;
    m_cursorPos = 0;
    resetSelection(0);
    viewport()->update();
}


void QHexView::showFromOffset(std::size_t offset)
{
    if(m_pdata && offset < m_pdata->size())
    {
        setCursorPos(offset * 2);

        int cursorY = m_cursorPos / (2 * BYTES_PER_LINE);

        verticalScrollBar() -> setValue(cursorY);
    }
}

void QHexView::clear()
{
    verticalScrollBar()->setValue(0);
}


QSize QHexView::fullSize() const
{
    if(!m_pdata)
        return QSize(0, 0);

    std::size_t width = m_posAscii + (BYTES_PER_LINE * m_charWidth);
    std::size_t height = m_pdata->size() / BYTES_PER_LINE;
    if(m_pdata->size() % BYTES_PER_LINE)
        height++;

    height *= m_charHeight;

    return QSize(width, height);
}


namespace {

    #define STRAIGHT_3(start) \
        (start), ((start)+1), ((start)+2)
    #define STRAIGHT_4(start) \
        (start), ((start)+1), ((start)+2), ((start)+3)
    #define STRAIGHT_ROW(start) \
        STRAIGHT_4(start), STRAIGHT_4((start)+4), STRAIGHT_4((start)+8), STRAIGHT_4((start)+12)
    #define STRAIGHT_15(start) \
        STRAIGHT_4(start), STRAIGHT_4((start)+4), STRAIGHT_4((start)+8), STRAIGHT_3((start)+12)

    #define ASCII_6_ROWS(codeOf127) \
        STRAIGHT_ROW(0x20), \
        STRAIGHT_ROW(0x30), \
        STRAIGHT_ROW(0x40), \
        STRAIGHT_ROW(0x50), \
        STRAIGHT_ROW(0x60), \
        STRAIGHT_15(0x70), codeOf127

    #define IBM_PRINTABLE_ASCII  ASCII_6_ROWS(0x2302)

    #define IBM_TABLE_ASCII \
        0x00B7, 0x263A, 0x263B, 0x2665,   0x2666, 0x2663, 0x2660, 0x25CF,   0x25D8, 0x25CB, 0x25D9, 0x2642,   0x2640, 0x266A, 0x266B, 0x263C, \
        0x25BA, 0x25C4, 0x2195, 0x203C,   0x00B6, 0x00A7, 0x25AC, 0x21A8,   0x2191, 0x2193, 0x2192, 0x2190,   0x221F, 0x2194, 0x25B2, 0x25BC, \
        IBM_PRINTABLE_ASCII

    #define IBM_TABLE_PSEUDOGRAPHICS \
        0x2591, 0x2592, 0x2593, 0x2502,   0x2524, 0x2561, 0x2562, 0x2556,   0x2555, 0x2563, 0x2551, 0x2557,   0x255D, 0x255C, 0x255B, 0x2510, \
        0x2514, 0x2534, 0x252C, 0x251C,   0x2500, 0x253C, 0x255E, 0x255F,   0x255A, 0x2554, 0x2569, 0x2566,   0x2560, 0x2550, 0x256C, 0x2567, \
        0x2568, 0x2564, 0x2565, 0x2559,   0x2558, 0x2552, 0x2553, 0x256B,   0x256A, 0x2518, 0x250C, 0x2588,   0x2584, 0x258C, 0x2590, 0x2580

    #define IBM_TABLE_F8_FF \
        0x00B0, 0x2022, 0x2219, 0x221A,   0x2116, 0x00A4, 0x25A0, 0x2423
        // 2022 and 2219 are larger bullets than 00

    #define CYRILLIC_CAPITAL_ROW_1   STRAIGHT_ROW(0x0410)
    #define CYRILLIC_CAPITAL_ROW_2   STRAIGHT_ROW(0x0420)
    #define CYRILLIC_SMALL_ROW_1     STRAIGHT_ROW(0x0430)
    #define CYRILLIC_SMALL_ROW_2     STRAIGHT_ROW(0x0440)

    uint16_t CP_866[256] = {
        IBM_TABLE_ASCII,
        CYRILLIC_CAPITAL_ROW_1,
        CYRILLIC_CAPITAL_ROW_2,
        CYRILLIC_SMALL_ROW_1,
        IBM_TABLE_PSEUDOGRAPHICS,
        CYRILLIC_SMALL_ROW_2,
        0x0401, 0x0451, 0x0404, 0x0454,   0x0407, 0x0457, 0x040E, 0x045E,   IBM_TABLE_F8_FF };

}   // anon namespace

void QHexView::paintEvent(QPaintEvent *event)
{
    if(!m_pdata)
        return;
    QPainter painter(viewport());

    QSize areaSize = viewport()->size();
    QSize  widgetSize = fullSize();
    verticalScrollBar()->setPageStep(areaSize.height() / m_charHeight);
    verticalScrollBar()->setRange(0, (widgetSize.height() - areaSize.height()) / m_charHeight + 1);

    int firstLineIdx = verticalScrollBar() -> value();

    int lastLineIdx = firstLineIdx + areaSize.height() / m_charHeight;
    if(lastLineIdx > static_cast<long>(m_pdata->size() / BYTES_PER_LINE))
    {
        lastLineIdx = m_pdata->size() / BYTES_PER_LINE;
        if(m_pdata->size() % BYTES_PER_LINE)
            lastLineIdx++;
    }

    painter.fillRect(event->rect(), this->palette().color(QPalette::Base));

    QColor addressAreaColor = QColor(0xd4, 0xd4, 0xd4, 0xff);
    painter.fillRect(QRect(m_posAddr, event->rect().top(), m_posHex - GAP_ADR_HEX + 2 , height()), addressAreaColor);

    int linePos = m_posAscii - (GAP_HEX_ASCII / 2);
    painter.setPen(Qt::gray);

    painter.drawLine(linePos, event->rect().top(), linePos, height());

    painter.setPen(Qt::black);

    int yPosStart = m_charHeight;

    QBrush def = painter.brush();
    QBrush selected = QBrush(QColor(0x6d, 0x9e, 0xff, 0xff));
    QByteArray data = m_pdata->getData(firstLineIdx * BYTES_PER_LINE, (lastLineIdx - firstLineIdx) * BYTES_PER_LINE);

    for (int lineIdx = firstLineIdx, yPos = yPosStart;  lineIdx < lastLineIdx; lineIdx += 1, yPos += m_charHeight)
    {
        QString address = QString("%1").arg(lineIdx * 16, 10, 16, QChar('0'));
        painter.drawText(m_posAddr, yPos, address);

        for(int xPos = m_posHex, i=0; i<BYTES_PER_LINE && ((lineIdx - firstLineIdx) * BYTES_PER_LINE + i) < data.size(); i++, xPos += 3 * m_charWidth)
        {
            std::size_t pos = (lineIdx * BYTES_PER_LINE + i) * 2;
            if(pos >= m_selectBegin && pos < m_selectEnd)
            {
                painter.setBackground(selected);
                painter.setBackgroundMode(Qt::OpaqueMode);
            }

            QString val = QString::number((data.at((lineIdx - firstLineIdx) * BYTES_PER_LINE + i) & 0xF0) >> 4, 16);
            painter.drawText(xPos, yPos, val);


            if((pos+1) >= m_selectBegin && (pos+1) < m_selectEnd)
            {
                painter.setBackground(selected);
                painter.setBackgroundMode(Qt::OpaqueMode);
            }
            else
            {
                painter.setBackground(def);
                painter.setBackgroundMode(Qt::OpaqueMode);
            }

            val = QString::number((data.at((lineIdx - firstLineIdx) * BYTES_PER_LINE + i) & 0xF), 16);
            painter.drawText(xPos+m_charWidth, yPos, val);

            painter.setBackground(def);
            painter.setBackgroundMode(Qt::OpaqueMode);

        }

        for (int xPosAscii = m_posAscii, i=0; ((lineIdx - firstLineIdx) * BYTES_PER_LINE + i) < data.size() && (i < BYTES_PER_LINE); i++, xPosAscii += m_charWidth)
        {
            unsigned char ch = data[(lineIdx - firstLineIdx) * BYTES_PER_LINE + i];
            //if (ch < 0x20)
            //ch = '.';

            painter.drawText(xPosAscii, yPos, QChar(CP_866[ch]));
        }

    }

    if (hasFocus())
    {
        int x = (m_cursorPos % (2 * BYTES_PER_LINE));
        int y = m_cursorPos / (2 * BYTES_PER_LINE);
        y -= firstLineIdx;
        int cursorX = (((x / 2) * 3) + (x % 2)) * m_charWidth + m_posHex;
        int cursorY = y * m_charHeight + 4;
        painter.fillRect(cursorX, cursorY, 2, m_charHeight, this->palette().color(QPalette::WindowText));
    }
}


void QHexView::keyPressEvent(QKeyEvent *event)
{
    bool setVisible = false;

/*****************************************************************************/
/* Cursor movements */
/*****************************************************************************/
    if(event->matches(QKeySequence::MoveToNextChar))
    {
        setCursorPos(m_cursorPos + 1);
        resetSelection(m_cursorPos);
        setVisible = true;
    }
    if(event->matches(QKeySequence::MoveToPreviousChar))
    {
        setCursorPos(m_cursorPos - 1);
        resetSelection(m_cursorPos);
        setVisible = true;
    }
    if(event->matches(QKeySequence::MoveToEndOfLine))
    {
        setCursorPos(m_cursorPos | ((BYTES_PER_LINE * 2) - 1));
        resetSelection(m_cursorPos);
        setVisible = true;
    }
    if(event->matches(QKeySequence::MoveToStartOfLine))
    {
        setCursorPos(m_cursorPos | (m_cursorPos % (BYTES_PER_LINE * 2)));
        resetSelection(m_cursorPos);
        setVisible = true;
    }
    if(event->matches(QKeySequence::MoveToPreviousLine))
    {
        setCursorPos(m_cursorPos - BYTES_PER_LINE * 2);
        resetSelection(m_cursorPos);
        setVisible = true;
    }
    if(event->matches(QKeySequence::MoveToNextLine))
    {
        setCursorPos(m_cursorPos + BYTES_PER_LINE * 2);
        resetSelection(m_cursorPos);
        setVisible = true;
    }

    if(event->matches(QKeySequence::MoveToNextPage))
    {
        setCursorPos(m_cursorPos + (viewport()->height() / m_charHeight - 1) * 2 * BYTES_PER_LINE);
        resetSelection(m_cursorPos);
        setVisible = true;
    }
    if(event->matches(QKeySequence::MoveToPreviousPage))
    {
        setCursorPos(m_cursorPos - (viewport()->height() / m_charHeight - 1) * 2 * BYTES_PER_LINE);
        resetSelection(m_cursorPos);
        setVisible = true;
    }
    if(event->matches(QKeySequence::MoveToEndOfDocument))
    {
        if(m_pdata)
            setCursorPos(m_pdata->size() * 2);
        resetSelection(m_cursorPos);
        setVisible = true;
    }
    if(event->matches(QKeySequence::MoveToStartOfDocument))
    {
        setCursorPos(0);
        resetSelection(m_cursorPos);
        setVisible = true;
    }

/*****************************************************************************/
/* Select commands */
/*****************************************************************************/
    if (event->matches(QKeySequence::SelectAll))
    {
        resetSelection(0);
        if(m_pdata)
            setSelection(2 * m_pdata->size() + 1);
        setVisible = true;
    }
    if (event->matches(QKeySequence::SelectNextChar))
    {
        int pos = m_cursorPos + 1;
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }
    if (event->matches(QKeySequence::SelectPreviousChar))
    {
        int pos = m_cursorPos - 1;
        setSelection(pos);
        setCursorPos(pos);
        setVisible = true;
    }
    if (event->matches(QKeySequence::SelectEndOfLine))
    {
        int pos = m_cursorPos - (m_cursorPos % (2 * BYTES_PER_LINE)) + (2 * BYTES_PER_LINE);
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }
    if (event->matches(QKeySequence::SelectStartOfLine))
    {
        int pos = m_cursorPos - (m_cursorPos % (2 * BYTES_PER_LINE));
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }
    if (event->matches(QKeySequence::SelectPreviousLine))
    {
        int pos = m_cursorPos - (2 * BYTES_PER_LINE);
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }
    if (event->matches(QKeySequence::SelectNextLine))
    {
        int pos = m_cursorPos + (2 * BYTES_PER_LINE);
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }

    if (event->matches(QKeySequence::SelectNextPage))
    {
        int pos = m_cursorPos + (((viewport()->height() / m_charHeight) - 1) * 2 * BYTES_PER_LINE);
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }
    if (event->matches(QKeySequence::SelectPreviousPage))
    {
        int pos = m_cursorPos - (((viewport()->height() / m_charHeight) - 1) * 2 * BYTES_PER_LINE);
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }
    if (event->matches(QKeySequence::SelectEndOfDocument))
    {
        int pos = 0;
        if(m_pdata)
            pos = m_pdata->size() * 2;
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }
    if (event->matches(QKeySequence::SelectStartOfDocument))
    {
        int pos = 0;
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }

    if (event->matches(QKeySequence::Copy))
    {
        if(m_pdata)
        {
            QString res;
            int idx = 0;
            int copyOffset = 0;

            QByteArray data = m_pdata->getData(m_selectBegin / 2, (m_selectEnd - m_selectBegin) / 2 + 1);
            if(m_selectBegin % 2)
            {
                res += QString::number((data.at((idx+1) / 2) & 0xF), 16);
                res += " ";
                idx++;
                copyOffset = 1;
            }

            int selectedSize = m_selectEnd - m_selectBegin;
            for (;idx < selectedSize; idx+= 2)
            {
                QString val = QString::number((data.at((copyOffset + idx) / 2) & 0xF0) >> 4, 16);
                if(idx + 1 < selectedSize)
                {
                    val += QString::number((data.at((copyOffset + idx) / 2) & 0xF), 16);
                    val += " ";
                }
                res += val;

                if((idx/2) % BYTES_PER_LINE == (BYTES_PER_LINE - 1))
                    res += "\n";
            }
            QClipboard *clipboard = QApplication::clipboard();
            clipboard -> setText(res);
        }
    }

    if(setVisible)
        ensureVisible();
    viewport() -> update();
}

void QHexView::mouseMoveEvent(QMouseEvent * event)
{
    int actPos = cursorPos(event->pos());
    setCursorPos(actPos);
    setSelection(actPos);

    viewport() -> update();
}

void QHexView::mousePressEvent(QMouseEvent * event)
{
    int cPos = cursorPos(event->pos());

    if((QApplication::keyboardModifiers() & Qt::ShiftModifier) && event -> button() == Qt::LeftButton)
        setSelection(cPos);
    else
        resetSelection(cPos);

    setCursorPos(cPos);

    viewport() -> update();
}


std::size_t QHexView::cursorPos(const QPoint &position)
{
    int pos = -1;

    if ((position.x() >= static_cast<ptrdiff_t>(m_posHex)) && (position.x() < static_cast<ptrdiff_t>(m_posHex + HEXCHARS_IN_LINE * m_charWidth)))
    {
        int x = (position.x() - m_posHex) / m_charWidth;
        if ((x % 3) == 0)
            x = (x / 3) * 2;
        else
            x = ((x / 3) * 2) + 1;

        int firstLineIdx = verticalScrollBar() -> value();
        int y = (position.y() / m_charHeight) * 2 * BYTES_PER_LINE;
        pos = x + y + firstLineIdx * BYTES_PER_LINE * 2;
    }
    return pos;
}


void QHexView::resetSelection()
{
    m_selectBegin = m_selectInit;
    m_selectEnd = m_selectInit;
}

void QHexView::resetSelection(int pos)
{
    if (pos < 0)
        pos = 0;

    m_selectInit = pos;
    m_selectBegin = pos;
    m_selectEnd = pos;
}

void QHexView::setSelection(int pos)
{
    if (pos < 0)
        pos = 0;

    if (pos >= static_cast<ptrdiff_t>(m_selectInit))
    {
        m_selectEnd = pos;
        m_selectBegin = m_selectInit;
    }
    else
    {
        m_selectBegin = pos;
        m_selectEnd = m_selectInit;
    }
}


void QHexView::setCursorPos(int position)
{
    if(position < 0)
        position = 0;

    int maxPos = 0;
    if(m_pdata)
    {
        maxPos = m_pdata->size() * 2;
        if(m_pdata->size() % BYTES_PER_LINE)
            maxPos++;
    }

    if(position > maxPos)
        position = maxPos;

    m_cursorPos = position;
}

void QHexView::ensureVisible()
{
    QSize areaSize = viewport()->size();

    int firstLineIdx = verticalScrollBar() -> value();
    int lastLineIdx = firstLineIdx + areaSize.height() / m_charHeight;

    int cursorY = m_cursorPos / (2 * BYTES_PER_LINE);

    if(cursorY < firstLineIdx)
        verticalScrollBar() -> setValue(cursorY);
    else if(cursorY >= lastLineIdx)
        verticalScrollBar() -> setValue(cursorY - areaSize.height() / m_charHeight + 1);
}



QHexView::DataStorageArray::DataStorageArray(const QByteArray &arr)
{
    m_data = arr;
}

QHexView::DataStorageArray::DataStorageArray(QByteArray &&arr)
{
    m_data = std::move(arr);
}

QByteArray QHexView::DataStorageArray::getData(std::size_t position, std::size_t length)
{
    return m_data.mid(position, length);
}


std::size_t QHexView::DataStorageArray::size()
{
    return m_data.count();
}


QHexView::DataStorageFile::DataStorageFile(const QString &fileName): m_file(fileName)
{
    m_file.open(QIODevice::ReadOnly);
    if(!m_file.isOpen())
        throw std::runtime_error(std::string("Failed to open file `") + fileName.toStdString() + "`");
}

QByteArray QHexView::DataStorageFile::getData(std::size_t position, std::size_t length)
{
    m_file.seek(position);
    return m_file.read(length);
}


std::size_t QHexView::DataStorageFile::size()
{
    return m_file.size();
}
