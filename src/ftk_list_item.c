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
    FtkBitmap* bg_normal;
    FtkBitmap* bg_active;

    int state;

	void* listener_ctx;
	FtkListener listener;
}PrivInfo;

#define LIST_ITEM_STATE_NORMAL  0
#define LIST_ITEM_STATE_ACTIVE  1
#define LIST_ITEM_STATE_SELECTED 2

int ftk_list_item_is_selected(FtkWidget* thiz)
{
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL, RET_FAIL);

    return (priv->state == LIST_ITEM_STATE_SELECTED) ? 1 : 0;
}

Ret ftk_list_item_set_selected(FtkWidget* thiz, int selected)
{
    int state = LIST_ITEM_STATE_NORMAL;
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL, RET_FAIL);

    state = selected ? LIST_ITEM_STATE_SELECTED : LIST_ITEM_STATE_NORMAL;

    if(priv->state == state)
    {
        return RET_OK;
    }

    priv->state = state;

    ftk_widget_invalidate(thiz);

    return RET_OK;
}

static Ret ftk_list_item_set_active(FtkWidget* thiz)
{
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL, RET_FAIL);

    if(priv->state == LIST_ITEM_STATE_ACTIVE)
    {
        return RET_OK;
    }

    priv->state = LIST_ITEM_STATE_ACTIVE;

    ftk_widget_invalidate(thiz);

    return RET_OK;
}

static Ret ftk_list_item_on_event(FtkWidget* thiz, FtkEvent* event)
{
	Ret ret = RET_OK;
	DECL_PRIV0(thiz, priv);

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

            ret = ftk_list_item_set_selected(thiz, 1);

            break;
        }
		case FTK_EVT_MOUSE_DOWN:
		{
            ftk_list_grab(ftk_widget_parent(thiz), thiz);

            ret = ftk_list_item_set_active(thiz);

            break;
		}
		case FTK_EVT_RESIZE:
		case FTK_EVT_MOVE_RESIZE:
		{
			break;
		}
        case FTK_EVT_MOUSE_MOVE:
        {
            /* int x = event->u.mouse.x; */
            /* int y = event->u.mouse.y; */
            /* ftk_logi("--> x=%d,y=%d\n", x, y); */
            break;
        }
		default:break;
	}

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
        if(priv->bg_normal != NULL)
        {
            ftk_bitmap_unref(priv->bg_normal);
        }
        if(priv->bg_active != NULL)
        {
            ftk_bitmap_unref(priv->bg_active);
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

    if(priv->state == LIST_ITEM_STATE_SELECTED)
    {
        ftk_canvas_draw_bg_image(canvas, priv->bg_selected, FTK_BG_FOUR_CORNER, x, y, width, height);
    }
    else if(priv->state == LIST_ITEM_STATE_NORMAL)
    {
        ftk_canvas_draw_bg_image(canvas, priv->bg_normal, FTK_BG_FOUR_CORNER, x, y, width, height);
    }
    else if(priv->state == LIST_ITEM_STATE_ACTIVE)
    {
        ftk_canvas_draw_bg_image(canvas, priv->bg_active, FTK_BG_FOUR_CORNER, x, y, width, height);
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

Ret ftk_list_item_set_bg_normal(FtkWidget* thiz, FtkBitmap* bitmap)
{
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL && bitmap != NULL, RET_FAIL);

    priv->bg_normal = bitmap;

    return RET_OK;
}

Ret ftk_list_item_set_bg_active(FtkWidget* thiz, FtkBitmap* bitmap)
{
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL && bitmap != NULL, RET_FAIL);

    priv->bg_active = bitmap;

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

		ftk_widget_init(thiz, FTK_LIST_ITEM, 0, x, y, width, height, FTK_ATTR_INSENSITIVE|FTK_ATTR_BG_FOUR_CORNER);
		ftk_widget_append_child(parent, thiz);

        priv->bg_selected = NULL;
        priv->bg_normal = NULL;
        priv->bg_active = NULL;
        priv->state = LIST_ITEM_STATE_NORMAL;
	}
	else
	{
		FTK_ZFREE(thiz, sizeof(FtkWidget));
	}

	return thiz;
}

