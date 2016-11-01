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
 * @file ftk_list.c
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
 *
 */
/* ----------------------------------------------------------------------------*/
#include "ftk_key.h"
#include "ftk_event.h"
#include "ftk_globals.h"
#include "ftk_window.h"
#include "ftk_list.h"

typedef struct _ListPrivInfo
{
	/* int current; */
    int visible_nr;
    int visible_start;
	/* int item_height; */

	/* int is_active; */
	/* int top_margin; */
	/* int botton_margin; */
	/* int scrolled_by_me; */
	/* FtkListModel*  model; */
	/* FtkListRender* render; */
	/* FtkWidget* vscroll_bar; */
	
    /* FtkBitmap* bg_normal; */
    /* FtkBitmap* bg_focus; */
    /* FtkBitmap* bg_active; */

	void* listener_ctx;
	FtkListener listener;
}PrivInfo;

static Ret ftk_list_on_event(FtkWidget* thiz, FtkEvent* event)
{
	Ret ret = RET_FAIL;
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
		case FTK_EVT_MOUSE_DOWN:
		{
			/* ret = ftk_list_view_on_mouse_event(thiz, event); */
			break;
		}
		case FTK_EVT_RESIZE:
		case FTK_EVT_MOVE_RESIZE:
		{
			/* if(priv->item_height > 0) */
			/* { */
				/* priv->visible_nr = ftk_widget_height(thiz)/priv->item_height; */
			/* } */
			break;
		}
		default:break;
	}

	return ret;
}

static Ret ftk_list_on_paint(FtkWidget* thiz)
{
	DECL_PRIV0(thiz, priv);
	/* FTK_BEGIN_PAINT(x, y, width, height, canvas); */
	/* FTK_END_PAINT(); */
    return RET_OK;
}

static void ftk_list_destroy(FtkWidget* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV0(thiz, priv);

		/* ftk_list_render_destroy(priv->render); */
		/* ftk_list_model_unref(priv->model); */
		FTK_ZFREE(priv, sizeof(PrivInfo));
	}

	return;
}

void ftk_list_set_visible_nr(FtkWidget* thiz, int nr)
{
    DECL_PRIV0(thiz, priv);

    priv->visible_nr = nr;
}

FtkWidget* ftk_list_create(FtkWidget* parent, int x, int y, int width, int height)
{
    FtkWidget* thiz = (FtkWidget*)FTK_ZALLOC(sizeof(FtkWidget));
	return_val_if_fail(thiz != NULL, NULL);
	
	thiz->priv_subclass[0] = (PrivInfo*)FTK_ZALLOC(sizeof(PrivInfo));
	if(thiz->priv_subclass[0] != NULL)
	{
		DECL_PRIV0(thiz, priv);
		thiz->on_event = ftk_list_on_event;
		thiz->on_paint = ftk_list_on_paint;
		thiz->destroy  = ftk_list_destroy;

		ftk_widget_init(thiz, FTK_LIST, 0, x, y, width, height, FTK_ATTR_INSENSITIVE|FTK_ATTR_BG_FOUR_CORNER);
		ftk_widget_append_child(parent, thiz);
	}
	else
	{
		FTK_ZFREE(thiz, sizeof(FtkWidget));
	}

	return thiz;
}

