#include "findtoolbar.h"

#include <QtGui/QAction>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QToolButton>
#include <widgets/filterlineedit.h>

namespace GuiSystem {

class FindToolBarPrivate
{
public:
    QGridLayout *layout;

    QMenu *lineEditMenu;
    FilterLineEdit *findLineEdit;

    QToolButton *findPrevButton;
    QToolButton *findNextButton;
    QToolButton *closeButton;

    FilterLineEdit *replaceLineEdit;
    QToolButton *replaceButton;
    QToolButton *replaceNextButton;
    QToolButton *replaceAllButton;

    QSpacerItem *findSpacer;
    QSpacerItem *replaceSpacer;

    QAction *caseSensitiveAction;
    QAction *wholeWordAction;
    QAction *regularExpressionAction;

    QAction *findPrevAction;
    QAction *findNextAction;
    QAction *closeAction;
    QAction *replaceNextAction;

    IFind *find;
};

} // namespace GuiSystem

using namespace GuiSystem;

FindToolBar::FindToolBar(QWidget *parent) :
    QWidget(parent),
    d(new FindToolBarPrivate)
{
    d->find = 0;

    setupActions();
    setupUi();
    retranslateUi();
    setupConnections();

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
}

FindToolBar::~FindToolBar()
{
    delete d;
}

IFind *FindToolBar::find() const
{
    return d->find;
}

void FindToolBar::setFind(IFind *find)
{
    d->find = find;

    updateUi();
}

IFind::FindFlags FindToolBar::currentFlags() const
{
    IFind::FindFlags result = 0;

    if (d->caseSensitiveAction->isChecked())
        result |= IFind::FindCaseSensitively;
    if (d->caseSensitiveAction->isChecked())
        result |= IFind::FindWholeWords;
    if (d->regularExpressionAction->isChecked())
        result |= IFind::FindRegularExpression;

    return result;
}

QString FindToolBar::findString() const
{
    return d->findLineEdit->text();
}

QString FindToolBar::replaceString() const
{
    return d->replaceLineEdit->text();
}

void FindToolBar::onFilterChanged()
{
    if (!d->find)
        return;

    d->find->findIncremental(findString(), currentFlags());
    d->find->highlightAll(findString(), currentFlags());
}

void FindToolBar::findPrev()
{
    if (d->find)
        d->find->findStep(findString(), currentFlags() | IFind::FindBackward);
}

void FindToolBar::findNext()
{
    if (d->find)
        d->find->findStep(findString(), currentFlags());
}

void FindToolBar::close()
{
    if (d->find)
        d->find->clearResults();
    QWidget::close();
}

void FindToolBar::replace()
{
    if (d->find)
        d->find->replace(findString(), replaceString(), currentFlags());
}

void FindToolBar::replaceNext()
{
    if (d->find)
        d->find->replaceStep(findString(), replaceString(), currentFlags());
}

void FindToolBar::replaceAll()
{
    if (d->find)
        d->find->replaceAll(findString(), replaceString(), currentFlags());
}

/*!
  \brief Opens find toolbar - toolbar becomes visible and find lineedit gets focus.
*/
void FindToolBar::openFind()
{
    setVisible(true);
    d->findLineEdit->setFocus();
}

