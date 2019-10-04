#include <CherrySoda/Utility/String.h>

#include <cstdarg>

using cherrysoda::String;

const String FormatString(const char* format, ...)
{
	char buffer[256];
	va_list args;
	va_start(args, format);
	std::vsprintf(buffer, format, args);
	va_end(args);
	return String(buffer);
}