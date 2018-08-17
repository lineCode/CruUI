#pragma once

//include std headers
#include <string>
#include <stdexcept>

//handy macros for copy control
#define CRU_NO_COPY(className) \
className(const className&) = delete; \
className& operator = (const className&) = delete;

#define CRU_NO_MOVE(className) \
className(className&&) = delete; \
className& operator = (className&&) = delete;

#define CRU_NO_COPY_MOVE(className) CRU_NO_COPY(className) CRU_NO_MOVE(className)


//basic classes
namespace cru
{
	using String = std::wstring;

	class Object
	{
	public:
		Object() = default;
		Object(const Object&) = default;
		Object& operator = (const Object&) = default;
		Object(Object&&) = default;
		Object& operator = (Object&&) = default;
		virtual ~Object() = default;
	};

	struct Interface
	{
		virtual ~Interface() = default;
	};

	enum class FlowControl
	{
		Continue,
		Break
	};
}
