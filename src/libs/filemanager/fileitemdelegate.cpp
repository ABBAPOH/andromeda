#include "fileitemdelegate.h"
#include "fileitemdelegate_p.h"

#include <QtCore/QDate>
#include <QtCore/QFileInfo>

#include <QtGui/QApplication>
#include <QtGui/QKeyEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QPainter>
#include <QtGui/QPixmapCache>
#include <QtGui/QTextFrame>

#include <IO/QMimeDatabase>

#include <qmath.h>

#include "filemanagersettings.h"

#define QFIXED_MAX (INT_MAX/256)

using namespace FileManager;

static QString getSuffix(const QString &fileName)
{
    QMimeDatabase db;

    QString suffix = db.suffixForFileName(fileName);
    if (suffix.isEmpty())
        suffix = QFileInfo(fileName).suffix();

    return suffix;
}

/*!
    \class FileTextEdit
    \internal
*/
FileTextEdit::FileTextEdit(QWidget *parent) :
    QTextEdit(parent)
{
}

void FileTextEdit::resizeEvent(QResizeEvent *e)
{
    QTextEdit::resizeEvent(e);
    if (alignment().testFlag(Qt::AlignVCenter))
        realignVCenter(this);
}

void FileTextEdit::showEvent(QShowEvent *e)
{
    QTextEdit::showEvent(e);
    if (alignment().testFlag(Qt::AlignVCenter))
        realignVCenter(this);
}

void FileTextEdit::realignVCenter(QTextEdit * pTextEdit)
{
    // retrieve total text document height and widget height
    int nDocHeight = (int)pTextEdit->document()->size().height();
    if(nDocHeight == 0)									// if no document, do nothing
        return;
    int nCtrlHeight= pTextEdit->height();				// the TextEdit control height

    // access the document top frame and its format
    QTextFrame * pFrame = pTextEdit->document()->rootFrame();
    QTextFrameFormat frameFmt = pFrame->frameFormat();

    // get current top margin and compute the 'black' height of the document
    qreal nTopMargin = (int)frameFmt.topMargin();			// the frame top margin
    qreal nBBDocH = nDocHeight - nTopMargin;				// the height of the document 'bounding box'

    // compute and set appropriate frame top margin
    if(nCtrlHeight <= nBBDocH)			// if the control is shorter than the document
        nTopMargin = 2;									// set a nominal top margin
    else								// if the control is taller than the document
        nTopMargin = (nCtrlHeight - nBBDocH)/2 + 2;		// set half of the excess as top margin
    frameFmt.setTopMargin(nTopMargin);

    // SET A BORDER!! (using the same colour as the background)
    frameFmt.setBorder(0.1);
    frameFmt.setBorderBrush(QColor(0xFFFFFF));
    pFrame->setFrameFormat(frameFmt);					// apply the new format
}

static bool shouldDrawSelection(QStyle *style)
{
    if (!style)
        return false;

    const QString className = style->metaObject()->className();
    if (className == "QWindowsVistaStyle")
        return false;
    else if (className == "Oxygen::Style")
        return false;

    return true;
}

inline static QString replaceNewLine(QString text)
{
    const QChar nl = QLatin1Char('\n');
    for (int i = 0; i < text.count(); ++i)
        if (text.at(i) == nl)
            text[i] = QChar::LineSeparator;
    return text;
}

/*!
    \class FileItemDelegatePrivate
    \internal
*/

QString FileItemDelegatePrivate::displayText(const QVariant &value, const QLocale &locale) const
{
    QString text;
    switch (value.userType()) {
    case QMetaType::Float:
    case QVariant::Double:
        text = locale.toString(value.toReal());
        break;
    case QVariant::Int:
    case QVariant::LongLong:
        text = locale.toString(value.toLongLong());
        break;
    case QVariant::UInt:
    case QVariant::ULongLong:
        text = locale.toString(value.toULongLong());
        break;
    case QVariant::Date:
        text = locale.toString(value.toDate(), QLocale::ShortFormat);
        break;
    case QVariant::Time:
        text = locale.toString(value.toTime(), QLocale::ShortFormat);
        break;
    case QVariant::DateTime:
        text = locale.toString(value.toDateTime().date(), QLocale::ShortFormat);
        text += QLatin1Char(' ');
        text += locale.toString(value.toDateTime().time(), QLocale::ShortFormat);
        break;
    default:
        // convert new lines into line separators
        text = value.toString();
        for (int i = 0; i < text.count(); ++i) {
            if (text.at(i) == QLatin1Char('\n'))
                text[i] = QChar::LineSeparator;
        }
        break;
    }
    return text;
}

