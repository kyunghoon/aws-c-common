/*
 * Copyright 2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_byte_buf_init_copy_harness() {
    /* data structure */
    struct aws_byte_buf *dest;

    /* parameters */
    struct aws_allocator *allocator;
    struct aws_byte_buf src;

    /* assumptions */
    __CPROVER_assume(is_bounded_byte_buf(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));
    ASSUME_VALID_MEMORY(dest);
    ASSUME_CAN_FAIL_ALLOCATOR(allocator);

    /* save current state of the data structure */
    struct aws_byte_buf old = src;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array(src.buffer, src.len, &old_byte);

    /* operation under verification */
    if (!aws_byte_buf_init_copy(dest, allocator, &src)) {
        /* assertions */
        assert(aws_byte_buf_is_valid(dest));
        assert(is_byte_buf_expected_alloc(dest));
        assert(dest->len == src.len);
        assert(dest->capacity == src.capacity);
        assert_bytes_match(dest->buffer, src.buffer, dest->len);
        assert(aws_byte_buf_is_valid(&src));
        if (src.len > 0) {
            assert_byte_from_buffer_matches(src.buffer, &old_byte);
            assert_byte_from_buffer_matches(dest->buffer, &old_byte);
        }
    }
}