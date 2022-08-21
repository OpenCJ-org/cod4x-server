#include "shared_defs.h"
#include "gsc_player.hpp"
#include "gsc_utils.hpp"

PCL int OnInit()
{
    Plugin_ScrAddMethod("addVelocity"                           , Gsc_Player_Velocity_Add);
    Plugin_ScrAddMethod("getip"                                 , Gsc_Player_GetIP);
    Plugin_ScrAddMethod("getQueuedReliableMessages"             , gsc_getQueuedReliableMessages);
    Plugin_ScrAddMethod("issprinting", gsc_player_issprinting);
    Plugin_ScrAddMethod("client_download", gsc_client_download);


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