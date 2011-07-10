#ifndef NAVIGATIONPANELVIEW_H
#define NAVIGATIONPANELVIEW_H

#include <iview.h>
#include <iviewfactory.h>

namespace FileManagerPlugin {

class NavigationPanel;
class NavigationPanelView : public GuiSystem::IView
{
    Q_OBJECT
public:
    explicit NavigationPanelView(QObject *parent = 0);

    void initialize();

    QString type() const;

    QWidget *widget() const;

private:
    NavigationPanel *m_widget;
};

class NavigationPanelFactory : public GuiSystem::IViewFactory
{
    Q_OBJECT
public:
    explicit NavigationPanelFactory(QObject * parent = 0) : GuiSystem::IViewFactory(parent) {}
    ~NavigationPanelFactory() {}

    QString id() const;

protected:
    GuiSystem::IView *createView() const;
};

} // namespace FileManagerPlugin

#endif // NAVIGATIONPANELVIEW_H
