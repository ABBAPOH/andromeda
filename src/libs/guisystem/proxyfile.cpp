#include "proxyfile.h"

using namespace GuiSystem;

/*!
  \class ProxyFile

  \brief This class redirects method calls and signals to and from other IFile.
*/

/*!
  \brief Constructs a ProxyFile with the given \a parent and zero source IFile.
*/
ProxyFile::ProxyFile(QObject *parent) :
    IFile(parent),
    m_sourceFile(0)
{
}

/*!
  \brief Returns current source IFile or 0 if no IFile set.
*/
IFile * ProxyFile::sourceFile() const
{
    return m_sourceFile;
}

/*!
  \brief Sets source IFile to \a file.

  If you source file is not set, or set to 0, all methods will fallback to
  default values and default behavior.
*/
void ProxyFile::setSourceFile(IFile *file)
{
    if (m_sourceFile == file)
        return;

    if (m_sourceFile) {
        disconnect(m_sourceFile, 0, this, 0);
    }

    m_sourceFile = file;

    if (m_sourceFile) {
        connect(m_sourceFile, SIGNAL(modificationChanged(bool)), this, SIGNAL(modificationChanged(bool)));
        connect(m_sourceFile, SIGNAL(readOnlyChanged(bool)), this, SIGNAL(readOnlyChanged(bool)));
    }

    bool modified = file ? file->isModified() : false;
    bool readOnly = file ? file->isReadOnly() : true;

    emit modificationChanged(modified);
    emit readOnlyChanged(readOnly);
}

/*!
  \reimp
*/
bool ProxyFile::isModified() const
{
    if (m_sourceFile)
        return m_sourceFile->isModified();

    return IFile::isModified();
}

/*!
  \reimp
*/
bool ProxyFile::isReadOnly() const
{
    if (m_sourceFile)
        return m_sourceFile->isReadOnly();

    return IFile::isReadOnly();
}

/*!
  \reimp
*/
void ProxyFile::setModified(bool modified)
{
    if (m_sourceFile)
        m_sourceFile->setModified(modified);

    IFile::setModified(modified);
}

/*!
  \reimp
*/
void ProxyFile::setReadOnly(bool readOnly)
{
    if (m_sourceFile)
        m_sourceFile->setReadOnly(readOnly);

    IFile::setReadOnly(readOnly);
}

/*!
  \reimp
*/
void ProxyFile::save(const QUrl &url)
{
    if (m_sourceFile)
        m_sourceFile->save(url);

    IFile::save(url);
}
