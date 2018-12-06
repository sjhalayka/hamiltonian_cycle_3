#pragma once

#include "uv_camera.h"
#include "primitives.h"
#include "mesh.h"


#include <cstdlib>
#include <GL/glut.h>


#include <vector>
using std::vector;

#include <string>
using std::string;

#include <sstream>
using std::ostringstream;
using std::istringstream;


void idle_func(void);
void init_opengl(const int &width, const int &height);
void reshape_func(int width, int height);
void display_func(void);
void keyboard_func(unsigned char key, int x, int y);
void mouse_func(int button, int state, int x, int y);
void motion_func(int x, int y);
void passive_motion_func(int x, int y);
void output_to_mesh2(void);

void render_string(int x, const int y, void *font, const string &text);
void draw_objects(void);


vector<vertex_3> vertices;

vertex_3 background_colour(1.0f, 1.0f, 1.0f);
vertex_3 control_list_colour(0.1f, 0.1f, 0.1f);

bool draw_mesh = true;
bool draw_axis = true;
bool draw_control_list = true;
bool smooth_shading = false;

uv_camera main_camera;

GLint win_id = 0;
GLint win_x = 800, win_y = 600;
float camera_w = 4;
float camera_fov = 45;
float camera_x_transform = 0;
float camera_y_transform = 0;
float u_spacer = 0.01;
float v_spacer = 0.5*u_spacer;
float w_spacer = 0.1;
float camera_near = 0.01;
float camera_far = 30.0;

bool lmb_down = false;
bool mmb_down = false;
bool rmb_down = false;
int mouse_x = 0;
int mouse_y = 0;


