#include "PltObject.h"
#include <mysql/mysql.h> //you can change this to path where
//your libmysqlclient-dev headers are placed

using namespace std;
int TypeMistmatch(string pattern,PltArgs args)
{
  int k = 0;
  for(int k=0;k<pattern.length();k+=1)
  {
    if(pattern[k]!=args[k].type)
      return k;//argument number k is causing a type mismatch
  }
  return -1;//all good
}
extern "C"
{
  PltObject connect(PltArgs args)
  {
    if(args.size()!=4)
      return Plt_Err(VALUE_ERROR,"4 arguments needed!");
    int k = TypeMistmatch("ssss",args);
    if(k!=-1)
      return Plt_Err(TYPE_ERROR,"Invalid type of argument "+to_string(k));
    MYSQL* conn = new MYSQL;
    mysql_init(conn);
    if(!mysql_real_connect(conn, args[0].s.c_str(),
         args[1].s.c_str(), args[2].s.c_str(),args[3].s.c_str(),0,NULL,0)) {
        return Plt_Err(UNKNOWN_ERROR,mysql_error(conn));
    }
    PltObject ret;
    ret.type = 'c';
    ret.ptr = (void*)conn;
    ret.s = "MYSQL Connection";
    return ret;
  }
  PltObject query(PltArgs args)
  {
    if(args.size()!=2)
      return Plt_Err(VALUE_ERROR,"2 arguments needed!");
    if(args[0].type!='c' || args[0].s!="MYSQL Connection")
      return Plt_Err(TYPE_ERROR,"First argument should be of type Mysql_connection");
    if(args[1].type!='s')
      return Plt_Err(TYPE_ERROR,"Second argument should be a string!");
    MYSQL* conn = (MYSQL*)args[0].ptr;
    string query = args[1].s;
    if(mysql_query(conn,query.c_str())!=0)
       return Plt_Err(UNKNOWN_ERROR,mysql_error(conn));
    PltObject ret;
    return ret;
  }

  PltObject get_result(PltArgs args)
  {
    if(args.size()!=1)
      return Plt_Err(VALUE_ERROR,"2 arguments needed!");
    if(args[0].type!='c' || args[0].s!="MYSQL Connection")
      return Plt_Err(TYPE_ERROR,"First argument should be of type Mysql_connection");
    MYSQL* conn = (MYSQL*)args[0].ptr;
    MYSQL_RES* res = mysql_store_result(conn);
    int totalrows = mysql_num_rows(res);
    int numfields = mysql_num_fields(res);
    MYSQL_FIELD *mfield;
    MYSQL_ROW row;
    PltList result;//will be deleted by interpreter automatically
    while((row = mysql_fetch_row(res)))
    {
      PltList currRow;
      for(size_t i = 0; i < numfields; i++)
      {
        string val = row[i];
        currRow.push_back(val);
      }
      PltObject r = currRow;
      result.push_back(r);
    }
    mysql_free_result(res);
    PltObject ret = result;
    return ret;
  }
  PltObject close_connection(PltArgs args)
  {
    if(args.size()!=1)
      return Plt_Err(VALUE_ERROR,"2 arguments needed!");
    if(args[0].type!='c' || args[0].s!="MYSQL Connection")
      return Plt_Err(TYPE_ERROR,"First argument should be of type Mysql_connection");
    MYSQL* conn = (MYSQL*)args[0].ptr;
    mysql_close(conn);
    delete conn;
    PltObject ret;
    
    return ret;
  }

}
