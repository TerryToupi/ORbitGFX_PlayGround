#include <renderGraph/renderGraph.hpp>
#include <loaders/meshLoader.hpp>

#include <iostream>
#include <window.hpp>

Meshes meshes;
EditorCamera cam;

void mainLoop()
{ 
	double x, y;
	gfx::Window::instance->GetMousePos(&x, &y);
	
	if (gfx::Window::instance->GetMouseButton(Button::BUTTON_RIGHT))
		cam.processMouseMovement(x, y, true);
	else
		cam.processMouseMovement(x, y, false);

	if (gfx::Window::instance->GetKey(Key::W))
		cam.processKeyboard(true, false, false, false, 0.005);
	if (gfx::Window::instance->GetKey(Key::S))
		cam.processKeyboard(false, true, false, false, 0.005);
	if (gfx::Window::instance->GetKey(Key::A))
		cam.processKeyboard(false, false, true, false, 0.005);
	if (gfx::Window::instance->GetKey(Key::D))
		cam.processKeyboard(false, false, false, true, 0.005);

    renderGraph::render(meshes, cam);
}

int main()
{
    gfx::Renderer::INIT();

    loadMesh("/meshes/dragon.obj", meshes, Vec3(0.0, 0.0, 0.0), Vec3(0.0, 0.0, 0.0), Vec3(1.0, 1.0, 1.0));

    renderGraph::init();
    cam = EditorCamera(Vec3(0.0f, 0.0f, 0.0f));
    gfx::Window::instance->Run(mainLoop);
    renderGraph::destroy();

    gfx::Renderer::SHUTDOWN();

    return 0;
}