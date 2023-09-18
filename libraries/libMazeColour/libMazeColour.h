#ifndef LIBMAZECOLOUR_H
#define LIBMAZECOLOUR_H
#include <Arduino.h>

int determineColor(int printing_or_not, int s2_color, int s3_color, int out_color, int cal_min_r, int cal_max_r, int cal_min_g, int cal_max_g, int cal_min_b, int cal_max_b, int black_threshold, int white_threshold);

#endif 