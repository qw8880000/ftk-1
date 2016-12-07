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
#include "ftk_util.h"
#include "ftk_xul.h"
#include "ftk_list.h"
#include "ftk_list_item.h"


/*-----------------------------------------------------------------------------
 * terminology 
 *
 *      index    - index of the items
 *      position - index of the data
 *      selected - index of the position which selected
 *      total   - total of data
 *      scrap items - non-visible items
 *      active items - visible items
 *-----------------------------------------------------------------------------*/

typedef struct _ListItemInfo
{
    int active;
    FtkWidget* widget;
}ListItemInfo;

typedef struct _ListPrivInfo
{
    int initialized;

    int y_offset;
    int item_height;
    int position;
    int index;
    int total;

    int items_nr;
    ListItemInfo* item_array;

    int visible_nr;

    char* item_style;
    int move_support;

    int selected;
    FtkWidget* grab_widget;

    int mouse_pressed;
    int last_mouse_pos;
    int delta;
	
	void* listener_ctx;
	FtkListener listener;

    FtkListCallBacks callbacks;
}PrivInfo;

static ListItemInfo* ftk_list_get_item(FtkWidget* thiz, int index)
{
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL && priv != NULL, NULL);
    return_val_if_fail(index >=0 && priv->items_nr > 0, NULL);

    return priv->item_array + (index % priv->items_nr);
}

static Ret ftk_list_on_item_change(FtkWidget* thiz, FtkWidget* item, int position, int active)
{
    int c = 0;
    FtkWidget* container = item;
    FtkWidget* iter = NULL;
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);
    return_val_if_fail(item != NULL, RET_FAIL);

    if(priv->callbacks.validate_item == NULL)
    {
        return RET_OK;
    }

    iter = ftk_widget_child(container);
    for(; iter != NULL; iter = ftk_widget_next(iter), c++)
    {
        priv->callbacks.validate_item(thiz, position, c, iter, 1, priv->callbacks.validate_item_ctx);
    }

    return RET_OK;
}

static Ret ftk_list_set_item_active(FtkWidget* thiz, int index)
{
    ListItemInfo* item = ftk_list_get_item(thiz, index);
	DECL_PRIV0(thiz, priv);
    int position = priv->position + (index - priv->index);
    return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);
    return_val_if_fail(item != NULL, RET_FAIL);
    return_val_if_fail(item->widget != NULL, RET_FAIL);

    if(item->active == 0)       /* scrap change to active */
    {
        item->active = 1;                  
        ftk_list_on_item_change(thiz, item->widget, position, 1);
    }

    if(position == priv->selected)
    {
        ftk_list_item_set_selected(item->widget, 1);
    }
    else
    {
        ftk_list_item_set_selected(item->widget, 0);
    }

    ftk_widget_set_visible(item->widget, 1);

    return RET_OK;
}

static Ret ftk_list_set_item_scrap(FtkWidget* thiz, int index)
{
    ListItemInfo* item = ftk_list_get_item(thiz, index);
    return_val_if_fail(item != NULL, RET_FAIL);
    return_val_if_fail(item->widget != NULL, RET_FAIL);

    item->active = 0;
    ftk_widget_set_visible(item->widget, 0);
    ftk_widget_hide_self(item->widget, 1);

    return RET_OK;
}

static Ret ftk_list_set_item_position(FtkWidget* thiz, int index, int y)
{
    ListItemInfo* item = ftk_list_get_item(thiz, index);
    return_val_if_fail(item != NULL, RET_FAIL);
    return_val_if_fail(item->widget != NULL, RET_FAIL);

    /* ftk_widget_move(item->widget, ftk_widget_left(item->widget), y); */
    ftk_widget_set_top(item->widget, y);

    return RET_OK;
}

