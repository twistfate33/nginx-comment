
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_PALLOC_H_INCLUDED_
#define _NGX_PALLOC_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>

/*
�ڴ�س�ʼ����ʱ����Ĳ���size��ʾpagesize�������ڴ��ʱ��
��������pagesize���з��䣬�ڴ�صĲ������ݽṹ�洢�����
pagesize��С���ڴ�ͷ�����ڴ����һ�������������pagesize��С��
�ڴ�����ӳ�������Ϊ�˴洢һЩ�ڴ�������Ϣ��������ͷ
�����ͨԪ�ض�ռ��һЩ�����ֽ�,��������ͷ����ͨ����Ԫ��
���ڵ��ڴ��Ĵ�С����һ���Ĵ�СΪpagesize

*/
/*
 * NGX_MAX_ALLOC_FROM_POOL should be (ngx_pagesize - 1), i.e. 4095 on x86.
 * On Windows NT it decreases a number of locked pages in a kernel.
 */
#define NGX_MAX_ALLOC_FROM_POOL  (ngx_pagesize - 1)

#define NGX_DEFAULT_POOL_SIZE    (16 * 1024)

#define NGX_POOL_ALIGNMENT       16
#define NGX_MIN_POOL_SIZE                                                     \
    ngx_align((sizeof(ngx_pool_t) + 2 * sizeof(ngx_pool_large_t)),            \
              NGX_POOL_ALIGNMENT)


typedef void (*ngx_pool_cleanup_pt)(void *data);

typedef struct ngx_pool_cleanup_s  ngx_pool_cleanup_t;

struct ngx_pool_cleanup_s {
    ngx_pool_cleanup_pt   handler;
    void                 *data;
    ngx_pool_cleanup_t   *next;
};


typedef struct ngx_pool_large_s  ngx_pool_large_t;

struct ngx_pool_large_s {
    ngx_pool_large_t     *next;
    void                 *alloc;
};

/*
last��	��һ��unsigned char ���͵�ָ�룬�������/��ǰ�ڴ�ط��䵽
		ĩλ��ַ������һ�η���Ӵ˴���ʼ��

end���ڴ�ؽ���λ�ã�

next��	�ڴ�������кܶ���ڴ棬��Щ�ڴ�����ͨ����ָ����
		�������ģ�nextָ����һ���ڴ档

failed���ڴ�ط���ʧ�ܴ�����

*/
typedef struct {
    u_char               *last; // ��ǰ���䵽�ڴ��Ǹ�λ�ã�last֮ǰ���ڴ����ѷ����ȥ
    u_char               *end;  // �ڴ�ҳβ����end-last����֪����ʣ������ڴ����ʹ��
    ngx_pool_t           *next; // ָ����һ���ڴ�ҳ
    ngx_uint_t            failed; // ���ڴ�ҳ����ʧ�ܴ���
} ngx_pool_data_t;


/*
d���ڴ�ص����ݿ飻

max���ڴ�����ݿ�����ֵ��

current��ָ��ǰ�ڴ�أ�

chain����ָ��ҽ�һ��ngx_chain_t�ṹ��

large������ڴ�������������ռ䳬��max�����ʹ�ã�

cleanup���ͷ��ڴ�ص�callback

log����־��Ϣ

*/
struct ngx_pool_s {
    ngx_pool_data_t       d;
	// ����ͷ����ͨԪ��ռ�õĶ����ֽ����ݣ�������ֶ�ֻ��
	// �ڴ������ͷӵ�У����ڹ����ڴ�����ݽṹ��������ͷ
	// �ڵ�ֻ��ngx_pool_data_t�ֶΣ��ο�ngx_palloc_block

	// �ڴ���������޶�������ֵ����ngx_palloc_large���з���
    size_t                max; 
	// ��ǰʹ�õ��ڴ�ҳ
    ngx_pool_t           *current;
	// ��ngx_buf���
    ngx_chain_t          *chain;
	//�������max���ڴ��ͳһ�ŵ�large������
    ngx_pool_large_t     *large;
	// ��Դ�ͷž��
    ngx_pool_cleanup_t   *cleanup;
    ngx_log_t            *log;
};


typedef struct {
    ngx_fd_t              fd;
    u_char               *name;
    ngx_log_t            *log;
} ngx_pool_cleanup_file_t;


void *ngx_alloc(size_t size, ngx_log_t *log);
void *ngx_calloc(size_t size, ngx_log_t *log);

ngx_pool_t *ngx_create_pool(size_t size, ngx_log_t *log);
void ngx_destroy_pool(ngx_pool_t *pool);
void ngx_reset_pool(ngx_pool_t *pool);

void *ngx_palloc(ngx_pool_t *pool, size_t size);
void *ngx_pnalloc(ngx_pool_t *pool, size_t size);
void *ngx_pcalloc(ngx_pool_t *pool, size_t size);
void *ngx_pmemalign(ngx_pool_t *pool, size_t size, size_t alignment);
ngx_int_t ngx_pfree(ngx_pool_t *pool, void *p);


ngx_pool_cleanup_t *ngx_pool_cleanup_add(ngx_pool_t *p, size_t size);
void ngx_pool_run_cleanup_file(ngx_pool_t *p, ngx_fd_t fd);
void ngx_pool_cleanup_file(void *data);
void ngx_pool_delete_file(void *data);


#endif /* _NGX_PALLOC_H_INCLUDED_ */