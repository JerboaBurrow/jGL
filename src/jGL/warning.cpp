#include <jGL/warning.h>

void TODO(std::string context)
{
#ifndef NO_WARN
    Log log;

    WARN("Function is unimplemented: "+context) >> log;
#endif
}
