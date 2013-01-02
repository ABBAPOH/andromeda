#ifndef FILTERLINEEDIT_H
#define FILTERLINEEDIT_H

#include "widgets_global.h"

#include "fancylineedit.h"

class WIDGETS_EXPORT FilterLineEdit : public FancyLineEdit
{
    Q_OBJECT
    Q_DISABLE_COPY(FilterLineEdit)
public:
    explicit FilterLineEdit(QWidget *parent = 0);

signals:
    void filterChanged(const QString &);

private slots:
    void slotTextChanged();

private:
    QString m_lastFilterText;
};

#endif // FILTERLINEEDIT_H
