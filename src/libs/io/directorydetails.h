/*
 *  Copyright (c) 2010, 2011 Carlos Pais <freemind@lavabit.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DIRECTORY_DETAILS_H
#define DIRECTORY_DETAILS_H

#include "io_global.h"

#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QThread>

class IO_EXPORT DirectoryDetails : public QThread
{
public:
    explicit DirectoryDetails(const QString &, QObject *parent = 0);
    explicit DirectoryDetails(QObject *parent = 0);

    int totalFolders() const { return m_totalFolders; }
    int totalFiles() const { return m_totalFiles; }
    qint64 totalSize() const { return m_totalSize; }

    void stopProcessing() { m_stopRequest = true; }

protected:
    void run();

private:
    QString m_dirPath;
    int m_totalFolders;
    int m_totalFiles;
    qint64 m_totalSize;
    volatile bool m_stopRequest;
};

#endif