static Ret ftk_list_relayout(FtkWidget* thiz)
{
    int i = 0;
    int index = 0;
    int visible_nr = 0;
	DECL_PRIV0(thiz, priv);
    int total = priv->total;
    return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

    if(total - priv->position <= priv->visible_nr)
    {
        visible_nr = (total - priv->position >= 0) ? (total - priv->position) : 0;
    }
    else
    {
        if(priv->y_offset != 0)
        {
            visible_nr = priv->visible_nr + 1;
        }
        else
        {
            visible_nr = priv->visible_nr;
        }
    }

    for(i = 0; i < visible_nr; i++)             /* show */
    {
        index = priv->index + i;
        ftk_list_set_item_active(thiz, index);
        ftk_list_set_item_position(thiz, index, priv->y_offset + priv->item_height * i);
        /* ftk_logi("-->%s, visible_nr=%d, y=%d\n", __func__, visible_nr, priv->y_offset + priv->item_height * i); */
    }

    for(i = 0; i < priv->items_nr - visible_nr; i++) /* hide */
    {
        index = priv->index + visible_nr + i;
        ftk_list_set_item_scrap(thiz, index);
    }

    ftk_widget_invalidate(thiz);
   
    return RET_OK;
}

static Ret ftk_list_reset_items(FtkWidget* thiz)
{
    int i = 0;
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

    for(i = 0; i < priv->items_nr; i++)
    {
        ftk_list_set_item_scrap(thiz, i);
    }

    return RET_OK;
}


static Ret ftk_list_create_items(FtkWidget* thiz, int nr)
{
    int i = 0;
    FtkWidget* parent = thiz;
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);
    return_val_if_fail(priv->callbacks.get_item != NULL, RET_FAIL);

    for(i = 0; i < nr; i++)
    {
        ListItemInfo* item = priv->item_array + i;
        FtkWidget* list_item = priv->callbacks.get_item(thiz, i, priv->callbacks.get_item_ctx);

        if(list_item != NULL)
        {
            ftk_widget_append_child(parent, list_item);
            /* ftk_widget_set_visible(list_item, 0); */
    
            item->widget = list_item;
            item->active = 0;
        }
    }

    return RET_OK;
}

Ret ftk_list_set_item_height(FtkWidget* thiz, int item_height)
{
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

    priv->item_height =item_height;

    return RET_OK;
}

Ret ftk_list_init(FtkWidget* thiz, FtkListCallBacks* callbacks)
{
	DECL_PRIV0(thiz, priv);
    int item_height = priv->item_height;
    return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);
    return_val_if_fail(item_height > 0, RET_FAIL);

    if(priv->initialized)
    {
        return RET_OK;
    }

    priv->y_offset = 0;
	priv->item_height = item_height;
    priv->visible_nr = ftk_widget_height(thiz)/item_height;

    priv->items_nr = priv->visible_nr * 2;
    priv->item_array = (ListItemInfo*)FTK_ALLOC(sizeof(ListItemInfo) * priv->items_nr);
    memset(priv->item_array, 0, sizeof(ListItemInfo) * priv->items_nr);

    priv->initialized = 1;
    priv->position = 0;
    priv->index = 0;
    priv->delta = 6;
    priv->selected = 0;
    memcpy(&priv->callbacks, callbacks, sizeof(FtkListCallBacks));

    ftk_list_create_items(thiz, priv->items_nr);

    /* ftk_logi("-->%s, item_height=%d,visible_nr=%d\n", __func__, priv->item_height, priv->visible_nr); */
    ftk_list_relayout(thiz);

    return RET_OK;
}

