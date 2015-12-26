#include "aos_log.h"
#include "aos_util.h"
#include "aos_string.h"
#include "aos_status.h"
#include "oss_auth.h"
#include "oss_util.h"
#include "oss_api.h"
#include "oss_config.h"
#include "oss_sample_util.h"

void put_object_from_buffer()
{
    aos_pool_t *p;
    aos_string_t bucket;
    aos_string_t object;
    int is_oss_domain = 1;
    aos_table_t *headers;
    aos_table_t *resp_headers;
    oss_request_options_t *options;
    aos_list_t buffer;
    aos_buf_t *content;
    char *str = "test oss c sdk";
    aos_status_t *s;

    aos_pool_create(&p, NULL);
    options = oss_request_options_create(p);
    init_sample_request_options(options, is_oss_domain);
    headers = aos_table_make(p, 1);
    apr_table_set(headers, "x-oss-meta-author", "oss");
    aos_str_set(&bucket, BUCKET_NAME);
    aos_str_set(&object, OBJECT_NAME);

    aos_list_init(&buffer);
    content = aos_buf_pack(options->pool, str, strlen(str));
    aos_list_add_tail(&content->node, &buffer);

    s = oss_put_object_from_buffer(options, &bucket, &object, 
				   &buffer, headers, &resp_headers);

    if (NULL != s && 2 == s->code / 100) {
        printf("put object from buffer succeeded\n");
    }
    else {
	printf("put object from buffer failed\n");      
    }    

    aos_pool_destroy(p);
}

void put_object_from_file()
{
    aos_pool_t *p;
    aos_string_t bucket;
    aos_string_t object;
    int is_oss_domain = 1;
    aos_table_t *headers;
    aos_table_t *resp_headers;
    oss_request_options_t *options;
    char *filename = __FILE__;
    aos_status_t *s;
    aos_string_t file;

    aos_pool_create(&p, NULL);
    options = oss_request_options_create(p);
    init_sample_request_options(options, is_oss_domain);
    headers = aos_table_make(options->pool, 0);
    aos_str_set(&bucket, BUCKET_NAME);
    aos_str_set(&object, OBJECT_NAME);
    aos_str_set(&file, filename);

    s = oss_put_object_from_file(options, &bucket, &object, &file, 
                                 headers, &resp_headers);

    if (NULL != s && 2 == s->code / 100) {
        printf("put object from file succeeded\n");
    }
    else {
	printf("put object from file failed\n");
    }

    aos_pool_destroy(p);
}

void put_object_by_signed_url()
{
    aos_pool_t *p;
    aos_string_t bucket;
    aos_string_t object;
    aos_string_t url;
    int is_oss_domain = 1;
    aos_http_request_t *request = NULL;
    aos_table_t *headers;
    aos_table_t *resp_headers;
    oss_request_options_t *options;
    char *filename = __FILE__;
    aos_status_t *s;
    aos_string_t file;
    char *signed_url = NULL;
    int64_t expires_time;

    aos_pool_create(&p, NULL);

    options = oss_request_options_create(p);
    init_sample_request_options(options, is_oss_domain);

    // create request
    request = aos_http_request_create(p);
    request->method = HTTP_PUT;

    // create headers
    headers = aos_table_make(options->pool, 0);

    // set value
    aos_str_set(&bucket, BUCKET_NAME);
    aos_str_set(&object, OBJECT_NAME);
    aos_str_set(&file, filename);

    // expires time
    expires_time = apr_time_now() / 1000000 + 120;    

    // generate signed url for put 
    signed_url = oss_gen_signed_url(options, &bucket, &object, 
                                    expires_time, request);
    aos_str_set(&url, signed_url);
    
    printf("signed put url : %s\n", signed_url);

    // put object by signed url
    s = oss_put_object_from_file_by_url(options, &url, &file, 
                                        headers, &resp_headers);

    if (NULL != s && 2 == s->code / 100) {
        printf("put object by signed url succeeded\n");
    }
    else {
	printf("put object by signed url failed\n");
    }

    aos_pool_destroy(p);
}



int main(int argc, char *argv[])
{
    //aos_http_io_initialize first 
    if (aos_http_io_initialize("oss_sample", 0) != AOSE_OK) {
        exit(1);
    }

    put_object_from_buffer();
    put_object_from_file();
    put_object_by_signed_url();
    
    //aos_http_io_deinitialize last
    aos_http_io_deinitialize();

    return 0;
}