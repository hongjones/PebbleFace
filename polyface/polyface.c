/*
 * =====================================================================================
 * 
 *     Modulename:
 *       Filename: polyface.c
 *
 *    Description:  
 *    Corporation:
 * 
 *         Author:  hong jones
 *        Created:  2016年12月11日 17时06分32秒
 *
 * =====================================================================================
 *
 * =====================================================================================
 * 
 *   MODIFICATION HISTORY :
 *    
 *		     DATE :
 *		     DESC :
 * =====================================================================================
 */	
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "maibu_sdk.h"
#include "maibu_res.h"

//#define JH_DEBUG

/*窗口ID, 通过该窗口ID获取窗口句柄*/
static int32_t g_app_mwd_window_id = -1;

// 创建并显示图片图层
int32_t display_layer_bmp(P_Window p_window,uint8_t x, uint8_t y, uint8_t h, uint8_t w,enum GAlign how_to_align,enum GColor black_or_white, int bmp_key)
{

	GBitmap bmp_point;
	P_Layer temp_P_Layer = NULL;
	GRect temp_frame;

	temp_frame.origin.x = x;
	temp_frame.origin.y = y;
	temp_frame.size.h = h;
	temp_frame.size.w = w;

	res_get_user_bitmap(bmp_key, &bmp_point);
	//LayerBitmap layer_bitmap_struct_l = {bmp_point, *temp_p_frame, how_to_align};
	LayerBitmap layer_bitmap_struct_l = {bmp_point, temp_frame, how_to_align};
 	temp_P_Layer = app_layer_create_bitmap(&layer_bitmap_struct_l);
	if(temp_P_Layer != NULL)
	{
		app_layer_set_bg_color(temp_P_Layer, black_or_white);
		return app_window_add_layer(p_window, temp_P_Layer);
	}

	return 0;
}

////////////////////////
//Init window and piclayers
///////
P_Window init_mwd_window()
{
	P_Window p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

	display_layer_bmp(p_window, 0, 0, 128, 128
		, GAlignCenter, GColorWhite, RES_BITMAP_POLY_BK);

	Geometry *geometry[11]; /*创建存储几何图形的指针数组, 几何图形数量不能超过指针数组大小*/
	uint8_t num = 0;
	LayerGeometry layer_geometry;	/*几何图层结构体*/
	memset(geometry, 0, sizeof(geometry));

	/*直线*/
	//Line l = {{0,0}, {128,128}};	//直线的起始点坐标

	/*几何结构体，依次为几何类型、填充类型、填充颜色、具体的几何图结构*/
	//Geometry lg = {GeometryTypeLine, FillOutline, GColorBlack, (void*)&l}; 
	//geometry[num++] = &lg;	//存入指针数组中


	struct date_time t;
	app_service_get_datetime(&t);

	uint8_t x0 = 19; 
	uint8_t y0 = 109; 
	uint8_t h0 = 10; 

	uint8_t y = t.year%100; 
	uint8_t mo = t.mon; 
	uint8_t d = t.mday; 

	uint8_t h = t.hour; 
	uint8_t  mi = t.min; 

	/*多边形*/
	//GPoint ppoint[4] = {{25,15}, {60,15}, {60,105}, {25,105}};//多边形各个点坐标
	GPoint ppoint[12] = { {x0,y0}
			,{x0,y0-(y/10)*h0}, {x0+h0,y0-(y%10)*h0}
			, {x0+2*h0,y0-(mo/10)*h0}, {x0+3*h0,y0-(mo%10)*h0}
			, {x0+4*h0,y0-(d/10)*h0}, {x0+5*h0,y0-(d%10)*h0}
			, {x0+6*h0,y0-(h/10)*h0}, {x0+7*h0,y0-(h%10)*h0}
			, {x0+8*h0,y0-(mi/10)*h0}, {x0+9*h0,y0-(mi%10)*h0}
		,{x0+9*h0,y0} };//多边形各个点坐标
	Polygon p = {12, ppoint};	
	Geometry pg = {GeometryTypePolygon, FillOutline, GColorBlack,(void*)&p}; 
	geometry[num++] = &pg;

	/*圆*/
	//Circle c = {{64,64}, 20};//圆心为{64,64},半径为50	
	//Geometry cg = {GeometryTypeCircle, FillArea, GColorBlack, (void*)&c}; 
	Circle c[10];	
	Geometry cg[10];
	int i;
	for(i=0; i<10; i++) { 
		c[i].center.x = ppoint[i+1].x;
		c[i].center.y = ppoint[i+1].y;
		c[i].radius = 2;
		cg[i].type = GeometryTypeCircle;
		cg[i].fill_type = FillArea;
		cg[i].color = GColorBlack;
		cg[i].element = &c[i];
		geometry[num++] = &cg[i];
	}


	/*初始化几何结构体*/
	layer_geometry.num = num;
	layer_geometry.p_g = geometry;

	/*创建几何图层*/
	P_Layer	 layer = app_layer_create_geometry(&layer_geometry);
	app_window_add_layer(p_window, layer);

	return p_window;
}

//////////////////////////////
//
//重新载入并刷新窗口所有图层
void window_reloading(void)
{
	/*根据窗口ID获取窗口句柄*/
	P_Window p_old_window = app_window_stack_get_window_by_id(g_app_mwd_window_id); 
	if (NULL != p_old_window)
	{
		P_Window p_window = init_mwd_window();
		if (NULL != p_window)
		{
			g_app_mwd_window_id = app_window_stack_replace_window(p_old_window, p_window);
		}	
	}
	
}

/////////////////////////
void app_mwd_watch_time_change(enum SysEventType type, void *context)
{
	//如果系统事件是时间更改
	if (type == SysEventTypeTimeChange)
	{
		window_reloading();
	}
}

int main()
{

#ifdef JH_DEBUG	
	simulator_init();
#endif


	//注册一个事件通知回调，当有时间改变时，立即更新时间
	maibu_service_sys_event_subscribe(app_mwd_watch_time_change);

	/*创建显示时间窗口*/
	P_Window p_window = init_mwd_window();
	/*放入窗口栈显示*/
	g_app_mwd_window_id = app_window_stack_push(p_window);


#ifdef JH_DEBUG	
	simulator_wait();
#endif

	return 0;
}
