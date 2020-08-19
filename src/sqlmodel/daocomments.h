#pragma once

#include "dataaccessobject.h"

class DaoComments : public DataAccessObject
{
public:
   DaoComments(sqlite3* db);
   std::string getComment(size_t lineNumber);
   void setComment(size_t lineNumber, std::string const& comment);
};
