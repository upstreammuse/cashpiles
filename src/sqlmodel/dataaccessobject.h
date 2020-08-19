#pragma once

#include <string>

struct sqlite3;

class DataAccessObject
{
public:
   static int genericCallback(void*, int argc, char** argv, char** colName);

public:
   DataAccessObject(sqlite3* db);

protected:
   void exec(std::string const& sql);

protected:
   sqlite3* DB;
};
