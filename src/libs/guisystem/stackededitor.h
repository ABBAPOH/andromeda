#ifndef STACKEDEDITOR_H
#define STACKEDEDITOR_H

#include "abstracteditor.h"

namespace GuiSystem {

class StackedEditorPrivate;
class StackedEditor : public AbstractEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(StackedEditor)

public:
    explicit StackedEditor(QWidget *parent = 0);
    ~StackedEditor();

    Capabilities capabilities() const;

    QUrl url() const;

    QIcon icon() const;
    QString title() const;
    QString windowTitle() const;

    AbstractHistory *history() const;

    virtual void restoreState(const QByteArray &state);
    virtual QByteArray saveState() const;

public slots:
    void open(const QUrl &url = QUrl());
    void close();

    void cancel();
    void refresh();

private slots:
    void onUrlChanged(const QUrl &url);
    void onIndexChanged(int index);
    void onDestroy(QObject*);

private:
    StackedEditorPrivate *d;
};

} // namespace GuiSystem

#endif // STACKEDEDITOR_H
