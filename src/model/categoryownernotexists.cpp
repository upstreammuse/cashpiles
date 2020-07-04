#include "categoryownernotexists.h"

using namespace model;

using std::string;

CategoryOwnerNotExists::CategoryOwnerNotExists(string const& owner) :
   logic_error("Budget category owner '" + owner + "' does not exist"),
   m_owner(owner)
{
}

string CategoryOwnerNotExists::owner() const
{
   return m_owner;
}
