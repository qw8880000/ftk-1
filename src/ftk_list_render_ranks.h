#ifndef  __FTK_LIST_RENDER_RANKS_H__
#define  __FTK_LIST_RENDER_RANKS_H__

FTK_BEGIN_DECLS

#include "ftk_list_render.h"

typedef Ret (*FtkListRenderPaintListener)(void* ctx, int pos, int row, int col, FtkWidget* cell);

FtkListRender* ftk_list_render_ranks_create(void);

FTK_END_DECLS

#endif   /* ----- #ifndef __FTK_LIST_RENDER_RANKS_H__ ----- */
