#ifndef TESTVIEW_H
#define TESTVIEW_H

#include <GuiSystem>

using namespace GuiSystem;

class TestView : public QObject, public IView
{
    Q_OBJECT
public:
    explicit TestView(QObject *parent = 0);

    virtual QString type() const
    {
        return "TestType";
    }

    virtual QWidget *widget() const
    {
        return m_widget;
    }
    virtual QToolBar *toolBar() const
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
    virtual QString id() const
    {
        return "TestViewFactory";
    }

    virtual QString type() const
    {
        return "TestView";
    }

    virtual IView *createView(QObject *parent) const
    {
        return new TestView;
    }
};

#endif // TESTVIEW_H
