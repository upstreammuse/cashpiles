#include "categorynotexists.h"

using namespace model;

using std::string;

CategoryNotExists::CategoryNotExists(string const& category) :
   logic_error("Budget category '" + category + "' does not exist"),
   m_category(category)
{
}

string CategoryNotExists::category() const
{
   return m_category;
}
