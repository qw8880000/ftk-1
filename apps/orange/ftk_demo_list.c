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

Ret demo_list_item_validate(FtkWidget* thiz, int position, int col, FtkWidget* cell, int visible, void* ctx)
{
    FtkWidget* list = thiz;
    return_val_if_fail(thiz != NULL && cell != NULL, RET_FAIL);

    char* buffer[128];
    FtkColor color = {.a=0xff, .r=0xff, .g=0x00, .b=0x00};

    ftk_logi("%s, %d\n", __func__, position);
    switch(col)
    {
        case 1:
        {
            snprintf(buffer, sizeof(buffer), "%d.abcd", position);
            ftk_label_set_color(cell, color);
            ftk_widget_set_text(cell, buffer);
            break;
        }
        default:break;
    }


    return RET_OK;
}

FtkWidget* demo_list_get_item(FtkWidget* list, int index, void* ctx)
{
    return ftk_orange_xul_load("xul/list_style.xul");
}

static void ftk_demo_list_init(FtkWidget* win)
{
    FtkWidget* list = ftk_widget_lookup(win, IDX_LIST);
    FtkListCallBacks callbacks = {
        .validate_item = demo_list_item_validate,
        .validate_item_ctx = NULL,
        .get_item = demo_list_get_item,
        .get_item_ctx = NULL
    };

    ftk_list_set_total(list, 20);
    ftk_list_init(list, &callbacks);
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
            break;
        }
        case IDX_NEXT:
        {
            ftk_list_page_next(list);
            break;
        }
        case IDX_REMOVE:
        {
            ftk_list_remove(list, 0);
            break;
        }
        case IDX_CLEAR:
        {
            ftk_list_reset(list);
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

