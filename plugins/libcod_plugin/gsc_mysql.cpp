#include "gsc_mysql.hpp"
#include "gsc_utils.hpp"
#include "include.hpp"

/*
    Spike: Had the problem, that a query failed but no mysql_errno() was set
    Reason: mysql_query() didnt even got executed, because the str was undefined
    So the function quittet with Plugin_Scr_AddInt(0)
    Now its undefined, and i shall test it every time
*/

#include <mysql.h>
#include <pthread.h>

#include <unistd.h>

struct mysql_async_task
{
    mysql_async_task *prev;
    mysql_async_task *next;
    int id;
    MYSQL_RES *result;
    bool done;
    bool started;
    bool save;
    char query[MAX_STRING_CHARS + 1];
};

struct mysql_async_connection
{
    mysql_async_connection *prev;
    mysql_async_connection *next;
    mysql_async_task* task;
    MYSQL *connection;
};

mysql_async_connection *first_async_connection = NULL;
mysql_async_task *first_async_task = NULL;
MYSQL *cod_mysql_connection = NULL;
pthread_mutex_t lock_async_mysql;

void *mysql_async_execute_query(void *input_c) //cannot be called from gsc, is threaded.
{
    mysql_async_connection *c = (mysql_async_connection *) input_c;
    int res = mysql_query(c->connection, c->task->query);
    if(!res && c->task->save)
        c->task->result = mysql_store_result(c->connection);
    else if(res)
    {
        //mysql show error here?
    }
    c->task->done = true;
    c->task = NULL;
    return NULL;
}

void *mysql_async_query_handler(void* input_nothing) //is threaded after initialize
{
    static bool started = false;
    if(started)
    {
        Plugin_Printf("scriptengine> async handler already started. Returning\n");
        return NULL;
    }
    started = true;
    mysql_async_connection *c = first_async_connection;
    if(c == NULL)
    {
        Plugin_Printf("scriptengine> async handler started before any connection was initialized\n"); //this should never happen
        started = false;
        return NULL;
    }
    mysql_async_task *q;
    while(true)
    {
        pthread_mutex_lock(&lock_async_mysql);
        q = first_async_task;
        c = first_async_connection;
        while(q != NULL)
        {
            if(!q->started)
            {
                while(c != NULL && c->task != NULL)
                    c = c->next;
                if(c == NULL)
                {
                    //out of free connections
                    break;
                }
                q->started = true;
                c->task = q;
                pthread_t query_doer;
                int error = pthread_create(&query_doer, NULL, mysql_async_execute_query, c);
                if(error)
                {
                    Plugin_Printf("error: %d\n", error);
                    Plugin_Printf("Error detaching async handler thread\n");
                    return NULL;
                }
                pthread_detach(query_doer);
                c = c->next;
            }
            q = q->next;
        }
        pthread_mutex_unlock(&lock_async_mysql);
        usleep(10000);
    }
    return NULL;
}

int mysql_async_query_initializer(char *sql, bool save) //cannot be called from gsc, helper function
{
    static int id = 0;
    id++;
    pthread_mutex_lock(&lock_async_mysql);
    mysql_async_task *current = first_async_task;
    while(current != NULL && current->next != NULL)
        current = current->next;
    mysql_async_task *newtask = new mysql_async_task;
    newtask->id = id;
    strncpy(newtask->query, sql, MAX_STRING_CHARS);
    newtask->prev = current;
    newtask->result = NULL;
    newtask->save = save;
    newtask->done = false;
    newtask->next = NULL;
    newtask->started = false;
    if(current != NULL)
        current->next = newtask;
    else
        first_async_task = newtask;
    Plugin_Scr_AddInt(id);
    pthread_mutex_unlock(&lock_async_mysql);
    return id;
}


void gsc_mysql_async_create_query_nosave()
{
    char *sql = Plugin_Scr_GetString(0);

    int id = mysql_async_query_initializer(sql, false);
    Plugin_Scr_AddInt(id);
    return;
}

void gsc_mysql_async_create_query()
{
    char *sql = Plugin_Scr_GetString(0);

    int id = mysql_async_query_initializer(sql, true);
//    Plugin_Printf("mysql-id = %d\n", id);
    Plugin_Scr_AddInt(id);
    return;
}

void gsc_mysql_async_getdone_list()
{
    pthread_mutex_lock(&lock_async_mysql);
    mysql_async_task *current = first_async_task;
    Plugin_Scr_MakeArray();
    while(current != NULL)
    {
        if(current->done)
        {
            Plugin_Scr_AddInt((int)current->id);
            Plugin_Scr_AddArray();
        }
        current = current->next;
    }
    pthread_mutex_unlock(&lock_async_mysql);
}

