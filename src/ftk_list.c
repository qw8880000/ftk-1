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
#include "ftk_list_model.h"
#include "ftk_list_render.h"

typedef struct _ListPrivInfo
{
    int current;
    int visible_nr;
    int visible_start;
	/* int item_height; */

	/* int is_active; */
	/* int top_margin; */
	/* int botton_margin; */
	/* int scrolled_by_me; */
    FtkListModel*  model;
    FtkListRender* render;
	/* FtkWidget* vscroll_bar; */
	
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
	int total = ftk_list_model_get_total(priv->model);
    int i = 0;

    if((priv->visible_start + priv->visible_nr) >= total)
    {
        int visible_start = total - priv->visible_nr;
        priv->visible_start = (visible_start >= 0) ? visible_start : 0;
    }

	if(priv->current >= total)
	{
		priv->current = total - 1;
	}

    ftk_list_render_paint(priv->render, NULL, priv->visible_start, priv->visible_nr, 0, 0, 0);

    return RET_OK;
}

static void ftk_list_destroy(FtkWidget* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV0(thiz, priv);

        ftk_list_model_unref(priv->model);
        ftk_list_render_destroy(priv->render);

		FTK_ZFREE(priv, sizeof(PrivInfo));
	}

	return;
}

void ftk_list_set_visible_nr(FtkWidget* thiz, int nr)
{
    DECL_PRIV0(thiz, priv);
	return_if_fail(thiz != NULL && priv != NULL);

    priv->visible_nr = nr;
}

void ftk_list_page_prev(FtkWidget* thiz)
{
    DECL_PRIV0(thiz, priv);
	return_if_fail(thiz != NULL && priv != NULL);

    priv->visible_start = (priv->visible_start - priv->visible_nr) > 0 ? (priv->visible_start - priv->visible_nr) : 0;

    ftk_widget_invalidate(thiz);
}

void ftk_list_page_next(FtkWidget* thiz)
{
    DECL_PRIV0(thiz, priv);
    int total = ftk_list_model_get_total(priv->model);
	return_if_fail(thiz != NULL && priv != NULL);
    
    priv->visible_start = (priv->visible_start + priv->visible_nr) < total ? (priv->visible_start + priv->visible_nr) : priv->visible_start;

    ftk_widget_invalidate(thiz);
}

int ftk_list_get_total_page_num(FtkWidget* thiz)
{
    DECL_PRIV0(thiz, priv);
    int total = ftk_list_model_get_total(priv->model);
	return_val_if_fail(thiz != NULL && priv != NULL, 0);

    if(priv->visible_nr == 0)
    {
        return 0;
    }

    return (total % priv->visible_nr == 0) ? (total / priv->visible_nr) : (total / priv->visible_nr + 1);
}

int ftk_list_get_cur_page_num(FtkWidget* thiz)
{
    DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, 0);

    if(priv->visible_nr == 0)
    {
        return 0;
    }

    return priv->visible_start / priv->visible_nr;
}


#include "ftk_list_model_ranks.h"
#include "ftk_list_render_ranks.h"

static Ret ftk_list_init(FtkWidget* thiz, FtkListModel* model, FtkListRender* render)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL && render != NULL && model != NULL, RET_FAIL);
	
    priv->model = model;
    priv->render = render;
    priv->visible_start = 0;
    priv->current = 0;

	ftk_list_render_init(render, model, thiz);

	return RET_OK;
}

FtkWidget* ftk_list_create(FtkWidget* parent, int x, int y, int width, int height)
{
    FtkWidget* thiz = (FtkWidget*)FTK_ZALLOC(sizeof(FtkWidget));
	return_val_if_fail(thiz != NULL, NULL);
	
	thiz->priv_subclass[0] = (PrivInfo*)FTK_ZALLOC(sizeof(PrivInfo));
	if(thiz->priv_subclass[0] != NULL)
	{
		/* DECL_PRIV0(thiz, priv); */
		thiz->on_event = ftk_list_on_event;
		thiz->on_paint = ftk_list_on_paint;
		thiz->destroy  = ftk_list_destroy;

		ftk_widget_init(thiz, FTK_LIST, 0, x, y, width, height, FTK_ATTR_INSENSITIVE|FTK_ATTR_BG_FOUR_CORNER);
        ftk_widget_append_child(parent, thiz);

        ftk_list_init(thiz, ftk_list_model_ranks_create(), ftk_list_render_ranks_create());
	}
	else
	{
		FTK_ZFREE(thiz, sizeof(FtkWidget));
	}

	return thiz;
}


