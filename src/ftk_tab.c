/*
 * File: ftk_tab.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   tab widget
 *
 * Copyright (c) 2009 - 2010  Li XianJing <xianjimli@hotmail.com>
 *
 * Licensed under the Academic Free License version 2.1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * History:
 * ================================================================
 * 2010-09-23 Li XianJing <xianjimli@hotmail.com> created
 *
 * History:
 * ================================================================
 * 2016-12-01 wangjl <qw8880000@126.com> modify 
 */

#include "ftk_tab.h"
#include "ftk_tab_title.h"
#include "ftk_theme.h"
#include "ftk_globals.h"
#include "ftk_window.h"

#define FTK_TAB_VIEW_MAX    20

typedef struct _TabPrivInfo
{
    int title_nr;
    int active_title;
    FtkWidget* title_array[FTK_TAB_VIEW_MAX];

    int page_nr;
    int active_page;
	FtkWidget* page_array[FTK_TAB_VIEW_MAX];

}PrivInfo;

static Ret ftk_tab_on_event(FtkWidget* thiz, FtkEvent* event)
{
    return RET_OK;
}

static Ret ftk_tab_on_paint(FtkWidget* thiz)
{
    return RET_OK;
}

static void ftk_tab_destroy(FtkWidget* thiz)
{
    return ;
}

FtkWidget* ftk_tab_create(FtkWidget* parent, int x, int y, int width, int height)
{
	FtkWidget* thiz = (FtkWidget*)FTK_ZALLOC(sizeof(FtkWidget));
	return_val_if_fail(thiz != NULL, NULL);
	
	thiz->priv_subclass[0] = (PrivInfo*)FTK_ZALLOC(sizeof(PrivInfo));
	if(thiz->priv_subclass[0] != NULL)
	{
		DECL_PRIV0(thiz, priv);
        memset(priv, 0, sizeof(PrivInfo));

		thiz->on_event = ftk_tab_on_event;
		thiz->on_paint = ftk_tab_on_paint;
		thiz->destroy  = ftk_tab_destroy;

		ftk_widget_init(thiz, FTK_TAB, 0, x, y, width, height, FTK_ATTR_TRANSPARENT|FTK_ATTR_INSENSITIVE);
		ftk_widget_append_child(parent, thiz);
	}
	else
	{
		FTK_FREE(thiz);
	}

	return thiz;
}

static int ftk_tab_get_title_index(FtkWidget* thiz, FtkWidget* title)
{
    int i = 0;
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL && priv != NULL, -1);
    return_val_if_fail(title != NULL, -1);

    for(i = 0; i < priv->title_nr; i++)
    {
        if(priv->title_array[i] == title)
        {
            return i;
        }
    }

    return -1;
}

static Ret ftk_tab_title_clicked(void* ctx, void* obj)
{
    FtkWidget* thiz = (FtkWidget*)ctx;
    FtkWidget* title = (FtkWidget*)obj;
	DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);
    return_val_if_fail(title != NULL, RET_FAIL);

    if(ftk_tab_get_title_index(thiz, title) != -1)
    {
        ftk_tab_set_selected(thiz, ftk_tab_get_title_index(thiz, title));
    }

    return RET_OK;
}

Ret ftk_tab_add_title(FtkWidget* thiz, FtkWidget* title)
{
	DECL_PRIV0(thiz, priv);

    return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);
    return_val_if_fail(priv->title_nr >=0 && priv->title_nr < FTK_TAB_VIEW_MAX, RET_FAIL);

    priv->title_array[priv->title_nr] = title;
    priv->title_nr++;

	ftk_tab_title_set_clicked_listener(title, ftk_tab_title_clicked, thiz);

    return RET_OK;
}

Ret ftk_tab_add_page(FtkWidget* thiz, FtkWidget* page)
{
    DECL_PRIV0(thiz, priv);

    return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);
    return_val_if_fail(priv->page_nr >=0 && priv->page_nr < FTK_TAB_VIEW_MAX, RET_FAIL);

    priv->page_array[priv->page_nr] = page;
    priv->page_nr++;

    return RET_OK;
}

Ret ftk_tab_set_selected(FtkWidget* thiz, int index)
{
    int i = 0;
    DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);
    return_val_if_fail(index >=0 && index < priv->title_nr, RET_FAIL);

    for(i = 0; i < priv->title_nr; i++)
    {
        FtkWidget* title = priv->title_array[i];
        FtkWidget* page = priv->page_array[i];

        if(title == NULL || page == NULL)
        {
            break;
        }

        if(i == index)
        {
            ftk_tab_title_set_selected(title, 1);
            ftk_widget_set_visible(page, 1);
            ftk_widget_hide_self(page, 0);
        }
        else
        {
            ftk_tab_title_set_selected(title, 0);
            ftk_widget_set_visible(page, 0);
            ftk_widget_hide_self(page, 1);
        }
    }

    ftk_widget_invalidate(thiz);

    return RET_OK;
}

