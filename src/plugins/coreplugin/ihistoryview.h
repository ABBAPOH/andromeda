#ifndef IHISTORYVIEW_H
#define IHISTORYVIEW_H

#include <iview.h>

namespace CorePlugin {

class IHistoryView : public GuiSystem::IView
{
    Q_OBJECT
public:
    explicit IHistoryView(QObject *parent = 0) : IView(parent) {}

    virtual void back() {}
    virtual void forward() {}
    virtual bool canGoBack() { return false; }
    virtual bool canGoForward() { return false; }

signals:
    void canGoBackChanged();
    void canGoForwardChanged();

public slots:

};

} // namespace CorePlugin

#endif // IHISTORYVIEW_H
