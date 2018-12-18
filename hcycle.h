#pragma once

#include "uv_camera.h"
#include "primitives.h"



#include <cstdlib>
#include <GL/glut.h>


#include <vector>
using std::vector;

#include <set>
using std::set;


#include <string>
using std::string;

#include <sstream>
using std::ostringstream;
using std::istringstream;


#include <random>
using std::mt19937;


class country
{
public:
	size_t id;
	size_t capitol_id;
};

class province
{
public:
	size_t id;
	size_t capitol_id;
};

class city
{
public:
	size_t id;
	float x, y;
};


vector<country> countries;
vector<province> provinces;

vector<city> all_cities; // cities per world
vector<vector<city> > federal_cities; // cities per country
vector<vector<vector<city> > > provincial_cities; // cities per province

vector<city> federal_capitol_cities;
vector<vector<city> > provincial_capitol_cities;

vector<vertex_3> country_colours;
vector<vertex_3> province_colours;

vector<vector<size_t> > cities_per_country;
vector<size_t> country_per_city;	

vector<vector<size_t> > provinces_per_country;
vector<size_t> country_per_province;
vector<vector<size_t> > cities_per_province;
vector<size_t> province_per_city;

vector<triangle> all_cities_tris;
vector<triangle> federal_capitol_tris;
vector<vector<triangle> > provincial_capitol_tris;
vector<vector<vector<triangle> > > provincial_cities_tris;

vector<vector<bool> > all_cities_graph;
vector<vector<bool> > federal_capitols_graph;
vector<vector<vector<bool> > > provincial_capitols_graph;
vector<vector<vector<vector<bool> > > > provincial_cities_graph;



vector<vertex_3> debug_vertices;


vector<size_t> s;


vector<size_t> final_path;

size_t num_countries = 10;// 60;
size_t num_provinces_per_country = 10;// 60;



















									  /*
									  * check if the vertex v can be added at index 'pos' in the Hamiltonian Cycle
									  */

bool isSafe(int v, vector<vector<bool> > graph, size_t path[], int pos)
{
	if (graph[path[pos - 1]][v] == 0)
		return false;

	for (int i = 0; i < pos; i++)
		if (path[i] == v)
			return false;

	return true;
}



/* solve hamiltonian cycle problem */
bool hamCycleUtil(vector<vector<bool> > graph, size_t path[], int pos)
{
	cout << "hamCycleUtil begin" << endl;

	const size_t V = graph[0].size();

	if (pos == V)
	{
		if (graph[path[pos - 1]][path[0]] == 1)
			return true;
		else
			return false;
	}

	for (int v = 1; v < V; v++)
	{
		if (isSafe(v, graph, path, pos))
		{
			path[pos] = v;

			if (hamCycleUtil(graph, path, pos + 1) == true)
				return true;

			path[pos] = -1;
		}
	}

	cout << "hamCycleUtil end" << endl;

	return false;
}

void printSolution(size_t path[], const size_t V)
{
	cout << "Solution Exists:";
	cout << " Following is one Hamiltonian Cycle \n" << endl;

	for (int i = 0; i < V; i++)
		cout << path[i] << "  ";

	cout << path[0] << endl;
}


/* solves the Hamiltonian Cycle problem using Backtracking.*/
bool hamCycle(vector<vector<bool> > graph, vector<size_t> &path)
{
	const size_t V = graph[0].size();

	path.resize(V);

	for (size_t i = 0; i < V; i++)
		path[i] = -1;

	path[0] = 0;

	if (hamCycleUtil(graph, &path[0], 1) == false)
	{
		cout << "\nSolution does not exist" << endl;

		return false;
	}

	printSolution(&path[0], V);

	return true;
}


bool are_connected(size_t index0, size_t index1, const vector< vector<bool> > &graph)
{
//	cout << "indices: " <<  index0 << " " << index1 << endl;

	//cout << graph.size() << endl;

	//for (size_t i = 0; i < graph[0].size(); i++)
	//	cout << graph[i].size() << ' ';

	//cout << endl;


	if (graph[index0][index1] && graph[index1][index0])
		return true;

	return false;
}

