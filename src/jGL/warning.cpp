#include <jGL/warning.h>

void TODO(std::string context)
{
    Log log;

    WARN("Function is unimplemented: "+context) >> log;
}
