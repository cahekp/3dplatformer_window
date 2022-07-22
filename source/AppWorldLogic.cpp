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
#include <UnigineNode.h>
#include <UniginePlayers.h>
#include <UnigineWorld.h>
#include <UnigineInput.h>
#include <UnigineVisualizer.h>
#include <UnigineApp.h>
#include <UnigineMathLibGeometry.h>

using namespace Unigine;
using namespace Math;

namespace
{
	NodePtr mat_ball;
	PlayerPtr player;
}

int AppWorldLogic::init()
{
	mat_ball = World::getNodeByName("material_ball");
	player = Game::getPlayer();
	return 1;
}

int AppWorldLogic::update()
{
	float ifps = Game::getIFps();
	int x = (int)Input::isKeyPressed(Input::KEY_RIGHT) - (int)Input::isKeyPressed(Input::KEY_LEFT);
	int y = (int)Input::isKeyPressed(Input::KEY_UP) - (int)Input::isKeyPressed(Input::KEY_DOWN);
	mat_ball->setPosition(mat_ball->getPosition() - Vec3(3.0f * x * ifps, 3.0f * y * ifps, 0));

	vec4 rect = vec4(0.2f, 0.2f, 0.8f, 0.8f);
	Visualizer::setEnabled(true);
	Visualizer::renderRectangle(rect, vec4_white);

	int screen_x, screen_y;
	player->getScreenPosition(screen_x, screen_y, mat_ball->getPosition());
	Vec2 n = Vec2(itof(screen_x) / App::getWidth(), itof(screen_y) / App::getHeight());
	
	vec3 cam_to_mat_ball_dir = player->getDirectionFromScreen(screen_x, screen_y);
	
	vec3 cam_left_border_dir = player->getDirectionFromScreen(rect.x * App::getWidth(), screen_y);
	vec3 cam_right_border_dir = player->getDirectionFromScreen(rect.z * App::getWidth(), screen_y);
	vec3 cam_top_border_dir = player->getDirectionFromScreen(screen_x, rect.y * App::getHeight());
	vec3 cam_bottom_border_dir = player->getDirectionFromScreen(screen_x, rect.w * App::getHeight());
	
	//Visualizer::renderSphere(0.01f, translate(player->getPosition() + cam_left_border_dir), vec4_green);
	//Visualizer::renderSphere(0.01f, translate(player->getPosition() + cam_right_border_dir), vec4_red);
	//Visualizer::renderSphere(0.01f, translate(player->getPosition() + cam_top_border_dir), vec4_blue);
	//Visualizer::renderSphere(0.01f, translate(player->getPosition() + cam_bottom_border_dir), vec4(1,1,0,1));

	auto get_ray_plane_intersecton = []
		(const Vec3 &ray_origin, const vec3 &ray_vector,
		 const Vec3 &plane_point, const vec3 &plane_normal) -> Vec3
	{
			return ray_origin -
				Vec3(ray_vector) * dot(ray_origin - plane_point, Vec3(plane_normal)) /
				dot(ray_vector, plane_normal);
	};
	Vec3 cam_pos = player->getPosition();
	Vec3 plane_point = mat_ball->getPosition();
	vec3 plane_normal = -player->getDirection(); // vec3_up;
	vec3 ray_from =
		n.x > rect.z ? cam_right_border_dir :
		n.x < rect.x ? cam_left_border_dir :
		n.y > rect.w ? cam_bottom_border_dir :
		n.y < rect.y ? cam_top_border_dir :
		cam_to_mat_ball_dir; // no move
	
	Vec3 old_intersection; rayPlaneIntersection(cam_pos, ray_from, plane_point, plane_normal, old_intersection);
	Vec3 new_intersection; rayPlaneIntersection(cam_pos, cam_to_mat_ball_dir, plane_point, plane_normal, new_intersection);
	
	player->setPosition(cam_pos + (new_intersection - old_intersection));

	return 1;
}

int AppWorldLogic::shutdown()
{
	return 1;
}

