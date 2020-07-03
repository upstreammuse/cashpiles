#pragma once

#include <stdexcept>

namespace model { class Rubbish; }

class model::Rubbish : public std::logic_error
{
public:
   explicit Rubbish(std::string const& message);
};