bool is_cycle_connected(const vector<size_t> &cycle, const vector< vector<bool> > &graph)
{
	if (cycle.size() < 2)
		return false;

	for (size_t i = 0; i < cycle.size() - 1; i++)
	{
		bool conn = are_connected(cycle[i], cycle[i + 1], graph);

		if (false == conn)
			return false;
	}

	return true;
}

bool is_cycle_hamiltonian(const vector<size_t> &cycle, const vector< vector<bool> > &graph)
{
	if (cycle.size() < 2)
		return false;

	if (cycle[0] != 0 || cycle[cycle.size() - 1] != 0)
		return false;

	for (size_t i = 0; i < cycle.size() - 1; i++)
	{
		bool conn = are_connected(cycle[i], cycle[i + 1], graph);

		if (false == conn)
			return false;
	}

	return true;
}


void try_getting_connected_string(vector<size_t> &cycle, const vector< vector<bool> > &graph)
{
	if (cycle.size() < 3)
	{
		cout << "Invalid cycle" << endl;
		return;
	}

	random_shuffle(cycle.begin() + 1, cycle.end() - 1);

	for (size_t i = 0; i < cycle.size() - 2; i++)
	{
		if (false == are_connected(cycle[i], cycle[i + 1], graph))
		{
			vector<size_t> swaps;

			for (size_t j = i + 2; j < cycle.size() - 1; j++)
				if (true == are_connected(cycle[i], cycle[j], graph))
					swaps.push_back(j);

			if (swaps.size() == 0)
			{
				return;
			}
			else
			{
				random_shuffle(swaps.begin(), swaps.end());
				size_t temp = cycle[i + 1];
				cycle[i + 1] = cycle[swaps[0]];
				cycle[swaps[0]] = temp;
			}
		}
	}
}


void get_n_distinct_indices(size_t n, size_t count, vector<size_t> &out, std::mt19937 &g)
{
	if (n > count)
		n = count;

	std::uniform_int_distribution<int> d(0, static_cast<unsigned int>(count - 1));

	set<size_t> numbers;

	// make sure there are no duplicates
	while (numbers.size() < n)
		numbers.insert(d(g));

	out.clear();

	for (set<size_t>::const_iterator ci = numbers.begin(); ci != numbers.end(); ci++)
		out.push_back(*ci);
}





void idle_func(void);
void init_opengl(const int &width, const int &height);
void reshape_func(int width, int height);
void display_func(void);
void keyboard_func(unsigned char key, int x, int y);
void mouse_func(int button, int state, int x, int y);
void motion_func(int x, int y);
void passive_motion_func(int x, int y);

void render_string(int x, const int y, void *font, const string &text);



vertex_3 background_colour(1.0f, 1.0f, 1.0f);
vertex_3 control_list_colour(0.1f, 0.1f, 0.1f);

bool draw_mesh = true;
bool draw_axis = true;
bool draw_control_list = true;
bool smooth_shading = false;

uv_camera main_camera;

GLint win_id = 0;
GLint win_x = 1024, win_y = 768;
float camera_w = 5000;
float camera_fov = 45;
float camera_x_transform = -2500;
float camera_y_transform = -1500;
float u_spacer = 0.01f;
float v_spacer = 0.5f*u_spacer;
float w_spacer = 0.1f;
float camera_near = 1.0f;
float camera_far = 20000.0f;

bool lmb_down = false;
bool mmb_down = false;
bool rmb_down = false;
int mouse_x = 0;
int mouse_y = 0;



