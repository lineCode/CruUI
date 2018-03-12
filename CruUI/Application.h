#pragma once

#include "Base.h"

#include <functional>
#include <memory>

namespace cru {
    namespace ui {
        class WindowManager;
    }

    namespace graph {
        class GraphManager;
    }

    class Application : public Object {
    public:
        static Application* GetInstance();
    private:
        static Application* theInstance;

    public:
        Application(HINSTANCE h_instance);
        ~Application() override;
        NO_COPY_MOVE(Application)

    public:
        int Run();
        void Quit(int quit_code);

        ui::WindowManager* GetWindowManager();
        graph::GraphManager* GetGraphManager();

        HINSTANCE GetInstanceHandle();

    private:
        HINSTANCE h_instance_;
        std::unique_ptr<ui::WindowManager> window_manager_;
        std::unique_ptr<graph::GraphManager> graph_manager_;
    };


    using InvokeLaterAction = std::function<void()>;
    void InvokeLater(const InvokeLaterAction& action);
}