void FileItemDelegatePrivate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    QVariant value = index.data(Qt::FontRole);
    if (value.isValid() && !value.isNull()) {
        option->font = qvariant_cast<QFont>(value).resolve(option->font);
        option->fontMetrics = QFontMetrics(option->font);
    }

    value = index.data(Qt::TextAlignmentRole);
    if (value.isValid() && !value.isNull())
        option->displayAlignment = Qt::Alignment(value.toInt());

    value = index.data(Qt::ForegroundRole);
    if (value.canConvert<QBrush>())
        option->palette.setBrush(QPalette::Text, qvariant_cast<QBrush>(value));

    if (QStyleOptionViewItemV4 *v4 = qstyleoption_cast<QStyleOptionViewItemV4 *>(option)) {
        v4->index = index;
        QVariant value = index.data(Qt::CheckStateRole);
        if (value.isValid() && !value.isNull()) {
            v4->features |= QStyleOptionViewItemV2::HasCheckIndicator;
            v4->checkState = static_cast<Qt::CheckState>(value.toInt());
        }

        value = index.data(Qt::DecorationRole);
        if (value.isValid() && !value.isNull()) {
            v4->features |= QStyleOptionViewItemV2::HasDecoration;
            switch (value.type()) {
            case QVariant::Icon: {
                v4->icon = qvariant_cast<QIcon>(value);
                QIcon::Mode mode;
                if (!(option->state & QStyle::State_Enabled))
                    mode = QIcon::Disabled;
                else if (option->state & QStyle::State_Selected)
                    mode = QIcon::Selected;
                else
                    mode = QIcon::Normal;
                QIcon::State state = option->state & QStyle::State_Open ? QIcon::On : QIcon::Off;
                v4->decorationSize = v4->icon.actualSize(option->decorationSize, mode, state);
                break;
            }
            case QVariant::Color: {
                QPixmap pixmap(option->decorationSize);
                pixmap.fill(qvariant_cast<QColor>(value));
                v4->icon = QIcon(pixmap);
                break;
            }
            case QVariant::Image: {
                QImage image = qvariant_cast<QImage>(value);
                v4->icon = QIcon(QPixmap::fromImage(image));
                v4->decorationSize = image.size();
                break;
            }
            case QVariant::Pixmap: {
                QPixmap pixmap = qvariant_cast<QPixmap>(value);
                v4->icon = QIcon(pixmap);
                v4->decorationSize = pixmap.size();
                break;
            }
            default:
                break;
            }
        }

        value = index.data(Qt::DisplayRole);
        if (value.isValid() && !value.isNull()) {
            v4->features |= QStyleOptionViewItemV2::HasDisplay;
            v4->text = displayText(value, v4->locale);
        }

        v4->backgroundBrush = qvariant_cast<QBrush>(index.data(Qt::BackgroundRole));
    }
}