static Ret ftk_list_mouse_move(FtkWidget* thiz, FtkEvent* event)
{

#define MOVE_UP 1
#define MOVE_DOWN 2
    /* int x = event->u.mouse.x; */
    int y = event->u.mouse.y;
	DECL_PRIV0(thiz, priv);
    int total = priv->total;
    int move = (y - priv->last_mouse_pos > 0) ? MOVE_DOWN : MOVE_UP;
    int offset = 0;
    int cross_item_nr = 0;

    return_val_if_fail(priv->items_nr > 0, RET_FAIL);
    return_val_if_fail(priv->item_height > 0, RET_FAIL);
    return_val_if_fail(priv->visible_nr > 0, RET_FAIL);

    if(priv->total <= 0) {
        return RET_OK;
    }

    if(!priv->mouse_pressed){
        return RET_OK;
    }

    if(abs(y - priv->last_mouse_pos) < priv->delta)
    {
        return RET_OK;
    }

    /*-----------------------------------------------------------------------------
     *
     * y_offset---->    |      |    |      up
     *       0 ---->  --+------+--  |      ^
     * position---->    |    A |    |      |
     *                  +------+    |      |
     *                  |      |    |      |
     *                  |    B |    |      |
     *                  |      |    v      |
     *                  +------+  down     |
     *                  |      |                  
     *                  |    c |                  
     *                  |      |                  
     *                  +------+                  
     *
     *-----------------------------------------------------------------------------*/

#if 0
    if(move == MOVE_UP)            /* up */
    {
        if(priv->y_offset == 0 && priv->position + priv->visible_nr >= total)
        {
            return RET_OK;
        }

        /* priv->y_offset -= abs(y - priv->last_mouse_pos); */
        priv->y_offset -= priv->delta;

        if(priv->y_offset < 0 && abs(priv->y_offset) >= priv->item_height)
        {
            priv->index = (priv->index + 1) % priv->items_nr;
            priv->position += 1;

            if(priv->position + priv->visible_nr >= total)
            {
                priv->y_offset = 0;
            }
            else
            {
                priv->y_offset = priv->y_offset + priv->item_height;
            }
        }
    }
    else                                    /* down */
    {
        if((priv->y_offset <= 0 && priv->y_offset + priv->delta >=0) && priv->position <= 0)
        {
            priv->y_offset = 0;
            return RET_OK;
        }

        priv->y_offset += priv->delta;
        /* priv->y_offset += abs(y - priv->last_mouse_pos); */
        if(priv->y_offset > 0 && (priv->y_offset - priv->delta) <= 0)
        {
            priv->index = (priv->index - 1 + priv->items_nr) % priv->items_nr;
            priv->position -= 1;

            priv->y_offset = priv->y_offset - priv->item_height;

        }
    }
#else

    if(move == MOVE_UP)            /* up */
    {
        if(priv->y_offset == 0 && priv->position + priv->visible_nr >= total)
        {
            return RET_OK;
        }

        offset = abs(priv->y_offset) + abs(y - priv->last_mouse_pos);
        priv->y_offset = -(offset % priv->item_height);

        if(offset >= priv->item_height)
        {
            cross_item_nr = offset / priv->item_height;
            if(cross_item_nr + priv->position + priv->visible_nr >= total)
            {
                priv->y_offset = 0;
                cross_item_nr = total - (priv->position + priv->visible_nr);
            }

            priv->index = (priv->index + cross_item_nr) % priv->items_nr;
            priv->position += cross_item_nr;
        }
    }
    else                                    /* down */
    {
        /* if((priv->y_offset <= 0 && priv->y_offset + priv->delta >=0) && priv->position <= 0) */
        if(priv->y_offset == 0 && priv->position <= 0)
        {
            priv->y_offset = 0;
            return RET_OK;
        }

        offset = abs(y - priv->last_mouse_pos) - abs(priv->y_offset);
        priv->y_offset = -(abs(offset - priv->item_height) % priv->item_height);

        if(offset >= 0)
        {
            cross_item_nr = offset / priv->item_height + 1;
            if(priv->position <= 0)
            {
                priv->y_offset = 0;
            }
            if(priv->position - cross_item_nr <= 0)
            {
                cross_item_nr = priv->position;
            }

            priv->index = (priv->index - cross_item_nr + priv->items_nr) % priv->items_nr;
            priv->position -= cross_item_nr;
        }
    }

#endif

    priv->last_mouse_pos = y;
    ftk_list_relayout(thiz);

    return RET_OK;
}

static int ftk_list_get_item_index(FtkWidget* thiz, FtkWidget* widget)
{
    int i = 0;
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL && priv != NULL, -1);
    return_val_if_fail(widget != NULL, -1);

    for(i = 0; i < priv->items_nr; i++)
    {
        ListItemInfo* item = ftk_list_get_item(thiz, i);
        if(item->widget == widget)
        {
            return i;
        }
    }

    return -1;
}

static Ret ftk_list_set_selected(FtkWidget* thiz, FtkWidget* item)
{
    int index = ftk_list_get_item_index(thiz, item);
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);
    return_val_if_fail(priv->items_nr > 0, RET_FAIL);

    if(index != -1)
    {
        priv->selected = priv->position + (index - priv->index + priv->items_nr) % priv->items_nr;
        /* ftk_logi("--> %s : position=%d, index=%d, offset=%d\n", __func__, priv->position, priv->index, index); */

    }

    return RET_OK;
}

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
	
	if (ftk_widget_has_attr(thiz, FTK_ATTR_IGNORE_EVENT))
	{
		return thiz;
	}
	else
	{
		return NULL;
	}
}

