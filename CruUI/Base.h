#pragma once

//Include system headers.
#include <Windows.h>
#pragma comment(lib, "D3D11.lib")
#include <d3d11.h>

#pragma comment(lib, "D2d1.lib")
#include <d2d1_1.h>

#pragma comment(lib, "DWrite.lib")
#include <dwrite.h>

#include <dxgi1_2.h>
#include <wrl/client.h>



#include <string>
#include <stdexcept>

#define NO_COPY(className) \
className(const className&) = delete; \
className& operator = (const className&) = delete;

#define NO_MOVE(className) \
className(className&&) = delete; \
className& operator = (className&&) = delete;

#define NO_COPY_MOVE(className) NO_COPY(className) NO_MOVE(className)

namespace cru {
    using String = std::wstring;

    class Object {
    public:
        Object() = default;
        Object(const Object&) = default;
        Object& operator = (const Object&) = default;
        Object(Object&&) = default;
        Object& operator = (Object&&) = default;
        virtual ~Object();
    };

    struct Interface {
        virtual ~Interface();
    };

    inline void ThrowIfFailed(HRESULT h_result) {
        if (FAILED(h_result))
            throw std::runtime_error("A HRESULT test failed.");
    }

    inline void ThrowIfFailed(HRESULT h_result, const std::string& message) {
        if (FAILED(h_result))
            throw std::runtime_error(message);
    }
}