void FileItemDelegatePrivate::drawDisplay(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QString &text) const
{
    const QWidget *widget = this->widget(option);
    QStyle *style = widget ? widget->style() : QApplication::style();
    QPalette::ColorGroup cg = option.state & QStyle::State_Enabled
                              ? QPalette::Normal : QPalette::Disabled;
    if (cg == QPalette::Normal && !(option.state & QStyle::State_Active))
        cg = QPalette::Inactive;
    if (option.state & QStyle::State_Selected) {
        bool drawSelection = shouldDrawSelection(style);
        if (drawSelection)
            painter->fillRect(rect, option.palette.color(cg, QPalette::Highlight));
        painter->setPen(option.palette.color(cg, QPalette::HighlightedText));
    } else {
        painter->setPen(option.palette.color(cg, QPalette::Text));
    }

    if (text.isEmpty())
        return;

    if (option.state & QStyle::State_Editing) {
        painter->save();
        painter->setPen(option.palette.color(cg, QPalette::Text));
        painter->drawRect(rect.adjusted(0, 0, -1, -1));
        painter->restore();
    }

    const QStyleOptionViewItemV4 opt = option;

    const int textMargin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, widget) + 1;
    QRect textRect = rect.adjusted(textMargin, 0, -textMargin, 0); // remove width padding
    const bool wrapText = opt.features & QStyleOptionViewItemV2::WrapText;
    textOption.setWrapMode(wrapText ? QTextOption::WrapAtWordBoundaryOrAnywhere : QTextOption::ManualWrap);
    textOption.setTextDirection(option.direction);
    textOption.setAlignment(QStyle::visualAlignment(option.direction, option.displayAlignment));
    textLayout.setTextOption(textOption);
    textLayout.setFont(option.font);
    textLayout.setText(::replaceNewLine(text));

    QSizeF textLayoutSize = doTextLayout(textRect.width());

    if (textRect.width() < textLayoutSize.width()
        || textRect.height() < textLayoutSize.height()) {
        QString elided;

        int maxLines = qMin(textRect.height() / opt.fontMetrics.height(), textLayout.lineCount());
        for (int i = 0; i < maxLines - 1; ++i) {
            QTextLine line = textLayout.lineAt(i);
            elided += text.mid(line.textStart(), line.textLength());
        }
        QTextLine lastLine = textLayout.lineAt(maxLines - 1);
        elided.append(option.fontMetrics.elidedText(text.mid(lastLine.textStart()),
                                                    option.textElideMode,
                                                    textRect.width()));
        textLayout.setText(elided);
        textLayoutSize = doTextLayout(textRect.width());
    }

    const QSize layoutSize(textRect.width(), int(textLayoutSize.height()));
    const QRect layoutRect = QStyle::alignedRect(option.direction, option.displayAlignment,
                                                  layoutSize, textRect);

    // if we still overflow even after eliding the text, enable clipping
    if (/*!hasClipping() &&*/ (textRect.width() < textLayoutSize.width()
                           || textRect.height() < textLayoutSize.height())) {
        painter->save();
        painter->setClipRect(layoutRect);
        textLayout.draw(painter, layoutRect.topLeft(), QVector<QTextLayout::FormatRange>(), layoutRect);
        painter->restore();
    } else {
        textLayout.draw(painter, layoutRect.topLeft(), QVector<QTextLayout::FormatRange>(), layoutRect);
    }
}

void FileItemDelegatePrivate::drawCheck(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, Qt::CheckState state) const
{
    if (!rect.isValid())
        return;

    QStyleOptionViewItem opt(option);
    opt.rect = rect;
    opt.state = opt.state & ~QStyle::State_HasFocus;

    switch (state) {
    case Qt::Unchecked:
        opt.state |= QStyle::State_Off;
        break;
    case Qt::PartiallyChecked:
        opt.state |= QStyle::State_NoChange;
        break;
    case Qt::Checked:
        opt.state |= QStyle::State_On;
        break;
    }

    const QWidget *w = widget(option);
    QStyle *style = w ? w->style() : QApplication::style();
    style->drawPrimitive(QStyle::PE_IndicatorViewItemCheck, &opt, painter, w);
}

void FileItemDelegatePrivate::drawDecoration(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QPixmap &pixmap) const
{
    // if we have an icon, we ignore the pixmap
    if (!tmp.icon.isNull()) {
        tmp.icon.paint(painter, rect, option.decorationAlignment,
                          tmp.mode, tmp.state);
        return;
    }

    if (pixmap.isNull() || !rect.isValid())
        return;
    QPoint p = QStyle::alignedRect(option.direction, option.decorationAlignment,
                                   pixmap.size(), rect).topLeft();
    if (option.state & QStyle::State_Selected) {
        QPixmap *pm = selected(pixmap, option.palette, option.state & QStyle::State_Enabled);
        painter->drawPixmap(p, *pm);
    } else {
        painter->drawPixmap(p, pixmap);
    }
}

