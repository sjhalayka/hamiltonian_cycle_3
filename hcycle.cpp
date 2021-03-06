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
	std::mt19937 g(static_cast<unsigned int>(time(0)));

	cout << "Getting countries" << endl;

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

	cout << "Getting provinces" << endl;

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

	cout << "Getting provincial cities" << endl;

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
			if (j % 1000 == 0)
				cout << i << " " << j << endl;

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

	cout << "Making LUTs" << endl;

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


	cout << "Getting pseudorandom colours per country and province" << endl;


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
}

void read_triangles(const char *const file_name, const vector<vertex_3> &vertices, vector<triangle> &tris, vector< vector<bool> > &graph)
{
	tris.clear();

	ifstream tri_file("triangles.txt");

	string line;

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
		tri.vertex[0] = vertices[tri0_index];
		tri.vertex[1] = vertices[tri1_index];
		tri.vertex[2] = vertices[tri2_index];

		tris.push_back(tri);
	}
}


void get_triangles_and_edge_matrices(void)
{
	cout << "Getting triangulation for all cities" << endl;

	// for each world make triangulation and graph of vertices of all cities
	// convert cities into a vector of vertices
	vector<vertex_3> all_vertices;

	for (size_t i = 0; i < all_cities.size(); i++)
	{
		vertex_3 v;
		v.x = all_cities[i].x;
		v.y = all_cities[i].y;
		v.z = 0;

		all_vertices.push_back(v);
	}

	// write vertices to disk
	ofstream vfile("vertices.txt");

	vfile << "2 rbox " << all_vertices.size() << " D2" << endl;
	vfile << all_vertices.size() << endl;

	for (size_t i = 0; i < all_vertices.size(); i++)
		vfile << all_vertices[i].x << " " << all_vertices[i].y << endl;

	// run qdelaunay
	system("qdelaunay i < vertices.txt > triangles.txt");

	// read triangles from disk... for each triangle, add graph edges
	vector<bool> all_cities_graph_seed(all_vertices.size(), false);
	//vector< vector<bool> > all_cities_graph(all_vertices.size(), all_cities_graph_seed);
	all_cities_graph.resize(all_vertices.size(), all_cities_graph_seed);

	read_triangles("triangles.txt", all_vertices, all_cities_tris, all_cities_graph);




	cout << "Getting triangulation for federal capitols" << endl;

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
	vfile.close();
	vfile.open("vertices.txt");

	vfile << "2 rbox " << federal_capitol_vertices.size() << " D2" << endl;
	vfile << federal_capitol_vertices.size() << endl;

	for (size_t i = 0; i < federal_capitol_vertices.size(); i++)
		vfile << federal_capitol_vertices[i].x << " " << federal_capitol_vertices[i].y << endl;

	// run qdelaunay
	system("qdelaunay i < vertices.txt > triangles.txt");

	// read triangles from disk... for each triangle, add graph edges
	vector<bool> federal_capitols_graph_seed(federal_capitol_vertices.size(), false);
	//vector< vector<bool> > federal_capitols_graph(federal_capitol_vertices.size(), federal_capitols_graph_seed);
	federal_capitols_graph.resize(federal_capitol_vertices.size(), federal_capitols_graph_seed);

	read_triangles("triangles.txt", federal_capitol_vertices, federal_capitol_tris, federal_capitols_graph);



	cout << "Getting triangulation for provincial capitols" << endl;

	provincial_capitol_tris.resize(num_provinces_per_country);
	provincial_capitols_graph.resize(num_provinces_per_country);

	for (size_t i = 0; i < num_countries; i++)
	{
		vector<vertex_3> provincial_capitol_vertices;

		for (size_t j = 0; j < num_provinces_per_country; j++)
		{
			vertex_3 v;
			v.x = provincial_capitol_cities[i][j].x;
			v.y = provincial_capitol_cities[i][j].y;
			v.z = 0;

			provincial_capitol_vertices.push_back(v);
		}

		// write vertices to disk
		vfile.close();
		vfile.open("vertices.txt");

		vfile << "2 rbox " << provincial_capitol_vertices.size() << " D2" << endl;
		vfile << provincial_capitol_vertices.size() << endl;

		for (size_t j = 0; j < provincial_capitol_vertices.size(); j++)
			vfile << provincial_capitol_vertices[j].x << " " << provincial_capitol_vertices[j].y << endl;

		// run qdelaunay
		system("qdelaunay i < vertices.txt > triangles.txt");

		// read triangles from disk... for each triangle, add graph edges
		vector<bool> provincial_capitols_graph_seed(provincial_capitol_vertices.size(), false);
		//vector< vector<bool> > provincial_capitols_graph(provincial_capitol_vertices.size(), provincial_capitols_graph_seed);
		provincial_capitols_graph[i].resize(provincial_capitol_vertices.size(), provincial_capitols_graph_seed);

		read_triangles("triangles.txt", provincial_capitol_vertices, provincial_capitol_tris[i], provincial_capitols_graph[i]);
	}


	cout << "Getting triangulation for provincial cities" << endl;

	// For each country, for each province, make triangulation and graph for vertices of cities 
	provincial_cities_graph.resize(num_provinces_per_country*num_countries);

	for (size_t i = 0; i < provincial_cities_graph.size(); i++)
		provincial_cities_graph[i].resize(num_provinces_per_country);

	provincial_cities_tris.resize(num_provinces_per_country*num_countries);

	for (size_t i = 0; i < provincial_cities_tris.size(); i++)
		provincial_cities_tris[i].resize(num_provinces_per_country);

	for (size_t i = 0; i < num_countries; i++)
	{
		for (size_t j = 0; j < num_provinces_per_country; j++)
		{
			if (j % 1000 == 0)
				cout << i << " " << j << endl;

			vector<vertex_3> provincial_cities_vertices;

			for (size_t k = 0; k < provincial_cities[i][j].size(); k++)
			{
				vertex_3 v;
				v.x = provincial_cities[i][j][k].x;
				v.y = provincial_cities[i][j][k].y;
				v.z = 0;

				provincial_cities_vertices.push_back(v);
			}

			vector<bool> provincial_cities_graph_seed(provincial_cities_vertices.size(), false);
			provincial_cities_graph[i][j].resize(provincial_cities_vertices.size(), provincial_cities_graph_seed);

			if (provincial_cities_vertices.size() == 0)
			{
				// This should never happen, 
				// since a city can be a federal capitol and 
				// provincial capitol and provincial city,
				// all at the same time.
			}
			if (provincial_cities_vertices.size() == 1)
			{
				triangle tri;
				tri.vertex[0] = provincial_cities_vertices[0];
				tri.vertex[1] = provincial_cities_vertices[0];
				tri.vertex[2] = provincial_cities_vertices[0];

				provincial_cities_tris[i][j].push_back(tri);

				// No graph edges are to be gotten from 1 vertex
			}
			else if (provincial_cities_vertices.size() == 2)
			{
				triangle tri;
				tri.vertex[0] = provincial_cities_vertices[0];
				tri.vertex[1] = provincial_cities_vertices[1];
				tri.vertex[2] = provincial_cities_vertices[1];

				provincial_cities_tris[i][j].push_back(tri);

				provincial_cities_graph[i][j][0][1] = true;
				provincial_cities_graph[i][j][1][0] = true;
			}
			else if (provincial_cities_vertices.size() == 3)
			{
				triangle tri;
				tri.vertex[0] = provincial_cities_vertices[0];
				tri.vertex[1] = provincial_cities_vertices[1];
				tri.vertex[2] = provincial_cities_vertices[2];

				provincial_cities_tris[i][j].push_back(tri);

				provincial_cities_graph[i][j][0][1] = true;
				provincial_cities_graph[i][j][1][0] = true;

				provincial_cities_graph[i][j][1][2] = true;
				provincial_cities_graph[i][j][2][1] = true;

				provincial_cities_graph[i][j][2][0] = true;
				provincial_cities_graph[i][j][0][2] = true;
			}
			else
			{
				// write 4+ vertices to disk
				vfile.close();
				vfile.open("vertices.txt");

				vfile << "2 rbox " << provincial_cities_vertices.size() << " D2" << endl;
				vfile << provincial_cities_vertices.size() << endl;

				for (size_t j = 0; j < provincial_cities_vertices.size(); j++)
					vfile << provincial_cities_vertices[j].x << " " << provincial_cities_vertices[j].y << endl;

				// run qdelaunay
				system("qdelaunay i < vertices.txt > triangles.txt");

				// read triangles from disk... for each triangle, add graph edges

				read_triangles("triangles.txt", provincial_cities_vertices, provincial_cities_tris[i][j], provincial_cities_graph[i][j]);
			}
		}
	}


}

