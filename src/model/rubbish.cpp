#include "rubbish.h"

using namespace model;

using std::string;

Rubbish::Rubbish(string const& message) :
   logic_error(message)
{
}
