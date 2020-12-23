#ifdef _WIN64
#include <Windows.h>
#include "WindowsPlatform.h"
#include "DirectXAPI.h"
#include "DirectXRenderer.h"
#include "DirectXRenderSystem.h"
#endif
#ifdef __EMSCRIPTEN__
#include "EmscriptenPlatform.h"
#include <emscripten/html5.h>
#endif
#include "OpenGLAPI.h"
#include "PortableGame.h"
#include "Mesh.h"
#include "GLRenderer.h"
#include "GLRenderSystem.h"
#include "Camera.h"
#include <entt.hpp>
#include <imgui.h>
#include <examples/imgui_impl_win32.h>
#include <examples/imgui_impl_dx11.h>
//#include <examples/imgui_impl_opengl3.h>
//#include <ImGuizmo.h>
#include "TransformSystem.h"
#include "CameraSystem.h"
#include "GizmoSystem.h"
#include "MeshLoaderSystem.h"
#include <typeinfo>
#include <map>
#include <random>
#include <json.hpp>
#include <iomanip>
#include "InspectorGUI.h"
//#include "SerializationSystem.h"
#include "misc_components.h"
// Thanks to https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
#include <filesystem>

using json = nlohmann::json;
class RandomColor
{
	bool foo;
public:
	RandomColor() {};
};
IPlatform* plat;
IGraphicsAPI* graph;
GameWindow* window;
entt::registry registry;

bool show_demo_window = true;

//template<class T>
//void TrySerializeComponent(json& master)
//{
//	std::string typeName = typeid(T).name();
//	//std::ofstream jsonFile = std::ofstream(typeName);
//	nlohmann::json json;
//	registry.each([&json](entt::entity e) {
//		if (registry.has<T>(e))
//		{
//			T& obj = registry.get<T>(e);
//			std::string stringified = std::to_string((int)e);
//			json[stringified] = obj;
//		}
//	});
//	//jsonFile << json << std::endl;
//	master[typeName] = json;
//}
//void Serialize(const char* saveFileName)
//{
//	json saveJson;
//	json entitiesJson;
//	std::vector<std::string> stringifiedEntities;
//	registry.each([&stringifiedEntities](entt::entity e)
//	{
//		stringifiedEntities.push_back(std::to_string((int)e));
//	});
//	entitiesJson = stringifiedEntities;
//	//std::ofstream entitiesFile("entities.json");
//	//entitiesFile << entitiesJson << std::endl;
//	saveJson["entities"] = entitiesJson;
//	TrySerializeComponent<Transform>(saveJson);
//	TrySerializeComponent<Mesh>(saveJson);
//	TrySerializeComponent<Camera>(saveJson);
//	TrySerializeComponent<Renderer>(saveJson);
//	TrySerializeComponent<Name>(saveJson);
//	TrySerializeComponent<Rotator>(saveJson);
//	char fullSavePath[150];
//	strcpy_s(fullSavePath, saveFileName);
//	strcat_s(fullSavePath, ".pg");
//	std::ofstream saveFile(fullSavePath);
//	saveFile << std::setw(4) << saveJson << std::endl;
//}
//template<class T>
//void TryDeserializeComponent(std::map<std::string, entt::entity> entityMap, std::vector<std::string> storedEntities, json master)
//{
//	//std::string g = std::string(typeid(T).name()).append(".json");
//	//std::ifstream inFile(g);
//	//json objJson;
//	//inFile >> objJson;
//	json entitiesJson = master["entities"];
//	const char* className = typeid(T).name();
//	json objJson = master[className];
//	for (auto it = storedEntities.begin(); it != storedEntities.end(); ++it)
//	{
//		if (objJson.find(*it) != objJson.end())
//		{
//			auto compIter = objJson[*it];
//			T obj;
//			from_json(compIter, obj);
//			registry.emplace<T>(entityMap[*it], std::move(obj));
//		}
//	}
//}
//void Deserialize(const char* saveFileName)
//{
//	//GizmoSystem::DeselectAll();
//	registry.clear();
//	std::vector<std::string> storedEntities;
//	std::ifstream saveFile(saveFileName);
//	json saveJson;
//	saveFile >> saveJson;
//	json entitiesJson = saveJson["entities"];
//	storedEntities = entitiesJson.get<std::vector<std::string>>();
//	std::map<std::string, entt::entity> entityMap;
//	for (auto it = storedEntities.begin(); it != storedEntities.end(); ++it)
//	{
//		entt::entity e = registry.create();
//		entityMap.emplace(*it, e);
//	}
//	TryDeserializeComponent<Mesh>(entityMap, storedEntities, saveJson);
//	TryDeserializeComponent<Camera>(entityMap, storedEntities, saveJson);
//	TryDeserializeComponent<Renderer>(entityMap, storedEntities, saveJson);
//	TryDeserializeComponent<Transform>(entityMap, storedEntities, saveJson);
//	TryDeserializeComponent<Name>(entityMap, storedEntities, saveJson);
//	TryDeserializeComponent<Rotator>(entityMap, storedEntities, saveJson);
//	auto compView = registry.view<Mesh>();
//	for (auto v : compView)
//	{
//		Mesh& m = registry.get<Mesh>(v);
//		MeshLoaderSystem::LoadMesh(m.path.c_str(), m);
//	}
//	auto rendererView = registry.view<Renderer>();
//	auto cameraView = registry.view<Camera>();
//	Camera& camera = registry.get<Camera>(cameraView[0]);
//	for (auto rv : rendererView)
//	{
//		Renderer& renderer = registry.get<Renderer>(rv);
//		Load(renderer, camera);
//	}
//	auto meshView = registry.view<Mesh, Transform>();
//	Transform& meshTransform = registry.get<Transform>(*meshView.begin());
//	//GizmoSystem::Select(*meshView.begin());
//}

