#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <algorithm>
using namespace std;

#include "hcycle.h"

#include "string_utilities.h"
using namespace string_utilities;



void populate_globe(void)
{
	std::mt19937 g(0);// static_cast<unsigned int>(time(0)));

	// Break up into countries
	vector<size_t> indices;

	// Get 25 distinct city ids
	get_n_distinct_indices(num_countries, all_cities.size(), indices, g);

	// Fill federal capitols
	for (size_t i = 0; i < num_countries; i++)
		federal_capitol_cities.push_back(all_cities[indices[i]]);

	// Store countries
	for (size_t i = 0; i < num_countries; i++)
	{
		country c;

		c.id = i;
		c.capitol_id = federal_capitol_cities[i].id;

		countries.push_back(c);
	}

	// Allocate room for 25 vectors
	federal_cities.resize(num_countries);

	// For each city in the world
	for (size_t i = 0; i < all_cities.size(); i++)
	{
		float closest_distance = 1e20f;
		size_t closest_country_id = 0;

		// For each country
		for (size_t j = 0; j < countries.size(); j++)
		{
			// Find federal capitol location
			vertex_3 country_centre;
			country_centre.x = all_cities[countries[j].capitol_id].x;
			country_centre.y = all_cities[countries[j].capitol_id].y;

			// Measure distance, finding the shortest distance
			float distance = sqrtf(powf(country_centre.x - all_cities[i].x, 2.0f) + powf(country_centre.y - all_cities[i].y, 2.0f));

			if (distance <= closest_distance)
			{
				closest_distance = distance;
				closest_country_id = j;
			}
		}

		// Populate federal cities breakdown
		federal_cities[closest_country_id].push_back(all_cities[i]);
	}

	// Break up into provinces
	provincial_capitol_cities.resize(num_provinces_per_country);

	// For each country
	for (size_t i = 0; i < num_countries; i++)
	{
		// get 25 distinct incides
		get_n_distinct_indices(num_provinces_per_country, federal_cities[i].size(), indices, g);

		// Fill provincial capitols
		for (size_t j = 0; j < num_provinces_per_country; j++)
			provincial_capitol_cities[i].push_back(federal_cities[i][indices[j]]);

		// Store provinces
		for (size_t j = 0; j < num_provinces_per_country; j++)
		{
			province p;

			p.id = i * num_provinces_per_country + j;
			p.capitol_id = provincial_capitol_cities[i][j].id;

			provinces.push_back(p);
		}
	}

	// alloc room for 25x25 vectors
	provincial_cities.resize(num_countries);

	for (size_t i = 0; i < provincial_cities.size(); i++)
		provincial_cities[i].resize(num_provinces_per_country);

	// For each country
	for (size_t i = 0; i < num_countries; i++)
	{
		// For each city in the country
		for (size_t j = 0; j < federal_cities[i].size(); j++)
		{
			float closest_distance = 1e20f;
			size_t closest_province_id = 0;

			// For each province
			for (size_t k = 0; k < provinces.size(); k++)
			{
				// Find provincial capitol location
				vertex_3 province_centre;
				province_centre.x = all_cities[provinces[k].capitol_id].x;
				province_centre.y = all_cities[provinces[k].capitol_id].y;

				// Measure distance, finding the shortest distance
				float distance = sqrtf(powf(province_centre.x - federal_cities[i][j].x, 2.0f) + powf(province_centre.y - federal_cities[i][j].y, 2.0f));

				if (distance <= closest_distance)
				{
					closest_distance = distance;
					closest_province_id = k;
				}
			}

			// Populate provincial cities breakdown
			// Convert federal_cities[closest_country_id].push_back(all_cities[i]); ...
			provincial_cities[closest_province_id / num_provinces_per_country][closest_province_id % num_provinces_per_country].push_back(federal_cities[i][j]);
		}
	}

	// Get LUTs
	cities_per_country.resize(num_countries);
	country_per_city.resize(all_cities.size());

	provinces_per_country.resize(num_countries);
	country_per_province.resize(num_countries*num_provinces_per_country);

	cities_per_province.resize(num_countries*num_provinces_per_country);
	province_per_city.resize(all_cities.size());

	for (size_t i = 0; i < provincial_cities.size(); i++)
	{
		for (size_t j = 0; j < provincial_cities[i].size(); j++)
		{
			for (size_t k = 0; k < provincial_cities[i][j].size(); k++)
			{
				size_t country_id = i;
				size_t provincial_id = i*num_provinces_per_country + j;
				size_t city_id = provincial_cities[i][j][k].id;

				cities_per_country[country_id].push_back(city_id);
				country_per_city[city_id] = country_id;

				provinces_per_country[country_id].push_back(provincial_id);
				country_per_province[provincial_id] = country_id;

				cities_per_province[provincial_id].push_back(city_id);
				province_per_city[city_id] = provincial_id;
			}
		}
	}


	/*
	// Break up into municipalities
	size_t running_municipality_id = 0;

	municipal_capitol_cities.resize(num_countries);

	for (size_t i = 0; i < municipal_capitol_cities.size(); i++)
		municipal_capitol_cities[i].resize(num_provinces_per_country);

	// For each country
	for (size_t i = 0; i < num_countries; i++)
	{
		// For each province
		for (size_t j = 0; j < num_provinces_per_country; j++)
		{
			// Take into account that the number of cities per municipality 
			// might be less than the maximum
			size_t num_municipalities = provincial_cities[i][j].size();

			if (num_municipalities > max_num_municipalities_per_province)
				num_municipalities = max_num_municipalities_per_province;

			//if (num_municipalities < num_municipalities_per_province)
			//	cout << num_municipalities << endl;

			get_n_distinct_indices(num_municipalities, num_municipalities, indices, g);

			// Fill municipal capitols
			for (size_t k = 0; k < num_municipalities; k++)
				municipal_capitol_cities[i][j].push_back(provincial_cities[i][j][indices[k]]);

			// Store municipalities
			for (size_t k = 0; k < num_municipalities; k++)
			{
				municipality m;

				m.id = running_municipality_id++;
				m.capitol_id = municipal_capitol_cities[i][j][k].id;

				municipalities.push_back(m);
			}
		}
	}

	// alloc room for 25x25xn vectors
	municipal_cities.resize(num_countries);

	for (size_t i = 0; i < municipal_cities.size(); i++)
		municipal_cities[i].resize(num_provinces_per_country);

	for (size_t i = 0; i < municipal_cities.size(); i++)
	{
		for (size_t j = 0; j < municipal_cities[i].size(); j++)
		{
			size_t num_municipalities = provincial_cities[i][j].size();

			municipal_cities[i][j].resize(num_municipalities);
		}
	}


	// For each country
	for (size_t i = 0; i < num_countries; i++)
	{
		// For each province 
		for (size_t j = 0; j < num_provinces_per_country; j++)
		{
			size_t num_municipalities = provincial_cities[i][j].size();

			// For each city in the province
			for (size_t k = 0; k < provincial_cities[i][j].size(); k++)
			{
				float closest_distance = 1e20f;
				size_t closest_municipality_id = 0;

				// For each municipality
				for (size_t l = 0; l < municipalities.size(); l++)
				{
					// Find municipal capitol location
					vertex_3 municipality_centre;
					municipality_centre.x = all_cities[municipalities[l].capitol_id].x;
					municipality_centre.y = all_cities[municipalities[l].capitol_id].y;

					// Measure distance, finding the shortest distance
					float distance = sqrtf(powf(municipality_centre.x - provincial_cities[i][j][k].x, 2.0f) + powf(municipality_centre.y - provincial_cities[i][j][k].y, 2.0f));

					if (distance <= closest_distance)
					{
						closest_distance = distance;
						closest_municipality_id = l;
					}
				}

				// Populate municipal cities breakdown
				// Convert federal_cities[closest_country_id].push_back(all_cities[i]); ...
				//municipal_cities[i][j][closest_municipality_id].push_back(provincial_cities[i][j][k]);
			}
		}
	}

	*/








	country_colours.resize(num_countries);

	for (size_t i = 0; i < country_colours.size(); i++)
	{
		country_colours[i].x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		country_colours[i].y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		country_colours[i].z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	}

	province_colours.resize(num_countries*num_provinces_per_country);

	for (size_t i = 0; i < province_colours.size(); i++)
	{
		province_colours[i].x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		province_colours[i].y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		province_colours[i].z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	}

	/*
	municipality_colours.resize(num_countries*num_provinces_per_country * 200000);

	for (size_t i = 0; i < municipality_colours.size(); i++)
	{
		municipality_colours[i].x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		municipality_colours[i].y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		municipality_colours[i].z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	}
	*/
}






