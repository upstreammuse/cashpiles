#include "daoaccounts.h"

#include <cassert>
#include <cstring>
#include <sstream>
#include "sqlite3.h"

using std::runtime_error;
using std::string;
using std::stringstream;
using util::Date;

DaoAccounts::DaoAccounts(sqlite3* db) :
   DataAccessObject {db}
{
   auto sql = "CREATE TABLE IF NOT EXISTS Accounts ("
              "Name TEXT NOT NULL,"
              "OnBudget INT NOT NULL,"
              "ClosedDate DATE,"
              "ClosedLine INT REFERENCES Comments(LineNumber),"
              "OpenedDate DATE NOT NULL,"
              "OpenedLine INT NOT NULL REFERENCES Comments(LineNumber),"
              "PRIMARY KEY (Name))";
   exec(sql);
}

void DaoAccounts::createAccount(
      Date const& date, bool onBudget, std::string const& name,
      size_t commentLine)
{
   auto sql = "INSERT INTO Accounts VALUES ("
              "$name, $onBudget, NULL, NULL, $date, $commentLine)";

   sqlite3_stmt* stmt;
   auto rc = sqlite3_prepare_v2(DB, sql, -1, &stmt, nullptr);
   if (rc != SQLITE_OK)
   {
      sqlite3_finalize(stmt);
      throw runtime_error("Unable to compile SQL");
   }

   rc = sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
   if (rc != SQLITE_OK)
   {
      sqlite3_finalize(stmt);
      throw runtime_error("Unable to bind SQL account name");
   }

   rc = sqlite3_bind_int(stmt, 2, onBudget ? 1 : 0);
   if (rc != SQLITE_OK)
   {
      sqlite3_finalize(stmt);
      throw runtime_error("Unable to bind SQL account on budget");
   }



   // TODO Date

   rc = sqlite3_bind_int(stmt, 4, commentLine);
   if (rc != SQLITE_OK)
   {
      sqlite3_finalize(stmt);
      throw runtime_error("Unable to bind SQL account comment line");
   }

   while (sqlite3_step(stmt) == SQLITE_ROW)
   {
   }
   sqlite3_finalize(stmt);
}

void DaoAccounts::openAccount(string const& name)
{
   auto sql = "UPDATE Accounts"
              "SET ClosedDate = NULL, ClosedLine = NULL"
              "WHERE AccountName = $name";

   sqlite3_stmt* stmt;
   auto rc = sqlite3_prepare_v2(DB, sql, -1, &stmt, nullptr);
   if (rc != SQLITE_OK)
   {
      sqlite3_finalize(stmt);
      throw runtime_error("Unable to compile SQL");
   }

   rc = sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
   if (rc != SQLITE_OK)
   {
      sqlite3_finalize(stmt);
      throw runtime_error("Unable to bind SQL account name");
   }

   while (sqlite3_step(stmt) == SQLITE_ROW)
   {
   }
   sqlite3_finalize(stmt);
}

void DaoAccounts::closeAccount(
      Date const& date, string const& name, size_t commentLine)
{
   auto sql = "UPDATE Accounts"
              "SET ClosedDate = date($date), ClosedLine = $lineNumber"
              "WHERE AccountName = $name";

   sqlite3_stmt* stmt;
   auto rc = sqlite3_prepare_v2(DB, sql, -1, &stmt, nullptr);
   if (rc != SQLITE_OK)
   {
      sqlite3_finalize(stmt);
      throw runtime_error("Unable to compile SQL");
   }

   rc = sqlite3_bind_text(stmt, 1, date.toString("yyyy-MM-dd").c_str());
   if (rc != SQLITE_OK)
   {
      sqlite3_finalize(stmt);
      throw runtime_error("Unable to bind SQL account close date");
   }

   rc = sqlite3_bind_int(stmt, 2, commentLine);
   if (rc != SQLITE_OK)
   {
      sqlite3_finalize(stmt);
      throw runtime_error("Unable to bind SQL account close line");
   }

   rc = sqlite3_bind_text(stmt, 3, name);
   if (rc != SQLITE_OK)
   {
      sqlite3_finalize(stmt);
      throw runtime_error("Unable to bind SQL account name");
   }

   while (sqlite3_step(stmt) == SQLITE_ROW)
   {
   }
   sqlite3_finalize(stmt);
}
