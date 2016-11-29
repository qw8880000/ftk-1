/* --------------------------------------------------------------------------*/
/* * 
 * @file main.c
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
#include "ftk_app_orange.h"

#ifdef HAS_MAIN
int main(int argc, char* argv[])
#else
Ret ftk_main(int argc, char* argv[])
#endif
{
#ifdef HAS_MAIN
	ftk_init(argc, argv);
#endif
	ftk_app_run(ftk_app_orange_create(), argc, argv);	
#ifdef HAS_MAIN
	ftk_run();
#endif

	return RET_OK;
}
