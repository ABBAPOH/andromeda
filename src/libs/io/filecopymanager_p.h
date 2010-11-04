#ifndef FILECOPYMANAGER_P_H
#define FILECOPYMANAGER_P_H

#include "filecopymanager.h"

namespace IO {

class FileCopyManagerPrivate
{
public:
    FileCopyManagerPrivate();
    int poolSize;
    QList<QtFileCopier *> pool;
};

}

#endif // FILECOPYMANAGER_P_H