void gsc_mysql_async_getresult_and_free() //same as above, but takes the id of a function instead and returns 0 (not done), undefined (not found) or the mem address of result
{
    int id = Plugin_Scr_GetInt(0);
    pthread_mutex_lock(&lock_async_mysql);
    mysql_async_task *c = first_async_task;
    if(c != NULL)
    {
        while(c != NULL && c->id != id)
            c = c->next;
    }
    if(c != NULL)
    {
        if(!c->done)
        {
            Plugin_Scr_AddUndefined(); //not done yet
            pthread_mutex_unlock(&lock_async_mysql);
            return;
        }
        if(c->next != NULL)
            c->next->prev = c->prev;
        if(c->prev != NULL)
            c->prev->next = c->next;
        else
            first_async_task = c->next;
        if(c->save)
        {
            int ret = (int)c->result;
            Plugin_Scr_AddInt(ret);
        }
        else
            Plugin_Scr_AddInt(0);
        delete c;
        pthread_mutex_unlock(&lock_async_mysql);
        return;
    }
    else
    {
        Plugin_Printf("scriptengine> mysql async query id not found\n");
        Plugin_Scr_AddUndefined();
        pthread_mutex_unlock(&lock_async_mysql);
        return;
    }
}

void gsc_mysql_async_initializer()//returns array with mysql connection handlers
{
    if(first_async_connection != NULL)
    {
        Plugin_Printf("scriptengine> Async mysql already initialized. Returning before adding additional connections\n");
        Plugin_Scr_AddUndefined();
        return;
    }
    if(pthread_mutex_init(&lock_async_mysql, NULL) != 0)
    {
        Plugin_Printf("Async mutex initialization failed\n");
        Plugin_Scr_AddUndefined();
        return;
    }
    int port = Plugin_Scr_GetInt(4); 
    int connection_count = Plugin_Scr_GetInt(5);
    char *host = Plugin_Scr_GetString(0); 
    char *user = Plugin_Scr_GetString(1); 
    char *pass = Plugin_Scr_GetString(2); 
    char *db = Plugin_Scr_GetString(3);

    if(Plugin_Scr_GetType(0) != 2 || Plugin_Scr_GetType(1) != 2 || Plugin_Scr_GetType(2) != 2 || Plugin_Scr_GetType(3) != 2 || Plugin_Scr_GetType(4) != 6 || Plugin_Scr_GetType(5) != 6)
    {
        Plugin_Printf("scriptengine> wrongs args for mysql_async_initializer(...);\n");
        Plugin_Scr_AddUndefined();
        return;
    }
    if(connection_count <= 0)
    {
        Plugin_Printf("Need a positive connection_count in mysql_async_initializer\n");
        Plugin_Scr_AddUndefined();
        return;
    }
    int i;
    Plugin_Scr_MakeArray();
    mysql_async_connection *current = first_async_connection;
    for(i = 0; i < connection_count; i++)
    {
        mysql_async_connection *newconnection = new mysql_async_connection;
        newconnection->next = NULL;
        newconnection->connection = mysql_init(NULL);
        newconnection->connection = mysql_real_connect((MYSQL*)newconnection->connection, host, user, pass, db, port, NULL, 0);
        bool reconnect = true;
        mysql_options(newconnection->connection, MYSQL_OPT_RECONNECT, &reconnect);
        newconnection->task = NULL;
        if(current == NULL)
        {
            newconnection->prev = NULL;
            first_async_connection = newconnection;
        }
        else
        {
            while(current->next != NULL)
                current = current->next;
            current->next = newconnection;
            newconnection->prev = current;
        }
        current = newconnection;
        Plugin_Scr_AddInt((int)newconnection->connection);
        Plugin_Scr_AddArray();
    }

    pthread_t async_handler;
    if(pthread_create(&async_handler, NULL, mysql_async_query_handler, NULL))
    {
        Plugin_Printf("Error detaching async handler thread\n");
        return;
    }
    pthread_detach(async_handler);
}

void gsc_mysql_init()
{
    MYSQL *connection = mysql_init(NULL);
    if(connection != NULL)
    {
        Plugin_Scr_AddInt((int)connection);
    }
    else
    {
        Plugin_Scr_AddUndefined();
    }
}

void gsc_mysql_reuse_connection()
{
    if(cod_mysql_connection == NULL)
    {
        Plugin_Scr_AddUndefined();
        return;
    }
    else
    {
        Plugin_Scr_AddInt((int) cod_mysql_connection);
        return;
    }
}