int main(int argc, char **argv)
{
	srand(0);// static_cast<unsigned int>(time(0)));

	ifstream city_file("cities.csv");
	string line;

	// Skip first line
	getline(city_file, line);

	while (getline(city_file, line))
	{
		if ("" == line)
			continue;

		vector<string> tokens = stl_str_tok(",", line);

		if (tokens.size() != 3)
			continue;

		istringstream iss;
		city c;

		iss.str(tokens[0]);
		iss >> c.id;

		iss.clear();
		iss.str(tokens[1]);
		iss >> c.x;

		iss.clear();
		iss.str(tokens[2]);
		iss >> c.y;

		all_cities.push_back(c);
	}

	all_cities.resize(2000);


	populate_globe();


	// for each world, make triangulation and graph for vertices of federal capitols
	
	// convert federal capitols into a vector of vertices
	vector<vertex_3> federal_capitol_vertices;
	
	for (size_t i = 0; i < num_countries; i++)
	{
		vertex_3 v;
		v.x = federal_capitol_cities[i].x;
		v.y = federal_capitol_cities[i].y;
		v.z = 0;

		federal_capitol_vertices.push_back(v);
	}
	
	// write vertices to disk
	ofstream vfile("vertices.txt");

	vfile << "2 rbox " << federal_capitol_vertices.size() << " D2" << endl;
	vfile << federal_capitol_vertices.size() << endl;

	for(size_t i = 0; i < federal_capitol_vertices.size(); i++)
		vfile << federal_capitol_vertices[i].x << " " << federal_capitol_vertices[i].y << endl;

	// run qdelaunay
	system("qdelaunay s i < vertices.txt > triangles.txt");

	// read triangles from disk
	// for each triangle, add graph edges
	vector<bool> g(federal_capitol_vertices.size(), false);
	vector< vector<bool> > graph(federal_capitol_vertices.size(), g);


	ifstream tri_file("triangles.txt");

	// Skip first line
	getline(tri_file, line);

	while (getline(tri_file, line))
	{
		if ("" == line)
			continue;

		vector<string> tokens = stl_str_tok(" ", line);

		if (tokens.size() != 3)
			continue;

		istringstream iss;

		size_t tri0_index = 0;
		size_t tri1_index = 0;
		size_t tri2_index = 0;

		iss.str(tokens[0]);
		iss >> tri0_index;

		iss.clear();
		iss.str(tokens[1]);
		iss >> tri1_index;

		iss.clear();
		iss.str(tokens[2]);
		iss >> tri2_index;

		graph[tri0_index][tri1_index] = true;
		graph[tri1_index][tri0_index] = true;


		graph[tri1_index][tri2_index] = true;
		graph[tri2_index][tri1_index] = true;


		graph[tri2_index][tri0_index] = true;
		graph[tri0_index][tri2_index] = true;


		triangle tri;

		tri.vertex[0] = federal_capitol_vertices[tri0_index];
		tri.vertex[1] = federal_capitol_vertices[tri1_index];
		tri.vertex[2] = federal_capitol_vertices[tri2_index];

		federal_capitol_tris.push_back(tri);
	}

	cout << "triangle count " << federal_capitol_tris.size() << endl;
















	// for each country, make triangulation and graph for vertices of provincial capitols
	// convert provincial capitols into a vector of vector vertices
	// write vertices to disk
	// run qdelaunay
	// read triangles from disk
	// for each triangle, add graph edges

	// For each country, for each province, make triangulation and graph for vertices of cities 



//	vector<bool> g(vertices.size(), false);
//	vector< vector<bool> > graph(vertices.size(), g);







	// qhull s i < vertices.txt > triangles.txt


	//cout << "3 rbox " << vertices.size() << " s D3" << endl;
	//cout << vertices.size() << endl;

	//for(size_t i = 0; i < vertices.size(); i++)
	//	cout << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << endl;

	// return 0;





	glutInit(&argc, argv);
	init_opengl(win_x, win_y);
	glutReshapeFunc(reshape_func);
	glutIdleFunc(idle_func);
	glutDisplayFunc(display_func);
	glutKeyboardFunc(keyboard_func);
	glutMouseFunc(mouse_func);
	glutMotionFunc(motion_func);
	glutPassiveMotionFunc(passive_motion_func);
	//glutIgnoreKeyRepeat(1);
	glutMainLoop();
	glutDestroyWindow(win_id);

	return 0;
}



