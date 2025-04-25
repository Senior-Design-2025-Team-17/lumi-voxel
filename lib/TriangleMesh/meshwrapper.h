#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool meshTransform(uint8_t* data_buffer, uint8_t Nb_bytes);

bool meshTris(uint8_t* data_buffer, uint8_t Nb_bytes);

bool meshVerts(uint8_t* data_buffer, uint8_t Nb_bytes);

bool colorMode(uint8_t* data, uint8_t count);

#ifdef __cplusplus
}
#endif