void FileItemDelegatePrivate::drawFocus(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const
{
    if ((option.state & QStyle::State_HasFocus) == 0 || !rect.isValid())
        return;
    QStyleOptionFocusRect opt;
    opt.QStyleOption::operator=(option);
    opt.rect = rect;
    opt.state |= QStyle::State_KeyboardFocusChange;
    opt.state |= QStyle::State_Item;
    QPalette::ColorGroup cg = (option.state & QStyle::State_Enabled)
                              ? QPalette::Normal : QPalette::Disabled;
    opt.backgroundColor = option.palette.color(cg, (option.state & QStyle::State_Selected)
                                             ? QPalette::Highlight : QPalette::Window);
    const QWidget *widget = this->widget(option);
    QStyle *style = widget ? widget->style() : QApplication::style();
    style->drawPrimitive(QStyle::PE_FrameFocusRect, &opt, painter, widget);
}

void FileItemDelegatePrivate::drawBackground(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 opt(option);
    opt.showDecorationSelected = true; // hack for win vista style to draw selection correctly

    const QWidget *w = widget(option);
    QStyle *style = w ? w->style() : QApplication::style();
    if (!shouldDrawSelection(style)) {
        style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, w);
    } else {
        if (opt.decorationPosition == QStyleOptionViewItem::Left
                 || opt.decorationPosition == QStyleOptionViewItem::Right ) {
            if (opt.state & QStyle::State_Selected)
                painter->fillRect(option.rect, opt.palette.color(QPalette::Highlight));
        }
    }
}

void FileItemDelegatePrivate::doLayout(const QStyleOptionViewItem &option, QRect *checkRect, QRect *pixmapRect, QRect *textRect, bool hint) const
{
    Q_ASSERT(checkRect && pixmapRect && textRect);

    const QWidget *widget = this->widget(option);
    QStyle *style = widget ? widget->style() : QApplication::style();
    const bool hasCheck = checkRect->isValid();
    const bool hasPixmap = pixmapRect->isValid();
    const bool hasText = textRect->isValid();
    const int textMargin = hasText ? style->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, widget) + 1 : 0;
    const int pixmapMargin = hasPixmap ? style->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, widget) + 1 : 0;
    const int checkMargin = hasCheck ? style->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, widget) + 1 : 0;
    int x = option.rect.left();
    int y = option.rect.top();
    int w, h;

    textRect->adjust(-textMargin, 0, textMargin, 0); // add width padding
    if (textRect->height() == 0 && (!hasPixmap || !hint)) {
        //if there is no text, we still want to have a decent height for the item sizeHint and the editor size
        textRect->setHeight(option.fontMetrics.height());
    }

    QSize pm(0, 0);
    if (hasPixmap) {
        pm = pixmapRect->size();
        pm.rwidth() += 2 * pixmapMargin;
    }
    if (hint) {
        h = qMax(checkRect->height(), qMax(textRect->height(), pm.height()));
        if (option.decorationPosition == QStyleOptionViewItem::Left
            || option.decorationPosition == QStyleOptionViewItem::Right) {
            w = textRect->width() + pm.width();
        } else {
            w = qMax(textRect->width(), pm.width());
        }
    } else {
        w = option.rect.width();
        h = option.rect.height();
    }

    int cw = 0;
    QRect check;
    if (hasCheck) {
        cw = checkRect->width() + 2 * checkMargin;
        if (hint) w += cw;
        if (option.direction == Qt::RightToLeft) {
            check.setRect(x + w - cw, y, cw, h);
        } else {
            check.setRect(x + checkMargin, y, cw, h);
        }
    }

    // at this point w should be the *total* width

    QRect display;
    QRect decoration;
    switch (option.decorationPosition) {
    case QStyleOptionViewItem::Top: {
        if (hasPixmap)
            pm.setHeight(pm.height() + pixmapMargin); // add space
        h = hint ? textRect->height() : h - pm.height();

        if (option.direction == Qt::RightToLeft) {
            decoration.setRect(x, y, w - cw, pm.height());
            display.setRect(x, y + pm.height(), w - cw, h);
        } else {
            decoration.setRect(x + cw, y, w - cw, pm.height());
            display.setRect(x + cw, y + pm.height(), w - cw, h);
        }
        break; }
    case QStyleOptionViewItem::Bottom: {
        if (hasText)
            textRect->setHeight(textRect->height() + textMargin); // add space
        h = hint ? textRect->height() + pm.height() : h;

        if (option.direction == Qt::RightToLeft) {
            display.setRect(x, y, w - cw, textRect->height());
            decoration.setRect(x, y + textRect->height(), w - cw, h - textRect->height());
        } else {
            display.setRect(x + cw, y, w - cw, textRect->height());
            decoration.setRect(x + cw, y + textRect->height(), w - cw, h - textRect->height());
        }
        break; }
    case QStyleOptionViewItem::Left: {
        if (option.direction == Qt::LeftToRight) {
            decoration.setRect(x + cw, y, pm.width(), h);
            display.setRect(decoration.right() + 1, y, w - pm.width() - cw, h);
        } else {
            display.setRect(x, y, w - pm.width() - cw, h);
            decoration.setRect(display.right() + 1, y, pm.width(), h);
        }
        break; }
    case QStyleOptionViewItem::Right: {
        if (option.direction == Qt::LeftToRight) {
            display.setRect(x + cw, y, w - pm.width() - cw, h);
            decoration.setRect(display.right() + 1, y, pm.width(), h);
        } else {
            decoration.setRect(x, y, pm.width(), h);
            display.setRect(decoration.right() + 1, y, w - pm.width() - cw, h);
        }
        break; }
    default:
        qWarning("doLayout: decoration position is invalid");
        decoration = *pixmapRect;
        break;
    }

    if (!hint) { // we only need to do the internal layout if we are going to paint
        *checkRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
                                         checkRect->size(), check);
        *pixmapRect = QStyle::alignedRect(option.direction, option.decorationAlignment,
                                          pixmapRect->size(), decoration);
        // the text takes up all available space, unless the decoration is not shown as selected
        if (option.showDecorationSelected)
            *textRect = display;
        else
            *textRect = QStyle::alignedRect(option.direction, option.displayAlignment,
                                            textRect->size().boundedTo(display.size()), display);
    } else {
        *checkRect = check;
        *pixmapRect = decoration;
        *textRect = display;
    }
}

