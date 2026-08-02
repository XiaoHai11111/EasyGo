/**
 * @file    Scene3DModel.h
 * @brief   3D 场景页数据模型定义
 * @details 本文件定义 EasyGo 的3D 场景页数据模型定义，遵循 Page/View/Model 分层架构，固件与模拟器共用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#ifndef __SCENE3D_MODEL_H
#define __SCENE3D_MODEL_H

#include "App/Utils/AccountSystem/Account.h"
#include "lvgl.h"

#include "type.h"
#include "mesh_cube.h"


namespace Page
{
	class Scene3DModel
	{
	private:
        Account* account;

        Matrix4 m_world;
		Vector3i mesh_rotation = { 0, 0, 0 };
		Vector3i mesh_position = { 0, 0, 0 };

		int proj_nodes[NODECOUNT][2];    // projected nodes (x,y)

	public:
		void draw_vertex( uint16_t color, lv_obj_t* canvas);
		void draw_wireframe( uint16_t color, lv_obj_t* canvas);
		void draw_flat_color(const int(*n)[2], uint16_t color, lv_obj_t* canvas);
		void clear_dirty(const int(*n)[2], lv_obj_t* canvas);

        void  Init();
		void Update(lv_obj_t* canvas);
	};
}

#endif
