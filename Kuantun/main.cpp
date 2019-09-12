#include <SDL.h>
#include <iostream>
#include "FreeImage.h"
#include <string>

//#define USING_NANA // Comentar para usar SDL
#ifdef USING_NANA
#include <nana/gui.hpp>
#include <nana/paint/graphics.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/menu.hpp>
#include <nana/gui/widgets/tabbar.hpp>
#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/treebox.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/spinbox.hpp>
#include <nana/gui/widgets/slider.hpp>
#include <nana/gui/widgets/progress.hpp>
#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/widget.hpp>
#include <nana/gui/filebox.hpp>
#include <nana/gui/timer.hpp>
#include <nana/paint/pixel_buffer.hpp>

#include "Scene.h"

int main(int argc, char* args[]){
	using namespace nana;
	bool changed = true;
	nana::size ss = screen::desktop_size();
	unsigned int scene_width = ss.width/8;
	unsigned int scene_height = ss.height/8;
	API::window_icon_default(nana::paint::image("bender.ico"));
	form fm = form(rectangle{ screen().desktop_size() }, form::appear::decorate<form::appear::minimize, form::appear::maximize, form::appear::taskbar>());
	fm.caption("Bender");
	fm.zoom(true);
	nana::treebox tree(fm, { int(ss.width) - 300, 35, 300 , 300 });
	tree.bgcolor(fm.bgcolor());
	auto node = tree.insert("cam", "Camera");
	auto node2 = tree.insert("sce", "Scene");
	auto nodeobj = tree.insert(node2, "obj", "Meshes");
	auto nodelights = tree.insert(node2, "lig", "Lights");
	int triangles, vertices;
	triangles = 0;
	vertices = 0;
	
	Scene* scene = Scene::load("assets/blank.scn");
	nana::timer frame_timer;

	label tris(fm, rectangle( 10, 7*int(ss.height)/8, 200, 30 ));
	tris.caption("Triangles: 0, vertices: 0");

	//Camara
	label fov_lab(fm, nana::rectangle(int(ss.width) / 2 - (int(ss.width) * 4) / 16, 5*int(ss.height)/8, 100, 30));
	fov_lab.caption("Field of view: ");
	spinbox fov_sb(fm, nana::rectangle(int(ss.width) / 2 - (int(ss.width) * 4) / 16 + 100, 5*int(ss.height) / 8, 100, 30));
	fov_sb.range(50.0, 150.0,3.0);
	fov_sb.caption(to_string(scene->cam.fov));
	fov_sb.hide();
	fov_lab.hide();
	fov_sb.events().text_changed([&fov_sb, &scene, &fm](const arg_spinbox &arg) {
		scene->cam.fov = fov_sb.to_double();
		fm.focus();
	});

	//Translate
	label ts_lab(fm, nana::rectangle(int(ss.width) / 2 - (int(ss.width) * 4) / 16, 5 * int(ss.height) / 8, 100, 30));
	ts_lab.caption("Translate: ");
	label point_lab(fm, nana::rectangle(int(ss.width) / 2 - (int(ss.width) * 4) / 16, 5 * int(ss.height) / 8, 100, 30));
	point_lab.caption("Position: ");
	label dir_lab(fm, nana::rectangle(int(ss.width) / 2 - (int(ss.width) * 4) / 16, 5 * int(ss.height) / 8, 100, 30));
	dir_lab.caption("Position: ");
	label int_lab(fm, nana::rectangle(int(ss.width) / 2 - (int(ss.width) * 4) / 16, 40 + 5 * int(ss.height) / 8, 100, 30));
	int_lab.caption("Intensity: ");
	spinbox value_x(fm, nana::rectangle(int(ss.width) / 2 - (int(ss.width) * 4) / 16 + 100, 5 * int(ss.height) / 8, 100, 30));
	spinbox value_y(fm, nana::rectangle(int(ss.width) / 2 - (int(ss.width) * 4) / 16 + 200, 5 * int(ss.height) / 8, 100, 30));
	spinbox value_z(fm, nana::rectangle(int(ss.width) / 2 - (int(ss.width) * 4) / 16 + 300, 5 * int(ss.height) / 8, 100, 30));
	spinbox intensity_sb(fm, nana::rectangle(int(ss.width) / 2 - (int(ss.width) * 4) / 16 + 100, 40 + 5 * int(ss.height) / 8, 100, 30));
	button accept(fm, nana::rectangle(int(ss.width) / 2 - (int(ss.width) * 4) / 16 + 450, 5 * int(ss.height) / 8, 100, 30));
	intensity_sb.range(-1000.0, 1000.0, 0.1);
	value_x.range(-1000.0, 1000.0, 0.3);
	value_y.range(-1000.0, 1000.0, 0.3);
	value_z.range(-1000.0, 1000.0, 0.3);
	accept.caption("Accept");

	intensity_sb.events().text_changed([&intensity_sb, &scene, &fm, &tree](const arg_spinbox &arg) {
		auto node = tree.selected();
		string sel = node.text();
		if (sel.substr(0, 6) == "Light ") {
			int index = stoi(sel.substr(6, sel.size()));
			scene->lights[index]->intensity = intensity_sb.to_double();
		}
		fm.focus();
	});

	accept.events().click([&scene, &tree, &value_x, &value_y, &value_z, &fm](const arg_click &arg) {
		auto node = tree.selected();
		string sel = node.text();
		if (sel.substr(0, 5) == "Mesh ") {
			int index = stoi(sel.substr(5, sel.size()));
			scene->objects[index].translate(Vector3D(value_x.to_double(), value_y.to_double(), value_z.to_double()));
		}
		else if (sel.substr(0, 6) == "Light ") {
			int index = stoi(sel.substr(6, sel.size()));
			DirectionalLight* directional = dynamic_cast<DirectionalLight*>((scene->lights[index]));
			PointLight* point = dynamic_cast<PointLight*>((scene->lights[index]));
			if (point)
				point->position = Vector3D(Vector3D(value_x.to_double(), value_y.to_double(), value_z.to_double()));
			else if (directional)
				directional->direction = Vector3D(Vector3D(value_x.to_double(), value_y.to_double(), value_z.to_double())).GetNormalized();
		}
		fm.focus();
	});

	point_lab.hide();
	ts_lab.hide();
	value_x.hide();
	value_y.hide();
	value_z.hide();
	accept.hide();
	intensity_sb.hide();
	int_lab.hide();
	dir_lab.hide();

	nana::menubar mn(fm);
	mn.push_back("File");
	mn.at(0).append("Open scene", [&fm, &scene, &triangles, &vertices, &tris, &tree, &nodeobj, &nodelights, &node2, &node, &changed](nana::menu::item_proxy& ip)	{
		nana::form fm2(fm);
		filebox fbox(fm, true);
		if (fbox.show()) {
			if (scene) {
				changed = true;
				delete scene;
				scene = Scene::load(fbox.file());
				triangles = 0;
				vertices = 0;
				tree.erase("cam");
				tree.erase("sce");
				node = tree.insert("cam", "Camera");
				node2 = tree.insert("sce", "Scene");
				nodeobj = tree.insert(node2, "obj", "Meshes");
				nodelights = tree.insert(node2, "lig", "Lights");
				for (unsigned int j = 0; j < scene->objects.size(); j++) {
					for (unsigned int i = 0; i < scene->objects[j].parts.size(); i++) {
						triangles += scene->objects[j].parts[i]->vertexCount / 9;
						vertices += scene->objects[j].parts[i]->vertexCount / 3;
					}
					tree.insert(nodeobj, "m" + to_string(j), "Mesh " + to_string(j));
				}
				for (unsigned int j = 0; j < scene->lights.size(); j++) {
					tree.insert(nodelights, "l" + to_string(j), "Light " + to_string(j));
				}
				tris.caption("Triangles: " + to_string(triangles) + ", vertices : " + to_string(vertices));
			}
		}
	});
	mn.at(0).append("Save scene", [&fm, &scene](nana::menu::item_proxy& ip) {
		nana::form fm2(fm);
		filebox fbox(fm, true);
		if (fbox.show()) {
			scene->save(fbox.file());
		}
	});
	mn.at(0).append("Add mesh", [&fm, scene, &changed, &tree, &nodeobj, &triangles, &vertices, &tris](nana::menu::item_proxy& ip) {
		changed = true;
		filebox fbox(fm, true);
		fbox.add_filter("Object .obj", "*.obj");
		if (fbox.show()) {
			Mesh m = Mesh(fbox.file().substr(0, fbox.file().size() - 4));
			for (unsigned int i = 0; i < m.parts.size(); i++) {
				triangles += m.parts[i]->vertexCount / 9;
				vertices += m.parts[i]->vertexCount / 3;
			}
			scene->objects.push_back(m);
			tris.caption("Triangles: "+to_string(triangles)+", vertices : " + to_string(vertices));
			tree.insert(nodeobj, "m" + to_string(scene->objects.size()-1), "Mesh " + to_string(scene->objects.size() - 1));
		}
		fm.focus();
	});
	mn.at(0).append("Add directional light", [&fm, scene, &changed, &tree, &nodelights](nana::menu::item_proxy& ip) {
		changed = true;
		scene->lights.push_back(new DirectionalLight(Vector3D(0, -1, 0), 0.1f));
		tree.insert(nodelights, "l" + to_string(scene->lights.size() - 1), "Light " + to_string(scene->lights.size() - 1));
		fm.focus();
	});
	mn.at(0).append("Add point light", [&fm, scene, &changed, &tree, &nodelights](nana::menu::item_proxy& ip) {
		changed = true;
		scene->lights.push_back(new PointLight(Vector3D(0, 0, 0), 100.0f));
		tree.insert(nodelights, "l" + to_string(scene->lights.size() - 1), "Light " + to_string(scene->lights.size() - 1));
		fm.focus();
	});
	mn.push_back("Render");
	mn.at(1).append("Render here", [&fm, &scene, &changed](nana::menu::item_proxy& ip) {
		unsigned int width, height;
		width = 1280;
		height = 720;
		unsigned int ss = 1;
		
		nana::form formRender(fm,nana::size(400,170));
		nana::button render(formRender, rectangle{200,120,150,30 });
		render.caption("Start");
		label res(formRender, nana::rectangle(10, 10, 100, 30));
		res.caption("Resolution: ");
		combox cres(formRender, nana::rectangle(200, 10, 150, 30));
		cres.push_back(("1920x1080"));
		cres.push_back(("1280x720"));
		cres.push_back(("800x600"));
		cres.push_back(("420x260"));

		label supersample(formRender, nana::rectangle(10, 70, 100, 30));
		supersample.caption("Supersample: ");
		combox csuper(formRender, nana::rectangle(200, 70, 150, 30));
		csuper.push_back(("Quadruple"));
		csuper.push_back(("Triple"));
		csuper.push_back(("Double"));
		csuper.push_back(("Simple"));

		render.events().click([&scene, &width, &height, &ss, &fm, &changed, &csuper, &cres, &formRender] {
			if (csuper.option() == 0) {
				ss = 4;
			}
			else if (csuper.option() == 1) {
				ss = 3;
			}
			else if (csuper.option() == 2) {
				ss = 2;
			}
			else if (csuper.option() == 3) {
				ss = 1;
			}
			if (cres.option() == 0) {
				width = 1920;
				height = 1080;
			}
			else if (cres.option() == 1) {
				width = 1280;
				height = 720;
			}
			else if (cres.option() == 2) {
				width = 800;
				height = 600;
			}
			else if (cres.option() == 3) {
				width = 420;
				height = 260;
			}
			nana::form renderized(fm, nana::size(width, height));
			renderized.caption("Render");
			drawing{ renderized }.draw([&scene, ss, width, height, &changed, &formRender](paint::graphics& graph) {
				paint::pixel_buffer pxbuf(width, height);
				auto pxbuf_ptr = pxbuf.raw_ptr(0);
				if (changed) {
					changed = false;
					if (scene->photonMap) delete scene->photonMap;
					if (scene->causticMap) delete scene->causticMap;
					scene->genPhotonMap();
				}
				auto pixels = scene->draw(width, height, ss);
				for (int i = 0; i < width; i++) {
					for (int j = 0; j < height; j++) {
						RGBA px = pixels[j * width + i];
						px.clamp();
						pixel_color_t & pxcol = pxbuf_ptr[j * width + i];
						pxcol.element.red = static_cast<decltype(pxcol.element.red)>(px.r * 255);
						pxcol.element.green = static_cast<decltype(pxcol.element.green)>(px.g * 255);
						pxcol.element.blue = static_cast<decltype(pxcol.element.blue)>(px.b * 255);
						pxcol.element.alpha_channel = 255;
					}
				}
				pxbuf.paste(graph.handle(), { 0, 0 });
				delete[] pixels;
			});
			formRender.close();
			renderized.show();
			renderized.modality();
		});
		
		formRender.show();
		formRender.modality();
	});
	mn.at(1).append("Render to file", [&fm, &scene, &changed](nana::menu::item_proxy& ip) {
		unsigned int width, height;
		width = 1280;
		height = 720;
		unsigned int ss = 1;
		string path = "scene.png";
		nana::form formRender(fm, nana::size(400, 170));
		nana::button render(formRender, rectangle{ 200,120,150,30 });
		render.caption("Start");
		label res(formRender, nana::rectangle(10, 10, 100, 30));
		res.caption("Resolution: ");
		combox cres(formRender, nana::rectangle(200, 10, 150, 30));
		cres.push_back(("1920x1080"));
		cres.push_back(("1280x720"));
		cres.push_back(("800x600"));
		cres.push_back(("420x260"));

		label supersample(formRender, nana::rectangle(10, 70, 100, 30));
		supersample.caption("Supersample: ");
		combox csuper(formRender, nana::rectangle(200, 70, 150, 30));
		csuper.push_back(("Quadruple"));
		csuper.push_back(("Triple"));
		csuper.push_back(("Double"));
		csuper.push_back(("Simple"));

		filebox fbox(fm, true);
		fbox.add_filter("Image .png", "*.png");
		if (fbox.show()) {
			path = fbox.file();
		}

		render.events().click([scene, &width, &height, &ss, &fm, &changed, &csuper, &cres, &path, &formRender] {
			if (csuper.option() == 0) {
				ss = 4;
			}
			else if (csuper.option() == 1) {
				ss = 3;
			}
			else if (csuper.option() == 2) {
				ss = 2;
			}
			else if (csuper.option() == 3) {
				ss = 1;
			}
			if (cres.option() == 0) {
				width = 1920;
				height = 1080;
			}
			else if (cres.option() == 1) {
				width = 1280;
				height = 720;
			}
			else if (cres.option() == 2) {
				width = 800;
				height = 600;
			}
			else if (cres.option() == 3) {
				width = 420;
				height = 260;
			}
			if (changed) {
				changed = false;
				if (scene->photonMap) delete scene->photonMap;
				if (scene->causticMap) delete scene->causticMap;
				scene->genPhotonMap();
			}
			Uint32* pxl = new Uint32[width * height];
			auto pixels = scene->draw(width, height, ss);
			for (int i = 0; i < width; i++) {
				for (int j = 0; j < height; j++) {
					pxl[i * height + j] = pixels[i*height + j].toUint32();
				}
			}
			Uint32 rmask = 0xff000000;
			Uint32 gmask = 0x00ff0000;
			Uint32 bmask = 0x0000ff00;
			Uint32 amask = 0;
			int depth = 32;
			int pitch = 4 * width;
			SDL_Surface* surf = SDL_CreateRGBSurfaceFrom((void*)pxl, width, height, depth, pitch, rmask, gmask, bmask, amask);
			SDL_SaveBMP(surf, path.c_str());
			SDL_FreeSurface(surf);
			delete[] pxl;
			delete[] pixels;
			formRender.close();
		});
		formRender.show();
		formRender.modality();
	});

	for (int i = 0; i < scene->objects.size(); i++) {
		tree.insert(nodeobj, "m"+to_string(i), "Mesh " + to_string(i));
	}
	for (int i = 0; i < scene->lights.size(); i++) {
		tree.insert(nodelights, "l"+to_string(i), "Light " + to_string(i));
	}

	tree.events().selected([&scene, &tree, ss, &fm, &fov_sb, &fov_lab, &ts_lab, &value_x, &value_y, &value_z, &accept, &point_lab, &intensity_sb, &int_lab, &dir_lab] (const arg_treebox &arg){
		auto node = tree.selected();
		string sel = node.text();
		if (sel.substr(0,5) == "Mesh "){
			int index = stoi(sel.substr(5, sel.size()));
			ts_lab.show();
			value_x.show();
			accept.show();
			value_y.show();
			value_z.show();
			fov_sb.hide();
			dir_lab.hide();
			fov_lab.hide();
			intensity_sb.hide();
			int_lab.hide();
			value_x.caption(to_string(0.0));
			value_y.caption(to_string(0.0));
			value_z.caption(to_string(0.0));
		}
		else if (sel.substr(0, 6) == "Light ") {
			int index = stoi(sel.substr(6, sel.size()));
			DirectionalLight* directional = dynamic_cast<DirectionalLight*>((scene->lights[index]));
			PointLight* point = dynamic_cast<PointLight*>((scene->lights[index]));
			if (point){
				value_x.caption(to_string(point->position.x));
				value_y.caption(to_string(point->position.y));
				value_z.caption(to_string(point->position.z));
				point_lab.show();
				value_x.show();
				value_y.show();
				value_z.show();
				fov_sb.hide();
				fov_lab.hide();
				ts_lab.hide();
				dir_lab.hide();
				accept.show();
				int_lab.show();
				intensity_sb.show();
				intensity_sb.caption(to_string(point->intensity));
			}
			else if (directional){
				value_x.caption(to_string(directional->direction.x));
				value_y.caption(to_string(directional->direction.y));
				value_z.caption(to_string(directional->direction.z));
				point_lab.hide();
				value_x.show();
				value_y.show();
				value_z.show();
				fov_sb.hide();
				dir_lab.show();
				fov_lab.hide();
				ts_lab.hide();
				accept.show();
				int_lab.show();
				intensity_sb.show();
				intensity_sb.caption(to_string(directional->intensity));
			}
		}
		else if (sel == "Camera") {
			fov_sb.show();
			fov_lab.show();
			ts_lab.hide();
			accept.hide();
			value_x.hide();
			value_y.hide();
			dir_lab.hide();
			value_z.hide();
			intensity_sb.hide();
			int_lab.hide();
		}
		fm.focus();
	});

	frame_timer.elapse([&fm]() {
		API::refresh_window(fm);
	});

	drawing{ fm }.draw([&scene, ss,scene_width, scene_height](paint::graphics& graph) {
		paint::pixel_buffer pxbuf(scene_width, scene_height);
		auto pxbuf_ptr = pxbuf.raw_ptr(0);
		auto pixels = scene->quickDraw(scene_width, scene_height);
		for (int i = 0; i < scene_width; i++) {
			for (int j = 0; j < scene_height; j++) {
				RGBA px = pixels[j * scene_width + i];
				px.clamp();
				pixel_color_t & pxcol = pxbuf_ptr[j * scene_width + i];
				pxcol.element.red = static_cast<decltype(pxcol.element.red)>(px.r * 255);
				pxcol.element.green = static_cast<decltype(pxcol.element.green)>(px.g * 255);
				pxcol.element.blue = static_cast<decltype(pxcol.element.blue)>(px.b * 255);
				pxcol.element.alpha_channel = 255;
			}
		}
		int w = int(ss.width) / 2 - (int(ss.width) * 4) / 16;
		pxbuf.stretch(rectangle{ pxbuf.size() }, graph.handle(), rectangle{ w, 25, scene_width * 4 , scene_height * 4 });
		delete[] pixels;
	});

	fm.events().key_press([scene](const nana::arg_keyboard &ak) {
		if (ak.key == nana::keyboard::os_arrow_up) {
			scene->cam.reference += scene->cam.up * 0.1f;
		}
		else if (ak.key == nana::keyboard::os_arrow_down) {
			scene->cam.reference -= scene->cam.up *0.1f;
		}
		else if (ak.key == nana::keyboard::os_arrow_right) {
			Vector3D right = (scene->cam.reference - scene->cam.position).GetCross(scene->cam.up).GetNormalized();
			scene->cam.reference += right *0.1f;
		}
		else if (ak.key == nana::keyboard::os_arrow_left) {
			Vector3D right = (scene->cam.reference - scene->cam.position).GetCross(scene->cam.up).GetNormalized();
			scene->cam.reference -= right *0.1f;
		}
	});

	fm.events().key_char([scene](const nana::arg_keyboard &ak) {
		if (ak.key == 'w' || ak.key == 'W') {
			Vector3D dir = (scene->cam.reference - scene->cam.position).GetNormalized() * 0.1f;
			scene->cam.position += dir;
			scene->cam.reference += dir;		
		}
		else if (ak.key == 's' || ak.key == 'S') {
			Vector3D dir = (scene->cam.reference - scene->cam.position).GetNormalized() * 0.1f;
			scene->cam.position -= dir;
			scene->cam.position -= dir;		
		}
		else if (ak.key == 'd' || ak.key == 'D') {
			Vector3D right = (scene->cam.reference - scene->cam.position).GetCross(scene->cam.up).GetNormalized();
			scene->cam.position += right * 0.1f;
			scene->cam.reference += right * 0.1f;
		}
		else if (ak.key == 'a' || ak.key == 'A') {
			Vector3D right = (scene->cam.reference - scene->cam.position).GetCross(scene->cam.up).GetNormalized();
			scene->cam.position -= right * 0.1f;
			scene->cam.reference -= right * 0.1f;
		}
	});
	
	frame_timer.interval(250);
	frame_timer.start();
	fm.show();
	nana::exec();
	return 0;
}
#else
#include "Scene.h"

