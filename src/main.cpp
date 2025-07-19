#include <renderGraph/renderGraph.hpp>
#include <loaders/meshLoader.hpp> 
#include <frameworks/imgui/imguiRenderer.hpp>

#include <iostream>
#include <window.hpp>

Meshes meshes;
EditorCamera cam;

utils::Handle<gfx::BindGroupLayout> material;

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

	gfx::ImguiRenderer::instance->Begin();
	bool showDemoWindow = true;
	ImGui::ShowDemoWindow(&showDemoWindow);
	gfx::ImguiRenderer::instance->End();

    renderGraph::render(meshes, cam);
}

int main()
{
    gfx::Renderer::INIT();

	material = gfx::ResourceManager::instance->Create(gfx::BindGroupLayoutDescriptor{
			.textureBindings = {
				{.slot = 0}
			},
			.samplerBindings = {
				{.slot = 1}
			}
		});

    loadMesh("meshes/sponza/scene.gltf", meshes, material, Vec3(0.0, 0.0, 0.0), Vec3(0.0, 0.0, 0.0), Vec3(0.05, 0.05, 0.05));

    renderGraph::init(material);
    cam = EditorCamera(Vec3(0.0f, 0.0f, 0.0f));
    gfx::Window::instance->Run(mainLoop);
    renderGraph::destroy();

    gfx::Renderer::SHUTDOWN();

    return 0;
}