void idle_func(void)
{
	glutPostRedisplay();
}

void init_opengl(const int &width, const int &height)
{
	win_x = width;
	win_y = height;

	if (win_x < 1)
		win_x = 1;

	if (win_y < 1)
		win_y = 1;

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(win_x, win_y);
	win_id = glutCreateWindow("Binary Stereo Lithography file viewer");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glClearColor(background_colour.x, background_colour.y, background_colour.z, 1);
	glClearDepth(1.0f);

	main_camera.Set(0, 0, camera_w, camera_fov, win_x, win_y, camera_near, camera_far);
}

void reshape_func(int width, int height)
{
	win_x = width;
	win_y = height;

	if (win_x < 1)
		win_x = 1;

	if (win_y < 1)
		win_y = 1;

	glutSetWindow(win_id);
	glutReshapeWindow(win_x, win_y);
	glViewport(0, 0, win_x, win_y);

	main_camera.Set(main_camera.u, main_camera.v, main_camera.w, main_camera.fov, win_x, win_y, camera_near, camera_far);
}

// Text drawing code originally from "GLUT Tutorial -- Bitmap Fonts and Orthogonal Projections" by A R Fernandes
void render_string(int x, const int y, void *font, const string &text)
{
	for (size_t i = 0; i < text.length(); i++)
	{
		glRasterPos2i(x, y);
		glutBitmapCharacter(font, text[i]);
		x += glutBitmapWidth(font, text[i]) + 1;
	}
}
// End text drawing code.




