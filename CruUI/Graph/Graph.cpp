#include "Graph.h"
#include "Application.h"
#include "Exception.h"

namespace cru {
    namespace graph {
        using Microsoft::WRL::ComPtr;

        WindowRenderTarget::WindowRenderTarget(GraphManager* graph_manager, HWND hwnd)
		{
            this->graph_manager_ = graph_manager;

            auto d3d11_device = graph_manager->GetD3D11Device();
            auto dxgi_factory = graph_manager->GetDXGIFactory();
            auto d2d1_device_context = graph_manager->GetD2D1DeviceContext();

            // Allocate a descriptor.
            DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = { 0 };
            swap_chain_desc.Width = 0;                           // use automatic sizing
            swap_chain_desc.Height = 0;
            swap_chain_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // this is the most common swapchain format
            swap_chain_desc.Stereo = false;
            swap_chain_desc.SampleDesc.Count = 1;                // don't use multi-sampling
            swap_chain_desc.SampleDesc.Quality = 0;
            swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swap_chain_desc.BufferCount = 2;                     // use double buffering to enable flip
            swap_chain_desc.Scaling = DXGI_SCALING_NONE;
            swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // all apps must use this SwapEffect
            swap_chain_desc.Flags = 0;



            // Get the final swap chain for this window from the DXGI factory.
            ThrowIfFailed(
                dxgi_factory->CreateSwapChainForHwnd(
                    d3d11_device,
                    hwnd,
                    &swap_chain_desc,
                    nullptr,
                    nullptr,
                    &dxgi_swap_chain_
                )
            );

            CreateTargetBitmap();
        }

        WindowRenderTarget::~WindowRenderTarget()
		{

        }

        GraphManager* WindowRenderTarget::GetGraphManager()
		{
            return graph_manager_;
        }

        void WindowRenderTarget::ResizeBuffer(int width, int height)
		{
            auto graph_manager = graph_manager_;
            auto d3d11_device = graph_manager->GetD3D11Device();
            auto dxgi_factory = graph_manager->GetDXGIFactory();
            auto d2d1_device_context = graph_manager->GetD2D1DeviceContext();

            ComPtr<ID2D1Image> old_target;
            d2d1_device_context->GetTarget(&old_target);
            bool target_this = old_target == this->target_bitmap_;
            if (target_this)
                d2d1_device_context->SetTarget(nullptr);

            old_target = nullptr;
            target_bitmap_ = nullptr;

            ThrowIfFailed(
                dxgi_swap_chain_->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0)
            );

            CreateTargetBitmap();

            if (target_this)
                d2d1_device_context->SetTarget(target_bitmap_.Get());
        }

        void WindowRenderTarget::SetAsTarget()
		{
            graph_manager_->SetTarget(this);
        }

        void WindowRenderTarget::Present()
		{
            ThrowIfFailed(
                dxgi_swap_chain_->Present(1, 0)
            );
        }

