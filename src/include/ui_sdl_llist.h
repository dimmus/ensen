/**
	Build custom linked list, push new element, clear and print items
	@author Bertrand Martel
*/
#ifndef UI_SDL_LLIST_H
#define UI_SDL_LLIST_H

#include "SDL2/SDL.h"

typedef struct _caption_item caption_item;
struct _caption_item
{
	char  *caption_txt;
	int    caption_id;
	int    caption_color;
	caption_item *nxt;
};
typedef caption_item *captionlist;

typedef struct _coordinate_item coordinate_item;
struct _coordinate_item
{
	float  x;
	float  y;
	int    caption_id;
	coordinate_item *nxt;
};
typedef coordinate_item *coordlist;

typedef struct _surface_item surface_item;
struct _surface_item
{
	SDL_Surface  *surface;
	surface_item *nxt;
};
typedef surface_item *surfacelist;

/**
 * @brief add_point: push a new item to the end of coordinate table
 * @param list list of coordinate items
 * @param x x coordinate
 * @param y y coordinate
 */
coordlist add_point(coordlist list,int caption_id, float x,float y);

/**
 * @brief print_list_coord print coordinate table
 * @param list list of coordinate items
 */
void print_list_coord(coordlist list);

/**
 * @brief clear_coord clear coordinate table
 * @param list list of coordinate items
 */
coordlist clear_coord(coordlist list);

/**
 * @brief add_caption push a new item to the end of caption table
 * @param list list of caption items
 * @param caption_txt caption text
 * @param caption_id caption id
 * @param color caption color
 */
captionlist add_caption(captionlist list, char* caption_txt,int caption_id,int color);

/**
 * @brief print_list_caption print caption table
 * @param list list of caption items
 */
void print_list_caption(captionlist list);

/**
 * @brief clear_caption clear caption table
 * @param list list of caption items
 */
captionlist clear_caption(captionlist list);

/**
 * @brief push_back_surface push a new item to the end of surface table
 * @param list list of surface items
 * @param surface SDL surface ptr
 */
surfacelist push_back_surface(surfacelist list, SDL_Surface* surface);

/**
 * @brief clear_surface clear surface table
 * @param list list of surface items
 */
surfacelist clear_surface(surfacelist list);

#endif