#ifndef QTABBAREX_H
#define QTABBAREX_H

#include <QTabBar>

class QTimer;
class TabBar : public QTabBar
{
    Q_OBJECT
    Q_PROPERTY(bool switchTabsOnDrag READ switchTabsOnDrag WRITE setSwitchTabsOnDrag)
    Q_PROPERTY(bool switchOnHover READ switchOnHover WRITE setSwitchOnHover)
    Q_PROPERTY(int switchTimeout READ switchTimeout WRITE setSwitchTimeout)

public:
    explicit TabBar(QWidget *parent = 0);

    bool switchTabsOnDrag();
    void setSwitchTabsOnDrag(bool switchTabsOnDrag);

    bool switchOnHover();
    void setSwitchOnHover(bool switchOnHover);

    int switchTimeout();
    void setSwitchTimeout(int milliseconds);

signals:

private slots:
    void changeTab();

protected:
     void dragEnterEvent(QDragEnterEvent *event);
     void dragMoveEvent(QDragMoveEvent *event);
     void dragLeaveEvent(QDragLeaveEvent * event);

     QSize tabSizeHint(int index) const;

private:
     int m_tabIndex;
     QTimer * m_timer;
     bool m_switchTabsOnDrag;
     bool m_switchOnHover;
     int m_switchTimeout;
};

#endif // QTABBAREX_H
