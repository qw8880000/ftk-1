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
 * @file ftk_tab_title.c
 * @Brief  
 * @author wangjl
 * @version v1.0
 * @date 2016-11-30
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
#include "ftk_tab_title.h"
#include "ftk_window.h"
#include "ftk_canvas.h"
#include "ftk_theme.h"
#include "ftk_globals.h"

typedef struct _TabTitlePrivInfo
{
    int selected;
	FtkBitmap* bg_selected;

	FtkListener listener;
	void* listener_ctx;
}PrivInfo;

static Ret ftk_tab_title_on_event(FtkWidget* thiz, FtkEvent* event)
{
	Ret ret = RET_OK;
	DECL_PRIV0(thiz, priv);
	switch(event->type)
	{
		case FTK_EVT_MOUSE_DOWN:
		{
			ftk_widget_set_active(thiz, 1);
			ftk_window_grab(ftk_widget_toplevel(thiz), thiz);
			//ftk_logd("%s: FTK_EVT_MOUSE_DOWN: %s\n", __func__, ftk_widget_get_text(thiz));
			break;
		}
		case FTK_EVT_MOUSE_UP:
		{
			int x = event->u.mouse.x;
			int y = event->u.mouse.y;

			ftk_window_ungrab(ftk_widget_toplevel(thiz), thiz);
			if(ftk_widget_is_active(thiz))
			{
				ftk_widget_set_active(thiz, 0);
				if(FTK_POINT_IN_WIDGET(x, y, thiz))
				{
					ret = FTK_CALL_LISTENER(priv->listener, priv->listener_ctx, thiz);
				}
			}
			//ftk_logd("%s: FTK_EVT_MOUSE_UP: %s\n", __func__, ftk_widget_get_text(thiz));
			break;
		}
		case FTK_EVT_KEY_DOWN:
		{
			if(FTK_IS_ACTIVE_KEY(event->u.key.code) && !ftk_widget_is_active(thiz))
			{
				ftk_widget_set_active(thiz, 1);
			}
			break;
		}
		case FTK_EVT_KEY_UP:
		{
			if(FTK_IS_ACTIVE_KEY(event->u.key.code) && ftk_widget_is_active(thiz))
			{
				ftk_widget_set_active(thiz, 0);
				ret = FTK_CALL_LISTENER(priv->listener, priv->listener_ctx, thiz);
			}
			break;
		}
		default:break;
	}

	return ret;
}

static Ret ftk_tab_title_on_paint(FtkWidget* thiz)
{
	DECL_PRIV0(thiz, priv);
	FTK_BEGIN_PAINT(x, y, width, height, canvas);

    return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

	ftk_canvas_reset_gc(canvas, ftk_widget_get_gc(thiz)); 

    if(priv->selected)
    {
        ftk_canvas_draw_bg_image(canvas, priv->bg_selected, FTK_BG_FOUR_CORNER, x, y, width, height);
    }

	if(ftk_widget_get_text(thiz) != NULL)
	{
		int xoffset = 0;
		int yoffset = FTK_HALF(height);
		FtkTextLine line = {0};
		const char* text = ftk_widget_get_text(thiz);
		FtkTextLayout* text_layout = ftk_default_text_layout();

		ftk_text_layout_init(text_layout, text, -1, canvas, width); 
		if(ftk_text_layout_get_visual_line(text_layout, &line) == RET_OK)
		{
			xoffset = FTK_HALF(width - line.extent); 
			ftk_canvas_draw_string(canvas, x + xoffset, y + yoffset, line.text, line.len, 1);
		}
	}

	FTK_END_PAINT();
}

static void ftk_tab_title_destroy(FtkWidget* thiz)
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

FtkWidget* ftk_tab_title_create(FtkWidget* parent, int x, int y, int width, int height)
{
	FtkWidget* thiz = (FtkWidget*)FTK_ZALLOC(sizeof(FtkWidget));
	return_val_if_fail(thiz != NULL, NULL);
	
	thiz->priv_subclass[0] = (PrivInfo*)FTK_ZALLOC(sizeof(PrivInfo));
	if(thiz->priv_subclass[0] != NULL)
	{
		thiz->on_event = ftk_tab_title_on_event;
		thiz->on_paint = ftk_tab_title_on_paint;
		thiz->destroy  = ftk_tab_title_destroy;

		ftk_widget_init(thiz, FTK_TAB_TITLE, 0, x, y, width, height, FTK_ATTR_TRANSPARENT|FTK_ATTR_BG_FOUR_CORNER);
		ftk_widget_append_child(parent, thiz);
	}
	else
	{
		FTK_FREE(thiz);
	}

	return thiz;
}

Ret ftk_tab_title_set_clicked_listener(FtkWidget* thiz, FtkListener listener, void* ctx)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL, RET_FAIL);

	priv->listener_ctx = ctx;
	priv->listener = listener;

	return RET_OK;
}

Ret ftk_tab_title_set_selected(FtkWidget* thiz, int selected)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);

    priv->selected = selected;

	return RET_OK;
}

Ret ftk_tab_title_set_bg_selected(FtkWidget* thiz, FtkBitmap* bitmap)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);
	return_val_if_fail(bitmap != NULL, RET_FAIL);

    if(priv->bg_selected != NULL)
    {
        ftk_bitmap_unref(priv->bg_selected);
    }

    priv->bg_selected = bitmap;

	return RET_OK;
}

