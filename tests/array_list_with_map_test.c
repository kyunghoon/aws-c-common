/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list_with_map.h>
#include <aws/common/string.h>

#include <aws/testing/aws_test_harness.h>

static int s_array_list_with_map_sanitize_fn(struct aws_allocator *allocator, void *ctx) {
    (void)ctx;

    struct aws_array_list_with_map list_with_map;
    ASSERT_SUCCESS(
        aws_array_list_with_map_init(&list_with_map, allocator, aws_hash_ptr, aws_ptr_eq, sizeof(void *), 0));
    aws_array_list_with_map_clean_up(&list_with_map);
    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(array_list_with_map_sanitize_test, s_array_list_with_map_sanitize_fn)

static int s_array_list_with_map_insert_fn(struct aws_allocator *allocator, void *ctx) {
    (void)ctx;
    AWS_STATIC_STRING_FROM_LITERAL(foo, "foo");
    AWS_STATIC_STRING_FROM_LITERAL(bar, "bar");
    AWS_STATIC_STRING_FROM_LITERAL(foobar, "foobar");

    struct aws_array_list_with_map list_with_map;
    /* With only 1 initial element. */
    ASSERT_SUCCESS(aws_array_list_with_map_init(
        &list_with_map, allocator, aws_hash_string, aws_hash_callback_string_eq, sizeof(struct aws_string), 1));
    ASSERT_SUCCESS(aws_array_list_with_map_insert(&list_with_map, foobar));
    ASSERT_SUCCESS(aws_array_list_with_map_insert(&list_with_map, bar));
    ASSERT_SUCCESS(aws_array_list_with_map_insert(&list_with_map, foo));

    /* You cannot have duplicates */
    ASSERT_FAILS(aws_array_list_with_map_insert(&list_with_map, foobar));

    /* Check the size */
    ASSERT_UINT_EQUALS(aws_array_list_with_map_length(&list_with_map), 3);

    aws_array_list_with_map_clean_up(&list_with_map);
    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(array_list_with_map_insert_test, s_array_list_with_map_insert_fn)

static int s_array_list_with_map_get_random_fn(struct aws_allocator *allocator, void *ctx) {
    (void)ctx;
    AWS_STATIC_STRING_FROM_LITERAL(foo, "foo");

    struct aws_array_list_with_map list_with_map;
    /* With only 1 initial element. */
    ASSERT_SUCCESS(aws_array_list_with_map_init(
        &list_with_map, allocator, aws_hash_string, aws_hash_callback_string_eq, sizeof(struct aws_string), 1));
    ASSERT_SUCCESS(aws_array_list_with_map_insert(&list_with_map, foo));

    /* Check the size */
    ASSERT_UINT_EQUALS(aws_array_list_with_map_length(&list_with_map), 1);
    struct aws_string left_element;
    aws_array_list_with_map_get_random(&list_with_map, &left_element);
    ASSERT_TRUE(aws_string_eq(&left_element, foo));

    aws_array_list_with_map_clean_up(&list_with_map);
    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(array_list_with_map_get_random_test, s_array_list_with_map_get_random_fn)

static int s_array_list_with_map_exist_fn(struct aws_allocator *allocator, void *ctx) {
    (void)ctx;
    AWS_STATIC_STRING_FROM_LITERAL(foo, "foo");
    AWS_STATIC_STRING_FROM_LITERAL(bar, "bar");

    struct aws_array_list_with_map list_with_map;
    ASSERT_SUCCESS(aws_array_list_with_map_init(
        &list_with_map, allocator, aws_hash_string, aws_hash_callback_string_eq, sizeof(struct aws_string), 1));
    ASSERT_SUCCESS(aws_array_list_with_map_insert(&list_with_map, foo));

    ASSERT_TRUE(aws_array_list_with_map_exist(&list_with_map, foo));
    ASSERT_FALSE(aws_array_list_with_map_exist(&list_with_map, bar));

    aws_array_list_with_map_clean_up(&list_with_map);
    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(array_list_with_map_exist_test, s_array_list_with_map_exist_fn)

static int s_array_list_with_map_remove_fn(struct aws_allocator *allocator, void *ctx) {
    (void)ctx;
    AWS_STATIC_STRING_FROM_LITERAL(foo, "foo");
    AWS_STATIC_STRING_FROM_LITERAL(bar, "bar");
    AWS_STATIC_STRING_FROM_LITERAL(foobar, "foobar");

    struct aws_array_list_with_map list_with_map;
    /* With only 1 initial element. */
    ASSERT_SUCCESS(aws_array_list_with_map_init(
        &list_with_map, allocator, aws_hash_string, aws_hash_callback_string_eq, sizeof(struct aws_string), 1));
    ASSERT_SUCCESS(aws_array_list_with_map_insert(&list_with_map, bar));
    ASSERT_SUCCESS(aws_array_list_with_map_insert(&list_with_map, foobar));
    ASSERT_SUCCESS(aws_array_list_with_map_insert(&list_with_map, foo));

    ASSERT_SUCCESS(aws_array_list_with_map_remove(&list_with_map, foo));
    /* Check the size */
    ASSERT_UINT_EQUALS(aws_array_list_with_map_length(&list_with_map), 2);

    /* Should be fine to remove something not existing anymore? */
    ASSERT_SUCCESS(aws_array_list_with_map_remove(&list_with_map, foo));

    /* Remove all beside foobar, so, if we get one random, it will be foobar */
    ASSERT_SUCCESS(aws_array_list_with_map_remove(&list_with_map, bar));
    ASSERT_UINT_EQUALS(aws_array_list_with_map_length(&list_with_map), 1);
    struct aws_string left_element;
    aws_array_list_with_map_get_random(&list_with_map, &left_element);
    ASSERT_TRUE(aws_string_eq(&left_element, foobar));

    /* Remove last thing and make sure everything should still work */
    ASSERT_SUCCESS(aws_array_list_with_map_remove(&list_with_map, foobar));
    ASSERT_UINT_EQUALS(aws_array_list_with_map_length(&list_with_map), 0);
    ASSERT_SUCCESS(aws_array_list_with_map_insert(&list_with_map, foo));
    ASSERT_UINT_EQUALS(aws_array_list_with_map_length(&list_with_map), 1);
    aws_array_list_with_map_get_random(&list_with_map, &left_element);
    ASSERT_TRUE(aws_string_eq(&left_element, foo));

    aws_array_list_with_map_clean_up(&list_with_map);
    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(array_list_with_map_remove_test, s_array_list_with_map_remove_fn)
