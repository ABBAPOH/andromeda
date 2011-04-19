#ifndef FILEMANAGERVIEW_H
#define FILEMANAGERVIEW_H

#include <iview.h>
#include <iviewfactory.h>

namespace FileManagerPlugin {

class FileManagerWidget;
class FileManagerView : public GuiSystem::IView
{
    Q_OBJECT
public:
    explicit FileManagerView(QObject *parent = 0);

    virtual void initialize(GuiSystem::State *state);
    virtual QString type() const;
    virtual QWidget *widget() const;

signals:

public slots:

private:
    FileManagerWidget *m_widget;
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