void gsc_mysql_real_connect()
{
    MYSQL *m = (MYSQL*)Plugin_Scr_GetInt(0);
    char *host = Plugin_Scr_GetString(1);
    char *user = Plugin_Scr_GetString(2);
    char *pass = Plugin_Scr_GetString(3);
    char *db = Plugin_Scr_GetString(4);
    int port = Plugin_Scr_GetInt(5);
    if(m != NULL)
    {
        int mysql = (int) mysql_real_connect(m, host, user, pass, db, port, NULL, 0);

        bool reconnect = true;
        mysql_options((MYSQL*)mysql, MYSQL_OPT_RECONNECT, &reconnect);

        if(cod_mysql_connection == NULL)
            cod_mysql_connection = m;
        Plugin_Scr_AddInt((int)mysql);
    } else {
        Plugin_Scr_AddUndefined();
    }
}

void gsc_mysql_error()
{
    MYSQL *m = (MYSQL*)Plugin_Scr_GetInt(0);
    Plugin_Scr_AddString((char*)mysql_error(m));
}

void gsc_mysql_errno()
{
    MYSQL *m = (MYSQL*)Plugin_Scr_GetInt(0);
    Plugin_Scr_AddInt(mysql_errno(m));
}

void gsc_mysql_close()
{
    MYSQL *m = (MYSQL*)Plugin_Scr_GetInt(0);
    mysql_close(m);
    Plugin_Scr_AddUndefined();
}

void gsc_mysql_query()
{
    MYSQL *m = (MYSQL*)Plugin_Scr_GetInt(0);
    char* query = Plugin_Scr_GetString(1);
    Plugin_Scr_AddInt((int)mysql_query(m, query));
}

void gsc_mysql_affected_rows()
{
    MYSQL *m = (MYSQL*)Plugin_Scr_GetInt(0);
    Plugin_Scr_AddInt((int)mysql_affected_rows(m));
}

void gsc_mysql_store_result()
{
    MYSQL *m = (MYSQL*)Plugin_Scr_GetInt(0);
    Plugin_Scr_AddInt((int)mysql_store_result(m));
}

void gsc_mysql_num_rows()
{
    MYSQL_RES *m = (MYSQL_RES*)Plugin_Scr_GetInt(0);
    Plugin_Scr_AddInt((int)mysql_num_rows(m));
}

void gsc_mysql_num_fields()
{
    MYSQL_RES *m = (MYSQL_RES*)Plugin_Scr_GetInt(0);
    Plugin_Scr_AddInt((int)mysql_num_fields(m));
}

void gsc_mysql_field_seek()
{
    MYSQL_RES *m = (MYSQL_RES*)Plugin_Scr_GetInt(0);
    int offset = Plugin_Scr_GetInt(1);
    Plugin_Scr_AddInt((int)mysql_field_seek(m, offset));
}

void gsc_mysql_fetch_field()
{
    MYSQL_RES *res = (MYSQL_RES*)Plugin_Scr_GetInt(0);
    MYSQL_FIELD *f = mysql_fetch_field(res);
    if(!f)
        Plugin_Scr_AddUndefined();
    else
        Plugin_Scr_AddString(f->name);
}

void gsc_mysql_fetch_row()
{
    int result = Plugin_Scr_GetInt(0);

    #if DEBUG_MYSQL
    Com_Printf("gsc_mysql_fetch_row(result=%d)\n", result);
    #endif

    MYSQL_ROW row = mysql_fetch_row((MYSQL_RES *)result);
    if (!row)
    {
        #if DEBUG_MYSQL
        Com_Printf("row == NULL\n");
        #endif
        Plugin_Scr_AddUndefined();
        return;
    }

    Plugin_Scr_MakeArray();

    int numfields = mysql_num_fields((MYSQL_RES *)result);
    for (int i=0; i<numfields; i++)
    {
        if (row[i] == NULL)
            Plugin_Scr_AddUndefined();
        else
            Plugin_Scr_AddString(row[i]);

        #if DEBUG_MYSQL
        Com_Printf("row == \"%s\"\n", row[i]);
        #endif
        Plugin_Scr_AddArray();
    }
}

void gsc_mysql_free_result()
{
    MYSQL_RES *res = (MYSQL_RES*)Plugin_Scr_GetInt(0);
    mysql_free_result(res);
    Plugin_Scr_AddUndefined();
}

void gsc_mysql_real_escape_string()
{
    MYSQL *m = (MYSQL*)Plugin_Scr_GetInt(0);
    char* escape = Plugin_Scr_GetString(1);
    char* str = new char[strlen(escape) * 2 + 1];
    mysql_real_escape_string(m, str, escape, strlen(escape));
    Plugin_Scr_AddString(str);
    free(str);
}