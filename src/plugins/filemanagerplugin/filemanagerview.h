#ifndef FILEMANAGERVIEW_H
#define FILEMANAGERVIEW_H

#include <ieditor.h>
#include <iviewfactory.h>

namespace FileManagerPlugin {

class DualPaneWidget;
class FileManagerView : public CorePlugin::IEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(FileManagerView)

public:
    explicit FileManagerView(QObject *parent = 0);

    // from IView
    void initialize();
    QString type() const;
    QWidget *widget() const;

    // from IEditor
    bool open(const QString &path);
    bool open(const CorePlugin::HistoryItem &item);

    CorePlugin::HistoryItem currentItem() const;

private slots:
    void setDualPaneModeEnabled(bool on);
    void setViewMode(int);

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
    QString type() const;

protected:
    GuiSystem::IView *createView();
};

} // namespace FileManagerPlugin

#endif // FILEMANAGERVIEW_H
