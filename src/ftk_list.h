#ifndef  __FTK_LIST_H__
#define  __FTK_LIST_H__

#include "ftk_widget.h"
#include "ftk_list_model.h"

FTK_BEGIN_DECLS

typedef Ret (*FtkListValidateItem)(FtkWidget* thiz, int position, int col, FtkWidget* cell, int visible, void* ctx);
typedef FtkWidget* (*FtkListGetItem)(FtkWidget* thiz, int index, void* ctx);

typedef struct _FtkListCallBacks
{
    FtkListValidateItem validate_item;
    void* validate_item_ctx;

    FtkListGetItem get_item;
    void* get_item_ctx;
}FtkListCallBacks;

FtkWidget* ftk_list_create(FtkWidget* parent, int x, int y, int width, int height);

Ret ftk_list_set_move_support(FtkWidget* thiz, int support);
Ret ftk_list_set_total(FtkWidget* thiz, int nr);

Ret ftk_list_set_item_height(FtkWidget* thiz, int item_height);
Ret ftk_list_init(FtkWidget* thiz, FtkListCallBacks* callbacks);
// Ret ftk_list_refresh(FtkWidget* thiz);

Ret ftk_list_page_prev(FtkWidget* thiz);
Ret ftk_list_page_next(FtkWidget* thiz);
Ret ftk_list_reset(FtkWidget* thiz);
Ret ftk_list_remove(FtkWidget* thiz, int index);
int ftk_list_get_total_page_num(FtkWidget* thiz);
int ftk_list_get_cur_page_num(FtkWidget* thiz);

Ret ftk_list_grab(FtkWidget* thiz, FtkWidget* grab_widget);
Ret ftk_list_ungrab(FtkWidget* thiz, FtkWidget* grab_widget);

Ret ftk_list_set_clicked_listener(FtkWidget* thiz, FtkListener listener, void* ctx);
int ftk_list_get_selected_position(FtkWidget* thiz);

FTK_END_DECLS

#endif   /* ----- #ifndef __FTK_LIST_H__ ----- */