QRect FileItemDelegatePrivate::rect(const QStyleOptionViewItem &option, const QModelIndex &index, int role) const
{
    QVariant value = index.data(role);
    if (role == Qt::CheckStateRole)
        return check(option, option.rect, value);
    if (value.isValid() && !value.isNull()) {
        switch (value.type()) {
        case QVariant::Invalid:
            break;
        case QVariant::Pixmap:
            return QRect(QPoint(0, 0), qvariant_cast<QPixmap>(value).size());
        case QVariant::Image:
            return QRect(QPoint(0, 0), qvariant_cast<QImage>(value).size());
        case QVariant::Icon: {
            QIcon::Mode mode = iconMode(option.state);
            QIcon::State state = iconState(option.state);
            QIcon icon = qvariant_cast<QIcon>(value);
            QSize size = icon.actualSize(option.decorationSize, mode, state);
            return QRect(QPoint(0, 0), size); }
        case QVariant::Color:
            return QRect(QPoint(0, 0), option.decorationSize);
        case QVariant::String:
        default: {
            const QStyleOptionViewItemV4 *optV4 = qstyleoption_cast<const QStyleOptionViewItemV4 *>(&option);
            QLocale locale = optV4 ? optV4->locale : QLocale::system();
            QString text = displayText(value, locale);
            value = index.data(Qt::FontRole);
            QFont fnt = qvariant_cast<QFont>(value).resolve(option.font);
            return textRectangle(0, textLayoutBounds(option), fnt, text);
        }
        }
    }
    return QRect();
}

QPixmap FileItemDelegatePrivate::decoration(const QStyleOptionViewItem &option, const QVariant &variant) const
{
    switch (variant.type()) {
    case QVariant::Icon: {
        QIcon::Mode mode = iconMode(option.state);
        QIcon::State state = iconState(option.state);
        return qvariant_cast<QIcon>(variant).pixmap(option.decorationSize, mode, state); }
    case QVariant::Color: {
        static QPixmap pixmap(option.decorationSize);
        pixmap.fill(qvariant_cast<QColor>(variant));
        return pixmap; }
    default:
        break;
    }

    return qvariant_cast<QPixmap>(variant);
}

// hacky but faster version of "QString::sprintf("%d-%d", i, enabled)"
static QString qPixmapSerial(quint64 i, bool enabled)
{
    ushort arr[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-', ushort('0' + enabled) };
    ushort *ptr = &arr[16];

    while (i > 0) {
        // hey - it's our internal representation, so use the ascii character after '9'
        // instead of 'a' for hex
        *(--ptr) = '0' + i % 16;
        i >>= 4;
    }

    return QString((const QChar *)ptr, int(&arr[sizeof(arr) / sizeof(ushort)] - ptr));
}

QPixmap * FileItemDelegatePrivate::selected(const QPixmap &pixmap, const QPalette &palette, bool enabled) const
{
    QString key = qPixmapSerial(qt_pixmap_id(pixmap), enabled);
    QPixmap *pm = QPixmapCache::find(key);
    if (!pm) {
        QImage img = pixmap.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied);

        QColor color = palette.color(enabled ? QPalette::Normal : QPalette::Disabled,
                                     QPalette::Highlight);
        color.setAlphaF((qreal)0.3);

        QPainter painter(&img);
        painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        painter.fillRect(0, 0, img.width(), img.height(), color);
        painter.end();

        QPixmap selected = QPixmap(QPixmap::fromImage(img));
        int n = (img.byteCount() >> 10) + 1;
        if (QPixmapCache::cacheLimit() < n)
            QPixmapCache::setCacheLimit(n);

        QPixmapCache::insert(key, selected);
        pm = QPixmapCache::find(key);
    }
    return pm;
}

