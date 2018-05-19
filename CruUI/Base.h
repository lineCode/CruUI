#pragma once

//include system headers
#include <Windows.h>
#pragma comment(lib, "D3D11.lib")
#include <d3d11.h>

#pragma comment(lib, "D2d1.lib")
#include <d2d1_1.h>

#pragma comment(lib, "DWrite.lib")
#include <dwrite.h>

#include <dxgi1_2.h>
#include <wrl/client.h>


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
		virtual ~Object();
	};

	struct Interface
	{
		virtual ~Interface();
	};

	enum class FlowControl
	{
		Continue,
		Break
	};
}
