/**
	Create a new SDL window, plot points in a created graph featuring a cartesian coordinate system
	RUN: cd examples/plot_sdl & ../../build/examples/plot_sdl/runplot_sdl opensans.ttf
	@author Bertrand Martel, 2015
	@version 0.1
*/
#include <stdio.h>
#include "ui_sdl_plot.h"
#include "ui_sdl_llist.h"

int main(int argc, char* argv[])
{
	if ( argc != 2 )
	{
		printf( "usage: %s font file name (.ttf)\n", argv[0] );
		return EXIT_FAILURE;
	}

	// captions
	captionlist caption_list = NULL;
	caption_list=add_caption(caption_list,"Download",0,0x0000FF);
	caption_list=add_caption(caption_list,"Upload"  ,1,0xFF0000);
	//print_list_caption(caption_list);

	// coordinates
	coordlist coordinate_list=NULL;
	coordinate_list=add_point(coordinate_list,0,0,0);
	coordinate_list=add_point(coordinate_list,0,1,90);
	coordinate_list=add_point(coordinate_list,0,2,84);
	coordinate_list=add_point(coordinate_list,0,3,98);
	coordinate_list=add_point(coordinate_list,0,4,94);
	coordinate_list=add_point(coordinate_list,0,5,85);
	coordinate_list=add_point(coordinate_list,0,6,90);
	coordinate_list=add_point(coordinate_list,0,7,99);
	coordinate_list=add_point(coordinate_list,0,8,94);
	coordinate_list=add_point(coordinate_list,1,0,0);
	coordinate_list=add_point(coordinate_list,1,1,92);
	coordinate_list=add_point(coordinate_list,1,2,90);
	coordinate_list=add_point(coordinate_list,1,3,98);
	coordinate_list=add_point(coordinate_list,1,4,92);
	coordinate_list=add_point(coordinate_list,1,5,82);
	coordinate_list=add_point(coordinate_list,1,6,98);
	coordinate_list=add_point(coordinate_list,1,7,94);
	coordinate_list=add_point(coordinate_list,1,8,90);
	//print_list_coord(coordinate_list);

	// plot parameters
	plot_params params;
	params.screen_width=800;
	params.screen_heigth=640;
	params.plot_window_title="plot-sdl";
	params.font_text_path=argv[1];
	params.font_text_size=18;
	params.caption_text_x="Time (s)";
	params.caption_text_y="Speed (Mbit/s)";
	params.caption_list = caption_list;
	params.coordinate_list = coordinate_list;
	params.scale_x = 1;
	params.scale_y = 10;
	params.max_x = 8.5;
	params.max_y = 120;

	int ret = plot_graph(&params);

	if (ret == EXIT_FAILURE)
	{
		printf("plot_graph return with status %d\n",ret);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}