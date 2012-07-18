#include "errorsdialog.h"

#include <QtGui/QDialogButtonBox>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QStringListModel>
#include <QtGui/QVBoxLayout>

using namespace ExtensionSystem;

/*!
    \class ExtensionSystem::ErrorsDialog

    \brief Helper class for displaying errors from ExtensionSystem::PluginManager.
*/

/*!
    Creates ErrorsDialog with the given \a parent.
*/
ErrorsDialog::ErrorsDialog(QWidget *parent) :
    QDialog(parent),
    m_label(new QLabel(this)),
    m_view(new QListView(this)),
    m_model(new QStringListModel(this)),
    m_buttonBox(new QDialogButtonBox(this))
{
    m_view->setModel(m_model);
    m_view->setSpacing(10);
    m_view->setWordWrap(true);

    m_buttonBox->addButton(QDialogButtonBox::Close);

    QVBoxLayout * vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_label);
    vBoxLayout->addWidget(m_view);
    vBoxLayout->addWidget(m_buttonBox);

    connect(m_buttonBox, SIGNAL(accepted()), SLOT(accept()));
    connect(m_buttonBox, SIGNAL(rejected()), SLOT(reject()));

    resize(640, 480);
}

/*!
    Returns brief message of an error.
*/
QString ErrorsDialog::message() const
{
    return m_label->text();
}

/*!
    Sets brief message of an error.
*/
void ErrorsDialog::setMessage(const QString &message)
{
    m_label->setText(message);
}

/*!
    Sets list of error strings.
*/
void ErrorsDialog::setErrors(const QStringList &errors)
{
    m_model->setStringList(errors);
}
