#ifndef  __FTK_LIST_ITEM_H__
#define  __FTK_LIST_ITEM_H__

FTK_BEGIN_DECLS

FtkWidget* ftk_list_item_create(FtkWidget* parent, int x, int y, int width, int height);

Ret ftk_list_item_set_bg_selected(FtkWidget* thiz, FtkBitmap* bitmap);
Ret ftk_list_item_set_selected(FtkWidget* thiz, int selected);

FTK_END_DECLS

#endif   /* ----- #ifndef __FTK_LIST_ITEM_H__ ----- */
