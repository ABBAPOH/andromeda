#ifndef TESTVIEW_H
#define TESTVIEW_H

#include <GuiSystem>

using namespace GuiSystem;

class TestView : public IView
{
    Q_OBJECT
public:
    explicit TestView(QObject *parent = 0);

    QWidget *widget() const
    {
        return m_widget;
    }

    QToolBar *toolBar() const
    {
        return m_toolBar;
    }

signals:

public slots:

private:
    QWidget * m_widget;
    QToolBar *m_toolBar;

};

class TestViewFactory: public IViewFactory
{
public:
    QString id() const
    {
        return "TestViewFactory";
    }

    QString type() const
    {
        return "TestView";
    }

    IView *createView()
    {
        return new TestView(this);
    }
};

#endif // TESTVIEW_H
