#include "fileitemdelegate.h"
#include "fileitemdelegate_p.h"

#include <QtCore/QFileInfo>

#include <QtGui/QApplication>
#include <QtGui/QKeyEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QTextFrame>

#include <IO/QMimeDatabase>

#include "filemanagersettings.h"

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

/*!
    \class FileItemDelegate
*/
FileItemDelegate::FileItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

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

void FileItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
    QTextEdit *edit = qobject_cast<QTextEdit*>(editor);
    if (edit) {
        QRect r = editor->geometry();
        const int frameWidth = edit->frameWidth();

        const QStyleOptionViewItemV3 *optionV3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option);
        const QWidget *widget = optionV3 ? optionV3->widget : 0;
        const QStyle *style = widget ? widget->style() : qApp->style();
        const int textMargin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, &option, widget) + 1;

        r.adjust(-frameWidth - textMargin, -frameWidth, frameWidth + textMargin, frameWidth);
        editor->setGeometry(r);
    }
}

bool FileItemDelegate::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return) {
            QTextEdit *editor = qobject_cast<QTextEdit *>(object);
            if (editor) {
                emit commitData(editor);
                emit closeEditor(editor, QAbstractItemDelegate::NoHint);
                return true;
            }
        }
    }
    return QStyledItemDelegate::eventFilter(object, event);
}
