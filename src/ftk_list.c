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
#include "ftk_scroll_bar.h"


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

typedef struct _ListPrivInfo
{
    int initialized;

    int y_offset;
    int item_height;
    int position;
    int index;
    int total;

    int items_nr;

    int visible_nr;

    int move_support;

    int selected;
    /* FtkWidget* grab_widget; */

    int mouse_pressed;
    int last_mouse_pos;
    int delta;
    int moved;

	FtkWidget* scroll_bar;
    /* int scroll_bar_value; */
	
	void* listener_ctx;
	FtkListener listener;

    FtkListCallBacks callbacks;
}PrivInfo;

static FtkWidget* ftk_list_get_item(FtkWidget* thiz, int index)
{
    int i = 0;
    FtkWidget* iter = ftk_widget_child(thiz);
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL && priv != NULL, NULL);
    return_val_if_fail(index >=0 && priv->items_nr > 0, NULL);

    index =  index % priv->items_nr;            /* make sure the index is in (0~items_nr) */

    for( ; iter != NULL; iter = ftk_widget_next(iter))
    {
        if(ftk_widget_type(iter) != FTK_LIST_ITEM)
        {
            continue;
        }

        if(index == i)
        {
            return iter;
        }
        else
        {
            i++;
        }
    }

    return NULL;
}

static int ftk_list_get_item_index(FtkWidget* thiz, FtkWidget* item)
{
    int i = 0;
    FtkWidget* iter = ftk_widget_child(thiz);
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL && priv != NULL, -1);
    return_val_if_fail(index >=0 && priv->items_nr > 0, -1);

    for( ; iter != NULL; iter = ftk_widget_next(iter))
    {
        if(ftk_widget_type(iter) != FTK_LIST_ITEM)
        {
            continue;
        }

        if(item == iter)
        {
            return i;
        }
        else
        {
            i++;
        }
    }

    return -1;
}

static Ret ftk_list_reset_items(FtkWidget* thiz)
{
    int i = 0;
    FtkWidget* iter = ftk_widget_child(thiz);
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);
    return_val_if_fail(index >=0 && priv->items_nr > 0, RET_FAIL);

    for( ; iter != NULL; iter = ftk_widget_next(iter))
    {
        if(ftk_widget_type(iter) != FTK_LIST_ITEM)
        {
            continue;
        }

        ftk_list_item_set_scraped(iter, 1);
        ftk_list_item_set_selected(iter, 0);
    }

    return RET_OK;
}

static Ret ftk_list_on_item_refresh(FtkWidget* thiz, FtkWidget* item)
{
    int c = 0;
    int index = ftk_list_get_item_index(thiz, item);
    int position = -1;
    FtkWidget* iter = ftk_widget_child(item);

	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);
    return_val_if_fail(item != NULL, RET_FAIL);
    return_val_if_fail(index >= 0, RET_FAIL);

    if(priv->callbacks.validate_item == NULL)
    {
        return RET_OK;
    }

    position = priv->position + (index - priv->index + priv->items_nr) % priv->items_nr;

    for(; iter != NULL; iter = ftk_widget_next(iter), c++)
    {
        priv->callbacks.validate_item(thiz, position, c, iter, 1, priv->callbacks.validate_item_ctx);
    }

    return RET_OK;
}

static Ret ftk_list_set_item_active(FtkWidget* thiz, FtkWidget* item)
{
    int index = ftk_list_get_item_index(thiz, item);
    int position = -1;
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);
    return_val_if_fail(item != NULL, RET_FAIL);

    /* scrap change to active */
    if(ftk_list_item_is_scraped(item))       
    {
        ftk_list_item_set_scraped(item, 0);
        ftk_list_on_item_refresh(thiz, item);
    }

    /* set selected */
    position = priv->position + (index - priv->index + priv->items_nr) % priv->items_nr;
    if(priv->selected != position)
    {
        ftk_list_item_set_selected(item, 0);
    }
    else
    {
        ftk_list_item_set_selected(item, 1);
    }

    ftk_widget_set_visible(item, 1);
    ftk_widget_hide_self(item, 0);

    return RET_OK;
}

static Ret ftk_list_set_item_scrap(FtkWidget* thiz, FtkWidget* item)
{
    /* return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL); */
    return_val_if_fail(item != NULL, RET_FAIL);

    ftk_list_item_set_scraped(item, 1);

    ftk_widget_set_visible(item, 0);
    ftk_widget_hide_self(item, 1);

    return RET_OK;
}

