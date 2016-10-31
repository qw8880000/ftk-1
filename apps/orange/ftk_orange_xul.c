/* --------------------------------------------------------------------------*/
/* * 
 * @file ftk_orange_xul.c
 * @Brief  
 * @author wangjl
 * @version v1.0
 * @date 2016-10-29
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
#include "ftk_xul.h"

static const char* orange_translate_text(void* ctx, const char* text)
{
	return _(text);
}

static const char* orange_translate_path(const char* path, char out_path[FTK_MAX_PATH+1])
{
	ftk_snprintf(out_path, FTK_MAX_PATH, "%s/orange/%s", FTK_ROOT_DIR, path);
	ftk_normalize_path(out_path);

	ftk_logd("%s: %s --> %s\n", __func__, path, out_path);

	return out_path;
}

FtkWidget* ftk_orange_xul_load(const char* filename)
{
	FtkXulCallbacks callbacks = {0};
	char path[FTK_MAX_PATH+1] = {0};
	
	callbacks.ctx = NULL;
	callbacks.translate_text = orange_translate_text;
	callbacks.load_image = (FtkXulLoadImage)ftk_icon_cache_load;

	orange_translate_path(filename, path);

	return ftk_xul_load_file(path, &callbacks);
}

