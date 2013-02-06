#ifndef FILEITEMDELEGATE_P_H
#define FILEITEMDELEGATE_P_H

#include "fileitemdelegate.h"

#include <QtGui/QTextEdit>

namespace FileManager {

class FileTextEdit : public QTextEdit
{
    Q_OBJECT
    Q_DISABLE_COPY(FileTextEdit)
public:
    explicit FileTextEdit(QWidget *parent = 0);

protected:
    void resizeEvent(QResizeEvent *e);
    void showEvent(QShowEvent *e);

    void realignVCenter(QTextEdit *pTextEdit);
};

class FileItemDelegatePrivate
{
    Q_DECLARE_PUBLIC(FileItemDelegate)

public:
    explicit FileItemDelegatePrivate(FileItemDelegate *qq) : q_ptr(qq) {}

    QString displayText(const QVariant &value, const QLocale &locale) const;
    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const;

    void drawDisplay(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QString &text) const;
    void drawCheck(QPainter *painter, const QStyleOptionViewItem &option,
                               const QRect &rect, Qt::CheckState state) const;
    void drawDecoration(QPainter *painter, const QStyleOptionViewItem &option,
                                    const QRect &rect, const QPixmap &pixmap) const;
    void drawFocus(QPainter *painter, const QStyleOptionViewItem &option,
                               const QRect &rect) const;
    void drawBackground(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void doLayout(const QStyleOptionViewItem &option,
                  QRect *checkRect, QRect *iconRect, QRect *textRect, bool hint) const;

    QRect rect(const QStyleOptionViewItem &option, const QModelIndex &index, int role) const;

    QPixmap decoration(const QStyleOptionViewItem &option, const QVariant &variant) const;
    QPixmap *selected(const QPixmap &pixmap, const QPalette &palette, bool enabled) const;
    QRect check(const QStyleOptionViewItem &option, const QRect &bounding,
                const QVariant &value) const;
    QRect textRectangle(QPainter *painter, const QRect &rect,
                        const QFont &font, const QString &text) const;

    inline QIcon::Mode iconMode(QStyle::State state) const
    {
        if (!(state & QStyle::State_Enabled)) return QIcon::Disabled;
        if (state & QStyle::State_Selected) return QIcon::Selected;
        return QIcon::Normal;
    }

    inline QIcon::State iconState(QStyle::State state) const
    {
        return state & QStyle::State_Open ? QIcon::On : QIcon::Off;
    }

    inline static QString replaceNewLine(QString text)
    {
        const QChar nl = QLatin1Char('\n');
        for (int i = 0; i < text.count(); ++i)
            if (text.at(i) == nl)
                text[i] = QChar::LineSeparator;
        return text;
    }

    QRect textLayoutBounds(const QStyleOptionViewItemV2 &option) const;
    QSizeF doTextLayout(int lineWidth) const;
    mutable QTextLayout textLayout;
    mutable QTextOption textOption;

    const QWidget *widget(const QStyleOptionViewItem &option) const
    {
        if (const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option))
            return v3->widget;

        return 0;
    }

    // ### temporary hack until we have QStandardItemDelegate
    mutable struct Icon {
        QIcon icon;
        QIcon::Mode mode;
        QIcon::State state;
    } tmp;

    FileItemDelegate *q_ptr;
};

} // namespace FileManager

#endif // FILEITEMDELEGATE_P_H