static Ret ftk_list_relayout(FtkWidget* thiz)
{
    int i = 0;
    int visible_nr = 0;
	DECL_PRIV0(thiz, priv);
    int total = priv->total;
    return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);
    return_val_if_fail(priv->items_nr > 0, RET_FAIL);

    if(total - priv->position <= priv->visible_nr)
    {
        visible_nr = (total - priv->position >= 0) ? (total - priv->position) : 0;
    }
    else
    {
        visible_nr = (priv->y_offset != 0) ? (priv->visible_nr + 1) : (priv->visible_nr);
    }

    for(i = 0; i < visible_nr; i++)             /* show */
    {
        FtkWidget* item = ftk_list_get_item(thiz, priv->index + i);

        ftk_list_set_item_active(thiz, item);
        ftk_widget_set_top(item, priv->y_offset + priv->item_height * i); /* relayout the list item */
        /* ftk_logi("-->%s, visible_nr=%d, y=%d\n", __func__, visible_nr, priv->y_offset + priv->item_height * i); */
    }

    for(i = 0; i < priv->items_nr - visible_nr; i++) /* hide */
    {
        FtkWidget* item = ftk_list_get_item(thiz, priv->index + visible_nr + i);

        ftk_list_set_item_scrap(thiz, item);
    }

    ftk_widget_invalidate(thiz);
   
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
        FtkWidget* item = priv->callbacks.get_item(thiz, i, priv->callbacks.get_item_ctx);

        if(item != NULL)
        {
            ftk_widget_append_child(parent, item);
        }
    }

    return RET_OK;
}

static Ret ftk_list_move_scroll_bar(FtkWidget* thiz, FtkEvent* event)
{
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

    if(priv->scroll_bar != NULL)
    {
        if(ftk_scroll_bar_get_value(priv->scroll_bar) != priv->position)
        {
            ftk_scroll_bar_set_value(priv->scroll_bar, priv->position);
        }
    }

    return RET_OK;
}

static Ret ftk_list_move_items(FtkWidget* thiz, FtkEvent* event)
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

    priv->moved = 1;

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

    priv->last_mouse_pos = y;
    ftk_list_relayout(thiz);

    return RET_OK;
}

static Ret ftk_list_set_selected(FtkWidget* thiz, FtkWidget* item)
{
    int index = ftk_list_get_item_index(thiz, item);
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);
    return_val_if_fail(priv->items_nr > 0, RET_FAIL);
    return_val_if_fail(item != NULL, RET_FAIL);

    priv->selected = priv->position + (index - priv->index + priv->items_nr) % priv->items_nr;

    return RET_OK;
}

