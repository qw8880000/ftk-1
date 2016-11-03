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
#include "ftk_widget.h"
#include "ftk_globals.h"
#include "ftk_list.h"
#include "ftk_list_model_ranks.h"
#include "ftk_list_render_ranks.h"

typedef struct _ListRenderRanksPrivInfo
{
	FtkListModel* model;
	FtkWidget* list;

    FtkListRenderPaintListener paint_listerner;
    void* listerner_ctx;
}PrivInfo;

static Ret ftk_list_render_ranks_paint_row(FtkListRender* thiz, int visible_pos, int row, int visible)
{
	DECL_PRIV(thiz, priv);
    FtkWidget* list = priv->list;
    FtkListModel* model = priv->model;
    int c = 0;

    return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);
    return_val_if_fail(list != NULL, RET_FAIL);
    return_val_if_fail(model != NULL, RET_FAIL);


    if(visible)
    {
        FtkListModelRanksInfo info = {0};
        FtkListModelRanksInfo* p_info = &info;

        for(c=0; c<ftk_list_get_cols_nr(list); c++) 
        {
            FtkWidget* cell = ftk_list_get_cell(list, row, c);
            info.row_index = visible_pos;
            info.cell_index = c;
            ftk_list_model_get_data(model, 0, (void**)&p_info);

            ftk_widget_set_visible(cell, 1);
            if(info.text != NULL)
            {
                ftk_widget_set_text(cell, info.text);
            }
            else
            {
                ftk_widget_set_text(cell, " ");
            }

            if(priv->paint_listerner != NULL)
            {
                priv->paint_listerner(priv->listerner_ctx, visible_pos, row, c, cell);
            }
        }


        ftk_widget_set_insensitive(ftk_list_get_item(list, row), 0);
    }
    else
    {
        for(c=0; c<ftk_list_get_cols_nr(list); c++) 
        {
            ftk_widget_set_visible(ftk_list_get_cell(list, row, c), 0);
        }

        ftk_widget_set_insensitive(ftk_list_get_item(list, row), 1);
    }

    return RET_OK;
}

static Ret ftk_list_render_ranks_init(FtkListRender* thiz, FtkListModel* model, FtkWidget* list)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL && model != NULL, RET_FAIL);

	priv->model = model;
	priv->list = list;

	return RET_OK;
}


static Ret ftk_list_render_ranks_paint(FtkListRender* thiz, FtkCanvas* canvas, int visible_start, int visible_nr, int total, int w, int h)
{
    int r = 0;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);
    return_val_if_fail(visible_start < total, RET_FAIL);

    ftk_logi("%s-> visible_start=%d, visible_nr=%d, total=%d\n", __func__, visible_start, visible_nr, total);

    for(r=0; r<visible_nr; r++)
    {
        if(visible_start + r < total)     
        {
            ftk_list_render_ranks_paint_row(thiz, visible_start + r, r, 1);
        }
        else
        {
            ftk_list_render_ranks_paint_row(thiz, visible_start + r, r, 0);
        }
    }

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