void display_func(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw the model's components using OpenGL/GLUT primitives.
	draw_objects();

	if (true == draw_control_list)
	{
		// Text drawing code originally from "GLUT Tutorial -- Bitmap Fonts and Orthogonal Projections" by A R Fernandes
		// http://www.lighthouse3d.com/opengl/glut/index.php?bmpfontortho
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, win_x, 0, win_y);
		glScalef(1, -1, 1); // Neat. :)
		glTranslatef(0, static_cast<GLfloat>(-win_y), 0); // Neat. :)
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glColor3f(control_list_colour.x, control_list_colour.y, control_list_colour.z);

		int break_size = 22;
		int start = 20;
		ostringstream oss;

		render_string(10, start, GLUT_BITMAP_HELVETICA_18, string("Mouse controls:"));
		render_string(10, start + 1 * break_size, GLUT_BITMAP_HELVETICA_18, string("  LMB + drag: Rotate camera"));
		render_string(10, start + 2 * break_size, GLUT_BITMAP_HELVETICA_18, string("  RMB + drag: Zoom camera"));

		render_string(10, start + 4 * break_size, GLUT_BITMAP_HELVETICA_18, string("Keyboard controls:"));
		render_string(10, start + 5 * break_size, GLUT_BITMAP_HELVETICA_18, string("  q: Draw mesh"));
		render_string(10, start + 6 * break_size, GLUT_BITMAP_HELVETICA_18, string("  w: Draw axis"));
		render_string(10, start + 7 * break_size, GLUT_BITMAP_HELVETICA_18, string("  e: Draw text"));

		oss.clear();
		oss.str("");
		oss << "  s: Smooth shading -- ";

		if (true == smooth_shading)
			oss << "ON";
		else
			oss << "OFF";

		render_string(10, start + 8 * break_size, GLUT_BITMAP_HELVETICA_18, oss.str());

		render_string(10, start + 9 * break_size, GLUT_BITMAP_HELVETICA_18, string("  u: Rotate camera +u"));
		render_string(10, start + 10 * break_size, GLUT_BITMAP_HELVETICA_18, string("  i: Rotate camera -u"));
		render_string(10, start + 11 * break_size, GLUT_BITMAP_HELVETICA_18, string("  o: Rotate camera +v"));
		render_string(10, start + 12 * break_size, GLUT_BITMAP_HELVETICA_18, string("  p: Rotate camera -v"));



		vertex_3 eye = main_camera.eye;
		vertex_3 eye_norm = eye;
		eye_norm.normalize();

		oss.clear();
		oss.str("");
		oss << "Camera position: " << eye.x << ' ' << eye.y << ' ' << eye.z;
		render_string(10, win_y - 2 * break_size, GLUT_BITMAP_HELVETICA_18, oss.str());

		oss.clear();
		oss.str("");
		oss << "Camera position (normalized): " << eye_norm.x << ' ' << eye_norm.y << ' ' << eye_norm.z;
		render_string(10, win_y - break_size, GLUT_BITMAP_HELVETICA_18, oss.str());

		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		// End text drawing code.
	}

	glFlush();
	glutSwapBuffers();
}

