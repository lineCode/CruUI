#pragma once

#include "Base.h"

#include <memory>

namespace cru {
    namespace graph {
        class GraphManager;

        //Represents a window render target.
        class WindowRenderTarget : public Object {
        public:
            WindowRenderTarget(GraphManager* graph_manager, HWND hwnd);
            ~WindowRenderTarget() override;
            NO_COPY_MOVE(WindowRenderTarget)

        public:
            //Get the graph manager that created the render target.
            GraphManager* GetGraphManager();

            //Get the target bitmap which can be set as the ID2D1DeviceContext's target.
            ID2D1Bitmap1* GetTargetBitmap();

            //Resize the underlying buffer.
            void ResizeBuffer();

            //Present the data of the underlying buffer to the window.
            void Present();

        private:
            void CreateTargetBitmap();

        private:
            GraphManager* graph_manager_;
            Microsoft::WRL::ComPtr<IDXGISwapChain1> dxgi_swap_chain_;
            Microsoft::WRL::ComPtr<ID2D1Bitmap1> target_bitmap_;
        };

        struct Dpi {
            float x;
            float y;
        };

        class GraphManager : public Object {
        public:
            GraphManager();
            ~GraphManager() override;
            NO_COPY_MOVE(GraphManager)

        public:
            ID2D1Factory1* GetD2D1Factory();
            ID2D1DeviceContext* GetD2D1DeviceContext();
            ID3D11Device* GetD3D11Device();
            IDXGIFactory2* GetDXGIFactory();

            //Create a window render target with the HWND.
            std::shared_ptr<WindowRenderTarget> CreateWindowRenderTarget(HWND hwnd);

            //Get the desktop dpi.
            Dpi GetDpi();

            //Reload system metrics including desktop dpi.
            void ReloadSystemMetrics();

        private:
            Microsoft::WRL::ComPtr<ID3D11Device> d3d11_device_;
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3d11_device_context_;
            Microsoft::WRL::ComPtr<ID2D1Factory1> d2d1_factory_;
            Microsoft::WRL::ComPtr<ID2D1Device> d2d1_device_;
            Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2d1_device_context_;
            Microsoft::WRL::ComPtr<IDXGIFactory2> dxgi_factory_;
            Microsoft::WRL::ComPtr<IDWriteFactory> dwrite_factory_;
        };

        int DipToPixelX(float dipX);
        int DipToPixelY(float dipY);
        float PixelToDipX(int pixelX);
        float PixelToDipY(int pixelY);
    }
}
