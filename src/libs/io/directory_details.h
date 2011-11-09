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

#include <QtCore/QThread>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>

#include "io_global.h"

class IO_EXPORT DirectoryDetails : public QThread
{
    QString m_dirPath;
    qint64 m_totalSize;
    int m_totalFiles;
    int m_totalFolders;
    volatile bool m_run;
    
    public:
        explicit DirectoryDetails(const QString&, QObject *parent=0);
        DirectoryDetails(QObject *parent=0);
        void init();
        void run();
        int totalFiles() const { return m_totalFiles; }
        int totalFolders() const { return m_totalFolders; }
        qint64 totalSize() const { return m_totalSize; }
        void stopProcessing() { m_run = false; }

};

#endif
