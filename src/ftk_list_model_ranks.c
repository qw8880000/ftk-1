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
 * @file ftk_list_model_ranks.c
 * @Brief  
 * @author wangjl
 * @version v1.0
 * @date 2016-11-01
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
#include "ftk.h"
#include "ftk_list_model_ranks.h"

#define CELL_ADD_STEP    6
#define ROW_ADD_STEP    10

typedef struct _FtkListModelCell
{
    char* text;
}FtkListModelCell;

typedef struct _FtkListModelRow
{
    int cell_nr;
    int cell_alloc_nr;
    int cell_current;
    FtkListModelCell* cells;
}FtkListModelRow;

typedef struct _ListModelRanksPrivInfo
{   
    /* row */
	int row_nr;
	int row_alloc_nr;
    int row_current;
	FtkListModelRow* rows;
}PrivInfo;

static Ret ftk_list_model_ranks_destroy_row(FtkListModelRow* row)
{
    int c = 0;
    return_val_if_fail(row != NULL, RET_FAIL);

    for(c = 0; c < row->cell_nr; c++)
    {
        FtkListModelCell* cell = row->cells+c;
        if(cell->text != NULL)
        {
            FTK_FREE(cell->text);
        }
    }
    FTK_FREE(row->cells);

    memset(row, 0, sizeof(FtkListModelRow));

	return RET_OK;
}



static Ret ftk_list_model_ranks_extend_row(FtkListModel* thiz, size_t delta)
{
	int row_alloc_nr = 0;
	DECL_PRIV(thiz, priv);
	FtkListModelRow* rows = NULL;
	return_val_if_fail(thiz != NULL, RET_FAIL);

	if((priv->row_nr + delta) < priv->row_alloc_nr)
	{
		return RET_OK;
	}

	row_alloc_nr = (priv->row_alloc_nr + delta) + ROW_ADD_STEP;
	rows = (FtkListModelRow*)FTK_REALLOC(priv->rows, sizeof(FtkListModelRow) * row_alloc_nr);
	if(rows != NULL)
	{
        /* initial the new memory alloc */
        memset(rows+priv->row_alloc_nr, 0, sizeof(FtkListModelRow) * (row_alloc_nr - priv->row_alloc_nr));

		priv->rows = rows;
		priv->row_alloc_nr = row_alloc_nr;
	}

	return (priv->row_nr + delta) < priv->row_alloc_nr ? RET_OK : RET_FAIL;
}

static Ret ftk_list_model_ranks_extend_cell(FtkListModel* thiz, int row_index, size_t delta)
{
	int cell_alloc_nr = 0;
	DECL_PRIV(thiz, priv);
	FtkListModelRow* row = priv->rows + row_index;
    FtkListModelCell* cells = NULL;
	return_val_if_fail(thiz != NULL && priv->rows != NULL, RET_FAIL);

	if((row->cell_nr + delta) < row->cell_alloc_nr)
	{
		return RET_OK;
	}

	cell_alloc_nr = (row->cell_alloc_nr + delta) + CELL_ADD_STEP;
	cells = (FtkListModelCell*)FTK_REALLOC(row->cells, sizeof(FtkListModelCell) * cell_alloc_nr);
	if(cells != NULL)
	{
        /* initial the new memory alloc */
        memset(cells+row->cell_alloc_nr, 0, sizeof(FtkListModelCell) * (cell_alloc_nr - row->cell_alloc_nr));

		row->cells = cells;
		row->cell_alloc_nr = cell_alloc_nr;
	}

	return (row->cell_nr + delta) < row->cell_alloc_nr ? RET_OK : RET_FAIL;
}

static Ret ftk_list_model_ranks_add_cell(FtkListModel* thiz, FtkListModelRanksInfo* info)
{
	DECL_PRIV(thiz, priv);
    FtkListModelRow* row = NULL;
    FtkListModelCell* cell = NULL;

	return_val_if_fail(thiz != NULL && info != NULL, RET_FAIL);
    return_val_if_fail(info->cell_index >= 0, RET_FAIL);
    return_val_if_fail(ftk_list_model_ranks_extend_cell(thiz, info->row_index, 1) == RET_OK, RET_FAIL);

    row = priv->rows + info->row_index;
    cell = row->cells + row->cell_nr;

    cell->text = ftk_strdup(info->text);
    row->cell_nr++;

	return RET_OK;
}