#include "actionmanager.h"
void FindToolBar::setupActions()
{
    ActionManager *manager = ActionManager::instance();

    d->findPrevAction = new QAction(this);
    d->findPrevAction->setShortcut(QKeySequence::FindPrevious);
    manager->registerAction(d->findPrevAction, "Actions.FindPrevious");

    d->findNextAction = new QAction(this);
    d->findNextAction->setShortcut(QKeySequence::FindNext);
    manager->registerAction(d->findNextAction, "Actions.FindNext");

    d->closeAction = new QAction(this);
    d->closeAction->setShortcut(QKeySequence("Esc"));

    d->caseSensitiveAction = new QAction(this);
    d->caseSensitiveAction->setIcon(QIcon(QLatin1String(":/icons/casesensitively.png")));
    d->caseSensitiveAction->setCheckable(true);
    d->caseSensitiveAction->setChecked(false);

    d->wholeWordAction = new QAction(this);
    d->wholeWordAction->setIcon(QIcon(QLatin1String(":/icons/wholewords.png")));
    d->wholeWordAction->setCheckable(true);
    d->wholeWordAction->setChecked(false);

    d->regularExpressionAction = new QAction(this);
    d->regularExpressionAction->setIcon(QIcon(QLatin1String(":/icons/regexp.png")));
    d->regularExpressionAction->setCheckable(true);
    d->regularExpressionAction->setChecked(false);

    d->lineEditMenu = new QMenu(this);
    d->lineEditMenu->addAction(d->caseSensitiveAction);
    d->lineEditMenu->addAction(d->wholeWordAction);
    d->lineEditMenu->addAction(d->regularExpressionAction);

    addAction(d->findPrevAction);
    addAction(d->findNextAction);
    addAction(d->closeAction);
}

