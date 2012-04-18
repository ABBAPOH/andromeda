#include "qprograminfo.h"
#include "qprograminfo_p.h"

QProgramInfo::QProgramInfo() :
    d(new QProgramInfoData)
{
    d->valid = false;
}

QProgramInfo::QProgramInfo(const QProgramInfo &other) :
    d(other.d)
{
}

QProgramInfo &QProgramInfo::operator=(const QProgramInfo &other)
{
    if (this != &other)
        d.operator=(other.d);
    return *this;
}

QProgramInfo::QProgramInfo(const QProgramInfoData &data) :
    d(new QProgramInfoData(data))
{
    d->valid = true;
}

QProgramInfo::~QProgramInfo()
{
}

bool QProgramInfo::isValid() const
{
    return d->valid;
}

QString QProgramInfo::comment() const
{
    return d->comment;
}

QString QProgramInfo::copyright() const
{
    return d->copyright;
}

QString QProgramInfo::genericName() const
{
    return d->genericName;
}

QIcon QProgramInfo::icon() const
{
    return d->icon;
}

QString QProgramInfo::identifier() const
{
    return d->identifier;
}

QString QProgramInfo::name() const
{
    return d->name;
}

QString QProgramInfo::path() const
{
    return d->path;
}

QString QProgramInfo::version() const
{
    return d->version;
}

QDebug operator<<(QDebug s, const QProgramInfo &info)
{
    s << "QProgramInfo" << "(" <<
         "identifier =" << info.identifier() <<
         "name =" << info.name() <<
         "version =" << info.version() <<
         ")";
    return s;
}