QRect FileItemDelegatePrivate::check(const QStyleOptionViewItem &option, const QRect &bounding, const QVariant &value) const
{
    if (value.isValid()) {
        QStyleOptionButton opt;
        opt.QStyleOption::operator=(option);
        opt.rect = bounding;
        const QWidget *widget = this->widget(option); // cast
        QStyle *style = widget ? widget->style() : QApplication::style();
        return style->subElementRect(QStyle::SE_ViewItemCheckIndicator, &opt, widget);
    }
    return QRect();
}

QRect FileItemDelegatePrivate::textRectangle(QPainter * /*painter*/, const QRect &rect, const QFont &font, const QString &text) const
{
    textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    textLayout.setTextOption(textOption);
    textLayout.setFont(font);
    textLayout.setText(::replaceNewLine(text));
    QSizeF fpSize = doTextLayout(rect.width());
    const QSize size = QSize(qCeil(fpSize.width()), qCeil(fpSize.height()));
    // ###: textRectangle should take style option as argument
    const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;
    return QRect(0, 0, size.width() + 2 * textMargin, size.height());
}

QRect FileItemDelegatePrivate::textLayoutBounds(const QStyleOptionViewItemV2 &option) const
{
    QRect rect = option.rect;
    const bool wrapText = option.features & QStyleOptionViewItemV2::WrapText;
    switch (option.decorationPosition) {
    case QStyleOptionViewItem::Left:
    case QStyleOptionViewItem::Right:
        rect.setWidth(wrapText && rect.isValid() ? rect.width() : (QFIXED_MAX));
        break;
    case QStyleOptionViewItem::Top:
    case QStyleOptionViewItem::Bottom:
        // HACK: in filemanager, grid size is at least 32 pixels
        // bigger than the decoration size
        // TODO: pass grid size into the delegate via style option?
        rect.setWidth(wrapText ? option.decorationSize.width() + 32 : (QFIXED_MAX));
        break;
    }

    return rect;
}

QSizeF FileItemDelegatePrivate::doTextLayout(int lineWidth) const
{
    qreal height = 0;
    qreal widthUsed = 0;
    textLayout.beginLayout();
    while (true) {
        QTextLine line = textLayout.createLine();
        if (!line.isValid())
            break;
        line.setLineWidth(lineWidth);
        line.setPosition(QPointF(0, height));
        height += line.height();
        widthUsed = qMax(widthUsed, line.naturalTextWidth());
    }
    textLayout.endLayout();
    return QSizeF(widthUsed, height);
}

/*!
    \class FileItemDelegate

    FileItemDelegate is a special delegate that allows text to be wrapped at
    any point; also it provides corresponding item editing via QTextEdit.
*/

/*!
    Creates FileItemDelegate with the given \a parent
*/
FileItemDelegate::FileItemDelegate(QObject *parent) :
    QAbstractItemDelegate(parent),
    d_ptr(new FileItemDelegatePrivate(this))
{
}

/*!
    Destroys FileItemDelegate.
*/
FileItemDelegate::~FileItemDelegate()
{
    delete d_ptr;
}

/*!
    \reimp
*/
void FileItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_D(const FileItemDelegate);

    Q_ASSERT(index.isValid());

    QStyleOptionViewItemV4 opt(option);
    d->initStyleOption(&opt, index);
    opt.showDecorationSelected = true;

    // prepare
    painter->save();
