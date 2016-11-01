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
 * @file ftk_list_render_ranks.c
 * @Brief  
 * @author wangjl
 * @version v1.0
 * @date 2016-11-02
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
#include "ftk_log.h"
#include "ftk_util.h"
#include "ftk_globals.h"
#include "ftk_list_model_ranks.h"
#include "ftk_list_render_ranks.h"

typedef struct _ListRenderRanksPrivInfo
{
	FtkListModel* model;
	FtkWidget* list;
}PrivInfo;

static Ret ftk_list_render_ranks_init(FtkListRender* thiz, FtkListModel* model, FtkWidget* list)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL && model != NULL, RET_FAIL);

	priv->model = model;
	priv->list = list;

	return RET_OK;
}

static Ret ftk_list_render_ranks_paint(FtkListRender* thiz, FtkCanvas* canvas, int pos, int x, int y, int w, int h)
{
	DECL_PRIV(thiz, priv);
	
	return RET_OK;
}

static void ftk_list_render_ranks_destroy(FtkListRender* thiz)
{
	if(thiz != NULL)
	{
        DECL_PRIV(thiz, priv);
        FTK_ZFREE(thiz, sizeof(FtkListRender) + sizeof(PrivInfo));
	}

	return;
}

FtkListRender* ftk_list_render_ranks_create(void)
{
	FtkListRender* thiz = FTK_NEW_PRIV(FtkListRender);
	
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		thiz->init    = ftk_list_render_ranks_init;
		thiz->paint   = ftk_list_render_ranks_paint;
		thiz->destroy = ftk_list_render_ranks_destroy;
	}

	return thiz;
}


