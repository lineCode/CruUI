#pragma once

#include "global_macros.h"

#include <folly/String.h>

namespace cru
{
	using String = folly::basic_fbstring<wchar_t>;

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
