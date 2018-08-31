#pragma once

#include "system_headers.h"
#include <memory>

#include "base.h"


namespace cru
{
    namespace graph
	{
        class GraphManager;

        //Represents a window render target.
        class WindowRenderTarget : public Object
		{
        public:
            WindowRenderTarget(GraphManager* graph_manager, HWND hwnd);
            WindowRenderTarget(const WindowRenderTarget& other) = delete;
            WindowRenderTarget(WindowRenderTarget&& other) = delete;
            WindowRenderTarget& operator=(const WindowRenderTarget& other) = delete;
            WindowRenderTarget& operator=(WindowRenderTarget&& other) = delete;
            ~WindowRenderTarget() override;

        public:
            //Get the graph manager that created the render target.
            GraphManager* GetGraphManager() const
            {
                return graph_manager_;
            }

            //Get the d2d device context.
            inline Microsoft::WRL::ComPtr<ID2D1DeviceContext> GetD2DDeviceContext() const;

            //Get the target bitmap which can be set as the ID2D1DeviceContext's target.
            Microsoft::WRL::ComPtr<ID2D1Bitmap1> GetTargetBitmap() const
            {
                return target_bitmap_;
            }

            //Resize the underlying buffer.
            void ResizeBuffer(int width, int height);

            //Set this render target as the d2d device context's target.
            void SetAsTarget();

            //Present the data of the underlying buffer to the window.
            void Present();

        private:
            void CreateTargetBitmap();

        private:
            GraphManager* graph_manager_;
            Microsoft::WRL::ComPtr<IDXGISwapChain1> dxgi_swap_chain_;
            Microsoft::WRL::ComPtr<ID2D1Bitmap1> target_bitmap_;
        };

        struct Dpi
		{
            float x;
            float y;
        };

        class GraphManager : public Object
		{
        public:
            GraphManager();
            GraphManager(const GraphManager& other) = delete;
            GraphManager(GraphManager&& other) = delete;
            GraphManager& operator=(const GraphManager& other) = delete;
            GraphManager& operator=(GraphManager&& other) = delete;
            ~GraphManager() override;

        public:
            Microsoft::WRL::ComPtr<ID2D1Factory1> GetD2D1Factory() const
            {
                return d2d1_factory_;
            }

            Microsoft::WRL::ComPtr<ID2D1DeviceContext> GetD2D1DeviceContext() const
            {
                return d2d1_device_context_;
            }

            Microsoft::WRL::ComPtr<ID3D11Device> GetD3D11Device() const
            {
                return d3d11_device_;
            }

            Microsoft::WRL::ComPtr<IDXGIFactory2> GetDxgiFactory() const
            {
                return dxgi_factory_;
            }

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

        int DipToPixelX(float dip_x);
        int DipToPixelY(float dip_y);
        float PixelToDipX(int pixel_x);
        float PixelToDipY(int pixel_y);

        Microsoft::WRL::ComPtr<ID2D1DeviceContext> WindowRenderTarget::GetD2DDeviceContext() const
        {
            return graph_manager_->GetD2D1DeviceContext();
        }
    }
}
