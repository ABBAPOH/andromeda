#include "openwithmenu.h"

#include <QtCore/QSet>
#include <QtCore/QSettings>
#include <QtCore/QProcess>

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

#include <IO/QDriveInfo>
#include <IO/QMimeDatabase>

OpenWithMenu::OpenWithMenu(QWidget *parent) :
    QMenu(parent)
{
    setTitle(tr("Open with"));

    connect(this, SIGNAL(triggered(QAction*)), SLOT(onTriggered(QAction*)));
}

QList<QUrl> OpenWithMenu::urls() const
{
    return m_urls;
}

void OpenWithMenu::setUrls(const QList<QUrl> &urls)
{
    if (m_urls == urls)
        return;

    m_urls = urls;

    clear();

    QDefaultProgram defaultProgram;
    QList<QDefaultProgram> defaultPrograms = QDefaultProgram::defaultPrograms(urls);
    m_programs = defaultPrograms;

    if (!defaultPrograms.isEmpty())
        defaultProgram = defaultPrograms.takeFirst();

    if (defaultProgram.isValid()) {
        QAction *act = addAction(defaultProgram.icon(), tr("%1 (default)").arg(defaultProgram.name()));
        act->setData(0);
        addSeparator();
    }

    auto lessThan = [](const QDefaultProgram &a, const QDefaultProgram &b) {
        return a.name() < b.name();
    };
    qSort(defaultPrograms.begin(), defaultPrograms.end(), lessThan);

    for (int i = 0; i < defaultPrograms.count(); ++i) {
        const QDefaultProgram &program = defaultPrograms.at(i);
        QAction *act = addAction(program.icon(), program.name());
        act->setData(i + 1);
    }

    addSeparator();
    QAction *act = addAction(tr("Select program..."));
    connect(act, SIGNAL(triggered()), SLOT(selectProgram()));

    emit urlsChanged(urls);
}

void OpenWithMenu::setPaths(const QStringList &paths)
{
    QList<QUrl> urls;
    foreach (const QString &path, paths) {
        urls.append(QUrl::fromLocalFile(path));
    }
    setUrls(urls);
}

void OpenWithMenu::onTriggered(QAction *action)
{
    const QDefaultProgram &program = m_programs.at(action->data().toInt());

    program.openUrls(m_urls);
}

void OpenWithMenu::selectProgram()
{
    QSettings settings;
    QString programsFolder;
    QVariant value = settings.value(QLatin1String("filemanager/programsFolder"));
    if (value.isValid()) {
        programsFolder = value.toString();
    } else {
#if defined(Q_OS_MAC)
        programsFolder = QLatin1String("/Applications");
#elif defined(Q_OS_WIN)
        programsFolder = QDriveInfo::rootDrive().rootPath() + QLatin1String("/Program Files");
#elif defined(Q_OS_UNIX)
        programsFolder = QLatin1String("/usr/bin");
#endif
    }

#ifdef Q_OS_WIN
    QString filter = tr("Programs (*.exe *.cmd *.com *.bat);;All files (*)");
#else
    QString filter;
#endif

    QString programPath = QFileDialog::getOpenFileName(this, tr("Select program"), programsFolder, filter);
    if (programPath.isEmpty())
        return;

    settings.setValue(QLatin1String("filemanager/programsFolder"), QFileInfo(programPath).absolutePath());

    bool result = true;
    QStringList failedPaths;
    foreach (const QUrl &url, m_urls) {
        QString path;
        if (url.isLocalFile())
            path = url.toLocalFile();
        else
            path = url.toString();

        QString program;
        QStringList arguments;
#if defined(Q_OS_MAC)
        program = QLatin1String("open");
        arguments << QLatin1String("-a") << programPath << path;
#else
        program = programPath;
        arguments << path;
#endif
        bool r = QProcess::startDetached(program, arguments);
        if (!r)
            failedPaths.append(path);
        result &= r;
    }

    if (!result) {
        QMessageBox::warning(this,
                             tr("Can't open files"),
                             tr("Andromeda failed to open some files: %1").
                             arg(failedPaths.join(QLatin1String("\n"))));
    }
}
