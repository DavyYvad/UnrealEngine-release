/*
 * Copyright (c) 2019 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef PAS_RESERVED_MEMORY_PROVIDER_H
#define PAS_RESERVED_MEMORY_PROVIDER_H

#include "pas_heap_page_provider.h"
#include "pas_simple_large_free_heap.h"

PAS_BEGIN_EXTERN_C;

struct pas_reserved_memory_provider;
typedef struct pas_reserved_memory_provider pas_reserved_memory_provider;

struct pas_reserved_memory_provider {
    pas_simple_large_free_heap free_heap;
};

PAS_API pas_reserved_memory_provider* pas_reserved_memory_provider_create(uintptr_t begin, uintptr_t end);

PAS_API void pas_reserved_memory_provider_construct(
    pas_reserved_memory_provider* provider,
    uintptr_t begin,
    uintptr_t end);

PAS_API pas_allocation_result pas_reserved_memory_provider_try_allocate(
    size_t size,
    pas_alignment alignment,
    const char* name,
    pas_heap* heap,
    pas_physical_memory_transaction* transaction,
	pas_primordial_page_state desired_state,
    void* arg);

PAS_END_EXTERN_C;

#endif /* PAS_RESERVED_MEMORY_PROVIDER_H */

