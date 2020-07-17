#pragma once

#include <stdexcept>
#include <string>

namespace model { class CategoryOwnerNotExists; }

class model::CategoryOwnerNotExists : public std::logic_error
{
public:
   explicit CategoryOwnerNotExists(std::string const& owner);
   std::string owner() const;

private:
   std::string m_owner;
};
