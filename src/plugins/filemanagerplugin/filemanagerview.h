#ifndef FILEMANAGERVIEW_H
#define FILEMANAGERVIEW_H

#include <imainview.h>
#include <iviewfactory.h>

namespace FileManagerPlugin {

class DualPaneWidget;
class FileManagerView : public CorePlugin::IMainView
{
    Q_OBJECT
public:
    explicit FileManagerView(QObject *parent = 0);

    // from IView
    void initialize();
    QString type() const;
    QWidget *widget() const;

    // from IMainView
    bool open(const QString &path);
    bool open(const CorePlugin::HistoryItem &item);

    CorePlugin::HistoryItem currentItem() const;

private slots:
    void setDualPaneModeEnabled(bool on);

private:
    DualPaneWidget *m_widget;
    const GuiSystem::State *m_state;
};

class FileManagerFactory : public GuiSystem::IViewFactory
{
    Q_OBJECT
public:
    explicit FileManagerFactory(QObject * parent = 0) : GuiSystem::IViewFactory(parent) {}
    ~FileManagerFactory() {}

    QString id() const;

protected:
    GuiSystem::IView *createView() const;
};

} // namespace FileManagerPlugin

#endif // FILEMANAGERVIEW_H
