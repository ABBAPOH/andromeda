#ifndef FILEMANAGERVIEW_H
#define FILEMANAGERVIEW_H

#include <ihistoryview.h>
#include <iviewfactory.h>

namespace FileManagerPlugin {

class FileManagerWidget;
class FileManagerView : public CorePlugin::IHistoryView
{
    Q_OBJECT
public:
    explicit FileManagerView(QObject *parent = 0);

    // from IView
    virtual void initialize(GuiSystem::State *state);
    virtual QString type() const;
    virtual QWidget *widget() const;

    // from IHistoryView
    void back();
    void forward();

signals:

public slots:
    void onCurrentPathChange(const QString &path);

private:
    FileManagerWidget *m_widget;
    GuiSystem::State *m_state;
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
