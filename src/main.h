//
// Created by Tomas on 02/05/2017.
//

#ifndef MMAI_MAIN_H
#define MMAI_MAIN_H
#include <stdbool.h>

void file_selected(char *fname);
void start_playback(void);
void pause_playback(bool is_paused);
void outputfile_selected(char *fname);

#endif //MMAI_MAIN_H
