#ifndef  __FTK_LIST_H__
#define  __FTK_LIST_H__

#include "ftk_widget.h"
#include "ftk_list_model.h"

FTK_BEGIN_DECLS

FtkWidget* ftk_list_create(FtkWidget* parent, int x, int y, int width, int height);
Ret ftk_list_update(FtkWidget* thiz);
Ret ftk_list_page_prev(FtkWidget* thiz);
Ret ftk_list_page_next(FtkWidget* thiz);
Ret ftk_list_reset(FtkWidget* thiz);

int ftk_list_get_total_page_num(FtkWidget* thiz);
int ftk_list_get_cur_page_num(FtkWidget* thiz);
FtkListModel* ftk_list_get_model(FtkWidget* thiz);
int ftk_list_get_rows_nr(FtkWidget* thiz);
int ftk_list_get_cols_nr(FtkWidget* thiz);

Ret ftk_list_set_selected_item(FtkWidget* thiz, FtkWidget* item);
Ret ftk_list_set_rows_nr(FtkWidget* thiz, int nr);
Ret ftk_list_set_cols_nr(FtkWidget* thiz, int nr);

FtkWidget* ftk_list_get_item(FtkWidget* thiz, int row);
FtkWidget* ftk_list_get_cell(FtkWidget* thiz, int row, int col);

FTK_END_DECLS

#endif   /* ----- #ifndef __FTK_LIST_H__ ----- */
