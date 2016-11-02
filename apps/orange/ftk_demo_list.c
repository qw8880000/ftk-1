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

enum{
    IDX_LIST   = 2,
    IDX_PREV   = 100,
    IDX_NEXT   = 101,
    IDX_REMOVE = 102,
    IDX_CLEAR  = 103,

};

static void ftk_demo_list_init(FtkWidget* win)
{
    int r = 0;
    FtkListModelRanksInfo info = {0};
    FtkWidget* list = ftk_widget_lookup(win, IDX_LIST);
    char temp[32] = {0};

    for(r=0; r<20; r++)
    {
        info.row_index = r;
        info.cell_index = 0;
        info.text = "a";
        ftk_list_model_add(ftk_list_get_model(list),(void*)&info);

        info.row_index = r;
        info.cell_index = 1;
        snprintf(temp, sizeof(temp), "%d. hahah", r); 
        info.text = temp;
        ftk_list_model_add(ftk_list_get_model(list), (void*)&info);

        info.row_index = r;
        info.cell_index = 2;
        info.text = "b";
        ftk_list_model_add(ftk_list_get_model(list), (void*)&info);
    }

    ftk_list_update(list);
}

static Ret ftk_demo_list_on_button_clicked(void* ctx, void* obj)
{
    FtkWidget* win = (FtkWidget*)ctx;
    FtkWidget* button = (FtkWidget*)obj;
    FtkWidget* list = ftk_widget_lookup(win, IDX_LIST);

    switch(ftk_widget_id(button))
    {
        case IDX_PREV:
        {
            ftk_list_page_prev(list);
            ftk_widget_set_active(button, 1);
            break;
        }
        case IDX_NEXT:
        {
            ftk_list_page_next(list);
            ftk_widget_set_active(button, 1);
            break;
        }
        case IDX_REMOVE:
        {
            break;
        }
        case IDX_CLEAR:
        {
            break;
        }
        default:break;
    }

	return RET_OK;
}



static void ftk_demo_list_widget_init(FtkWidget* win)
{
    ftk_demo_list_init(win);

	ftk_button_set_clicked_listener(ftk_widget_lookup(win, IDX_PREV), ftk_demo_list_on_button_clicked, win);
	ftk_button_set_clicked_listener(ftk_widget_lookup(win, IDX_NEXT), ftk_demo_list_on_button_clicked, win);
	ftk_button_set_clicked_listener(ftk_widget_lookup(win, IDX_REMOVE), ftk_demo_list_on_button_clicked, win);
	ftk_button_set_clicked_listener(ftk_widget_lookup(win, IDX_CLEAR), ftk_demo_list_on_button_clicked, win);
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

