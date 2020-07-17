#pragma once

#include <stdexcept>
#include <string>

namespace model { class CategoryNotExists; }

class model::CategoryNotExists : public std::logic_error
{
public:
   explicit CategoryNotExists(std::string const& category);
   std::string category() const;

private:
   std::string m_category;
};
