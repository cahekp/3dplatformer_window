/* Copyright (C) 2005-2021, UNIGINE. All rights reserved.
 *
 * This file is a part of the UNIGINE 2 SDK.
 *
 * Your use and / or redistribution of this software in source and / or
 * binary form, with or without modification, is subject to: (i) your
 * ongoing acceptance of and compliance with the terms and conditions of
 * the UNIGINE License Agreement; and (ii) your inclusion of this notice
 * in any version of this software that you use or redistribute.
 * A copy of the UNIGINE License Agreement is available by contacting
 * UNIGINE. at http://unigine.com/
 */

#include "AppWorldLogic.h"
#include <UnigineGame.h>

#include <UnigineWorld.h>
#include <UnigineInput.h>
#include <UnigineVisualizer.h>
#include <UnigineApp.h>
#include <UnigineMathLibGeometry.h>

using namespace Unigine;
using namespace Math;

int AppWorldLogic::init()
{
	mat_ball = World::getNodeByName("material_ball");
	player = Game::getPlayer();
	return 1;
}

int AppWorldLogic::update()
{
	// settings:
	// rect - window size in screen coordinates (x=left, y=top, z=right, w=bottom)
	const ivec4 rect = ivec4(
		ftoi(0.2f * App::getWidth()),	// left
		ftoi(0.2f * App::getHeight()),	// top
		ftoi(0.8f * App::getWidth()),	// right
		ftoi(0.8f * App::getHeight()));	// bottom

	// get input and move material ball
	update_mat_ball();

	// calculate screen size of the material ball (based on BoundBox)
	ivec4 mb_rect = get_mat_ball_rectangle();
	
	// get 5 vectors: to material ball and 4 closest points on window size to material ball
	ivec2 mat_ball_center = ivec2((mb_rect.x + mb_rect.z) / 2, (mb_rect.y + mb_rect.w) / 2);
	vec3 cam_left_border_dir = player->getDirectionFromScreen(rect.x, mat_ball_center.y);
	vec3 cam_right_border_dir = player->getDirectionFromScreen(rect.z, mat_ball_center.y);
	vec3 cam_top_border_dir = player->getDirectionFromScreen(mat_ball_center.x, rect.y);
	vec3 cam_bottom_border_dir = player->getDirectionFromScreen(mat_ball_center.x, rect.w);
	
	// define a plane along which the camera will move
	Vec3 cam_pos = player->getPosition();
	Vec3 plane_point = mat_ball->getPosition();
	vec3 plane_normal = vec3_up; // -player->getDirection(); // uncomment it if you want to try different types of planes!
	
	// move camera
	if (mb_rect.z > rect.z) // out of border: right!
		move_camera_along_plane(cam_pos, cam_right_border_dir, player->getDirectionFromScreen(mb_rect.z, mat_ball_center.y), plane_point, plane_normal);
	else if (mb_rect.x < rect.x) // out of border: left!
		move_camera_along_plane(cam_pos, cam_left_border_dir, player->getDirectionFromScreen(mb_rect.x, mat_ball_center.y), plane_point, plane_normal);
	if (mb_rect.w > rect.w) // out of border: bottom!
		move_camera_along_plane(cam_pos, cam_bottom_border_dir, player->getDirectionFromScreen(mat_ball_center.x, mb_rect.w), plane_point, plane_normal);
	else if (mb_rect.y < rect.y) // out of border: top!
		move_camera_along_plane(cam_pos, cam_top_border_dir, player->getDirectionFromScreen(mat_ball_center.x, mb_rect.y), plane_point, plane_normal);

	// visualizer
	Visualizer::setEnabled(true);
	Visualizer::renderBoundBox(mat_ball->getWorldBoundBox(), mat4_identity, vec4(0,1,0,1));
	Visualizer::renderRectangle(vec4(
		itof(mb_rect.x) / App::getWidth(),
		1.0f - itof(mb_rect.y) / App::getHeight(),
		itof(mb_rect.z) / App::getWidth(),
		1.0f - itof(mb_rect.w) / App::getHeight()),
		vec4(1,1,0,1)); // show material ball bound rectangle
	Visualizer::renderRectangle(vec4(
		itof(rect.x) / App::getWidth(),
		1.0f - itof(rect.y) / App::getHeight(),
		itof(rect.z) / App::getWidth(),
		1.0f - itof(rect.w) / App::getHeight()),
		vec4_white); // show window
	Visualizer::renderSphere(0.01f, translate(cam_pos + cam_left_border_dir), vec4(1,0,0,1));
	Visualizer::renderSphere(0.01f, translate(cam_pos + cam_right_border_dir), vec4(0,1,0,1));
	Visualizer::renderSphere(0.01f, translate(cam_pos + cam_top_border_dir), vec4(0,0,1,1));
	Visualizer::renderSphere(0.01f, translate(cam_pos + cam_bottom_border_dir), vec4(0,1,1,1));

	return 1;
}

void AppWorldLogic::update_mat_ball()
{
	float ifps = Game::getIFps();
	int x = (int)Input::isKeyPressed(Input::KEY_RIGHT) - (int)Input::isKeyPressed(Input::KEY_LEFT);
	int y = (int)Input::isKeyPressed(Input::KEY_UP) - (int)Input::isKeyPressed(Input::KEY_DOWN);
	mat_ball->setPosition(mat_ball->getPosition() - Vec3(3.0f * x * ifps, 3.0f * y * ifps, 0));
}

ivec4 AppWorldLogic::get_mat_ball_rectangle() const
{
	// get 8 world points of material ball's bound box
	BoundBox bb = mat_ball->getWorldBoundBox();
	vec3 points[8];
	bb.getPoints(&points[0], 8);

	// convert 8 world points to screen points
	ivec2 points_i[8];
	for (int i = 0; i < 8; i++)
		player->getScreenPosition(points_i[i].x, points_i[i].y, points[i]);

	// calculate bound rectangle
	int left = INT_MAX;
	int right = INT_MIN;
	int top = INT_MAX;
	int bottom = INT_MIN;
	for (int i = 0; i < 8; i++)
	{
		ivec2 &p = points_i[i];
		if (p.x < left) left = p.x;
		else if (p.x > right) right = p.x;
		if (p.y < top) top = p.y;
		else if (p.y > bottom) bottom = p.y;
	}

	return ivec4(left, top, right, bottom);
}

void AppWorldLogic::move_camera_along_plane(const vec3 &cam_pos, const vec3 &ray_from, const vec3 &ray_to, const vec3 &plane_point, const vec3 &plane_normal)
{
	// calculate offset
	vec3 from_pos;
	vec3 to_pos;
	rayPlaneIntersection(cam_pos, ray_from, plane_point, plane_normal, from_pos);
	rayPlaneIntersection(cam_pos, ray_to, plane_point, plane_normal, to_pos);

	// move camera
	player->setPosition(cam_pos + (to_pos - from_pos));
}
