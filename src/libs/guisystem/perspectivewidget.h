#ifndef PERSPECTIVEWIDGET_H
#define PERSPECTIVEWIDGET_H

#include <QtGui/QWidget>
#include <QHash>

class QStackedLayout;

namespace GuiSystem {

class CentralWidget;
class Perspective;
class PerspectiveInstance;

class PerspectiveWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PerspectiveWidget(QWidget *parent = 0);

    static PerspectiveWidget *activeWidget();

    PerspectiveInstance * currentInstance() const;

    Perspective *currentPerspective() const;
    void openPerspective(const QString &perspective);
    void closePerspective(const QString &perspective);

    bool event(QEvent * e);

signals:

public slots:

protected:
    void createInstance(Perspective * p, int index);
    void hideViews();
    void showViews();

    QStackedLayout *m_layout;
    QList<PerspectiveInstance *> m_topInstances;
    PerspectiveInstance * m_currentInstance;
    QHash<Perspective *, PerspectiveInstance*> m_mapToInstace;
};

} // namespace GuiSystem

#endif // PERSPECTIVEWIDGET_H