void draw_objects(void)
{
	glDisable(GL_LIGHTING);

	glPushMatrix();

	glTranslatef(camera_x_transform, camera_y_transform, 0);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPointSize(20.0f);

	glBegin(GL_POINTS);

	glColor4f(1.0f, 0.5f, 0.0f, 1.0f);

	for (size_t i = 0; i < debug_vertices.size(); i++)
		glVertex3f(debug_vertices[i].x, debug_vertices[i].y, debug_vertices[i].z);

	glEnd();









	glLineWidth(1.0f);

	glBegin(GL_LINES);

	glColor4f(0.0f, 0.0f, 0.0f, 0.5f);

	for (size_t i = 0; i < federal_capitol_tris.size(); i++)
	{
		glVertex3f(federal_capitol_tris[i].vertex[0].x, federal_capitol_tris[i].vertex[0].y, federal_capitol_tris[i].vertex[0].z);
		glVertex3f(federal_capitol_tris[i].vertex[1].x, federal_capitol_tris[i].vertex[1].y, federal_capitol_tris[i].vertex[1].z);

		glVertex3f(federal_capitol_tris[i].vertex[1].x, federal_capitol_tris[i].vertex[1].y, federal_capitol_tris[i].vertex[1].z);
		glVertex3f(federal_capitol_tris[i].vertex[2].x, federal_capitol_tris[i].vertex[2].y, federal_capitol_tris[i].vertex[2].z);

		glVertex3f(federal_capitol_tris[i].vertex[2].x, federal_capitol_tris[i].vertex[2].y, federal_capitol_tris[i].vertex[2].z);
		glVertex3f(federal_capitol_tris[i].vertex[0].x, federal_capitol_tris[i].vertex[0].y, federal_capitol_tris[i].vertex[0].z);

	}

	glEnd();







	glBegin(GL_LINE_STRIP);

	glLineWidth(2);

	glColor3f(0.0f, 0.5f, 1.0f);

	for (size_t i = 0; i < s.size(); i++)
	{
		city c = provincial_capitol_cities[0][s[i]];
		glVertex3f(c.x, c.y, 0);
	}

	glEnd();



	//glLineWidth(1.0f);

	//glBegin(GL_LINES);

	//glColor3f(0.0f, 0.0f, 0.0f);

	//for (size_t i = 0; i < provincial_capitol_tris.size(); i++)
	//{
	//	for (size_t j = 0; j < provincial_capitol_tris[i].size(); j++)
	//	{
	//		glVertex3f(provincial_capitol_tris[i][j].vertex[0].x, provincial_capitol_tris[i][j].vertex[0].y, provincial_capitol_tris[i][j].vertex[0].z);
	//		glVertex3f(provincial_capitol_tris[i][j].vertex[1].x, provincial_capitol_tris[i][j].vertex[1].y, provincial_capitol_tris[i][j].vertex[1].z);

	//		glVertex3f(provincial_capitol_tris[i][j].vertex[1].x, provincial_capitol_tris[i][j].vertex[1].y, provincial_capitol_tris[i][j].vertex[1].z);
	//		glVertex3f(provincial_capitol_tris[i][j].vertex[2].x, provincial_capitol_tris[i][j].vertex[2].y, provincial_capitol_tris[i][j].vertex[2].z);

	//		glVertex3f(provincial_capitol_tris[i][j].vertex[2].x, provincial_capitol_tris[i][j].vertex[2].y, provincial_capitol_tris[i][j].vertex[2].z);
	//		glVertex3f(provincial_capitol_tris[i][j].vertex[0].x, provincial_capitol_tris[i][j].vertex[0].y, provincial_capitol_tris[i][j].vertex[0].z);
	//	}
	//}

	//glEnd();





	//glEnable(GL_BLEND);

	//glLineWidth(1.0f);

	//glBegin(GL_LINES);

	//glColor4f(0.0f, 0.0f, 0.0f, 0.5f);

	//for (size_t i = 0; i < provincial_cities_tris.size(); i++)
	//{
	//	for (size_t j = 0; j < provincial_cities_tris[i].size(); j++)
	//	{
	//		for (size_t k = 0; k < provincial_cities_tris[i][j].size(); k++)
	//		{
	//			glVertex3f(provincial_cities_tris[i][j][k].vertex[0].x, provincial_cities_tris[i][j][k].vertex[0].y, provincial_cities_tris[i][j][k].vertex[0].z);
	//			glVertex3f(provincial_cities_tris[i][j][k].vertex[1].x, provincial_cities_tris[i][j][k].vertex[1].y, provincial_cities_tris[i][j][k].vertex[1].z);

	//			glVertex3f(provincial_cities_tris[i][j][k].vertex[1].x, provincial_cities_tris[i][j][k].vertex[1].y, provincial_cities_tris[i][j][k].vertex[1].z);
	//			glVertex3f(provincial_cities_tris[i][j][k].vertex[2].x, provincial_cities_tris[i][j][k].vertex[2].y, provincial_cities_tris[i][j][k].vertex[2].z);

	//			glVertex3f(provincial_cities_tris[i][j][k].vertex[2].x, provincial_cities_tris[i][j][k].vertex[2].y, provincial_cities_tris[i][j][k].vertex[2].z);
	//			glVertex3f(provincial_cities_tris[i][j][k].vertex[0].x, provincial_cities_tris[i][j][k].vertex[0].y, provincial_cities_tris[i][j][k].vertex[0].z);
	//		}
	//	}
	//}

	//glEnd();




	size_t country_colour_index = 0;

	for (size_t i = 0; i < 1/*countries.size()*/; i++)
	{
		glColor3f(country_colours[country_colour_index].x, country_colours[country_colour_index].y, country_colours[country_colour_index].z);
		country_colour_index++;


		glPointSize(10.0f);

		glBegin(GL_POINTS);

		city c = all_cities[countries[i].capitol_id];
		glVertex3f(c.x, c.y, 0);

		glEnd();

		glPointSize(2.0f);

		glBegin(GL_POINTS);

		for (size_t j = 0; j < cities_per_country[i].size(); j++)
		{
			city c = all_cities[cities_per_country[i][j]];
			glVertex3f(c.x, c.y, 0);
		}

		glEnd();


		size_t provincial_colour_index = 0;

		for (size_t j = 0; j < provinces_per_country[i].size(); j++)
		{
			glColor4f(province_colours[provincial_colour_index].x, province_colours[provincial_colour_index].y, province_colours[provincial_colour_index].z, 1.0f);
			provincial_colour_index++;

			size_t province_index = provinces_per_country[i][j];

			glPointSize(5.0f);
			
			glBegin(GL_POINTS);

			city c = all_cities[provinces[province_index].capitol_id];
			glVertex3f(c.x, c.y, 0);

			glEnd();

			glPointSize(1.0f);

			glBegin(GL_POINTS);

			//for (size_t k = 0; k < cities_per_province[province_index].size(); k++)
			//{
			//	city c = all_cities[cities_per_province[province_index][k]];
			//	glVertex3f(c.x, c.y, 0);
			//}

			glEnd();
		}
	}

	glEnd();




	//glPointSize(5.0f);

	//glBegin(GL_POINTS);

	//size_t colour_index = 0;

	//for (size_t i = 0; i < provinces.size(); i++)
	//{
	//	glColor3f(province_colours[colour_index].x, province_colours[colour_index].y, province_colours[colour_index].z);
	//	colour_index++;

	//	city c = all_cities[provinces[i].capitol_id];
	//	glVertex3f(c.x, c.y, 0);
	//}

	//glEnd();





	//glPointSize(1.0f);

	//glColor3f(1.0f, 0.5f, 0.0f);

	//glBegin(GL_POINTS);

	//for (size_t i = 0; i < all_cities.size(); i++)
	//	glVertex3f(all_cities[i].x, all_cities[i].y, 0.0f);

	//glEnd();



	//glPointSize(1.0f);

	//for (size_t i = 0; i < federal_cities.size(); i++)
	//{
	//	glColor3f(country_colours[i].x, country_colours[i].y, country_colours[i].z);

	//	glBegin(GL_POINTS);

	//	//		cout << "  " << federal_cities[i].size() << endl;

	//	for (size_t j = 0; j < federal_cities[i].size(); j++)
	//		glVertex3f(federal_cities[i][j].x, federal_cities[i][j].y, 0.0f);

	//	glEnd();
	//}




	//glPointSize(1.0f);

	//size_t colour_index = 0;

	//for (size_t i = 0; i < provincial_cities.size(); i++)
	//{
	//	for (size_t j = 0; j < provincial_cities[i].size(); j++)
	//	{
	//		glColor3f(province_colours[colour_index].x, province_colours[colour_index].y, province_colours[colour_index].z);
	//		colour_index++;

	//		glBegin(GL_POINTS);

	//		for (size_t k = 0; k < provincial_cities[i][j].size(); k++)
	//			glVertex3f(provincial_cities[i][j][k].x, provincial_cities[i][j][k].y, 0.0f);

	//		glEnd();
	//	}
	//}



	//glPointSize(4.0f);

	//size_t colour_index = 0;

	//for (size_t i = 0; i < municipal_cities.size(); i++)
	//{
	//	for (size_t j = 0; j < municipal_cities[i].size(); j++)
	//	{
	//		for (size_t k = 0; k < municipal_cities[i][j].size(); k++)
	//		{
	//			//cout << municipal_cities[i][j][k].size() << endl;

	//			glColor3f(municipality_colours[colour_index].x, municipality_colours[colour_index].y, municipality_colours[colour_index].z);
	//			colour_index++;

	//			glBegin(GL_POINTS);

	//			for (size_t l = 0; l < municipal_cities[i][j][k].size(); l++)
	//			{

	//				glVertex3f(municipal_cities[i][j][k][l].x, municipal_cities[i][j][k][l].y, 0.0f);
	//			}

	//			glEnd();
	//		}
	//	}
	//}




	//glPointSize(10.0f);
	//glColor3f(0.0f, 0.0f, 0.0f);

	//glBegin(GL_POINTS);

	//for (size_t i = 0; i < federal_capitol_cities.size(); i++)
	//	glVertex3f(federal_capitol_cities[i].x, federal_capitol_cities[i].y, 0.0f);

	//glEnd();

	//colour_index = 0;

	//glPointSize(5.0f);

	//glBegin(GL_POINTS);

	//for (size_t i = 0; i < provincial_capitol_cities.size(); i++)
	//{
	//	for (size_t j = 0; j < provincial_capitol_cities[i].size(); j++)
	//	{
	//		glColor3f(province_colours[colour_index].x, province_colours[colour_index].y, province_colours[colour_index].z);
	//		colour_index++;

	//		glVertex3f(provincial_capitol_cities[i][j].x, provincial_capitol_cities[i][j].y, 0.0f);
	//	}
	//}

	//glEnd();


	//glPointSize(2.0f);
	//glColor3f(0.0f, 0.0f, 0.0f);

	//glBegin(GL_POINTS);

	//for (size_t i = 0; i < municipal_capitol_cities.size(); i++)
	//	for (size_t j = 0; j < municipal_capitol_cities[i].size(); j++)
	//		for (size_t k = 0; k < municipal_capitol_cities[i][j].size(); k++)
	//			glVertex3f(municipal_capitol_cities[i][j][k].x, municipal_capitol_cities[i][j][k].y, 0.0f);

	//glEnd();








	glLineWidth(2.0f);

	glBegin(GL_LINE_LOOP);

	glColor3f(1.0f, 0.5f, 0.0f);

	for (size_t i = 0; i < final_path.size(); i++)
	{
		city c = federal_capitol_cities[final_path[i]];
		glVertex3f(c.x, c.y, 0);
	}

	glEnd();







	// If we do draw the axis at all, make sure not to draw its outline.
	if (true == draw_axis)
	{
		glLineWidth(1.0f);

		glBegin(GL_LINES);

		glColor3f(1, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(1, 0, 0);
		glColor3f(0, 1, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 1, 0);
		glColor3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 1);

		glColor3f(0.5, 0.5, 0.5);
		glVertex3f(0, 0, 0);
		glVertex3f(-1, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, -1, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, -1);

		glEnd();
	}

	glPopMatrix();
}