int main(int argc, char **argv)
{
	srand(static_cast<unsigned int>(time(0)));

	cout << "Reading cities.csv" << endl;

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


	get_triangles_and_edge_matrices();


	vector<size_t> cycle(federal_capitol_cities.size(), 0);

	for (size_t i = 0; i < cycle.size(); i++)
		cycle[i] = i;

	cycle.push_back(0);

	//hamCycle(federal_capitols_graph, cycle);


	// Get countries' path
	while (1)
	{
		try_getting_connected_string(cycle, federal_capitols_graph);

		if (true == is_cycle_hamiltonian(cycle, federal_capitols_graph))
			break;
		else
			cout << "Could not find connected string for countries. Retrying." << endl;
	}

	final_path = cycle;

	cycle.pop_back();

	// For each country, A
	// Get two connected bordering countries, B and C

	size_t forward_index = 0;
	size_t backward_index = 0;
	float forward_closest_distance = 1e20f;
	size_t forward_closest_province_id = 0;
	float backward_closest_distance = 1e20f;
	size_t backward_closest_province_id = 0;

	for (size_t n = 0; n < 1/*num_countries*/; n++)
	{
		cout << n << " " << num_countries - 1 << endl;

		if (n == 0)
		{
			forward_index = cycle[1];
			backward_index = cycle[cycle.size() - 1];
		}
		else if(n == num_countries - 1)
		{
			forward_index = cycle[0];
			backward_index = cycle[n - 1];
		}
		else
		{
			forward_index = cycle[n + 1];
			backward_index = cycle[n - 1];
		}

		for (size_t i = 0; i < num_provinces_per_country; i++)
		{
			forward_closest_distance = 1e20f;
			forward_closest_province_id = 0;

			vertex_3 provincial_capitol_centre;

			// For each provincial capitol in A
			for (size_t j = 0; j < provincial_capitol_cities[i].size(); j++)
			{
				// Find forward provincial capitol location
				provincial_capitol_centre.x = provincial_capitol_cities[n][j].x;
				provincial_capitol_centre.y = provincial_capitol_cities[n][j].y;

				// Measure distance, finding the shortest distance
				float forward_distance =
					sqrtf(powf(provincial_capitol_centre.x - federal_capitol_cities[forward_index].x, 2.0f) +
						powf(provincial_capitol_centre.y - federal_capitol_cities[forward_index].y, 2.0f));

				if (forward_distance <= forward_closest_distance)
				{
					forward_closest_distance = forward_distance;
					forward_closest_province_id = j;
				}
			}

			backward_closest_distance = 1e20f;
			backward_closest_province_id = 0;

			// For each provincial capitol in A
			for (size_t j = 0; j < provincial_capitol_cities[i].size(); j++)
			{
				// Find backward provincial capitol location
				provincial_capitol_centre.x = provincial_capitol_cities[n][j].x;
				provincial_capitol_centre.y = provincial_capitol_cities[n][j].y;

				// Measure distance, finding the shortest distance
				float backward_distance =
					sqrtf(powf(provincial_capitol_centre.x - federal_capitol_cities[backward_index].x, 2.0f) +
						powf(provincial_capitol_centre.y - federal_capitol_cities[backward_index].y, 2.0f));

				if (backward_distance <= backward_closest_distance)
				{
					backward_closest_distance = backward_distance;
					backward_closest_province_id = j;
				}
			}
		}

		s.push_back(provinces[backward_closest_province_id].id);

		for (size_t i = 0; i < provincial_capitol_cities[n].size(); i++)
		{
			size_t id = provinces[i].id;

			if (id != provinces[backward_closest_province_id].id && id != provinces[forward_closest_province_id].id)
				s.push_back(id);
		}

		s.push_back(provinces[forward_closest_province_id].id);

		while (1)
		{
			try_getting_connected_string(s, provincial_capitols_graph[n]);

			if (true == is_cycle_connected(s, provincial_capitols_graph[n]))
			{
				cout << "cycle connected" << endl;
				break;
			}
			else
				cout << "Could not find connected string for provinces. Retrying." << endl;
		}

		cout << "done" << endl;

		vertex_3 debug_vertex;
		debug_vertex.x = all_cities[provinces[backward_closest_province_id].capitol_id].x;
		debug_vertex.y = all_cities[provinces[backward_closest_province_id].capitol_id].y;
		debug_vertices.push_back(debug_vertex);

		debug_vertex.x = all_cities[provinces[forward_closest_province_id].capitol_id].x;
		debug_vertex.y = all_cities[provinces[forward_closest_province_id].capitol_id].y;
		debug_vertices.push_back(debug_vertex);


	}
	//for (size_t n = 1; n < num_countries; n++)
	//{
	//	forward_index = cycle[1];
	//	backward_index = cycle[cycle.size() - 1];

	//	for (size_t i = 0; i < num_provinces_per_country; i++)
	//	{
	//		forward_closest_distance = 1e20f;
	//		forward_closest_province_id = 0;

	//		vertex_3 provincial_capitol_centre;

	//		// For each provincial capitol in A
	//		for (size_t j = 0; j < provincial_capitol_cities[i].size(); j++)
	//		{
	//			// Find forward provincial capitol location
	//			provincial_capitol_centre.x = all_cities[provincial_capitol_cities[n][j].id].x;
	//			provincial_capitol_centre.y = all_cities[provincial_capitol_cities[n][j].id].y;

	//			// Measure distance, finding the shortest distance
	//			float forward_distance =
	//				sqrtf(powf(provincial_capitol_centre.x - federal_capitol_cities[forward_index].x, 2.0f) +
	//					powf(provincial_capitol_centre.y - federal_capitol_cities[forward_index].y, 2.0f));

	//			if (forward_distance <= forward_closest_distance)
	//			{
	//				forward_closest_distance = forward_distance;
	//				forward_closest_province_id = j;
	//			}
	//		}

	//		backward_closest_distance = 1e20f;
	//		backward_closest_province_id = 0;

	//		// For each provincial capitol in A
	//		for (size_t j = 0; j < provincial_capitol_cities[i].size(); j++)
	//		{
	//			// Find backward provincial capitol location
	//			provincial_capitol_centre.x = all_cities[provincial_capitol_cities[n][j].id].x;
	//			provincial_capitol_centre.y = all_cities[provincial_capitol_cities[n][j].id].y;poppppppp

	//			// Measure distance, finding the shortest distance
	//			float backward_distance =
	//				sqrtf(powf(provincial_capitol_centre.x - federal_capitol_cities[backward_index].x, 2.0f) +
	//					powf(provincial_capitol_centre.y - federal_capitol_cities[backward_index].y, 2.0f));

	//			if (backward_distance <= backward_closest_distance)
	//			{
	//				backward_closest_distance = backward_distance;
	//				backward_closest_province_id = j;
	//			}
	//		}

	//		// Use these two provincial capitols in A as ends of a string that goes through
	//		// all of the provincial capitols in A.

	//	}
	//}





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
