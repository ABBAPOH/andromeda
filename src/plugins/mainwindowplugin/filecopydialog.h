#ifndef FILECOPYDIALOG_H
#define FILECOPYDIALOG_H

#include <QDialog>

class QScrollArea;
class QVBoxLayout;
namespace MainWindowPlugin {

class FileCopyDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FileCopyDialog(QWidget *parent = 0);

    void addWidget(QWidget *widget);

protected:
    void resizeEvent(QResizeEvent *);

private:
    void setupUi();
    QScrollArea *m_scrollArea;
    QVBoxLayout *m_layout;
    QWidget *m_widget;
};

}

#endif // FILECOPYDIALOG_H
