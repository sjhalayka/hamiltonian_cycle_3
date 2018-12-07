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
	size_t capitol_id; // city closest to barycentre
};

class county
{
public:
	size_t id;
	size_t capitol_id; // city closest to barycentre
};

class city
{
public:
	size_t id;
	float x, y;
};


vector<country> countries;
vector<province> provinces;
vector<county> counties;
vector<city> cities;

vector<vector<size_t> > cities_per_country;
vector<size_t> country_per_city;

vector<vector<size_t> > provinces_per_country;
vector<size_t> country_per_province;
vector<vector<size_t> > cities_per_province;
vector<size_t> province_per_city;

vector<vector<size_t> > counties_per_country;
vector<size_t> country_per_county;
vector<vector<size_t> > counties_per_province;
vector<size_t> province_per_county;
vector<vector<size_t> > cities_per_county;
vector<size_t> county_per_city;

vector<vertex_3> country_colours;
vector<city> federal_capitol_cities;

size_t num_countries = 25;
size_t num_provinces_per_country = 25;
size_t num_counties_per_province = 25;


void get_n_distinct_city_indices(size_t n, vector<size_t> &out, std::mt19937 &g)
{
	std::uniform_int_distribution<int> d(0, static_cast<unsigned int>(cities.size() - 1));

	set<size_t> numbers;

	// make sure there's no duplicates!!!
	while (numbers.size() < n)
		numbers.insert(d(g));

	out.clear();

	for (set<size_t>::const_iterator ci = numbers.begin(); ci != numbers.end(); ci++)
		out.push_back(*ci);
}


void populate_globe(void)
{
	vector<size_t> indices;

	std::mt19937 g(static_cast<unsigned int>(time(0)));

	get_n_distinct_city_indices(num_countries, indices, g);

	for (size_t i = 0; i < num_countries; i++)
		federal_capitol_cities.push_back(cities[indices[i]]);

	for (size_t i = 0; i < num_countries; i++)
	{
		country c;

		c.id = i;
		c.capitol_id = federal_capitol_cities[i].id;

		countries.push_back(c);
	}

	cities_per_country.resize(num_countries);
	country_per_city.resize(cities.size());
	country_colours.resize(num_countries);

	for (size_t i = 0; i < country_colours.size(); i++)
	{
		country_colours[i].x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		country_colours[i].y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		country_colours[i].z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	}

	for (size_t i = 0; i < cities.size(); i++)
	{
		float closest_distance = 1e20f;
		size_t closest_country_id = 0;

		for (size_t j = 0; j < countries.size(); j++)
		{
			vertex_3 country_centre;
			country_centre.x = cities[countries[j].capitol_id].x;
			country_centre.y = cities[countries[j].capitol_id].y;

			float distance = sqrtf(powf(country_centre.x - cities[i].x, 2.0f) + powf(country_centre.y - cities[i].y, 2.0f));

			if (distance <= closest_distance)
			{
				closest_distance = distance;
				closest_country_id = j;
			}
		}

		cities_per_country[closest_country_id].push_back(cities[i].id);
		country_per_city[i] = closest_country_id;
	}

	




	vector<vector<size_t> > cities_per_province;
	cities_per_province.resize(num_provinces_per_country);

	vector<vector<size_t> > cities_per_county;
	cities_per_province.resize(num_counties_per_province);


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
void draw_objects(void);

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