static Ret ftk_list_reset_scroll_bar(FtkWidget* thiz)
{
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

    if(priv->scroll_bar != NULL)
    {
        if(priv->total <= priv->visible_nr)
        {
            ftk_widget_set_visible(priv->scroll_bar, 0);
            ftk_widget_hide_self(priv->scroll_bar, 1);
        }
        else
        {
            ftk_widget_set_visible(priv->scroll_bar, 1);
            ftk_widget_hide_self(priv->scroll_bar, 0);
            ftk_scroll_bar_set_param(priv->scroll_bar, priv->position, priv->total, priv->visible_nr);
        }
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
 
#if 0
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
#endif

static Ret ftk_list_move_selected(FtkWidget* thiz , int offset)
{
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

    if(priv->total <= 0)
    {
        return RET_FAIL;
    }

    if(priv->selected == 0 && offset < 0)
    {
        return RET_FAIL;
    }

	if((priv->selected + 1) == priv->total && offset > 0)
    {
        return RET_FAIL;
    }

    priv->selected += offset;

	if(priv->selected < 0) priv->selected = 0;
	if(priv->selected >= priv->total) priv->selected = priv->total - 1;

    if(priv->selected < priv->position)
	{
		priv->position = priv->selected;
	}

	if(priv->selected > (priv->position + priv->visible_nr - 1))
	{
		priv->position = priv->selected - (priv->visible_nr - 1);
	}

    ftk_list_relayout(thiz);

    return RET_REMOVE;
}

static Ret ftk_list_on_key_event(FtkWidget* thiz, FtkEvent* event)
{
	Ret ret = RET_OK;
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);
    return_val_if_fail(event != NULL, RET_FAIL);

	if(event->type != FTK_EVT_KEY_DOWN){
        return RET_OK;
    }

	switch(event->u.key.code)
	{
        case FTK_KEY_DOWN:
        case FTK_KEY_RIGHT:
		{
		    ret = ftk_list_move_selected(thiz, 1);
			break;
		}
        case FTK_KEY_LEFT:
        case FTK_KEY_UP:
		{
		    ret = ftk_list_move_selected(thiz, -1);
			break;
		}
		case FTK_KEY_ENTER:
        {
            FtkWidget* item = ftk_list_get_item(thiz, priv->index + priv->selected - priv->position);
            FtkWidget* iter = ftk_widget_child(item);
            for(; iter != NULL; iter = ftk_widget_next(iter))
            {
                if(ftk_widget_is_insensitive(iter) || !ftk_widget_is_visible(iter)
                        || ftk_widget_has_attr(iter, FTK_ATTR_NO_FOCUS))
                {
                    continue;
                }

                ftk_window_set_focus(ftk_widget_toplevel(iter), iter);
                ret = RET_REMOVE;
                break;
            }
            break;
        }
		default: break;
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
            ret = ftk_list_on_key_event(thiz, event);
			break;
		}
		case FTK_EVT_MOUSE_UP:
        {
			int x = event->u.mouse.x;
			int y = event->u.mouse.y;

            ftk_window_ungrab(ftk_widget_toplevel(thiz), thiz);

            priv->mouse_pressed = 0;
            priv->last_mouse_pos = 0;

            /* ftk_list_dispatch_mouse_event(thiz, event); */

            if(priv->moved == 0)                /* if has moved, insensitive to MOUSE_UP */
            {
                if(FTK_POINT_IN_WIDGET(x, y, thiz))
                {
                    FtkWidget* target = ftk_list_find_target(thiz, x, y);

                    return_val_if_fail(target != NULL, RET_OK);

                    ftk_list_set_selected(thiz, target);
                    ret = FTK_CALL_LISTENER(priv->listener, priv->listener_ctx, thiz);

                    ftk_list_on_item_refresh(thiz, target); /* 点击后此item的内容有可能改变，需要刷新一次 */
                    /* ftk_widget_invalidate(thiz); */
                    ftk_list_relayout(thiz); /*  */
                }
            }

            break;
        }
		case FTK_EVT_MOUSE_DOWN:
		{
            ftk_window_grab(ftk_widget_toplevel(thiz), thiz);

            priv->mouse_pressed = 1;
            priv->last_mouse_pos = event->u.mouse.y;
            priv->moved = 0;

            /* ftk_list_dispatch_mouse_event(thiz, event); */

			break;
		}
		case FTK_EVT_MOUSE_MOVE:
        {
            if(priv->move_support)
            {
                ftk_list_move_scroll_bar(thiz, event);
                ftk_list_move_items(thiz, event);
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

		FTK_ZFREE(priv, sizeof(PrivInfo));
	}

	return;
}

Ret ftk_list_set_item_height(FtkWidget* thiz, int item_height)
{
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

    priv->item_height =item_height;

    return RET_OK;
}

Ret ftk_list_init(FtkWidget* thiz, int total, FtkListCallBacks* callbacks)
{
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);
    return_val_if_fail(priv->item_height > 0, RET_FAIL);

    if(priv->initialized)
    {
        return RET_OK;
    }

    priv->y_offset = 0;
    priv->visible_nr = ftk_widget_height(thiz) / priv->item_height;

    priv->items_nr = priv->visible_nr * 2;

    priv->initialized = 1;
    priv->position = 0;
    priv->index = 0;
    priv->delta = 5;
    priv->selected = 0;

    priv->total = total;

    memcpy(&priv->callbacks, callbacks, sizeof(FtkListCallBacks));

    ftk_list_reset_scroll_bar(thiz);

    ftk_list_create_items(thiz, priv->items_nr);

    ftk_list_relayout(thiz);

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

    ftk_list_reset_scroll_bar(thiz);

    return RET_OK;
}

int ftk_list_get_total(FtkWidget* thiz)
{
    DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, 0);

    return priv->total;
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
    priv->selected = -1;
    priv->y_offset = 0;

    ftk_list_reset_scroll_bar(thiz);
	ftk_list_reset_items(thiz);
    ftk_list_relayout(thiz);

    return RET_OK;
}

Ret ftk_list_remove(FtkWidget* thiz, int index)
{
    DECL_PRIV0(thiz, priv);
    /* int visible_nr = (priv->y_offset != 0) ? priv->visible_nr + 1 : priv->visible_nr; */
    int visible_nr = priv->visible_nr;
	return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);
    return_val_if_fail(index < 0, RET_FAIL);

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
            priv->index = (priv->index - visible_nr + priv->items_nr) % priv->items_nr;

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
        if(priv->position > 0)
        {
            priv->position = priv->position - 1;
            priv->index = (priv->index - 1 + priv->items_nr) % priv->items_nr;
        }
        else
        {
            priv->y_offset = 0;
        }

        priv->selected = (priv->selected > 0) ? priv->selected - 1 : 0;
    }

    ftk_list_reset_scroll_bar(thiz);
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

#if 0
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
#endif

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

Ret ftk_list_add_scroll_bar(FtkWidget* thiz, FtkWidget* scroll_bar)
{
    DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

	priv->scroll_bar = scroll_bar;

	return RET_OK;
}

Ret ftk_list_refresh(FtkWidget* thiz)
{
    DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

    ftk_list_reset_scroll_bar(thiz);
	ftk_list_reset_items(thiz);
    ftk_list_relayout(thiz);

	return RET_OK;
}

