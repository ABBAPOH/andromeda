#include "filterlineedit.h"

/*!
    \class FilterLineEdit

    \brief A fancy line edit customized for filtering purposes with a clear button.
*/

FilterLineEdit::FilterLineEdit(QWidget *parent) :
   FancyLineEdit(parent),
   m_lastFilterText(text())
{
    // KDE has custom icons for this. Notice that icon namings are counter intuitive.
    // If these icons are not available we use the freedesktop standard name before
    // falling back to a bundled resource.
    QIcon icon = QIcon::fromTheme(layoutDirection() == Qt::LeftToRight ?
                     QLatin1String("edit-clear-locationbar-rtl") :
                     QLatin1String("edit-clear-locationbar-ltr"),
                     QIcon::fromTheme(QLatin1String("edit-clear"), QIcon(QLatin1String(":/icons/editclear.png"))));

    setButtonPixmap(Right, icon.pixmap(16));
    setButtonVisible(Right, true);
    setPlaceholderText(tr("Filter"));
    setButtonToolTip(Right, tr("Clear text"));
    setAutoHideButton(Right, true);
    connect(this, SIGNAL(rightButtonClicked()), this, SLOT(clear()));
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(slotTextChanged()));
}

void FilterLineEdit::slotTextChanged()
{
    const QString newlyTypedText = text();
    if (newlyTypedText != m_lastFilterText) {
        m_lastFilterText = newlyTypedText;
        emit filterChanged(m_lastFilterText);
    }
}
