/* --------------------------------------------------------------------------*/
/* * 
 * @file ftk_app_orange.c
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
 */
/* ----------------------------------------------------------------------------*/

#include "ftk_xul.h"
#include "ftk_expr.h"
#include "ftk_app_orange.h"

typedef struct _PrivInfo
{
	FtkBitmap* icon;
}PrivInfo;

extern FtkWidget* ftk_demo_list_create(void);

static Ret ftk_orange_on_button_clicked(void* ctx, void* obj);


static FtkWidget* ftk_orange_create_window(void)
{

	FtkWidget* win = ftk_orange_xul_load("xul/main.xul");

    ftk_widget_show_all(win, 1);

    return win;
}

static Ret ftk_orange_on_shutdown(void* ctx, void* obj)
{
	ftk_widget_unref(ctx);

	return RET_OK;
}

static Ret ftk_orange_on_prepare_options_menu(void* ctx, FtkWidget* menu_panel)
{
	FtkWidget* item = ftk_menu_item_create(menu_panel);
	ftk_widget_set_text(item, _("Quit"));
	ftk_menu_item_set_clicked_listener(item, ftk_orange_on_shutdown, ctx);
	ftk_widget_show(item, 1);

	return RET_OK;
}

static Ret ftk_orange_on_button_clicked(void* ctx, void* obj)
{
    FtkWidget* win = (FtkWidget*)ctx;
    FtkWidget* button = (FtkWidget*)obj;

    switch(ftk_widget_id(button))
    {
        case 10:
        {
            ftk_demo_list_create();
            break;
        }
        default:break;
    }

	return RET_OK;
}

static void ftk_orange_widget_init(FtkWidget* win)
{
    FtkWidget* button = NULL;

	ftk_button_set_clicked_listener(ftk_widget_lookup(win, 10), ftk_orange_on_button_clicked, win);
}

static FtkBitmap* ftk_app_orange_get_icon(FtkApp* thiz)
{
	DECL_PRIV(thiz, priv);
	const char* name="demo.png";
	char file_name[FTK_MAX_PATH + 1] = {0};
	return_val_if_fail(priv != NULL, NULL);
	
	if(priv->icon != NULL) return priv->icon;
	
	snprintf(file_name, FTK_MAX_PATH, "%s/icons/%s", APP_DATA_DIR, name);
	priv->icon = ftk_bitmap_factory_load(ftk_default_bitmap_factory(), file_name);
	if(priv->icon != NULL) return priv->icon;

	snprintf(file_name, FTK_MAX_PATH, "%s/icons/%s", APP_LOCAL_DATA_DIR, name);
	priv->icon = ftk_bitmap_factory_load(ftk_default_bitmap_factory(), file_name);

	return priv->icon;
}


static const char* ftk_app_orange_get_name(FtkApp* thiz)
{
	return _("orange!");
}

static Ret ftk_app_orange_run(FtkApp* thiz, int argc, char* argv[])
{
	DECL_PRIV(thiz, priv);
	FtkWidget* win = NULL;
	return_val_if_fail(priv != NULL, RET_FAIL);

    ftk_set_log_level(FTK_LOG_I);

	win = ftk_orange_create_window();
	ftk_app_window_set_on_prepare_options_menu(win, ftk_orange_on_prepare_options_menu, win);
	ftk_widget_show_all(win, 1);

    ftk_orange_widget_init(win);

#ifdef HAS_MAIN
	FTK_QUIT_WHEN_WIDGET_CLOSE(win);
#endif	

	return RET_OK;
}

static void ftk_app_orange_destroy(FtkApp* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		ftk_bitmap_unref(priv->icon);
		FTK_FREE(thiz);
	}

	return;
}

FtkApp* ftk_app_orange_create(void)
{
	FtkApp* thiz = FTK_ZALLOC(sizeof(FtkApp) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		thiz->run  = ftk_app_orange_run;
		thiz->get_icon = ftk_app_orange_get_icon;
		thiz->get_name = ftk_app_orange_get_name;
		thiz->destroy = ftk_app_orange_destroy;
	}

	return thiz;
}

