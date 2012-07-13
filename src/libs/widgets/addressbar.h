#ifndef ADDRESSBAR_H
#define ADDRESSBAR_H

#include "fancylineedit.h"

#include <QtCore/QUrl>

class WIDGETS_EXPORT AddressBar : public FancyLineEdit
{
    Q_OBJECT
public:
    explicit AddressBar(QWidget *parent = 0);

    void setLoading(bool);

public slots:
    void setUrl(const QUrl &url);
    void startLoad();
    void finishLoad();
    void setLoadProgress(int);

signals:
    void canceled();
    void refresh();
    void open(const QUrl &url);

protected:
    void keyPressEvent(QKeyEvent *);
    void paintEvent(QPaintEvent *);

private slots:
    void onRightButtonClicked();

private:
    void updateUrl();
    QLinearGradient generateGradient(const QColor &color) const;

private:
    QUrl m_url;
    bool m_loading;
    int m_progress;
};

#endif // ADDRESSBAR_H
