#include "daocomments.h"

#include <cassert>
#include <cstring>
#include <sstream>
#include "sqlite3.h"

using std::runtime_error;
using std::string;
using std::stringstream;

DaoComments::DaoComments(sqlite3* db) :
   DataAccessObject {db}
{
   auto sql = "CREATE TABLE IF NOT EXISTS Comments ("
              "LineNumber INT NOT NULL,"
              "Comment TEXT,"
              "PRIMARY KEY (LineNumber))";
   exec(sql);
}

string DaoComments::getComment(size_t lineNumber)
{
   string retval;
   stringstream sql;
   sql << "SELECT Comment FROM Comments WHERE LineNumber = " << lineNumber;

   sqlite3_stmt* stmt;
   auto rc = sqlite3_prepare_v2(DB, sql.str().c_str(), -1, &stmt, nullptr);
   if (rc != SQLITE_OK)
   {
      sqlite3_finalize(stmt);
      throw runtime_error("Unable to compile SQL");
   }

   while (sqlite3_step(stmt) == SQLITE_ROW)
   {
      for (int i = 0; i < sqlite3_column_count(stmt); ++i)
      {
         auto comment = sqlite3_column_text(stmt, i);
         retval = comment ? reinterpret_cast<char const*>(comment) : "";
      }
   }
   sqlite3_finalize(stmt);
   return retval;
}

void DaoComments::setComment(size_t lineNumber, string const& comment)
{
   auto sql = "INSERT INTO Comments VALUES ($lineNumber, $comment)";

   sqlite3_stmt* stmt;
   auto rc = sqlite3_prepare_v2(DB, sql, -1, &stmt, nullptr);
   if (rc != SQLITE_OK)
   {
      sqlite3_finalize(stmt);
      throw runtime_error("Unable to compile SQL");
   }

   rc = sqlite3_bind_int(stmt, 1, lineNumber);
   if (rc != SQLITE_OK)
   {
      sqlite3_finalize(stmt);
      throw runtime_error("Unable to bind SQL int");
   }

   rc = sqlite3_bind_text(stmt, 2, comment.c_str(), -1, SQLITE_TRANSIENT);
   if (rc != SQLITE_OK)
   {
      fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(DB));
      sqlite3_finalize(stmt);
      throw runtime_error("Unable to bind SQL text");
   }

   while (sqlite3_step(stmt) == SQLITE_ROW)
   {
   }
   sqlite3_finalize(stmt);
}