void MakeMesh(const char* path, const char* name = "GameObject") {
	auto camView = registry.view<Camera>();
	auto [camera, camTransform] = registry.get<Camera, Transform>(camView[0]);
	glm::vec3 newMeshPos = camTransform.position + TransformSystem::CalculateForward(&camTransform);
	auto newMeshEntity = registry.create();
	Mesh& newMesh = registry.emplace<Mesh>(newMeshEntity, path);
	MeshLoaderSystem::LoadMesh(newMesh.path.c_str(), newMesh);
	Transform& meshTransform = registry.emplace<Transform>(newMeshEntity);
	meshTransform.position = newMeshPos;
	TransformSystem::CalculateWorldMatrix(&meshTransform);
	DirectXRenderer& newMeshRenderer = registry.emplace<DirectXRenderer>(newMeshEntity/*, plat->GetAssetPath("../../Shaders/GLSL/vertex.glsl"), plat->GetAssetPath("../../Shaders/GLSL/fragment.glsl")*/);
	DirectXRenderSystem::Load(newMeshRenderer, camera, (DirectXAPI*)graph, (WindowsPlatform*)plat);
	registry.emplace<RandomColor>(newMeshEntity);
	Name& nameComp = registry.emplace<Name>(newMeshEntity);
	nameComp = { name };
}

void MakeMesh(const char* path, glm::vec3 pos, const char* name = "GameObject") {
	auto camView = registry.view<Camera>();
	auto [camera, camTransform] = registry.get<Camera, Transform>(camView[0]);
	//glm::vec3 newMeshPos = camTransform.position + TransformSystem::CalculateForward(&camTransform);
	auto newMeshEntity = registry.create();
	Mesh& newMesh = registry.emplace<Mesh>(newMeshEntity, path);
	MeshLoaderSystem::LoadMesh(newMesh.path.c_str(), newMesh);
	Transform& meshTransform = registry.emplace<Transform>(newMeshEntity);
	meshTransform.position = pos;
	TransformSystem::CalculateWorldMatrix(&meshTransform);
	DirectXRenderer& newMeshRenderer = registry.emplace<DirectXRenderer>(newMeshEntity/*, plat->GetAssetPath("../../Shaders/GLSL/vertex.glsl"), plat->GetAssetPath("../../Shaders/GLSL/fragment.glsl")*/);
	DirectXRenderSystem::Load(newMeshRenderer, camera, (DirectXAPI*)graph, (WindowsPlatform*)plat);
	registry.emplace<RandomColor>(newMeshEntity);
	Name& nameComp = registry.emplace<Name>(newMeshEntity);
	nameComp = { name };
}

