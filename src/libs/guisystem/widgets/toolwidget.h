#ifndef TOOLWIDGET_H
#define TOOLWIDGET_H

#include "../guisystem_global.h"

#include <QtCore/QPointer>
#include <QtGui/QWidget>

namespace GuiSystem {

class AbstractEditor;
class ToolModel;

class GUISYSTEM_EXPORT ToolWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ToolWidget)

public:
    explicit ToolWidget(ToolModel &model, QWidget *parent = 0);

    ToolModel *model() const;

    AbstractEditor *editor() const;
    virtual void setEditor(AbstractEditor *editor);

signals:
    void editorChanged();

protected slots:
    virtual void onDocumentChanged();

protected:
    ToolModel *m_model;
    QPointer<AbstractEditor> m_editor;
};

} // namespace GuiSystem

#endif // TOOLWIDGET_H
