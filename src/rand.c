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

#include "bace/rand.h"

// state for global RNGs
PCG32RandomState g_pcg32_state = {0x853c49e6748fea9bULL, 0xda3e39cb94b95bdbULL};

void pcg32_srandom_r(PCG32RandomState *rng, u64 initstate, u64 initseq) {
  rng->state = 0U;
  rng->inc = (initseq << 1u) | 1u;
  pcg32_random_r(rng);
  rng->state += initstate;
  pcg32_random_r(rng);
}

void pcg32_srandom(u64 seed, u64 seq) {
  pcg32_srandom_r(&g_pcg32_state, seed, seq);
}

u32 pcg32_random_r(PCG32RandomState *rng) {
  u64 oldstate = rng->state;
  rng->state = oldstate * 6364136223846793005ULL + rng->inc;
  u32 xorshifted = (u32)(((oldstate >> 18u) ^ oldstate) >> 27u);
  u32 rot = oldstate >> 59u;
  return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

u32 pcg32_random(void) {
  return pcg32_random_r(&g_pcg32_state);
}

u32 pcg32_boundedrand_r(PCG32RandomState *rng, u32 bound) {
  u32 threshold = -bound % bound;
  for (;;) {
    u32 r = pcg32_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

u32 pcg32_boundedrand(u32 bound) {
  return pcg32_boundedrand_r(&g_pcg32_state, bound);
}

f32 pcg32_randomf(void) {
  return (f32)pcg32_random() / (f32)UINT32_MAX;
}

f32 pcg32_randomf_r(PCG32RandomState *rng) {
  return (f32)pcg32_random_r(rng) / (f32)UINT32_MAX;
}