void keyboard_func(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'q':
	{
		draw_mesh = !draw_mesh;
		break;
	}
	case 'w':
	{
		draw_axis = !draw_axis;
		break;
	}
	case 'e':
	{
		draw_control_list = !draw_control_list;
		break;
	}
	case 's':
	{
		smooth_shading = !smooth_shading;
		break;
	}
	case 'u':
	{
		main_camera.u -= u_spacer;
		main_camera.Set();
		break;
	}
	case 'i':
	{
		main_camera.u += u_spacer;
		main_camera.Set();
		break;
	}
	case 'o':
	{
		main_camera.v -= v_spacer;
		main_camera.Set();
		break;
	}
	case 'p':
	{
		main_camera.v += v_spacer;
		main_camera.Set();
		break;
	}

	default:
		break;
	}
}

void mouse_func(int button, int state, int x, int y)
{
	if (GLUT_LEFT_BUTTON == button)
	{
		if (GLUT_DOWN == state)
			lmb_down = true;
		else
			lmb_down = false;
	}
	else if (GLUT_MIDDLE_BUTTON == button)
	{
		if (GLUT_DOWN == state)
			mmb_down = true;
		else
			mmb_down = false;
	}
	else if (GLUT_RIGHT_BUTTON == button)
	{
		if (GLUT_DOWN == state)
			rmb_down = true;
		else
			rmb_down = false;
	}
}

void motion_func(int x, int y)
{
	int prev_mouse_x = mouse_x;
	int prev_mouse_y = mouse_y;

	mouse_x = x;
	mouse_y = y;

	int mouse_delta_x = mouse_x - prev_mouse_x;
	int mouse_delta_y = prev_mouse_y - mouse_y;

	if (true == lmb_down && (0 != mouse_delta_x || 0 != mouse_delta_y))
	{
		main_camera.u -= static_cast<float>(mouse_delta_y)*u_spacer;
		main_camera.v += static_cast<float>(mouse_delta_x)*v_spacer;
	}
	else if (true == rmb_down && (0 != mouse_delta_y))
	{
		main_camera.w -= static_cast<float>(mouse_delta_y)*w_spacer;

		if (main_camera.w < 1.1f)
			main_camera.w = 1.1f;
		else if (main_camera.w > 20.0f)
			main_camera.w = 20.0f;
	}

	main_camera.Set(); // Calculate new camera vectors.
}

void passive_motion_func(int x, int y)
{
	mouse_x = x;
	mouse_y = y;
}


