#include "qdefaultprograms.h"

#include "qmimedatabase.h"

QDefaultPrograms::QDefaultPrograms()
{
}

/*!
  \fn QProgramInfo QDefaultPrograms::progamInfo(const QString &application);

  \brief Returns information for a given \a application
*/

/*!
  \fn QString QDefaultPrograms::defaultProgram(const QString &mimeType);

  \brief Returns id of the default program that opens files with the given \a mimeType.
*/

/*!
  \brief Returns id of the default program that is capable to open given \a url.
*/
QString QDefaultPrograms::defaultProgram(const QUrl &url)
{
    QMimeDatabase db;
    QMimeType mimeType = db.mimeTypeForUrl(url);

//    if (!mimeType.isValid())
//        return;

//    QMimeType mt = mimeType;
//    while (mt.isValid()) {
//        qDebug() << mt.name();
//        qDebug() << QDefaultPrograms::defaultProgram(mt.name());
//        if (mt.parentMimeTypes().isEmpty())
//            break;
//        mt = db.mimeTypeForName(mt.parentMimeTypes().first());
//    }

    return defaultProgram(mimeType.name());
}

/*!
  \fn bool QDefaultPrograms::setDefaultProgram(const QString &mimeType, const QString &program);

  \brief Sets path to default program to open files with the given \a mimeType.
*/

/*!
  \fn QStringList QDefaultPrograms::defaultPrograms(const QString &mimeType);

  \brief Returns list of programs capabale of opening files with the given \a mimeType.
*/

/*!
  \fn QStringList QDefaultPrograms::defaultPrograms(const QUrl &url);

  \brief Returns list of programs capabale of opening given \a url.
*/

/*!
  \fn bool QDefaultPrograms::openFile(const QString &file, const QString &application);

  \brief Opens \a file in an \a application.

  Returns true if successful.
*/
