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
#include "ftk_list_item.h"

typedef struct _ListPrivInfo
{
    int rows_nr;
    int cols_nr;
    int visible_start;
    int total;

    FtkWidget* grab_widget;
	
	void* listener_ctx;
	FtkListener listener;

    void* paint_listener_ctx;
    FtkListPaintListener paint_listener;
}PrivInfo;

static FtkWidget* ftk_list_get_setlected_item(FtkWidget* thiz);
static Ret ftk_list_set_selected_item(FtkWidget* thiz, FtkWidget* item);

static FtkWidget* ftk_list_find_target(FtkWidget* thiz, int x, int y)
{
	FtkWidget* target = NULL;
	int left = ftk_widget_left_abs(thiz);
	int top  = ftk_widget_top_abs(thiz);
	int w    = ftk_widget_width(thiz);
	int h    = ftk_widget_height(thiz);

	if(!ftk_widget_is_visible(thiz))
	{
		return NULL;
	}

	if(x < left || y < top || (x > (left + w)) || (y > (top + h)))
	{
		return NULL;
	}

	if(thiz->children != NULL)
	{
		FtkWidget* iter = thiz->children;
		while(iter != NULL)
		{
			if((target = ftk_list_find_target(iter, x, y)) != NULL)
			{
				return target;
			}

			iter = ftk_widget_next(iter);
		}
	}
	
	if (ftk_widget_type(thiz) == FTK_LIST_ITEM)
	{
		return thiz;
	}
	else
	{
		return NULL;
	}
}

static Ret ftk_list_on_mouse_event(FtkWidget* thiz, FtkEvent* event)
{
	Ret ret = RET_NO_TARGET;
	FtkWidget* target = NULL;
	DECL_PRIV0(thiz, priv);

	if(priv->grab_widget != NULL)
	{
		return ftk_widget_event(priv->grab_widget, event);
	}

    /* dispatch event to ftk_list_item */
	if((target = ftk_list_find_target(thiz, event->u.mouse.x, event->u.mouse.y)) != NULL && target != thiz)
	{

		if(ftk_widget_type(target) == FTK_LIST_ITEM && ftk_widget_is_visible(target))
		{
            ret = ftk_widget_event(target, event);
        }
		else
		{
			ret = RET_IGNORED;
		}
	}

	return ret;
}

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
        {
            ftk_window_ungrab(ftk_widget_toplevel(thiz), thiz);

            ret = ftk_list_on_mouse_event(thiz, event);
            break;
        }
		case FTK_EVT_MOUSE_DOWN:
		{
            ftk_window_grab(ftk_widget_toplevel(thiz), thiz);

            ftk_list_item_set_selected(ftk_list_get_setlected_item(thiz), 0);

            ret = ftk_list_on_mouse_event(thiz, event);
			break;
		}
		case FTK_EVT_MOUSE_MOVE:
        {
            /* ftk_logi("%s->move\n", __func__); */
            break;
        }
		case FTK_EVT_RESIZE:
		case FTK_EVT_MOVE_RESIZE:
		{
			/* if(priv->item_height > 0) */
			/* { */
				/* priv->rows_nr = ftk_widget_height(thiz)/priv->item_height; */
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

    /* ftk_logi("%s\n", __func__); */

    return RET_OK;
}

static void ftk_list_destroy(FtkWidget* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV0(thiz, priv);

		FTK_ZFREE(priv, sizeof(PrivInfo));
	}

	return;
}

Ret ftk_list_set_paint_listener(FtkWidget* thiz, FtkListPaintListener listener, void* ctx)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL, RET_FAIL);

	priv->paint_listener_ctx = ctx;
	priv->paint_listener = listener;

	return RET_OK;
}

static Ret ftk_list_paint_row(FtkWidget* thiz, int visible_pos, int row, int visible)
{
    int c = 0;
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

    /* ftk_logi("%s\n", __func__); */

    for(c=0; c<ftk_list_get_cols_nr(thiz); c++) 
    {
        FtkWidget* cell = ftk_list_get_cell(thiz, row, c);

        ftk_widget_set_visible(cell, visible);

        if(priv->paint_listener != NULL)
        {
            priv->paint_listener(thiz, priv->paint_listener_ctx, visible_pos, row, c, cell, visible);
        }
    }

    return RET_OK;
}

static Ret ftk_list_paint(FtkWidget* thiz, int visible_start, int visible_nr, int total)
{
    int r = 0;
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);
    return_val_if_fail(visible_start < total || (visible_start == 0 && total == 0), RET_FAIL);

    /* ftk_logi("%s-> visible_start=%d, visible_nr=%d, total=%d\n", __func__, visible_start, visible_nr, total); */

    for(r=0; r<visible_nr; r++)
    {
        if(visible_start + r < total)     
        {
            ftk_list_paint_row(thiz, visible_start + r, r, 1);
        }
        else
        {
            ftk_list_paint_row(thiz, visible_start + r, r, 0);
        }
    }

	return RET_OK;
}

Ret ftk_list_grab(FtkWidget* thiz, FtkWidget* grab_widget)
{
    DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL, RET_FAIL);

    priv->grab_widget = grab_widget;

    return RET_OK;
}

Ret ftk_list_ungrab(FtkWidget* thiz, FtkWidget* grab_widget)
{
    DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL, RET_FAIL);

    priv->grab_widget = NULL;

    return RET_OK;
}

