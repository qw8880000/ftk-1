#ifndef  __FTK_LIST_MODEL_RANKS_H__
#define  __FTK_LIST_MODEL_RANKS_H__

#include "ftk_list_model.h"

FTK_BEGIN_DECLS

typedef struct _FtkListModelRanksInfo
{
    int row_index;
    int cell_index;
    char* text;
}FtkListModelRanksInfo;

FtkListModel* ftk_list_model_ranks_create(void);

FTK_END_DECLS

#endif   /* ----- #ifndef __FTK_LIST_MODEL_RANKS_H__ ----- */

