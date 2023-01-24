/**
	Build custom linked list, push new element, clear and print items
	@author Bertrand Martel
*/
#include <stdlib.h>
#include <stdio.h>

#include "SDL2/SDL.h"

#include "ui_sdl_llist.h"

/**
 * @brief clear_caption: clear caption table
 * @param list list of caption items
 */
captionlist clear_caption(captionlist list){

	if (list!=NULL)
	{
		caption_item * current = list;
		caption_item * next;

		while(current != NULL)
		{	
			next = current->nxt;
			free(current);
			current=next;
		}

		list=NULL;
	}
	return list;
}

/**
 * @brief add_caption: push a new item to the end of caption table
 * @param list list of caption items
 * @param caption_txt caption text
 * @param color caption color
 */
captionlist add_caption(captionlist liste, char * valeur,int caption_id,int color)
{
	caption_item* caption_new_item = malloc(sizeof(caption_item));
	caption_new_item->caption_txt = valeur;
	caption_new_item->caption_id=caption_id;
	caption_new_item->caption_color=color;

	caption_new_item->nxt = NULL;

	if(liste == NULL)
	{
		return caption_new_item;
	}
	else
	{
		caption_item* temp=liste;
		while(temp->nxt != NULL)
		{
			temp = temp->nxt;
		}
		temp->nxt = caption_new_item;
		return liste;
	}
}

/**
 * @brief print_list_caption: print caption table
 * @param list list of caption items
 */
void print_list_caption(captionlist list)
{

	caption_item *tmp = list;

	if (tmp!=NULL)
	{
		while(tmp != NULL)
		{
			printf("%s => %d;", tmp->caption_txt,tmp->caption_id);
			tmp = tmp->nxt;
		}
		printf("\n");
	}

}

/**
 * @brief clear_coord: clear coordinate table
 * @param list list of coordinate items
 */
coordlist clear_coord(coordlist list){

	if (list!=NULL)
	{
		coordinate_item * current = list;
		coordinate_item * next;

		while(current != NULL)
		{	
			next = current->nxt;
			free(current);
			current=next;
		}

		list=NULL;
	}
	return list;
}

/**
 * @brief add_point push a new item to the end of coordinate table
 * @param list list of coordinate items
 * @param caption_id caption identifier
 * @param x x coordinate
 * @param y y coordinate
 */
coordlist add_point(coordlist list,int caption_id, float x,float y)
{
	coordinate_item* coord_new_item = malloc(sizeof(coordinate_item));
	coord_new_item->x = x;
	coord_new_item->y = y;
	coord_new_item->caption_id=caption_id;
	coord_new_item->nxt = NULL;

	if(list == NULL)
	{
		return coord_new_item;
	}
	else
	{
		coordinate_item* temp=list;
		while(temp->nxt != NULL)
		{
			temp = temp->nxt;
		}
		temp->nxt = coord_new_item;
		return list;
	}
}

/**
 * @brief print_list_coord: print coordinate table
 * @param list list of coordinate items
 */
void print_list_coord(coordlist list)
{

	coordinate_item *tmp = list;

	if (tmp!=NULL)
	{
		while(tmp != NULL)
		{
			printf("(%f,%f) ", tmp->x,tmp->y);
			tmp = tmp->nxt;
		}
		printf("\n");
	}

}

/**
 * @brief clear_surface: clear surface table
 * @param list list of surface items
 */
surfacelist clear_surface(surfacelist list){

	if (list!=NULL)
	{
		surface_item * current = list;
		surface_item * next;

		while(current != NULL)
		{	
			next = current->nxt;
			SDL_FreeSurface(current->surface);
			free(current);
			current=next;
		}

		list=NULL;
	}
	return list;
}

/**
 * @brief push_back_surface: push a new item to the end of surface table
 * @param list list of surface items
 * @param surface sSDL surface ptr
 */
surfacelist push_back_surface(surfacelist list, SDL_Surface* surface)
{
	surface_item* surface_new_item = malloc(sizeof(surface_item));
	surface_new_item->surface = surface;

	surface_new_item->nxt = NULL;

	if(list == NULL)
	{
		return surface_new_item;
	}
	else
	{
		surface_item* temp=list;
		while(temp->nxt != NULL)
		{
			temp = temp->nxt;
		}
		temp->nxt = surface_new_item;
		return list;
	}
}