static Ret ftk_list_model_ranks_add(FtkListModel* thiz, void* data)
{
	DECL_PRIV(thiz, priv);
	FtkListModelRanksInfo* info = (FtkListModelRanksInfo*)data;

	return_val_if_fail(thiz != NULL && info != NULL, RET_FAIL);
    return_val_if_fail(info->row_index >= 0 && info->row_index <= priv->row_nr, RET_FAIL);
    return_val_if_fail(ftk_list_model_ranks_extend_row(thiz, 1) == RET_OK, RET_FAIL);

    if(ftk_list_model_ranks_add_cell(thiz, info) != RET_OK)
    {
        return RET_FAIL;
    }

    if(info->row_index >= priv->row_nr)
    {
		priv->row_nr++;
    }

    return RET_OK;
}

static Ret ftk_list_model_ranks_reset(FtkListModel* thiz)
{
    int r = 0;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && priv != NULL, RET_FAIL);
	
    for(r = 0; r < priv->row_nr; r++)
    {
        FtkListModelRow* row = priv->rows+r;
        ftk_list_model_ranks_destroy_row(row);
    }

    priv->row_nr = 0;

	return RET_OK;
}

static Ret ftk_list_model_ranks_remove(FtkListModel* thiz, size_t index)
{
	DECL_PRIV(thiz, priv);
    FtkListModelRow* row = priv->rows+index;
	return_val_if_fail(thiz != NULL, RET_FAIL);
    return_val_if_fail(index >=0 && index < priv->row_nr, RET_FAIL);

    ftk_list_model_ranks_destroy_row(row);

	if(index < (priv->row_nr - 1))
	{
		int i = index; 
		for(; i < priv->row_nr; i++)
		{
			memcpy(priv->rows+i, priv->rows+i+1, sizeof(FtkListModelRow));
		}
	}

	priv->row_nr--;

	return RET_OK;
}

static void ftk_list_model_ranks_destroy(FtkListModel* thiz)
{
	int r = 0;
	DECL_PRIV(thiz, priv);
    return_if_fail(priv != NULL);
	
    for(r = 0; r < priv->row_nr; r++)
    {
        FtkListModelRow* row = priv->rows+r;
        ftk_list_model_ranks_destroy_row(row);
    }
    FTK_FREE(priv->rows);
    FTK_ZFREE(thiz, sizeof(FtkListModel) + sizeof(PrivInfo));

    ftk_logd("%s\n", __func__);

	return;
}

static int ftk_list_model_ranks_get_total(FtkListModel* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, 0);

	return priv->row_nr;
}

static Ret  ftk_list_model_ranks_get_data(FtkListModel* thiz, size_t index, void** ret)
{
    DECL_PRIV(thiz, priv);
    FtkListModelRanksInfo* info = (FtkListModelRanksInfo*)*ret;
    FtkListModelRow* row = priv->rows + info->row_index;
    FtkListModelCell* cell = NULL;
	return_val_if_fail(priv != NULL && ret != NULL, RET_FAIL);
    return_val_if_fail(info->row_index >=0 && info->row_index < priv->row_nr, RET_FAIL);
    return_val_if_fail(info->cell_index >=0 && info->cell_index < row->cell_nr, RET_FAIL);
    
    cell = row->cells + info->cell_index;
    info->text = cell->text;

	return RET_OK;
}

FtkListModel* ftk_list_model_ranks_create(void)
{
	FtkListModel* thiz = FTK_NEW_PRIV(FtkListModel);
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		thiz->get_total = ftk_list_model_ranks_get_total;
		thiz->get_data  = ftk_list_model_ranks_get_data;
		thiz->add       = ftk_list_model_ranks_add;
		thiz->reset     = ftk_list_model_ranks_reset;
		thiz->remove    = ftk_list_model_ranks_remove;
		thiz->destroy   = ftk_list_model_ranks_destroy;

		thiz->ref = 1;

        priv->row_alloc_nr = 0;
        priv->row_nr = 0;
        priv->row_current = 0;
        priv->rows = NULL;

        ftk_logd("%s\n", __func__);
	}

	return thiz;
}

