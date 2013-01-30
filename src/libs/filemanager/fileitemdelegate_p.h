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

} // namespace FileManager

#endif // FILEITEMDELEGATE_P_H