//    if (hasClipping())
    painter->setClipRect(opt.rect);

    // get the data and the rectangles

    QVariant value;

    QPixmap pixmap;
    QRect decorationRect;
    value = index.data(Qt::DecorationRole);
    if (value.isValid()) {
        // ### we need the pixmap to call the virtual function
        pixmap = d->decoration(opt, value);
        if (value.type() == QVariant::Icon) {
            d->tmp.icon = qvariant_cast<QIcon>(value);
            d->tmp.mode = d->iconMode(option.state);
            d->tmp.state = d->iconState(option.state);
            const QSize size = d->tmp.icon.actualSize(option.decorationSize,
                                                      d->tmp.mode, d->tmp.state);
            decorationRect = QRect(QPoint(0, 0), size);
        } else {
            d->tmp.icon = QIcon();
            decorationRect = QRect(QPoint(0, 0), pixmap.size());
        }
    } else {
        d->tmp.icon = QIcon();
        decorationRect = QRect();
    }

    QString text;
    QRect displayRect;
    value = index.data(Qt::DisplayRole);
    if (value.isValid() && !value.isNull()) {
        text = d->displayText(value, opt.locale);
        displayRect = d->textRectangle(painter, d->textLayoutBounds(opt), opt.font, text);
    }

    QRect checkRect;
    Qt::CheckState checkState = Qt::Unchecked;
    value = index.data(Qt::CheckStateRole);
    if (value.isValid()) {
        checkState = static_cast<Qt::CheckState>(value.toInt());
        checkRect = d->check(opt, opt.rect, value);
    }

    // do the layout

    d->doLayout(opt, &checkRect, &decorationRect, &displayRect, false);

    // draw the item

    d->drawBackground(painter, opt, index);
    d->drawCheck(painter, opt, checkRect, checkState);
    d->drawDecoration(painter, opt, decorationRect, pixmap);
    d->drawDisplay(painter, opt, displayRect, text);
    d->drawFocus(painter, opt, displayRect);

    // done
    painter->restore();
}

/*!
    \reimp
*/
QSize FileItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_D(const FileItemDelegate);

    QVariant value = index.data(Qt::SizeHintRole);
    if (value.isValid())
        return qvariant_cast<QSize>(value);

    QStyleOptionViewItemV4 opt(option);
    opt.rect = QRect();

    QRect decorationRect = d->rect(opt, index, Qt::DecorationRole);
    QRect displayRect = d->rect(opt, index, Qt::DisplayRole);
    QRect checkRect = d->rect(opt, index, Qt::CheckStateRole);

    d->doLayout(opt, &checkRect, &decorationRect, &displayRect, true);

    return (decorationRect|displayRect|checkRect).size();
}

/*!
    \reimp
*/
QWidget * FileItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
    QTextEdit *textEdit = new FileTextEdit(parent);
    if (textEdit) {
        textEdit->setAttribute(Qt::WA_MacShowFocusRect);
        textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textEdit->setAcceptRichText(false);
        textEdit->setAlignment(option.displayAlignment);
        const QStyleOptionViewItemV2 *optionV2 = qstyleoption_cast<const QStyleOptionViewItemV2 *>(&option);
        if (optionV2) {
            bool wrap = optionV2->features.testFlag(QStyleOptionViewItemV2::WrapText);
            textEdit->setWordWrapMode(wrap ? QTextOption::WrapAtWordBoundaryOrAnywhere : QTextOption::NoWrap);
        }
    }

    return textEdit;
}

/*!
    \reimp
*/
void FileItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QTextEdit *textEdit = qobject_cast<QTextEdit*>(editor);
    if (textEdit) {
        QString text = index.data(Qt::EditRole).toString();
        textEdit->insertPlainText(text);

        QString suffix = getSuffix(text);

        if (!suffix.isEmpty()) {
            // The filename contains an extension. Assure that only the filename
            // gets selected.
            const int selectionLength = text.length() - suffix.length() - 1;
            QTextCursor cursor = textEdit->textCursor();
            cursor.movePosition(QTextCursor::StartOfBlock);
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, selectionLength);
            textEdit->setTextCursor(cursor);
        } else {
            textEdit->selectAll();
        }
    }
}

/*!
    \reimp
*/
void FileItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QTextEdit *textEdit = qobject_cast<QTextEdit *>(editor);

    if (!textEdit)
        return;

    QString text = textEdit->toPlainText();

    QString oldSuffix = getSuffix(index.data(Qt::EditRole).toString());
    QString newSuffix = getSuffix(text);
    QString newName;
    if (newSuffix.isEmpty())
        newName = text;
    else
        newName = text.left(text.length() - newSuffix.length() - 1);

    if (FileManagerSettings::globalSettings()->warnOnExtensionChange() && oldSuffix != newSuffix) {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Rename"));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("Are you sure you want to change the extension from \".%1\" to \".%2\"?").
                       arg(oldSuffix).arg(newSuffix));
        msgBox.setInformativeText("If you make this change, your file may open in a different application.");

        msgBox.addButton(tr("Use \".%1\"").arg(newSuffix), QMessageBox::AcceptRole);
        msgBox.addButton(tr("Keep \".%1\"").arg(oldSuffix), QMessageBox::RejectRole);