static Ret ftk_list_dispatch_mouse_event(FtkWidget* thiz, FtkEvent* event)
{
	Ret ret = RET_OK;
	FtkWidget* target = NULL;
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL, RET_FAIL);

	if(priv->grab_widget != NULL)
    {
        target = priv->grab_widget;
	}
    else
    {
        target = ftk_list_find_target(thiz, event->u.mouse.x, event->u.mouse.y);
    }

    ftk_widget_event(target, event);

    if(event->type == FTK_EVT_MOUSE_DOWN)
    {
        ftk_list_set_selected(thiz, target);
        ftk_list_relayout(thiz);
    }

	return ret;
}

static Ret ftk_list_on_event(FtkWidget* thiz, FtkEvent* event)
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
			int x = event->u.mouse.x;
			int y = event->u.mouse.y;

            ftk_window_ungrab(ftk_widget_toplevel(thiz), thiz);

            priv->mouse_pressed = 0;
            priv->last_mouse_pos = 0;
            
            ftk_list_dispatch_mouse_event(thiz, event);

            if(FTK_POINT_IN_WIDGET(x, y, thiz))
            {
                ret = FTK_CALL_LISTENER(priv->listener, priv->listener_ctx, thiz);
            }

            break;
        }
		case FTK_EVT_MOUSE_DOWN:
		{
            ftk_window_grab(ftk_widget_toplevel(thiz), thiz);

            priv->mouse_pressed = 1;
            priv->last_mouse_pos = event->u.mouse.y;

            ftk_list_dispatch_mouse_event(thiz, event);

			break;
		}
		case FTK_EVT_MOUSE_MOVE:
        {
            if(priv->move_support)
            {
                ftk_list_mouse_move(thiz, event);
            }
            break;
        }
		case FTK_EVT_RESIZE:
		case FTK_EVT_MOVE_RESIZE:
		{
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

        if(priv->item_array != NULL)
        {
            FTK_FREE(priv->item_array);
        }

        if(priv->item_style != NULL)
        {
            FTK_FREE(priv->item_style);
        }

		FTK_ZFREE(priv, sizeof(PrivInfo));
	}

	return;
}

FtkWidget* ftk_list_create(FtkWidget* parent, int x, int y, int width, int height)
{
    FtkWidget* thiz = (FtkWidget*)FTK_ZALLOC(sizeof(FtkWidget));
	return_val_if_fail(thiz != NULL, NULL);
	
	thiz->priv_subclass[0] = (PrivInfo*)FTK_ZALLOC(sizeof(PrivInfo));
	if(thiz->priv_subclass[0] != NULL)
	{
        DECL_PRIV0(thiz, priv);
        memset(priv, 0, sizeof(PrivInfo));

		thiz->on_event = ftk_list_on_event;
		thiz->on_paint = ftk_list_on_paint;
		thiz->destroy  = ftk_list_destroy;

        ftk_widget_init(thiz, FTK_LIST, 0, x, y, width, height, FTK_ATTR_BG_FOUR_CORNER | FTK_ATTR_OVERFLOW_HIDDEN);
        ftk_widget_append_child(parent, thiz);
	}
	else
	{
		FTK_ZFREE(thiz, sizeof(FtkWidget));
	}

	return thiz;
}

Ret ftk_list_set_move_support(FtkWidget* thiz, int support)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

	priv->move_support = support;

	return RET_OK;
}

Ret ftk_list_set_total(FtkWidget* thiz, int nr)
{
    DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

    priv->total = nr;

    return RET_OK;
}

