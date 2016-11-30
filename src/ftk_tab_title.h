#ifndef  __FTK_TAB_TITLE_H__
#define  __FTK_TAB_TITLE_H__

#include "ftk_widget.h"

FTK_BEGIN_DECLS

FtkWidget* ftk_tab_title_create(FtkWidget* parent, int x, int y, int width, int height);
Ret ftk_tab_title_set_clicked_listener(FtkWidget* thiz, FtkListener listener, void* ctx);
Ret ftk_tab_title_set_selected(FtkWidget* thiz, int selected);
Ret ftk_tab_title_set_bg_selected(FtkWidget* thiz, FtkBitmap* bitmap);

FTK_END_DECLS

#endif   /* ----- #ifndef __FTK_TAB_TITLE_H__ ----- */
