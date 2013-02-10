#include "qdefaultprogram.h"
#include "qdefaultprogram_p.h"

#include "qmimedatabase.h"

void QDefaultProgramPrivate::removeDuplicates(QDefaultProgramList &list)
{
    QDefaultProgramList result;
    foreach (const QDefaultProgram &program, list) {
        if (!result.contains(program))
            result.append(program);
    }
    qSwap(result, list);
}

QDefaultProgram::QDefaultProgram() :
    d(new QDefaultProgramData)
{
    d->valid = false;
}

QDefaultProgram::QDefaultProgram(const QDefaultProgram &other) :
    d(other.d)
{
}

QDefaultProgram &QDefaultProgram::operator=(const QDefaultProgram &other)
{
    if (this != &other)
        d.operator=(other.d);
    return *this;
}

QDefaultProgram::QDefaultProgram(const QDefaultProgramData &data) :
    d(new QDefaultProgramData(data))
{
    d->valid = true;
}

QDefaultProgram::~QDefaultProgram()
{
}

bool QDefaultProgram::isValid() const
{
    return d->valid;
}

QString QDefaultProgram::comment() const
{
    return d->comment;
}

QString QDefaultProgram::copyright() const
{
    return d->copyright;
}

QString QDefaultProgram::genericName() const
{
    return d->genericName;
}

QIcon QDefaultProgram::icon() const
{
    return d->icon;
}

QString QDefaultProgram::identifier() const
{
    return d->identifier;
}

QString QDefaultProgram::name() const
{
    return d->name;
}

QString QDefaultProgram::path() const
{
    return d->path;
}

QString QDefaultProgram::version() const
{
    return d->version;
}

bool QDefaultProgram::operator ==(const QDefaultProgram &other) const
{
#ifdef Q_OS_WIN // stupid windows
    return d->path.compare(other.d->path, Qt::CaseInsensitive) == 0;
#else
    return d->path == other.d->path;
#endif
}

static uint qHash(const QDefaultProgram &program)
{
    return qHash(program.path());
}

QDefaultProgramList QDefaultProgram::defaultPrograms(const QList<QUrl> &urls)
{
    if (urls.isEmpty())
        return QList<QDefaultProgram>();

    QList<QDefaultProgram> programs = QDefaultProgram::defaultPrograms(urls.first());

    QSet<QDefaultProgram> programsSet = programs.toSet();


    for (int i = 1; i < urls.count(); ++i) {
        const QUrl &url = urls.at(i);

        QList<QDefaultProgram> newPrograms = QDefaultProgram::defaultPrograms(url);
        QSet<QDefaultProgram> newSet = newPrograms.toSet();
        programsSet.intersect(newSet);
    }

    QList<QDefaultProgram> result;
    for (int i = 0; i < programs.count(); ++i) {
        const QDefaultProgram &program = programs.at(i);
        if (programsSet.contains(program))
            result.append(program);
    }

    return result;
}

QDebug operator<<(QDebug s, const QDefaultProgram &info)
{
    s << "QDefaultProgram" << "(" <<
         "identifier =" << info.identifier() <<
         "name =" << info.name() <<
         "version =" << info.version() <<
         ")";
    return s;
}

/*!
  \fn QDefaultProgram QDefaultProgram::progamInfo(const QString &application);

  \brief Returns information for a given \a application
*/

/*!
  \fn QString QDefaultProgram::defaultProgram(const QString &mimeType);

  \brief Returns id of the default program that opens files with the given \a mimeType.
*/

/*!
  \brief Returns id of the default program that is capable to open given \a url.
*/
QDefaultProgram QDefaultProgram::defaultProgram(const QUrl &url)
{
    QDefaultProgramList programs = defaultPrograms(url);
    if (programs.isEmpty())
        return QDefaultProgram();
    return programs.first();

//    QMimeDatabase db;
//    QMimeType mimeType = db.mimeTypeForUrl(url);

////    if (!mimeType.isValid())
////        return;

////    QMimeType mt = mimeType;
////    while (mt.isValid()) {
////        qDebug() << mt.name();
////        qDebug() << QDefaultProgram::defaultProgram(mt.name());
////        if (mt.parentMimeTypes().isEmpty())
////            break;
////        mt = db.mimeTypeForName(mt.parentMimeTypes().first());
////    }

//    return defaultProgram(mimeType.name());
}

/*!
  \fn bool QDefaultProgram::setDefaultProgram(const QString &mimeType, const QString &program);

  \brief Sets path to default program to open files with the given \a mimeType.
*/

/*!
  \fn QStringList QDefaultProgram::defaultPrograms(const QString &mimeType);

  \brief Returns list of programs capabale of opening files with the given \a mimeType.
*/

/*!
  \fn QStringList QDefaultProgram::defaultPrograms(const QUrl &url);

  \brief Returns list of programs capabale of opening given \a url.
*/

/*!
  \fn bool QDefaultProgram::openFile(const QString &file, const QString &application);

  \brief Opens \a file in an \a application.

  Returns true if successful.
*/
