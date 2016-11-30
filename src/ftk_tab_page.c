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
 * @file ftk_tab_page.c
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

#include "ftk_event.h"
#include "ftk_globals.h"
#include "ftk_window.h"
typedef struct _ListItemPrivInfo
{
	void* listener_ctx;
	FtkListener listener;
}PrivInfo;

static Ret ftk_tab_page_on_event(FtkWidget* thiz, FtkEvent* event)
{
	return RET_OK;
}


static void ftk_tab_page_destroy(FtkWidget* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV0(thiz, priv);

        FTK_ZFREE(priv, sizeof(PrivInfo));
	}

	return;
}

static Ret ftk_tab_page_on_paint(FtkWidget* thiz)
{

    FTK_END_PAINT();
}

FtkWidget* ftk_tab_page_create(FtkWidget* parent, int x, int y, int width, int height)
{
	FtkWidget* thiz = (FtkWidget*)FTK_ZALLOC(sizeof(FtkWidget));
	return_val_if_fail(thiz != NULL, NULL);
	
	thiz->priv_subclass[0] = (PrivInfo*)FTK_ZALLOC(sizeof(PrivInfo));
	if(thiz->priv_subclass[0] != NULL)
	{
		DECL_PRIV0(thiz, priv);
		thiz->on_event = ftk_tab_page_on_event;
		thiz->on_paint = ftk_tab_page_on_paint;
		thiz->destroy  = ftk_tab_page_destroy;

		ftk_widget_init(thiz, FTK_TAB_PAGE, 0, x, y, width, height, FTK_ATTR_INSENSITIVE|FTK_ATTR_BG_FOUR_CORNER);
		ftk_widget_append_child(parent, thiz);
	}
	else
	{
		FTK_ZFREE(thiz, sizeof(FtkWidget));
	}

	return thiz;
}