Ret ftk_list_page_prev(FtkWidget* thiz)
{
    DECL_PRIV0(thiz, priv);
    /* int visible_nr = (priv->y_offset != 0) ? priv->visible_nr + 1 : priv->visible_nr; */
    int visible_nr = priv->visible_nr;
	return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

    if(priv->move_support)
    {
        return RET_OK;
    }

    if(priv->total <= 0 || priv->items_nr <= 0)
    {
        return RET_OK;
    }

    priv->position = (priv->position - visible_nr) > 0 ? (priv->position - visible_nr) : 0;
    priv->index = (priv->index - visible_nr + priv->items_nr) % priv->items_nr;

    ftk_list_relayout(thiz);

    return RET_OK;
}

Ret ftk_list_page_next(FtkWidget* thiz)
{
    DECL_PRIV0(thiz, priv);
    /* int visible_nr = (priv->y_offset != 0) ? priv->visible_nr + 1 : priv->visible_nr; */
    int visible_nr = priv->visible_nr;
	return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

    if(priv->move_support)
    {
        return RET_OK;
    }
    if(priv->total <= 0 || priv->items_nr <= 0)
    {
        return RET_OK;
    }

    priv->position = (priv->position + visible_nr) < priv->total ? (priv->position + visible_nr) : priv->position;
    priv->index = (priv->index + visible_nr) % priv->items_nr;

    /* 最后一页，选中行可能需要调整 */
    if(priv->position + priv->selected >= priv->total)
    {
        priv->selected = priv->total - priv->position - 1;
    }

    ftk_list_relayout(thiz);

    return RET_OK;
}

Ret ftk_list_reset(FtkWidget* thiz)
{
    DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

    priv->position = 0;
    priv->index = 0;
    priv->total = 0;
    priv->selected = 0;
    priv->y_offset = 0;

    ftk_list_relayout(thiz);

    return RET_OK;
}

Ret ftk_list_remove(FtkWidget* thiz, int index)
{
    DECL_PRIV0(thiz, priv);
    /* int visible_nr = (priv->y_offset != 0) ? priv->visible_nr + 1 : priv->visible_nr; */
    int visible_nr = priv->visible_nr;
	return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

    if(priv->total <= 0)
    {
        return RET_OK;
    }

    priv->total = priv->total > 0 ? priv->total - 1: 0;

    if(priv->total == 0)
    {
        return ftk_list_reset(thiz);
    }

    if(priv->move_support == 0)
    {
        /* 判断是否需要返回前面一页 */
        if(priv->position >= priv->total)
        {
            priv->position = (priv->position - visible_nr) > 0 ? (priv->position - visible_nr) : 0;
            /* priv->index = (priv->index - visible_nr + priv->items_nr) % priv->items_nr; */
            priv->selected = priv->position;
        }
        else
        {
            /* 判断是否需要切换选中条 */
            if(priv->selected >= priv->total)
            {
                priv->selected = (priv->selected > 0) ? (priv->selected - 1) : 0;
            }
        }
    }
    else
    {
        priv->position = (priv->position > 0) ? priv->position - 1 : 0;
        if(priv->position == 0)
        {
            priv->y_offset = 0;
        }

        priv->selected = (priv->selected > 0) ? priv->selected - 1 : 0;
    }

    ftk_list_reset_items(thiz);  
    ftk_list_relayout(thiz);

    return RET_OK;
}

int ftk_list_get_total_page_num(FtkWidget* thiz)
{
    DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, 0);

    if(priv->visible_nr == 0)
    {
        return 0;
    }

    return (priv->total % priv->visible_nr == 0) ? (priv->total / priv->visible_nr) : (priv->total / priv->visible_nr + 1);
}

int ftk_list_get_cur_page_num(FtkWidget* thiz)
{
    DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, 0);

    if(priv->visible_nr == 0)
    {
        return 0;
    }

    return priv->position / priv->visible_nr;
}

Ret ftk_list_grab(FtkWidget* thiz, FtkWidget* grab_widget)
{
    DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

    priv->grab_widget = grab_widget;

    return RET_OK;
}

Ret ftk_list_ungrab(FtkWidget* thiz, FtkWidget* grab_widget)
{
    DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

    priv->grab_widget = NULL;

    return RET_OK;
}

Ret ftk_list_set_clicked_listener(FtkWidget* thiz, FtkListener listener, void* ctx)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

	priv->listener_ctx = ctx;
	priv->listener = listener;

	return RET_OK;
}

int ftk_list_get_selected_position(FtkWidget* thiz)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, -1);

	return priv->selected;
}