        void WindowRenderTarget::CreateTargetBitmap()
		{
            // Direct2D needs the dxgi version of the backbuffer surface pointer.
            ComPtr<IDXGISurface> dxgiBackBuffer;
            ThrowIfFailed(
                dxgi_swap_chain_->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer))
            );

            auto dpi = graph_manager_->GetDpi();

            auto bitmap_properties =
                D2D1::BitmapProperties1(
                    D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
                    D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
                    dpi.x,
                    dpi.y
                );

            // Get a D2D surface from the DXGI back buffer to use as the D2D render target.
            ThrowIfFailed(
                graph_manager_->GetD2D1DeviceContext()->CreateBitmapFromDxgiSurface(
                    dxgiBackBuffer.Get(),
                    &bitmap_properties,
                    &target_bitmap_
                )
            );

        }

        ID2D1DeviceContext * WindowRenderTarget::GetD2DDeviceContext()
		{
            return graph_manager_->GetD2D1DeviceContext();
        }

        ID2D1Bitmap1 * WindowRenderTarget::GetTargetBitmap()
		{
            return this->target_bitmap_.Get();
        }

        GraphManager::GraphManager()
		{
            UINT creation_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef _DEBUG
            creation_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

            D3D_FEATURE_LEVEL feature_levels[] =
            {
                D3D_FEATURE_LEVEL_11_1,
                D3D_FEATURE_LEVEL_11_0,
                D3D_FEATURE_LEVEL_10_1,
                D3D_FEATURE_LEVEL_10_0,
                D3D_FEATURE_LEVEL_9_3,
                D3D_FEATURE_LEVEL_9_2,
                D3D_FEATURE_LEVEL_9_1
            };


            ThrowIfFailed(D3D11CreateDevice(
                nullptr,
                D3D_DRIVER_TYPE_HARDWARE,
                nullptr,
                creation_flags,
                feature_levels,
                ARRAYSIZE(feature_levels),
                D3D11_SDK_VERSION,
                &d3d11_device_,
                nullptr,
                &d3d11_device_context_
            ));

            Microsoft::WRL::ComPtr<IDXGIDevice> dxgi_device;

            ThrowIfFailed(d3d11_device_.As(&dxgi_device));

            ThrowIfFailed(D2D1CreateFactory(
                D2D1_FACTORY_TYPE_SINGLE_THREADED,
                __uuidof(ID2D1Factory1),
                &d2d1_factory_
            ));

            ThrowIfFailed(d2d1_factory_->CreateDevice(dxgi_device.Get(), &d2d1_device_));

            ThrowIfFailed(d2d1_device_->CreateDeviceContext(
                D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
                &d2d1_device_context_
            ));

            // Identify the physical adapter (GPU or card) this device is runs on.
            ComPtr<IDXGIAdapter> dxgi_adapter;
            ThrowIfFailed(
                dxgi_device->GetAdapter(&dxgi_adapter)
            );

            // Get the factory object that created the DXGI device.
            ThrowIfFailed(
                dxgi_adapter->GetParent(IID_PPV_ARGS(&dxgi_factory_))
            );
        }

        GraphManager::~GraphManager()
		{

        }

        ID2D1Factory1* GraphManager::GetD2D1Factory()
		{
            return d2d1_factory_.Get();
        }

        ID2D1DeviceContext* GraphManager::GetD2D1DeviceContext()
		{
            return d2d1_device_context_.Get();
        }

        ID3D11Device * GraphManager::GetD3D11Device()
		{
            return d3d11_device_.Get();
        }

        IDXGIFactory2 * GraphManager::GetDXGIFactory()
		{
            return dxgi_factory_.Get();
        }

        std::shared_ptr<WindowRenderTarget> GraphManager::CreateWindowRenderTarget(HWND hwnd)
		{
            return std::make_shared<WindowRenderTarget>(this, hwnd);
        }

        void GraphManager::SetTarget(WindowRenderTarget * target)
		{
            d2d1_device_context_->SetTarget(target->GetTargetBitmap());
        }

        Dpi GraphManager::GetDpi()
		{
            Dpi dpi;
            d2d1_factory_->GetDesktopDpi(&dpi.x, &dpi.y);
            return dpi;
        }

        void GraphManager::ReloadSystemMetrics()
		{
            ThrowIfFailed(
                d2d1_factory_->ReloadSystemMetrics()
            );
        }

        inline int DipToPixelInternal(float dip, float dpi)
		{
            return static_cast<int>(dip * dpi / 96.0f);
        }

        int DipToPixelX(float dipX)
		{
            return DipToPixelInternal(dipX, Application::GetInstance()->GetGraphManager()->GetDpi().x);
        }

        int DipToPixelY(float dipY)
		{
            return DipToPixelInternal(dipY, Application::GetInstance()->GetGraphManager()->GetDpi().y);
        }

        inline float DipToPixelInternal(int pixel, float dpi)
		{
            return static_cast<float>(pixel) * 96.0f / dpi;
        }

        float PixelToDipX(int pixelX)
		{
            return DipToPixelInternal(pixelX, Application::GetInstance()->GetGraphManager()->GetDpi().x);
        }

        float PixelToDipY(int pixelY)
		{
            return DipToPixelInternal(pixelY, Application::GetInstance()->GetGraphManager()->GetDpi().y);
        }
    }
}
