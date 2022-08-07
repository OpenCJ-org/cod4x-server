#include "include.hpp"
#include "gsc_mysql.hpp"
#include "gsc_player.hpp"
#include "gsc_utils.hpp"

PCL int OnInit()
{
    Plugin_ScrAddMethod("aimbuttonpressed"                      , Gsc_Player_ButtonAds);
    Plugin_ScrAddMethod("leftbuttonpressed"                     , Gsc_Player_ButtonLeft);
    Plugin_ScrAddMethod("rightbuttonpressed"                    , Gsc_Player_ButtonRight);
    Plugin_ScrAddMethod("forwardbuttonpressed"                  , Gsc_Player_ButtonForward);
    Plugin_ScrAddMethod("backbuttonpressed"                     , Gsc_Player_ButtonBack);
    Plugin_ScrAddMethod("jumpbuttonpressed"                     , Gsc_Player_ButtonJump);
    Plugin_ScrAddMethod("leanleftbuttonpressed"                 , Gsc_Player_ButtonLeanLeft);
    Plugin_ScrAddMethod("leanrightbuttonpressed"                , Gsc_Player_ButtonLeanRight);
    Plugin_ScrAddMethod("setVelocity"                           , Gsc_Player_Velocity_Set);
    Plugin_ScrAddMethod("addVelocity"                           , Gsc_Player_Velocity_Add);
    Plugin_ScrAddMethod("getSpectatorClient"                    , Gsc_Player_SpectatorClientGet);
    Plugin_ScrAddMethod("getip"                                 , Gsc_Player_GetIP);
    Plugin_ScrAddMethod("getQueuedReliableMessages"             , gsc_getQueuedReliableMessages);
    Plugin_ScrAddMethod("save_initplayer", gsc_save_initplayer);
    Plugin_ScrAddMethod("save_createsave", gsc_save_createsave);
    Plugin_ScrAddMethod("save_setcompleted", gsc_save_setcompleted);
    Plugin_ScrAddMethod("save_loadpos", gsc_save_loadpos);
    Plugin_ScrAddMethod("save_setentity", gsc_save_setentity);
    Plugin_ScrAddMethod("save_getentity", gsc_save_getentity);
    Plugin_ScrAddMethod("save_setcheated", gsc_save_setcheated);
    Plugin_ScrAddMethod("save_getcheated", gsc_save_getcheated);
    Plugin_ScrAddMethod("save_setbigjump", gsc_save_setbigjump);
    Plugin_ScrAddMethod("save_getbigjump", gsc_save_getbigjump);
    Plugin_ScrAddMethod("save_addtrigger", gsc_save_addtrigger);
    Plugin_ScrAddMethod("save_gettriggers", gsc_save_gettriggers);
    Plugin_ScrAddMethod("save_setjetpack", gsc_save_setjetpack);
    Plugin_ScrAddMethod("save_getjetpack", gsc_save_getjetpack);
    Plugin_ScrAddMethod("save_setloadout", gsc_save_setloadout);
    Plugin_ScrAddMethod("save_getloadout", gsc_save_getloadout);
    Plugin_ScrAddMethod("save_setorigin", gsc_save_setorigin);
    Plugin_ScrAddMethod("save_getorigin", gsc_save_getorigin);
    Plugin_ScrAddMethod("save_setangles", gsc_save_setangles);
    Plugin_ScrAddMethod("save_getangles", gsc_save_getangles);
    Plugin_ScrAddMethod("save_addcp", gsc_save_addcp);
    Plugin_ScrAddMethod("save_getcps", gsc_save_getcps);
    Plugin_ScrAddMethod("save_addpassed_checkpoint", gsc_save_addpassed_checkpoint);
    Plugin_ScrAddMethod("save_getpassed_checkpoints", gsc_save_getpassed_checkpoints);
    Plugin_ScrAddMethod("save_copysave", gsc_save_copysave);
		Plugin_ScrAddMethod("save_setos_mode", gsc_save_setos_mode);
		Plugin_ScrAddMethod("save_setfunmode", gsc_save_setfunmode);
		Plugin_ScrAddMethod("save_settime_played", gsc_save_settime_played);
		Plugin_ScrAddMethod("save_setload_count", gsc_save_setload_count);
		Plugin_ScrAddMethod("save_setsave_count", gsc_save_setsave_count);
		Plugin_ScrAddMethod("save_setjump_count", gsc_save_setjump_count);
		Plugin_ScrAddMethod("save_setnadejumps", gsc_save_setnadejumps);
		Plugin_ScrAddMethod("save_setnadethrows", gsc_save_setnadethrows);
		Plugin_ScrAddMethod("save_setrun_id", gsc_save_setrun_id);
		Plugin_ScrAddMethod("save_setdistance_travelled", gsc_save_setdistance_travelled);
		Plugin_ScrAddMethod("save_setfps", gsc_save_setfps);
		Plugin_ScrAddMethod("save_setcansave", gsc_save_setcansave);
		Plugin_ScrAddMethod("save_getcansave", gsc_save_getcansave);
		Plugin_ScrAddMethod("save_getfps", gsc_save_getfps);
		Plugin_ScrAddMethod("issprinting", gsc_player_issprinting);
		Plugin_ScrAddMethod("client_download", gsc_client_download);
		Plugin_ScrAddMethod("save_setrpg_out", gsc_save_setrpg_out);
		Plugin_ScrAddMethod("save_getrpg_out", gsc_save_getrpg_out);
		Plugin_ScrAddMethod("save_getnadejumps", gsc_save_getnadejumps);

		Plugin_ScrAddFunction("save_init_cps", gsc_save_cps_init);

    Plugin_ScrAddFunction("getAscii"                            , Gsc_Utils_GetAscii);
    Plugin_ScrAddFunction("sprintf"                             , Gsc_Utils_sprintf);
    Plugin_ScrAddFunction("add_language"                        , Gsc_Utils_AddLanguage);
    Plugin_ScrAddFunction("load_languages"                      , Gsc_Utils_LoadLanguages);
    Plugin_ScrAddFunction("get_language_item"                   , Gsc_Utils_GetLanguageItem);
    Plugin_ScrAddFunction("file_exists"                         , Gsc_Utils_FileExists);
    Plugin_ScrAddFunction("Cmd_ExecuteString"                   , Gsc_Utils_CmdExecuteString);
    Plugin_ScrAddFunction("printf"                              , Gsc_Utils_Printf);
    Plugin_ScrAddFunction("fopen"                               , Gsc_Utils_Fopen);
    Plugin_ScrAddFunction("fread"                               , Gsc_Utils_Fread);
//    Plugin_ScrAddFunction("fwrite"                              , Gsc_Utils_Fwrite);
    Plugin_ScrAddFunction("fclose"                              , Gsc_Utils_Fclose);
    Plugin_ScrAddFunction("G_FindConfigstringIndex"             , Gsc_G_FindConfigstringIndex);
    Plugin_ScrAddFunction("sendGameServerCommand"               , Gsc_Utils_SendGameServerCommand);
    Plugin_ScrAddFunction("mysql_init"                          , gsc_mysql_init);
    Plugin_ScrAddFunction("mysql_real_connect"                  , gsc_mysql_real_connect);
    Plugin_ScrAddFunction("mysql_close"                         , gsc_mysql_close);
    Plugin_ScrAddFunction("mysql_query"                         , gsc_mysql_query);
    Plugin_ScrAddFunction("mysql_errno"                         , gsc_mysql_errno);
    Plugin_ScrAddFunction("mysql_error"                         , gsc_mysql_error);
    Plugin_ScrAddFunction("mysql_affected_rows"                 , gsc_mysql_affected_rows);
    Plugin_ScrAddFunction("mysql_store_result"                  , gsc_mysql_store_result);
    Plugin_ScrAddFunction("mysql_num_rows"                      , gsc_mysql_num_rows);
    Plugin_ScrAddFunction("mysql_num_fields"                    , gsc_mysql_num_fields);
    Plugin_ScrAddFunction("mysql_field_seek"                    , gsc_mysql_field_seek);
    Plugin_ScrAddFunction("mysql_fetch_field"                   , gsc_mysql_fetch_field);
    Plugin_ScrAddFunction("mysql_fetch_row"                     , gsc_mysql_fetch_row);
    Plugin_ScrAddFunction("mysql_free_result"                   , gsc_mysql_free_result);
    Plugin_ScrAddFunction("mysql_real_escape_string"            , gsc_mysql_real_escape_string);
    Plugin_ScrAddFunction("mysql_async_create_query"            , gsc_mysql_async_create_query);
    Plugin_ScrAddFunction("mysql_async_create_query_nosave"     , gsc_mysql_async_create_query_nosave);
    Plugin_ScrAddFunction("mysql_async_initializer"             , gsc_mysql_async_initializer);
    Plugin_ScrAddFunction("mysql_async_getdone_list"            , gsc_mysql_async_getdone_list);
    Plugin_ScrAddFunction("mysql_async_getresult_and_free"      , gsc_mysql_async_getresult_and_free);
    Plugin_ScrAddFunction("mysql_reuse_connection"              , gsc_mysql_reuse_connection);
    Plugin_ScrAddFunction("gethex"                              , gsc_utils_gethex);
    Plugin_ScrAddFunction("stringtofloat"                       , gsc_utils_stringToFloat);
    Plugin_ScrAddFunction("renice"                              , gsc_utils_renice);
    Plugin_ScrAddFunction("forcedlcvars"                        , gsc_misc_forcedlcvars);
    
    return 0;
}

PCL void OnInfoRequest(pluginInfo_t *info){ // Function used to obtain information about the plugin
    // Memory pointed by info is allocated by the server binary, just fill in the fields
    
    // =====  MANDATORY FIELDS  =====
    info->handlerVersion.major = PLUGIN_HANDLER_VERSION_MAJOR;
    info->handlerVersion.minor = PLUGIN_HANDLER_VERSION_MINOR;  // Requested handler version, we request the version compatible with this plugin lib
    
    // =====  OPTIONAL  FIELDS  =====
    info->pluginVersion.major = 1;
    info->pluginVersion.minor = 0;  // Plugin version
    strncpy(info->fullName,"Libcod",sizeof(info->fullName)); //Full plugin name
    strncpy(info->shortDescription,"This is the plugin's short description.",sizeof(info->shortDescription)); // Short plugin description
    strncpy(info->longDescription,"This is the plugin's long description.",sizeof(info->longDescription));
}