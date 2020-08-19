#include "dataaccessobject.h"

#include <stdexcept>
#include "sqlite3.h"

using std::runtime_error;
using std::string;

// TODO probably want to send this SQL debugging to a log of some kind
int DataAccessObject::genericCallback(
      void*, int argc, char** argv, char** colName)
{
   for (int i = 0; i < argc; ++i)
   {
      printf("%s = %s\n", colName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

DataAccessObject::DataAccessObject(sqlite3* db) :
   DB {db}
{
}

void DataAccessObject::exec(string const& sql)
{
   char* errorMsg = nullptr;
   auto rc = sqlite3_exec(DB, sql.c_str(), genericCallback, nullptr, &errorMsg);
   if (rc != SQLITE_OK)
   {
      runtime_error error(errorMsg);
      sqlite3_free(errorMsg);
      throw error;
   }
}