//        msgBox.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint);

        if (msgBox.exec() == QMessageBox::RejectRole) {
            model->setData(index, QString(QLatin1String("%1.%2")).arg(newName).arg(oldSuffix), Qt::EditRole);
            return;
        }
    }

    model->setData(index, text, Qt::EditRole);
}

/*!
    \reimp
*/
void FileItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!editor)
        return;

    Q_D(const FileItemDelegate);

    Q_ASSERT(index.isValid());
    QPixmap pixmap = d->decoration(option, index.data(Qt::DecorationRole));
    QString text = replaceNewLine(index.data(Qt::DisplayRole).toString());
    QRect pixmapRect = QRect(QPoint(0, 0), option.decorationSize).intersected(pixmap.rect());
    QRect textRect = d->textRectangle(0, option.rect, option.font, text);
    QRect checkRect = d->check(option, textRect, index.data(Qt::CheckStateRole));
    QStyleOptionViewItem opt = option;
    opt.showDecorationSelected = true; // let the editor take up all available space
    d->doLayout(opt, &checkRect, &pixmapRect, &textRect, false);
    editor->setGeometry(textRect);

    if (QTextEdit *edit = qobject_cast<QTextEdit*>(editor)) {
        const QWidget *widget = d->widget(option);
        const QStyle *style = widget ? widget->style() : QApplication::style();
        const int textMargin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, widget)+1;
        QTextFrame * frame = edit->document()->rootFrame();
        QTextFrameFormat format = frame->frameFormat();
        format.setLeftMargin(textMargin);
        format.setRightMargin(textMargin);
        frame->setFrameFormat(format);

        QRect rect = textRect;
        const int frameWidth = edit ? edit->frameWidth() : 0;

        rect.adjust(-frameWidth, -frameWidth, frameWidth, frameWidth);
        editor->setGeometry(rect);
    }
}

/*!
    \reimp
*/
bool FileItemDelegate::eventFilter(QObject *object, QEvent *event)
{
    QWidget *editor = qobject_cast<QWidget*>(object);
    if (!editor)
        return false;
    if (event->type() == QEvent::KeyPress) {
        switch (static_cast<QKeyEvent *>(event)->key()) {
        case Qt::Key_Tab:
            emit commitData(editor);
            emit closeEditor(editor, QAbstractItemDelegate::EditNextItem);
            return true;
        case Qt::Key_Backtab:
            emit commitData(editor);
            emit closeEditor(editor, QAbstractItemDelegate::EditPreviousItem);
            return true;
        case Qt::Key_Enter:
        case Qt::Key_Return:
            emit commitData(editor);
            emit closeEditor(editor, QAbstractItemDelegate::SubmitModelCache);
            return false;
        case Qt::Key_Escape:
            // don't commit data
            emit closeEditor(editor, QAbstractItemDelegate::RevertModelCache);
            break;
        default:
            return false;
        }
        if (editor->parentWidget())
            editor->parentWidget()->setFocus();
        return true;
    } else if (event->type() == QEvent::FocusOut || (event->type() == QEvent::Hide && editor->isWindow())) {
        //the Hide event will take care of he editors that are in fact complete dialogs
        if (!editor->isActiveWindow() || (QApplication::focusWidget() != editor)) {
            QWidget *w = QApplication::focusWidget();
            while (w) { // don't worry about focus changes internally in the editor
                if (w == editor)
                    return false;
                w = w->parentWidget();
            }
//#ifndef QT_NO_DRAGANDDROP
//            // The window may lose focus during an drag operation.
//            // i.e when dragging involves the taskbar on Windows.
//            if (QDragManager::self() && QDragManager::self()->object != 0)
//                return false;
//#endif

            emit commitData(editor);
            emit closeEditor(editor, NoHint);
        }
    } else if (event->type() == QEvent::ShortcutOverride) {
        if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_Escape) {
            event->accept();
            return true;
        }
    }
    return false;
}
