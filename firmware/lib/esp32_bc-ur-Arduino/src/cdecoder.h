#ifndef BC_UR_CDECODER_H
#define BC_UR_CDECODER_H

#include <stddef.h>

#define URDECODER_SIZE 288

int urcreate_decoder(void** const decoder);
void urcreate_placement_decoder(void* const decoder, size_t decoder_len);
void urfree_decoder(void* const decoder);
void urfree_placement_decoder(void* const decoder);

bool urreceive_part_decoder(void* const decoder, const char* string);
size_t urprocessed_parts_count_decoder(void* const decoder);
size_t urreceived_parts_count_decoder(void* const decoder);
size_t urexpected_part_count_decoder(void* const decoder);
double urestimated_percent_complete_decoder(void* const decoder);
bool uris_success_decoder(void* const decoder);
bool uris_failure_decoder(void* const decoder);
bool uris_complete_decoder(void* const decoder);
void urresult_ur_decoder(void* const decoder, uint8_t** result, size_t* result_len, const char** type);

#endif // BC_UR_CDECODER_H
