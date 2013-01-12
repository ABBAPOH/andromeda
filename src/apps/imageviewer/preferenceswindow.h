#ifndef PREFERENCESWINDOW_H
#define PREFERENCESWINDOW_H

#include <QtGui/QWidget>

namespace ImageViewer {

class PreferencesWindow : public QWidget
{
    Q_OBJECT
public:
    explicit PreferencesWindow(QWidget *parent = 0);
};

} // namespace ImageViewer

#endif // PREFERENCESWINDOW_H