void MakeRayFromCamera()
{
	auto camView = registry.view<Camera>();
	auto [camera, camTransform] = registry.get<Camera, Transform>(camView[0]);
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 mousePos = io.MousePos;
	//glm::vec2 mousePos = plat->GetInputSystem()->GetCursorPosition();
	// Blessed be this code taken from https://gamedev.stackexchange.com/questions/157674/simple-mouseray-picking-in-opengl
	glm::vec3 mouse_world_nearplane = glm::unProject(
		glm::vec3(mousePos.x, 600 - mousePos.y, 0.0f),
		camera.view, //view matrix
		camera.projection,
		glm::ivec4(0, 0, 800, 600));

	glm::vec3 mouse_world_farplane = glm::unProject(
		glm::vec3(mousePos.x, 600 - mousePos.y, 1.0f),
		camera.view, //view matrix
		camera.projection,
		glm::ivec4(0, 0, 800, 600));

	glm::vec3 camray = glm::normalize(mouse_world_farplane - mouse_world_nearplane);
	//camray.y = -camray.y;
	camray *= 30;
	glm::vec3 newMeshLocation = mouse_world_nearplane + camray;
	MakeMesh(plat->GetAssetPath("../../Assets/Models/cube.obj").c_str(), newMeshLocation);
	return;
	float ndcX = ((mousePos.x - window->x) / window->width) * 2.f - 1.f;
	float ndcY = (1.f - ((mousePos.y - window->y) / window->height)) * 2.f - 1.f;
	glm::mat4 viewProjInverse = camera.projection * camera.view;
	viewProjInverse = glm::inverse(viewProjInverse);
	glm::vec4 rayOrigin = glm::vec4(ndcX, ndcY, 0, 1) * viewProjInverse;
	rayOrigin /= rayOrigin.w;
	glm::vec4 rayEnd = glm::vec4(ndcX, ndcY, 1.0f - FLT_EPSILON, 1) * viewProjInverse;
	rayEnd /= rayEnd.w;
	glm::vec3 ray = glm::normalize(rayOrigin - rayEnd);
	ray *= 3;
	MakeMesh(plat->GetAssetPath("../../Assets/Models/cube.obj").c_str(), ray);
}


//void RandomizeVertexColors()
//{
//	auto rcView = registry.view<RandomColor, Mesh, Renderer, Transform>();
//	for (auto rc : rcView)
//	{
//		Mesh& m = registry.get<Mesh>(rc);
//		for (Vertex& v : m.rawVertices)
//		{
//			v.Color = { (rand() % 255) / 255.0f, (rand() % 255) / 255.0f, (rand() % 255) / 255.0f, 1 };
//		}
//		Renderer& renderer = registry.get<Renderer>(rc);
//		LoadMesh(renderer, m);
//		Transform& t = registry.get<Transform>(rc);
//		auto cameraView = registry.view<Camera>();
//		Camera& cam = registry.get<Camera>(*cameraView.begin());
//		UpdateRenderer(renderer, t, cam);
//	}
//}

