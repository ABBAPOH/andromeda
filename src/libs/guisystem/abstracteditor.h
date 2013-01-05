#ifndef ABSTRACTEDITOR_H
#define ABSTRACTEDITOR_H

#include "guisystem_global.h"

#include <QtCore/QUrl>
#include <QtGui/QWidget>

#include "actionmanager.h"

namespace GuiSystem {

class AbstractDocument;
class IFind;
class IHistory;

class GUISYSTEM_EXPORT AbstractEditor : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractEditor)
public:
    explicit AbstractEditor(AbstractDocument &document, QWidget *parent = 0);
    ~AbstractEditor();

    AbstractDocument *document() const;
    virtual void setDocument(AbstractDocument *document);

    virtual IFind *find() const;
    virtual IHistory *history() const;

    virtual void restoreDefaults() {}
    virtual bool restoreState(const QByteArray &state);
    virtual QByteArray saveState() const;

signals:
    void documentChanged();

protected:
    ActionManager *actionManager() const;
    void addAction(QAction *action, const QByteArray &id);
    void registerAction(QAction *action, const QByteArray &id);

private:
    AbstractDocument *m_document;
};

} // namespace GuiSystem

#endif // ABSTRACTEDITOR_H