void FindToolBar::setupUi()
{
    setObjectName(QLatin1String("FindToolBar"));

    d->layout = new QGridLayout(this);
    d->layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    setLayout(d->layout);
    d->layout->setContentsMargins(3, 3, 3, 3);
    d->layout->setSpacing(2);
    d->layout->setColumnStretch(0, 2);
    d->layout->setColumnStretch(5, 1);

    d->findLineEdit = new FilterLineEdit(this);
    d->findLineEdit->setObjectName(QLatin1String("findLineEdit"));
    d->findLineEdit->setPlaceholderText(QString());
    d->findLineEdit->setButtonMenu(FancyLineEdit::Left, d->lineEditMenu);
    d->findLineEdit->setButtonVisible(FancyLineEdit::Left, true);
    d->findLineEdit->setButtonPixmap(FancyLineEdit::Left, QPixmap(":/icons/magnifier.png"));
    d->findLineEdit->setStyleSheet(QLatin1String("QLineEdit { border : 1px solid grey; border-radius : 10px; }"));
    int minimumHeight = d->findLineEdit->minimumSizeHint().height();

    d->findPrevButton = new QToolButton(this);
    d->findPrevButton->setObjectName(QLatin1String("findPrevButton"));
    d->findPrevButton->setDefaultAction(d->findPrevAction);
    d->findPrevButton->setFocusPolicy(Qt::NoFocus);
    d->findPrevButton->setArrowType(Qt::LeftArrow);
    d->findPrevButton->setMaximumHeight(minimumHeight);
//    d->findPrevButton->setText("some text");
//    d->findPrevButton->setStyleSheet("background-color:rgba(0,0,0,0);");

    d->findNextButton = new QToolButton(this);
    d->findNextButton->setObjectName(QLatin1String("findNextButton"));
    d->findNextButton->setDefaultAction(d->findNextAction);
    d->findNextButton->setFocusPolicy(Qt::NoFocus);
    d->findNextButton->setArrowType(Qt::RightArrow);
    d->findNextButton->setMaximumHeight(minimumHeight);

    QHBoxLayout *findButtonLayout = new QHBoxLayout;
    findButtonLayout->setContentsMargins(0,0,0,0);
    findButtonLayout->setSpacing(1);
    findButtonLayout->addWidget(d->findPrevButton);
    findButtonLayout->addWidget(d->findNextButton);
//    d->findNextButton->setStyleSheet("background-color:rgba(0,0,0,0);");

    d->findSpacer = new QSpacerItem(50, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);

    d->closeButton = new QToolButton(this);
    d->closeButton->setObjectName(QLatin1String("closeButton"));
    d->closeButton->setDefaultAction(d->closeAction);
//    d->closeButton->setStyleSheet("background-color:rgba(0,0,0,0);");
    d->closeButton->setIcon(QIcon(QLatin1String(":/icons/cancelfind.png")));
    d->closeButton->setMaximumHeight(minimumHeight);

    d->replaceLineEdit = new FilterLineEdit(this);
    d->replaceLineEdit->setObjectName(QLatin1String("replaceLineEdit"));
    d->replaceLineEdit->setPlaceholderText(QString());
    d->replaceLineEdit->setButtonVisible(FancyLineEdit::Left, true);
    d->replaceLineEdit->setStyleSheet(QLatin1String("QLineEdit { border : 1px solid grey; border-radius : 10px; }"));

    d->replaceButton = new QToolButton(this);
    d->replaceButton->setObjectName(QLatin1String("replaceButton"));
    d->replaceButton->setFocusPolicy(Qt::NoFocus);

    d->replaceNextButton = new QToolButton(this);
    d->replaceNextButton->setObjectName(QLatin1String("replaceButton"));
    d->replaceNextButton->setFocusPolicy(Qt::NoFocus);

    d->replaceAllButton = new QToolButton(this);
    d->replaceAllButton->setObjectName(QLatin1String("replaceAllButton"));
    d->replaceAllButton->setFocusPolicy(Qt::NoFocus);

    d->replaceSpacer = new QSpacerItem(100, 10, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    d->layout->addWidget(d->findLineEdit, 0, 0);
    d->layout->addLayout(findButtonLayout, 0, 1);
//    d->layout->addWidget(d->findPrevButton, 0, 1, Qt::AlignRight);
//    d->layout->addWidget(d->findNextButton, 0, 2, Qt::AlignLeft);
    d->layout->addItem(d->findSpacer, 0, 5);
    d->layout->addWidget(d->closeButton, 0, 6);

    d->layout->addWidget(d->replaceLineEdit, 1, 0);
    d->layout->addWidget(d->replaceButton, 1, 1, 1, 2);
    d->layout->addWidget(d->replaceNextButton, 1, 3);
    d->layout->addWidget(d->replaceAllButton, 1, 4);
}

void FindToolBar::retranslateUi()
{
    d->caseSensitiveAction->setText(tr("Case Sensitive"));
    d->wholeWordAction->setText(tr("Whole Words Only"));
    d->regularExpressionAction->setText(tr("Use Regular Expressions"));

    d->findLineEdit->setPlaceholderText(tr("Find"));
    d->findPrevButton->setToolTip(tr("Find previous"));
    d->findNextButton->setToolTip(tr("Find next"));

    d->replaceLineEdit->setPlaceholderText(tr("Replace"));
    d->replaceButton->setText(tr("Replace"));
    d->replaceButton->setToolTip(tr("Replace"));
    d->replaceNextButton->setText(tr("Replace next"));
    d->replaceNextButton->setToolTip(tr("Replace next"));

    d->replaceAllButton->setText(tr("Replace all"));
    d->replaceAllButton->setToolTip(tr("Replace all"));
}

void FindToolBar::setupConnections()
{
    connect(d->findLineEdit, SIGNAL(filterChanged(QString)), SLOT(onFilterChanged()));

    connect(d->findPrevAction, SIGNAL(triggered()), SLOT(findPrev()));
    connect(d->findNextAction, SIGNAL(triggered()), SLOT(findNext()));

    connect(d->closeAction, SIGNAL(triggered()), SLOT(close()));
}

void FindToolBar::updateUi()
{
    bool visible = d->find->supportsReplace();

    d->replaceLineEdit->setVisible(visible);
    d->replaceButton->setVisible(visible);
    d->replaceNextButton->setVisible(visible);
    d->replaceAllButton->setVisible(visible);

    IFind::FindFlags flags = 0;
    if (d->find)
        flags = d->find->supportedFindFlags();

    d->caseSensitiveAction->setVisible(flags & IFind::FindCaseSensitively);
    d->wholeWordAction->setVisible(flags & IFind::FindWholeWords);
    d->regularExpressionAction->setVisible(flags & IFind::FindRegularExpression);
}