void Loop()
{
	
	//ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX11_NewFrame();
	//ImGui_ImplOpenGL_NewFrame();
	plat->NewGuiFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
	ImGuizmo::Enable(true);
	static float vertColorPick[4];
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::BeginMenu("New"))
		{
			if (ImGui::MenuItem("Cube"))
			{
				MakeMesh(plat->GetAssetPath("../../Assets/Models/cube.obj").c_str(), "Cube");
			}
			if (ImGui::MenuItem("Helix"))
			{
				MakeMesh(plat->GetAssetPath("../../Assets/Models/helix.obj").c_str(), "Helix");
			}
			if (ImGui::MenuItem("Cone"))
			{
				MakeMesh(plat->GetAssetPath("../../Assets/Models/cone.obj").c_str(), "Cone");
			}
			if (ImGui::MenuItem("Cylinder"))
			{
				MakeMesh(plat->GetAssetPath("../../Assets/Models/cylinder.obj").c_str(), "Cylinder");
			}
			if (ImGui::MenuItem("Sphere"))
			{
				MakeMesh(plat->GetAssetPath("../../Assets/Models/sphere.obj").c_str(), "Sphere");
			}
			if (ImGui::MenuItem("Torus"))
			{
				MakeMesh(plat->GetAssetPath("../../Assets/Models/torus.obj").c_str(), "Torus");
			}
			ImGui::EndMenu();
		}
		//if (ImGui::BeginMenu("Save"))
		//{
		//	static char saveFileName[100] = {};
		//	ImGui::InputText("Save as: ", saveFileName, 100);
		//	if (ImGui::Button("Save"))
		//	{
		//		Serialize(saveFileName);
		//	}
		//	ImGui::EndMenu();
		//}
		//if (ImGui::BeginMenu("Open"))
		//{
		//	for (const auto& saveFile : std::filesystem::directory_iterator("./"))
		//	{
		//		if (saveFile.path().extension().generic_string() == ".pg")
		//		{
		//			std::string fileStr = saveFile.path().generic_string();

		//			if (ImGui::MenuItem(fileStr.c_str()))
		//			{
		//				Deserialize(fileStr.c_str());
		//			}
		//		}
		//	}
		//	ImGui::EndMenu();
		//}
		ImGui::EndMenu();
	}
	//if (ImGui::BeginMenu("Components"))
	//{
	//	if (ImGui::BeginMenu("Color picker"))
	//	{
	//		ImGui::ColorPicker4("Vertex color: ", vertColorPick);
	//		ImGui::EndMenu();
	//	}
	//	if (ImGui::MenuItem("Random Vertex Colors"))
	//	{
	//		RandomizeVertexColors();
	//	}
	//	ImGui::EndMenu();
	//}
	ImGui::EndMainMenuBar();
	//ImGui::Begin("Vertex color picker", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove);
	//ImGui::Text("Vertex color: ");
	//ImGui::ColorPicker4("Vertex color: ", vertColorPick);
	//ImGui::End();

	plat->GetInputSystem()->GetKeyPressed();
	auto view = registry.view<Mesh, DirectXRenderer, Transform, Name>();
	ImGui::Begin("Entity List");
	ImGui::SetWindowPos({ 0, 20 });
	ImGui::SetWindowSize({ 200, 780 });
	for (auto renderable : view)
	{
		Name name = view.get<Name>(renderable);
		if (ImGui::MenuItem(name.nameString.c_str()))
		{
			GizmoSystem::DeselectAll();
			Transform& newSelected = view.get<Transform>(renderable);
			GizmoSystem::Select(renderable);
		}
	}
	ImGui::End();

	ImGui::Begin("Inspector");
	ImGui::SetWindowPos({ 600, 20 });
	ImGui::SetWindowSize({ 200, 780 });
	entt::entity selected = GizmoSystem::GetSelectedEntity();
	if (selected != entt::null)
	{
		Transform* t = registry.try_get<Transform>(selected);
		if (t)
		{
			ComponentGUI(*t);
		}
		//Renderer* r = registry.try_get<Renderer>(selected);
		//if (r)
		//{
		//	ComponentGUI(*r);
		//}
		DirectXRenderer* r = registry.try_get<DirectXRenderer>(selected);
		if (r)
		{
			ComponentGUI(*r);
		}
		//Rotator* rot = registry.try_get<Rotator>(selected);
		//if (rot)
		//{
		//	ComponentGUI(*rot);
		//}
		//if (ImGui::BeginMenu("+"))
		//{
		//	if (ImGui::MenuItem("Rotator"))
		//	{
		//		if (!registry.has<Rotator>(selected))
		//		{
		//			registry.emplace_or_replace<Rotator>(selected);
		//		}
		//	}
		//	ImGui::EndMenu();
		//}
	}
	ImGui::End();

	auto rotatorView = registry.view<Transform, Rotator>();
	for (auto rotEntity : rotatorView)
	{
		auto [transform, rot] = rotatorView.get<Transform, Rotator>(rotEntity);
		TransformSystem::Rotate(rot.deltaRot, &transform);
	}

	graph->ClearScreen();
	auto camEntityView = registry.view<Camera>();
	auto [camera, camTransform] = registry.get<Camera, Transform>(camEntityView[0]);
	TransformSystem::CalculateWorldMatrix(&camTransform);
	CameraSystem::CalculateViewMatrixLH(camera, camTransform);
	for (auto renderable : view)
	{
		DirectXRenderer& renderer = registry.get<DirectXRenderer>(renderable);
		Mesh& mesh = registry.get<Mesh>(renderable);
		Transform& meshTransform = registry.get<Transform>(renderable);
		// if RandomColor component is attached, do not assign every vertex a color
		//if (!registry.has<RandomColor>(renderable))
		//{
		//	for (auto it = mesh.rawVertices.begin(); it != mesh.rawVertices.end(); ++it)
		//	{
		//		it->Color = { vertColorPick[0], vertColorPick[1], vertColorPick[2], vertColorPick[3] };
		//	}
		//}
		DirectXRenderSystem::LoadMesh(renderer, mesh, ((DirectXAPI*)(graph))->device.Get());
		DirectXRenderSystem::UpdateRenderer(renderer, meshTransform, camera);
		//renderer.Update();
		//renderer.Draw();
		DirectXRenderSystem::Draw(renderer, ((DirectXAPI*)(graph))->context.Get());
	}
	auto transformView = registry.view<Transform>();
	//camera.transform = transform;
	GizmoSystem::DrawGizmo(camera, transformView);
	ImGui::Render();
	//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	graph->_SwapBuffers();
}

