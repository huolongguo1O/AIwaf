#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#define DEBUG
// waf 配置结构
typedef struct 
{
  ngx_int_t waf_counter;  // 计数配置
} ngx_http_waf_loc_conf_t;

int check(ngx_http_request_t *r);
// 上下文回调函数
static ngx_int_t ngx_http_waf_init( ngx_conf_t *cf );
static void *ngx_http_waf_create_loc_conf( ngx_conf_t *cf );

// 配置处理函数
static char *ngx_waf_set( ngx_conf_t *, ngx_command_t *, void * );
// 回调函数
static ngx_int_t ngx_http_waf_handler( ngx_http_request_t * );


// 配置项
static ngx_command_t ngx_http_waf_commands[] = {
  {
    ngx_string("waf"),    // 配置名称waf_counter
    NGX_HTTP_LOC_CONF | NGX_CONF_FLAG,  // 配置为bool类型，取值为on/off
    ngx_waf_set,            // 配置处理函数
    NGX_HTTP_LOC_CONF_OFFSET,
    0,
    NULL
  },
  ngx_null_command  // 以null结尾 
};

// 模块上下文
static ngx_http_module_t ngx_waf_ctx = {
  NULL, 
  ngx_http_waf_init,        // 读取该模块配置后调用 
  NULL, 
  NULL, 
  NULL, 
  NULL, 
  ngx_http_waf_create_loc_conf,   // 读取到location配置后调用(每个location创建一个)
  NULL
};

// 模块定义
ngx_module_t ngx_http_waf_module = {
  NGX_MODULE_V1,
  &ngx_waf_ctx,
  ngx_http_waf_commands,
  NGX_HTTP_MODULE,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NGX_MODULE_V1_PADDING
};



static char *ngx_waf_set( ngx_conf_t *cf, ngx_command_t *cmd, void *conf ) {
  ngx_http_waf_loc_conf_t *local_conf;
  local_conf = conf;
  char *rv = NULL;

  // 读取NGX_CONF_FLAG类型参数
  rv = ngx_conf_set_flag_slot(cf, cmd, conf);
  
  #ifdef DEBUG
  ngx_conf_log_error( NGX_LOG_INFO, cf, 0, "waf:%d", local_conf->waf_counter );
  #endif
  return rv;
}


static void *ngx_http_waf_create_loc_conf(ngx_conf_t *cf)
{
  ngx_http_waf_loc_conf_t* local_conf = NULL;
  local_conf = ngx_pcalloc( cf->pool, sizeof(ngx_http_waf_loc_conf_t) );
  if ( local_conf == NULL )
  {
    return NULL;
  }

  // 初始设置默认值
  local_conf->waf_counter = NGX_CONF_UNSET;
  return local_conf;
}

static ngx_int_t ngx_http_waf_init(ngx_conf_t *cf)
{
  ngx_http_handler_pt *h;
  ngx_http_core_main_conf_t *cmcf;

  cmcf = ngx_http_conf_get_module_main_conf( cf, ngx_http_core_module );

  h = ngx_array_push( &cmcf->phases[NGX_HTTP_ACCESS_PHASE].handlers );
  if (h == NULL) {
    return NGX_ERROR;
  }

  // 在 NGX_HTTP_CONTENT_PHASE 阶段设置回调函数
  *h = ngx_http_waf_handler;
  return NGX_OK;
}


static int ngx_waf_visited_times = 0; // 访问次数

static ngx_int_t ngx_http_waf_handler( ngx_http_request_t *r ) {
  // ngx_int_t rc;
  ngx_buf_t *b;
  ngx_chain_t out;
  ngx_http_waf_loc_conf_t *my_conf;
  ngx_uint_t content_length = 0;
  u_char ngx_waf_string[1024] = {0};
  #ifdef DEBUG
  ngx_log_error( NGX_LOG_EMERG, r->connection->log, 0, "ngx_http_waf_handler is called!" );
  #endif
  // 获取配置值 
  my_conf = ngx_http_get_module_loc_conf( r, ngx_http_waf_module );
  if ( my_conf->waf_counter == NGX_CONF_UNSET || my_conf->waf_counter == 0 )
  {
    //ngx_http_process_request(r);
    //ngx_http_handler(r); 
    return 0;
  }
  else
  {
    ngx_sprintf( ngx_waf_string, 
          "<h1>Visited Times:%d</h1>", 
          ++ngx_waf_visited_times );
  }
  #ifdef DEBUG
  ngx_log_error( NGX_LOG_EMERG, r->connection->log, 0, "waf_string:%s", ngx_waf_string );
  #endif
  if(!check(r)){
  #ifdef DEBUG
  ngx_log_error( NGX_LOG_EMERG, r->connection->log, 0, "Point one!" );
  #endif
    //ngx_http_process_request(r);
    //ngx_http_handler(r);
    //return NGX_OK;
      #ifdef DEBUG
  ngx_log_error( NGX_LOG_EMERG, r->connection->log, 0, "Point three!" );
  #endif
    return NGX_OK;
  }
    #ifdef DEBUG
  ngx_log_error( NGX_LOG_EMERG, r->connection->log, 0, "Point two!" );
  #endif
  ngx_sprintf( ngx_waf_string, 
          "<h1>Your request has been dropped by AIwaf.</h1>", 
          ++ngx_waf_visited_times );
  content_length = ngx_strlen( ngx_waf_string );
  ngx_log_error( NGX_LOG_EMERG, r->connection->log, 0, "Attack:%s", ngx_waf_string );
  // 分配响应缓冲区
  b = ngx_pcalloc( r->pool, sizeof(ngx_buf_t) );
  out.buf = b;  // attach 
  out.next = NULL;
  
  b->pos = ngx_waf_string;
  b->last = ngx_waf_string + content_length;
  b->memory = 1;
  b->last_buf = 1;

  // 设置响应
  ngx_str_set( &r->headers_out.content_type, "text/html" );
  r->headers_out.status = NGX_HTTP_FORBIDDEN;
  r->headers_out.content_length_n = content_length;
  
  // 发送响应
  /*rc =*/ ngx_http_send_header( r );
    #ifdef DEBUG
  ngx_log_error( NGX_LOG_EMERG, r->connection->log, 0, "Point four!" );
  #endif
  // 传递至其它过滤器处理
  //return NGX_ABORT; // 终止过滤器链
  return ngx_http_output_filter( r, &out );
}
