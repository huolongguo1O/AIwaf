#include <stdio.h>
#include <Python.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <ngx_http_request.h>
//#define DEBUG
PyObject *check_module;
PyObject *check_function ;
int is_init=0;
int init(){
   is_init=1;
   Py_Initialize();
   PyRun_SimpleString("import check");
   check_module = PyDict_GetItemString(PyImport_GetModuleDict(), "check");
   if (!check_module) {
     fprintf(stderr, "Error: Unable to get 'check' module.\n");
     return -1;
   }

   check_function = PyObject_GetAttrString(check_module, "check");
   if (!check_function) {
     fprintf(stderr, "Error: Unable to get 'check' function.\n");
     return -1;
   }
  #ifdef DEBUG
  printf("Point 2!\n");
  #endif
   return 0;
}

int check(ngx_http_request_t *r) {
   if(is_init==0) init();
   if(r->args.data==NULL){
      return 0;
   }
  #ifdef DEBUG
  //ngx_log_error( NGX_LOG_EMERG, r->connection->log, 0, "args:%s", r->args.data );
  #endif
  //return 0;
  char *toc = (char *)r->args.data;
  #ifdef DEBUG
  ngx_log_error( NGX_LOG_EMERG, r->connection->log, 0, "Point 1!" );
  #endif
   #ifdef DEBUG
  ngx_log_error( NGX_LOG_EMERG, r->connection->log, 0, "Point 4!" );
  #endif
  PyObject *args = Py_BuildValue("(s)", toc);
   #ifdef DEBUG
  ngx_log_error( NGX_LOG_EMERG, r->connection->log, 0, "Point 5!" );
  #endif
  PyObject *result = PyObject_CallObject(check_function, args);
 #ifdef DEBUG
  ngx_log_error( NGX_LOG_EMERG, r->connection->log, 0, "Point 6!" );
  #endif
  if (!result) {
     fprintf(stderr, "Error: Unable to call 'check' function.\n");
     return -1;
  }

  int return_value = PyLong_AsLong(result);
  ngx_log_error( NGX_LOG_EMERG, r->connection->log, 0,"%d",return_value);
  if (PyErr_Occurred()) {
     fprintf(stderr, "Error: Unable to convert return value to int.\n");
     return -1;
  }
  #ifdef DEBUG
  ngx_log_error( NGX_LOG_EMERG, r->connection->log, 0, "Point 3!" );
  #endif
  #ifdef DEBUG
  ngx_log_error( NGX_LOG_EMERG, r->connection->log, 0, "args:%s", r->args.data );
  #endif

  // Py_DECREF(args);
  // Py_DECREF(result);

  // Py_Finalize();

  return return_value;
}
