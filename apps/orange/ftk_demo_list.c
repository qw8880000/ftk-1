/* --------------------------------------------------------------------------*/
/* * 
 * @file ftk_demo_list.c
 * @Brief  
 * @author wangjl
 * @version v1.0
 * @date 2016-11-02
 *
 *           __   _,--="=--,_   __
 *          /  \."    .-.    "./  \
 *         /  ,/  _   : :   _  \/` \
 *         \  `| /o\  :_:  /o\ |\__/
 *          `-'| :="~` _ `~"=: |
 *             \`     (_)     `/
 *      .-"-.   \      |      /   .-"-.
 * .---{     }--|  /,.-'-.,\  |--{     }---.
 *  )  (_)_)_)  \_/`~-===-~`\_/  (_(_(_)  (
 * (        0 Error(s) 0 Warning(s)        )
 *  )       Jinle Wang@Starnet            (
 * '---------------------------------------'            
 *
 *
 */
/* ----------------------------------------------------------------------------*/
#include "ftk.h"
#include "ftk_orange_xul.h"

static void ftk_demo_list_widget_init(FtkWidget* win)
{
}

FtkWidget* ftk_demo_list_create(void)
{
	FtkWidget* win = ftk_orange_xul_load("xul/demo_list.xul");

    ftk_demo_list_widget_init(win);

    ftk_widget_show_all(win, 1);

#ifdef HAS_MAIN
	FTK_QUIT_WHEN_WIDGET_CLOSE(win);
#endif	

    return win;
}
