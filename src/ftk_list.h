#ifndef  __FTK_LIST_H__
#define  __FTK_LIST_H__

#include "ftk_widget.h"
#include "ftk_list_model.h"

FTK_BEGIN_DECLS

FtkWidget* ftk_list_create(FtkWidget* parent, int x, int y, int width, int height);
int ftk_list_update(FtkWidget* thiz);
void ftk_list_set_visible_nr(FtkWidget* thiz, int nr);
void ftk_list_page_prev(FtkWidget* thiz);
void ftk_list_page_next(FtkWidget* thiz);
int ftk_list_get_total_page_num(FtkWidget* thiz);
int ftk_list_get_cur_page_num(FtkWidget* thiz);
FtkListModel* ftk_list_get_model(FtkWidget* thiz);

FTK_END_DECLS

#endif   /* ----- #ifndef __FTK_LIST_H__ ----- */
