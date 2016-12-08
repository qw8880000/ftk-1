/* Copyright (C) 
 * 2016 - wangjl
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */
/* --------------------------------------------------------------------------*/
/* * 
 * @file ftk_list_item.c
 * @Brief  
 * @author wangjl
 * @version v1.0
 * @date 2016-10-31
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
 */
/* ----------------------------------------------------------------------------*/
#include "ftk_key.h"
#include "ftk_event.h"
#include "ftk_globals.h"
#include "ftk_window.h"
#include "ftk_list.h"

typedef struct _ListItemPrivInfo
{
    FtkBitmap* bg_selected;

    int is_selected;

	void* listener_ctx;
	FtkListener listener;
}PrivInfo;

Ret ftk_list_item_set_selected(FtkWidget* thiz, int selected)
{
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL, RET_FAIL);

    priv->is_selected = selected;

    ftk_widget_invalidate(thiz);

    return RET_OK;
}

static Ret ftk_list_item_on_event(FtkWidget* thiz, FtkEvent* event)
{
	Ret ret = RET_OK;
#if 0
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL, RET_FAIL);

	switch(event->type)
	{
		case FTK_EVT_KEY_UP:
		case FTK_EVT_KEY_DOWN:
		{
			/* ret = ftk_list_view_on_key_event(thiz, event); */
			break;
		}
		case FTK_EVT_MOUSE_UP:
        {
            ftk_list_ungrab(ftk_widget_parent(thiz), thiz);

            ftk_widget_set_active(thiz, 0);

            break;
        }
		case FTK_EVT_MOUSE_DOWN:
		{
            ftk_list_grab(ftk_widget_parent(thiz), thiz);

            ftk_widget_set_active(thiz, 1);

            break;
		}
		case FTK_EVT_RESIZE:
		case FTK_EVT_MOVE_RESIZE:
		{
			break;
		}
		default:break;
	}
#endif

	return ret;
}

static void ftk_list_item_destroy(FtkWidget* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV0(thiz, priv);

        if(priv->bg_selected != NULL)
        {
            ftk_bitmap_unref(priv->bg_selected);
        }

		FTK_ZFREE(priv, sizeof(PrivInfo));
	}

	return;
}

static Ret ftk_list_item_on_paint(FtkWidget* thiz)
{
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL, RET_FAIL);
    FTK_BEGIN_PAINT(x, y, width, height, canvas);

    if(priv->is_selected)
    {
        ftk_canvas_draw_bg_image(canvas, priv->bg_selected, FTK_BG_FOUR_CORNER, x, y, width, height);
    }

    FTK_END_PAINT();
}

Ret ftk_list_item_set_bg_selected(FtkWidget* thiz, FtkBitmap* bitmap)
{
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL && bitmap != NULL, RET_FAIL);

    priv->bg_selected = bitmap;

    return RET_OK;
}

FtkWidget* ftk_list_item_create(FtkWidget* parent, int x, int y, int width, int height)
{
	FtkWidget* thiz = (FtkWidget*)FTK_ZALLOC(sizeof(FtkWidget));
	return_val_if_fail(thiz != NULL, NULL);
	
	thiz->priv_subclass[0] = (PrivInfo*)FTK_ZALLOC(sizeof(PrivInfo));
	if(thiz->priv_subclass[0] != NULL)
	{
		DECL_PRIV0(thiz, priv);
        thiz->on_event = ftk_list_item_on_event;
		thiz->on_paint = ftk_list_item_on_paint;
		thiz->destroy  = ftk_list_item_destroy;

		ftk_widget_init(thiz, FTK_LIST_ITEM, 0, x, y, width, height, FTK_ATTR_IGNORE_EVENT|FTK_ATTR_BG_FOUR_CORNER);
		ftk_widget_append_child(parent, thiz);

        priv->bg_selected = NULL;
	}
	else
	{
		FTK_ZFREE(thiz, sizeof(FtkWidget));
	}

	return thiz;
}

