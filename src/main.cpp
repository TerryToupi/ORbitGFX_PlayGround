#include <renderGraph/renderGraph.hpp>

Meshes meshes;

int main()
{
    gfx::Renderer::INIT();

    renderGraph::init();
    gfx::Window::instance->Run([]() { renderGraph::render(meshes); });
    renderGraph::destroy();

    gfx::Renderer::SHUTDOWN();

    return 0;
}