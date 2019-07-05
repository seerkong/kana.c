#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include "../kon/kon.h"
// #include "./log.h"
#include <tbox/tbox.h>
// #include "./number_utils.h"

#define kon_make_fixnum2(n)    (  ((((kon_int_t)(n))*(kon_int_t)((kon_int_t)1<<KON_FIXNUM_BITS)) | KON_FIXNUM_TAG))
#define KON_MAKE_IMMEDIATE2(n)  (  ((n<<KON_EXTENDED_BITS) \
                                          + KON_EXTENDED_TAG))


#define TB_VECTOR_GROW_SIZE             (256)
#define __tb_debug__ 1

Kon kstate;

static void tb_vector_insert_cstr_test()
{
    // init
    tb_vector_ref_t vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_str(tb_true));
    tb_assert(vector == 0);
    

    if (vector)
    {
        kon_debug("in tb_vector_insert_cstr_test");

        tb_vector_insert_head(vector, "hi!");

        tb_vector_insert_tail(vector, "how");
        tb_vector_insert_tail(vector, "are");
        tb_vector_insert_tail(vector, "you");

        tb_for_all (tb_char_t const*, cstr, vector) {
            // trace
            // tb_trace_d("%s", cstr);
            kon_debug("%s", cstr);
        }

        tb_vector_remove_head(vector);

        tb_vector_remove_last(vector);

        tb_vector_exit(vector);
    }
}

static tb_void_t kon_test_vector_item_ptr_free(tb_element_ref_t element, tb_pointer_t buff)
{
    tb_assert_and_check_return(element && buff);

    tb_cpointer_t priv = element->priv;

    printf("vector item element  ptr addr %lx\n", (unsigned long)element);
    printf("vector item element->priv ptr addr %lx\n", (unsigned long)priv);
    printf("kon_test_vector_item_ptr_free buf arg addr %lx\n", (unsigned long)buff);
    
    // the data stored in the vector
    tb_pointer_t data = *((tb_pointer_t*)buff);
    
    printf("kon_test_vector_item_ptr_free *buf arg addr %lx\n", (unsigned long)data);

    // release
    // if (data) tb_free(data);

    // clear
    // *((tb_pointer_t*)buff) = tb_null;

}


static void tb_vector_insert_kon_test()
{
    // init
    tb_vector_ref_t vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_ptr(kon_test_vector_item_ptr_free, "private data"));
    tb_assert(vector == 0);


    if (vector)
    {
        kon_debug("in tb_vector_insert_cstr_test vector ptr %lx", (unsigned long)vector);
        Kon* test_kon_string = KON_MakeString(&kstate, "hi!");

        Kon* hi_ptr = KON_MakeString(&kstate, "hi!");
        kon_debug("hi kon str tag %lx", (unsigned long)hi_ptr);
        // 在头部插入元素
        tb_vector_insert_head(vector, hi_ptr);

        // 在尾部插入元素
        tb_vector_insert_tail(vector, KON_MakeString(&kstate, "how"));
        tb_vector_insert_tail(vector, KON_MakeString(&kstate, "are"));
        tb_vector_insert_tail(vector, KON_MakeString(&kstate, "you"));

        // 遍历打印所有元素
        tb_for_all (Kon*, item, vector)
        {
            kon_debug("kon tag %lx, %s", (unsigned long)item, KON_StringToCstr(item));
        }

        // 销毁vector容器
        tb_vector_exit(vector);
    }
}

static void tb_hashmap_str_test()
{

    // 初始化hash_map, 哈希桶大小8
    // 键：大小写敏感字符串
    // 值：long整型
    tb_hash_map_ref_t hash_map = tb_hash_map_init(8, tb_element_str(tb_true), tb_element_long());
    if (hash_map) {
        // 设置键值对："key" => 123
        tb_hash_map_insert(hash_map, "key", (tb_pointer_t)123);

        // 获取值
        tb_long_t value = (tb_long_t)tb_hash_map_get(hash_map, "key");
        printf("value for key in hash map %ld\n", value);
        // 退出hash_map
        tb_hash_map_exit(hash_map);
    }
    else {
        printf("hashmap create failed %ld\n", (long)hash_map);
    }
}