using namespace std;

bool is_numeric(char * input){
	int sizeOfString = strlen(input);
	int iteration = 0;
	bool isNumeric = true;
	while (iteration < sizeOfString) {
		if (!isdigit(input[iteration])) {
			isNumeric = false;
			break;
			iteration++;
		}
	}
	return isNumeric;
}

int main(int argc, char* args[]) {

	unsigned int WIDTH = 1920;
	unsigned int HEIGHT = 1080;
	unsigned int SAMPLES = 2;
	int type = 2;

	if (argc > 4) {
		WIDTH = atoi(args[1]);
		HEIGHT = atoi(args[2]);
		SAMPLES = atoi(args[3]);
		type = atoi(args[4]);
	}

	Uint32 rmask = 0xff000000;
	Uint32 gmask = 0x00ff0000;
	Uint32 bmask = 0x0000ff00;
	Uint32 amask = 0;
	int depth = 32;
	int pitch = 4 * WIDTH;

	SDL_Window* window = NULL;
	SDL_Surface* windowSurface = NULL;
	SDL_Surface* currentSurface = NULL;

	//Inicializar SDL

	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			std::cout << "No se pudo iniciar SDL, SDL_Error: " << SDL_GetError();
			return -1;
		}
		else {
			window = SDL_CreateWindow("Kuantum", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
			if (window == NULL) {
				std::cout << "No se pudo crear la ventana, SDL_Error: " << SDL_GetError();
				return -1;
			}
			else {
				windowSurface = SDL_GetWindowSurface(window);
			}
		}
	}


	//Pantalla de carga

	FREE_IMAGE_FORMAT filetype = FreeImage_GetFileType("assets/textures/loading.png", 0);
	FIBITMAP *freeimage_bitmap = FreeImage_Load(filetype, "assets/textures/loading.png", 0);
	FreeImage_FlipVertical(freeimage_bitmap);
	currentSurface = SDL_CreateRGBSurfaceFrom(
		FreeImage_GetBits(freeimage_bitmap),
		FreeImage_GetWidth(freeimage_bitmap),
		FreeImage_GetHeight(freeimage_bitmap),
		FreeImage_GetBPP(freeimage_bitmap),
		FreeImage_GetPitch(freeimage_bitmap),
		FreeImage_GetRedMask(freeimage_bitmap),
		FreeImage_GetGreenMask(freeimage_bitmap),
		FreeImage_GetBlueMask(freeimage_bitmap),
		0
	);
	SDL_BlitScaled(currentSurface, NULL, windowSurface, NULL);
	SDL_UpdateWindowSurface(window);
	FreeImage_Unload(freeimage_bitmap);
	SDL_FreeSurface(currentSurface);

	//Cambios en el control de la imagen
	Scene* scene;
	scene = new Scene(type);
	scene->genPhotonMap();

	Uint32* screenPixels = new Uint32[WIDTH*HEIGHT];
	RGBA* pixels;
	pixels = scene->draw(WIDTH, HEIGHT, 1);
	for (int j = 0; j < WIDTH; j++) {
		for (int i = 0; i < HEIGHT; i++) {
			screenPixels[j*HEIGHT + i] = pixels[j*HEIGHT + i].toUint32();
		}
	}
	delete[] pixels;
	delete scene;

	//Actualizar la surface
	currentSurface = SDL_CreateRGBSurfaceFrom((void*)screenPixels, WIDTH, HEIGHT, depth, pitch, rmask, gmask, bmask, amask);
	SDL_BlitSurface(currentSurface, NULL, windowSurface, NULL);
	SDL_UpdateWindowSurface(window);
	SDL_FreeSurface(currentSurface);

	//Esperar a la salida
	bool quit = false;
	SDL_Event e;
	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			switch (e.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_ESCAPE) {
					quit = true;
					break;
				}
			}
		}
	}

	//Cerrar SDL
	{
		SDL_DestroyWindow(window);
		window = NULL;
		SDL_Quit();
		delete[] screenPixels;
	}
	return 0;
}
#endif





