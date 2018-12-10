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

class municipality
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
vector<municipality> municipalities;

vector<city> all_cities;
vector<vector<city> > federal_cities;
vector<vector<vector<city> > > provincial_cities;
vector<vector<vector<vector<city> > > > municipal_cities;

vector<vertex_3> country_colours;
vector<vertex_3> province_colours;
vector<vertex_3> municipality_colours;

vector<city> federal_capitol_cities;
vector<vector<city> > provincial_capitol_cities;
vector<vector<vector<city> > > municipal_capitol_cities;



//vector<vector<size_t> > cities_per_country;
//vector<size_t> country_per_city;
//
//vector<vector<size_t> > provinces_per_country;
//vector<size_t> country_per_province;
//vector<vector<size_t> > cities_per_province;
//vector<size_t> province_per_city;
//
//vector<vector<size_t> > counties_per_country;
//vector<size_t> country_per_county;
//vector<vector<size_t> > counties_per_province;
//vector<size_t> province_per_county;
//vector<vector<size_t> > cities_per_county;
//vector<size_t> county_per_city;



size_t num_countries = 25;
size_t num_provinces_per_country = 25;
size_t max_num_municipalities_per_province = 25;


void get_n_distinct_indices(size_t n, size_t count, vector<size_t> &out, std::mt19937 &g)
{
	//if (n > count)
	//	return;

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

	glPointSize(1.0f);


	//cout << federal_cities.size() << endl;

		for (size_t i = 0; i < federal_cities.size(); i++)
		{
			glColor3f(country_colours[i].x, country_colours[i].y, country_colours[i].z);
	
			glBegin(GL_POINTS);
	
	//		cout << "  " << federal_cities[i].size() << endl;
	
			for (size_t j = 0; j < federal_cities[i].size(); j++)
				glVertex3f(federal_cities[i][j].x, federal_cities[i][j].y, 0.0f);
	
			glEnd();
		}

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




	//	for (size_t i = 0; i < cities_per_country.size(); i++)
	//	{
	////		cout << cities_per_country[i].size() << endl;
	//		
	//		glColor3f(country_colours[i].x, country_colours[i].y, country_colours[i].z);
	//
	//		glBegin(GL_POINTS);
	//
	//		for (size_t j = 0; j < cities_per_country[i].size(); j++)
	//			glVertex3f(federal_cities[cities_per_country[i][j]].x, federal_cities[cities_per_country[i][j]].y, 0.0f);
	//
	//		glEnd();
	//	}

	//	cout << endl;




	//glPointSize(1.0f);
	//glColor3f(0, 0, 0);

	//glBegin(GL_POINTS);

	//for (size_t i = 0; i < cities.size(); i++)
	//	glVertex3f(cities[i].x, cities[i].y, 0.0f);

	//glEnd();

	glPointSize(10.0f);
	glColor3f(0.0f, 0.0f, 0.0f);

	glBegin(GL_POINTS);

	for (size_t i = 0; i < countries.size(); i++)
		glVertex3f(federal_capitol_cities[i].x, federal_capitol_cities[i].y, 0.0f);

	glEnd();


	glPointSize(5.0f);
	glColor3f(0.0f, 0.0f, 0.0f);

	glBegin(GL_POINTS);

	for (size_t i = 0; i < num_countries; i++)
		for (size_t j = 0; j < num_provinces_per_country; j++)
			glVertex3f(provincial_capitol_cities[i][j].x, provincial_capitol_cities[i][j].y, 0.0f);

	glEnd();


	glPointSize(2.0f);
	glColor3f(0.0f, 0.0f, 0.0f);

	glBegin(GL_POINTS);

	for (size_t i = 0; i < num_countries; i++)
		for (size_t j = 0; j < num_provinces_per_country; j++)
			for (size_t k = 0; k < provincial_cities[i][j].size(); k++)
				glVertex3f(municipal_capitol_cities[i][j][k].x, municipal_capitol_cities[i][j][k].y, 0.0f);

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
