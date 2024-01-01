#include <jGL/warning.h>

void TODO(std::string context)
{
#ifndef NO_WARN
    jLog::Log log;

    jLog::WARN("Function is unimplemented: "+context) >> log;
#endif
}
