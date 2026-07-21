/*
 * PCG Random Number Generation for C.
 *
 * Copyright 2014 Melissa O'Neill <oneill@pcg-random.org>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For additional information about the PCG random number generation scheme,
 * including its license and other licensing options, visit
 *
 *       http://www.pcg-random.org
 */

// rand.h
//
// provides simple random number generation
//
// references: pcg-random

#ifndef _H_RAND
#define _H_RAND

#include "bace/base.h"

typedef struct {
  u64 state;
  u64 inc;
} PCG32RandomState;

// seed the `rng`.
// specified in two parts, state initializer and a sequence selection
// constant (a.k.a. stream id)
void pcg32_srandom_r(PCG32RandomState *rng, u64 initstate, u64 initseq);

// seed the global rng.
// specified in two parts, state initializer and a sequence selection
// constant (a.k.a. stream id)
void pcg32_srandom(u64 seed, u64 seq);

// generate a uniformly distributed 32-bit random number
u32 pcg32_random_r(PCG32RandomState *rng);

// generate a uniformly distributed 32-bit random number using the global rng.
u32 pcg32_random(void);

// generate a uniformly distributed number in [0, bound)
u32 pcg32_boundedrand_r(PCG32RandomState *rng, u32 bound);

// generate a uniformly distributed number in [0, bound)
// using the global `rng`
u32 pcg32_boundedrand(u32 bound);

// generate a random f32 in [0, 1]
f32 pcg32_randomf_r(PCG32RandomState *rng);

// generate a random f32 in [0, 1] using the global `rng`
f32 pcg32_randomf(void);

#endif // _H_RAND