int main(int argc, char* argv[])
{
#if defined(DEBUG) | defined(_DEBUG)
	// Enable memory leak detection as a quick and dirty
	// way of determining if we forgot to clean something up
	//  - You may want to use something more advanced, like Visual Leak Detector
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
	window = new GameWindow(0, 0, 800, 600);
	auto entity = registry.create();
	auto cameraEntity = registry.create();

	Transform& camTransform = registry.emplace<Transform>(cameraEntity);
	Camera& cam = registry.emplace<Camera>(cameraEntity, (float)window->width / window->height);
	camTransform.position += glm::vec3(0, 0, -3);
	//cam.transform = &camTransform;
	CameraSystem::CalculateProjectionMatrixLH(cam, (float)window->width / window->height);
	//Camera cam = Camera(glm::vec3(0, 0, -3), (float)window->width / window->height);
	float camMoveSpeed = .05f;
	glm::vec2 prevCursorPos{-1, -1}, currentCursorPos;

	// (Try to) Setup IMGUI	
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();


#ifdef _WIN64
	plat = new WindowsPlatform(window);
#elif defined __EMSCRIPTEN__
	plat = new EmscriptenPlatform(window);
#endif
#ifdef _WIN64
	graph = new DirectXAPI(window);
#elif defined __EMSCRIPTEN__
	graph = new OpenGLAPI(window, plat);
#endif
	plat->InitWindow();
	graph->Init();

	Mesh& mesh = registry.emplace<Mesh>(entity, plat->GetAssetPath("../../Assets/Models/cone.obj").c_str());
	MeshLoaderSystem::LoadMesh(mesh.path.c_str(), mesh);
	DirectXRenderer& renderer = registry.emplace<DirectXRenderer>(entity/*, plat->GetAssetPath("../../Shaders/GLSL/vertex.glsl"), plat->GetAssetPath("../../Shaders/GLSL/fragment.glsl")*/);
	Transform& t1 = registry.emplace<Transform>(entity);
	t1.position += glm::vec3(0, 1, 0);
	TransformSystem::CalculateWorldMatrix(&t1);
	Name& name = registry.emplace<Name>(entity);
	name = { "Cone" };
	DirectXRenderSystem::Load(renderer, cam, (DirectXAPI*)graph, (WindowsPlatform*)plat);
	//renderer.LoadMesh(mesh.GetRawVertices());
	DirectXRenderSystem::LoadMesh(renderer, mesh, ((DirectXAPI*)(graph))->device.Get());

	//GizmoSystem::Select(entity);
	//auto ent2 = registry.create();
	//Mesh& mesh2 = registry.emplace<Mesh>(ent2, plat->GetAssetPath("../../Assets/Models/helix.obj").c_str());
	//MeshLoaderSystem::LoadMesh(mesh2.path.c_str(), mesh2);
	//DirectXRenderer& renderer2 = registry.emplace<DirectXRenderer>(ent2/*, plat->GetAssetPath("../../Shaders/GLSL/vertex.glsl"), plat->GetAssetPath("../../Shaders/GLSL/fragment.glsl")*/);
	//Transform& t2 = registry.emplace<Transform>(ent2);
	//t2.position += glm::vec3(0, -2, 0);
	//TransformSystem::CalculateWorldMatrix(&t2);
	//Load(renderer2, cam, (DirectXAPI*)graph, (WindowsPlatform*)plat);
	//LoadMesh(renderer2, mesh2, ((DirectXAPI*)(graph))->device.Get());


	plat->GetInputSystem()->RegisterRightMouseFunction([]()
	{
		auto camView = registry.view<Camera>();
		auto [camera, camTransform] = registry.get<Camera, Transform>(camView[0]);
		glm::vec2 delta = plat->GetInputSystem()->GetCursorPosition() - plat->GetInputSystem()->GetPreviousCursorPosition();
		float camRotX = camTransform.rotation.x;
		bool tooFarUp = camRotX > 3.f / 2;
		bool tooFarDown = camRotX < -3.f / 2;
		// checks to see if camera is in danger of gimbal lock
		if (tooFarUp || tooFarDown)
		{
			float newCamRotX;
			// we allow the input to rotate the camera on the x axis
			// if it is "too far down" and going up or "too far up" 
			// and going down. Otherwise zero out the x axis input
			if ((tooFarUp && delta.y < 0) || (tooFarDown && delta.y > 0))
			{
				newCamRotX = delta.y;
			}
			else
			{
				newCamRotX = 0;
			}
			//cam.GetTransform()->Rotate(glm::vec3(newCamRotX * .005f, -delta.x * .005f, 0));
			TransformSystem::Rotate(glm::vec3(newCamRotX * .005f, delta.x * .005f, 0), &camTransform);
		}
		else
		{
			//cam.GetTransform()->Rotate(glm::vec3(delta.y * .005f, -delta.x * .005f, 0));
			TransformSystem::Rotate(glm::vec3(delta.y * .005f, delta.x * .005f, 0), &camTransform);
		}
	});

	auto MoveCamera = [](glm::vec3 dir) {
		return[dir]() {
			auto camView = registry.view<Camera>();
			auto [camera, camTransform] = registry.get<Camera, Transform>(camView[0]);
			TransformSystem::MoveRelative(dir, &camTransform);
		};
	};

	plat->GetInputSystem()->RegisterKeyPressFunction('w', MoveCamera(glm::vec3(0, 0, 1 * camMoveSpeed)));
	plat->GetInputSystem()->RegisterKeyPressFunction('s', MoveCamera(glm::vec3(0, 0, -1 * camMoveSpeed)));
	plat->GetInputSystem()->RegisterKeyPressFunction('a', MoveCamera(glm::vec3(-1 * camMoveSpeed, 0, 0)));
	plat->GetInputSystem()->RegisterKeyPressFunction('d', MoveCamera(glm::vec3(1 * camMoveSpeed, 0, 0)));
	plat->GetInputSystem()->RegisterKeyPressFunction('q', MoveCamera(glm::vec3(0, -1 * camMoveSpeed, 0)));
	plat->GetInputSystem()->RegisterKeyPressFunction('e', MoveCamera(glm::vec3(0, 1 * camMoveSpeed, 0)));

	// gizmo keyboard controls
	plat->GetInputSystem()->RegisterKeyPressFunction('r', []() {GizmoSystem::op = ImGuizmo::SCALE; });
	plat->GetInputSystem()->RegisterKeyPressFunction('e', []() {GizmoSystem::op = ImGuizmo::ROTATE; });
	plat->GetInputSystem()->RegisterKeyPressFunction('w', []() {GizmoSystem::op = ImGuizmo::TRANSLATE; });

	//plat->GetInputSystem()->RegisterKeyPressFunction('b', []() {Serialize(); });
	//plat->GetInputSystem()->RegisterKeyPressFunction('m', []() {Deserialize(); });

	// TODO: Remove this MakeRayFromCamera call
	plat->GetInputSystem()->RegisterKeyPressFunction('j', []() {MakeRayFromCamera(); });
	//plat->GetInputSystem()->RegisterKeyPressFunction('k', [&camTransform, &cam]() {camTransform.rotation = glm::vec3(0, 0, 0); TransformSystem::CalculateWorldMatrix(&camTransform); CameraSystem::CalculateViewMatrix(cam, camTransform); });
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(Loop, 0, 1);
#else
	while (plat->Run() == 0)
	{
		Loop();
	}
#endif
	delete window;
	delete plat;
	delete graph;
	ImGui::DestroyContext();
	return 0;
}
