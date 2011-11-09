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

#include "directory_details.h"
#include <QtCore/QtDebug>
#include <QtCore/QThread>

DirectoryDetails::DirectoryDetails(const QString& dirPath, QObject *parent):
    QThread(parent)
{
    init();
    m_dirPath = dirPath;
}

DirectoryDetails::DirectoryDetails(QObject *parent):
    QThread(parent)
{
    init();
    m_dirPath = QDir::currentPath();
}


void DirectoryDetails::init()
{
    m_totalFiles = 0;
    m_totalSize = 0;
    m_totalFolders = 0;
    m_run = true;
}


void DirectoryDetails::run()
{
    
    if ( m_dirPath.isEmpty() || (! QFile::exists(m_dirPath)) )
        return;
        
    QDirIterator it(m_dirPath, QDir::AllEntries | QDir::NoDotAndDotDot |
                                     QDir::Hidden, QDirIterator::Subdirectories);
                                     
    while (it.hasNext()) 
    {
        if ( ! m_run )
            return;
        
        QFileInfo info(it.next());
        if ( info.isDir() )
            m_totalFolders++;
        else
            m_totalFiles++;

        m_totalSize += info.size();
    }
}



