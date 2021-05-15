// My header
#include "u_Qstrings.h"

void str::append(QString& x, std::string_view s)
{
    x.append(QByteArray::fromRawData(s.data(), s.size()));
}
