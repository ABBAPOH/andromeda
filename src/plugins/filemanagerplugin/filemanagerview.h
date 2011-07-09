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
    virtual void initialize(/*GuiSystem::State *state*/);
    virtual QString type() const;
    virtual QWidget *widget() const;

    // from IMainView
    virtual bool open(const QString &path);
    virtual bool open(const HistoryItem &item);

    virtual HistoryItem currentItem() const;

signals:

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
    virtual ~FileManagerFactory() {}

    virtual QString id() const;

protected:
    virtual GuiSystem::IView *createView() const;
};

} // namespace FileManagerPlugin

#endif // FILEMANAGERVIEW_H
