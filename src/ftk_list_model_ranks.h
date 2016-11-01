#ifndef  __FTK_LIST_MODEL_RANKS_H__
#define  __FTK_LIST_MODEL_RANKS_H__

#include "ftk_list_model.h"

FTK_BEGIN_DECLS

typedef struct _FtkListModelCell
{
    char* text;
}FtkListModelCell;

typedef struct _FtkListModelRow
{
    int cell_nr;
    int current_cell;
    FtkListModelCell* cells;
}FtkListModelRow;

FtkListModel* ftk_list_model_ranks_create(size_t init_nr);

FTK_END_DECLS

#endif   /* ----- #ifndef __FTK_LIST_MODEL_RANKS_H__ ----- */
