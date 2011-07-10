#ifndef TESTVIEW2_H
#define TESTVIEW2_H

#include <GuiSystem>

using namespace GuiSystem;

class TestView2 : public IView
{
    Q_OBJECT
public:
    explicit TestView2(QObject *parent = 0);

    QString type() const
    {
        return "TestType 2";
    }

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


class TestViewFactory2: public IViewFactory
{
public:
    QString id() const
    {
        return "TestViewFactory2";
    }

    QString type() const
    {
        return "TestView ";
    }

    IView *createView() const
    {
        return new TestView2;
    }
};

#endif // TESTVIEW2_H
