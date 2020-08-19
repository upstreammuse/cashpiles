#pragma once

#include "../util/m_date.h"
#include "dataaccessobject.h"

class DaoAccounts : public DataAccessObject
{
public:
   DaoAccounts(sqlite3* db);
   void createAccount(
         util::Date const& date, bool onBudget, std::string const& name,
         size_t commentLine);
   void openAccount(std::string const& name);
   void closeAccount(
         util::Date const& date, std::string const& name, size_t commentLine);
   bool isAccountOpen(std::string const& name, util::Date const& date);
};