static tb_void_t kon_test_hash_item_ptr_free(tb_element_ref_t element, tb_pointer_t buff)
{
    // 断言检测
    tb_assert_and_check_return(element && buff);

    // 获取用户私有数据
    tb_cpointer_t priv = element->priv;
    printf("--------\n");
    printf("vector item element  ptr addr %lx\n", (unsigned long)element);
    printf("vector item element->priv ptr addr %lx\n", (unsigned long)priv);
    printf("kon_test_hash_item_ptr_free buf arg addr %lx\n", (unsigned long)buff);
    
    tb_pointer_t data = *((tb_pointer_t*)buff);
    // data是对应存到kon中的Kon*类型的指针地址
    printf("kon_test_hash_item_ptr_free *buf arg addr %lx\n", (unsigned long)data);

    /*
    
    
    // 释放它
    if (data) tb_free(data);

    // 清空成员数据
    *((tb_pointer_t*)buff) = tb_null;
    */
}

static void tb_hashmap_kon_test()
{
    printf("-----\n");
    // 初始化hash_map, 哈希桶大小8
    // 键：大小写敏感字符串
    // 值：long整型
    tb_hash_map_ref_t hash_map = tb_hash_map_init(8, tb_element_str(tb_true), tb_element_ptr(kon_test_hash_item_ptr_free, "private data"));
    if (hash_map) {
        // make string
        Kon* test_kon_string = KON_MakeString(&kstate, "asdfff");

        // make list
        Kon* list1 = kon_list1(&kstate, test_kon_string);
        
        printf("hashmap value list1 tag %lx\n", (unsigned long)list1);

        // 设置键值对："key" => list1
        tb_hash_map_insert(hash_map, "key", (tb_pointer_t)list1);



        // 获取值
        Kon* value = (Kon*)tb_hash_map_get(hash_map, "key");
        Kon* list1_car = kon_car(value);
        printf("value for key in hash map %s\n", KON_StringToCstr(KON_Stringify(&kstate, list1_car)));
        // 退出hash_map
        tb_hash_map_exit(hash_map);
    }
    else {
        printf("hashmap create failed %ld\n", (long)hash_map);
    }
}

int main(int argc, char const* argv[])
{
    
    if (!KON_Init(&kstate)) {
        fprintf(stderr, "Could not initialize!\n");
        
        return 1;
    }

    Kon* test_kon_float = KON_MakeFlonum(&kstate, 2.4);
    printf("test_kon_float addr %lx, value %lf\n", (unsigned long)test_kon_float, kon_flonum_value(test_kon_float));

    // 测试数字类型的kon转换成字符串类型的Kon
    Kon* float_to_str = KON_Stringify(&kstate, test_kon_float);
    printf("test_kon_float convert to kon str , value %s\n", KON_StringToCstr(float_to_str));

    Kon* test_kon_fixnum = kon_make_fixnum(72);
    printf("test_kon_fixnum 72 addr value %ld, value %lld\n", (long)test_kon_fixnum, kon_unbox_fixnum(test_kon_fixnum));

    // 测试数字类型的kon转换成字符串类型的Kon
    Kon* fixnum_to_str = KON_Stringify(&kstate, test_kon_fixnum);
    printf("test_kon_fixnum convert to kon str , value %s\n", KON_StringToCstr(fixnum_to_str));


    // make char
    Kon* test_kon_char = kon_make_character('a');
    printf("test_kon_char 'a' addr value %ld, value %d\n", (long)test_kon_fixnum, kon_unbox_character(test_kon_char));
    Kon* char_to_str = KON_Stringify(&kstate, test_kon_char);
    printf("test_kon_char convert to kon str , value %s\n", KON_StringToCstr(char_to_str));



    // make string
    Kon* test_kon_string = KON_MakeString(&kstate, "asdfff");
    printf("test_kon_string 'asdfff' is string? %d, value %s\n", (int)kon_is_string(test_kon_string), KON_StringToCstr(test_kon_string));

    // make list
    Kon* list1 = kon_list1(&kstate, test_kon_string);
    Kon* list1_car = kon_car(list1);
    printf("list1 car  value %s\n", KON_StringToCstr(KON_Stringify(&kstate, list1_car)));

    // vector使用
    tb_vector_insert_kon_test();

    tb_hashmap_kon_test();

    // char s1[40];    //要转换成的字符数组
    // itoa(KON_MAKE_IMMEDIATE2(44),s1,2);//itoa转为二进制
    // printf("KON_MAKE_IMMEDIATE(44)=%s\n", s1);

    // char s2[40];//要转换成的字符数组
    // double_to_str((double)5.242224, 2, s2);//itoa转为二进制
    // printf("kon_make_fixnum(3)=%s\n", s2);

    KON_Finish(&kstate);
    return 0;
}