Ret ftk_list_update(FtkWidget* thiz)
{
	DECL_PRIV0(thiz, priv);

    /* 最后一页，选中行可能需要调整 */
    if(priv->visible_start + priv->rows_nr > priv->total)
    {
        ftk_list_set_selected_item(thiz, ftk_list_get_item(thiz, priv->total - priv->visible_start-1));
    }

    ftk_list_paint(thiz, priv->visible_start, priv->rows_nr, priv->total);

    ftk_widget_invalidate(thiz);

    return RET_OK;
}

Ret ftk_list_set_total(FtkWidget* thiz, int nr)
{
    DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

    priv->total = nr;

    return RET_OK;
}

Ret ftk_list_set_rows_nr(FtkWidget* thiz, int nr)
{
    DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

    priv->rows_nr = nr;

    return RET_OK;
}

int ftk_list_get_rows_nr(FtkWidget* thiz)
{
    DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, 0);

    return priv->rows_nr;
}

int ftk_list_get_cols_nr(FtkWidget* thiz)
{
    DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, 0);

    return priv->cols_nr;
}

Ret ftk_list_set_cols_nr(FtkWidget* thiz, int nr)
{
    DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

    priv->cols_nr = nr;

    return RET_OK;
}

FtkWidget* ftk_list_get_item(FtkWidget* thiz, int row)
{
    int r = 0;
    FtkWidget* iter = ftk_widget_child(thiz);
    DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, NULL);

    for(; iter != NULL; iter = ftk_widget_next(iter))
    {
        if(ftk_widget_type(iter) != FTK_LIST_ITEM) 
        {
            continue;
        }

        if(r++ == row)
        {
            return iter;
        }
    }

    return NULL;
}

FtkWidget* ftk_list_get_cell(FtkWidget* thiz, int row, int col)
{
    int c = 0;
    FtkWidget* item = ftk_list_get_item(thiz, row);
    FtkWidget* iter = ftk_widget_child(item);
    DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, NULL);
	return_val_if_fail(item != NULL, NULL);

    for(; iter != NULL; iter = ftk_widget_next(iter))
    {
        if(c++ == col)
        {
            return iter;
        }
    }

    return NULL;
}

Ret ftk_list_page_prev(FtkWidget* thiz)
{
    DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

    priv->visible_start = (priv->visible_start - priv->rows_nr) > 0 ? (priv->visible_start - priv->rows_nr) : 0;

    ftk_list_update(thiz);

    return RET_OK;
}

Ret ftk_list_page_next(FtkWidget* thiz)
{
    DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);
    
    priv->visible_start = (priv->visible_start + priv->rows_nr) < priv->total ? (priv->visible_start + priv->rows_nr) : priv->visible_start;

    ftk_list_update(thiz);

    return RET_OK;
}

Ret ftk_list_reset(FtkWidget* thiz)
{
    DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

    ftk_list_item_set_selected(ftk_list_get_setlected_item(thiz), 0);
    priv->visible_start = 0;
    priv->total = 0;

    ftk_list_update(thiz);

    return RET_OK;
}

Ret ftk_list_remove(FtkWidget* thiz)
{
    DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

    priv->total = priv->total > 0 ? priv->total - 1: 0;

    if(priv->visible_start >= priv->total)
    {
        priv->visible_start = (priv->visible_start - priv->rows_nr > 0) ? (priv->visible_start - priv->rows_nr) : 0;
    }

    ftk_list_update(thiz);

    return RET_OK;
}

int ftk_list_get_total_page_num(FtkWidget* thiz)
{
    DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, 0);

    if(priv->rows_nr == 0)
    {
        return 0;
    }

    return (priv->total % priv->rows_nr == 0) ? (priv->total / priv->rows_nr) : (priv->total / priv->rows_nr + 1);
}

int ftk_list_get_cur_page_num(FtkWidget* thiz)
{
    DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, 0);

    if(priv->rows_nr == 0)
    {
        return 0;
    }

    return priv->visible_start / priv->rows_nr;
}

static FtkWidget* ftk_list_get_setlected_item(FtkWidget* thiz)
{
    int i = 0;
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL, NULL);

    for(i=0; i<priv->rows_nr; i++)
    {
        FtkWidget* item = ftk_list_get_item(thiz, i);

        if(ftk_list_item_is_selected(item))
        {
            return item;
        }
    }

    return NULL;
}

static Ret ftk_list_set_selected_item(FtkWidget* thiz, FtkWidget* item)
{
    FtkWidget* selected_item = ftk_list_get_setlected_item(thiz);
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && item != NULL, RET_FAIL);

    if(selected_item == NULL)
    {
        return RET_OK;
    }

    if(selected_item == item)
    {
        return RET_OK;
    }
    else 
    {
        ftk_list_item_set_selected(selected_item, 0);
        ftk_list_item_set_selected(item, 1);
    }

    return RET_OK;
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

        ftk_widget_init(thiz, FTK_LIST, 0, x, y, width, height, FTK_ATTR_BG_FOUR_CORNER);
        ftk_widget_append_child(parent, thiz);

        priv->grab_widget = NULL;
        priv->visible_start = 0;
        priv->total = 0;
        priv->rows_nr = 0;
        priv->cols_nr = 0;
	}
	else
	{
		FTK_ZFREE(thiz, sizeof(FtkWidget));
	}

	return thiz;
}


