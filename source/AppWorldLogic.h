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


#ifndef __APP_WORLD_LOGIC_H__
#define __APP_WORLD_LOGIC_H__

#include <UnigineLogic.h>
#include <UnigineStreams.h>
#include <UnigineNode.h>
#include <UniginePlayers.h>

class AppWorldLogic : public Unigine::WorldLogic
{
public:
	int init() override;
	int update() override;

private:
	void update_mat_ball();
	Unigine::Math::ivec4 get_mat_ball_rectangle() const;
	void move_camera_along_plane(
		const Unigine::Math::vec3 &cam_pos,
		const Unigine::Math::vec3 &ray_from,
		const Unigine::Math::vec3 &ray_to,
		const Unigine::Math::vec3 &plane_point,
		const Unigine::Math::vec3 &plane_normal);

	Unigine::NodePtr mat_ball;
	Unigine::PlayerPtr player;
};

#endif // __APP_WORLD_LOGIC_H__
