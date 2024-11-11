#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_BINS 300
#define MAX_BALLS 90000

int bins[MAX_BINS];
int last_batch_bins[MAX_BINS];
int m = 0, n = 0, d = 0, b = 0, k = 0;
int T = 10;
double beta;
char scheme[40];

void reset_bins() {
  for (int i = 0; i < m; i++) {
    bins[i] = 0;
    last_batch_bins[i] = 0;
  }
}

int find_min_bin(int d_choices[], int d) {
  if (b == 0) {
    int min_load = bins[d_choices[0]];
    int min_bin = d_choices[0];
    for (int i = 1; i < d; i++) {
      if (bins[d_choices[i]] < min_load) {
        min_load = bins[d_choices[i]];
        min_bin = d_choices[i];
      }
    }

    return min_bin;
  } else {
    int min_load = last_batch_bins[d_choices[0]];
    int min_bin = d_choices[0];
    for (int i = 1; i < d; i++) {
      if (last_batch_bins[d_choices[i]] < min_load) {
        min_load = last_batch_bins[d_choices[i]];
        min_bin = d_choices[i];
      }
    }

    return min_bin;
  }
}

int compare_ints(const void *a, const void *b) {
  return (*(int *)a - *(int *)b);
}

int query_is_above_median(int bin) {
  int *sorted_bins = malloc(m * sizeof(int));
  if (sorted_bins == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }
  memcpy(sorted_bins, bins, m * sizeof(int));
  qsort(sorted_bins, m, sizeof(int), compare_ints);
  int median_value = sorted_bins[m / 2];
  free(sorted_bins);
  return bins[bin] >= median_value;
}

int query_is_in_top_25_percent(int bin) {
  int *sorted_bins = malloc(m * sizeof(int));
  if (sorted_bins == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }
  memcpy(sorted_bins, bins, m * sizeof(int));
  qsort(sorted_bins, m, sizeof(int), compare_ints);
  int threshold_value = sorted_bins[(3 * m) / 4];
  free(sorted_bins);
  return bins[bin] >= threshold_value;
}

void allocate_one_choice() {
  for (int i = 0; i < n; i++) {
    if (b != 0 && i % b == 0) {
      memcpy(bins, last_batch_bins, MAX_BINS);
    }
    int chosen_bin = rand() % m;
    bins[chosen_bin]++;
  }
}


void allocate_two_choice() {
  for (int i = 0; i < n; i++) {
    int chosen_bin = 0;

    if (b != 0 && i % b == 0) {
      memcpy(bins, last_batch_bins, MAX_BINS);
    }
    int d_choices[2] = {rand() % m, rand() % m};

    if (k == 0) {
      chosen_bin = find_min_bin(d_choices, 2);
    } else {
      int is_above[2];
      for (int l = 0; l < 2; l++) {
        is_above[l] = query_is_above_median(d_choices[l]);
      }

      if (is_above[0] && !is_above[1]) {
        chosen_bin = d_choices[0];
      } else if (!is_above[0] && is_above[1]) {
        chosen_bin = d_choices[1];
      } else if (!is_above[0] && !is_above[1]) {
        // Both are below the median
        if (k > 1) {
          for (int l = 0; l < 2; l++) {
            is_above[l] = query_is_in_top_25_percent(d_choices[l]);
          }

          if (is_above[0] && !is_above[1]) {
            chosen_bin = d_choices[0];
          } else if (!is_above[0] && is_above[1]) {
            chosen_bin = d_choices[1];
          } else {
            // Both are not in the top 25% or both are in the top 25%
            chosen_bin = d_choices[rand() % 2];
          }
        } else {
          // k == 1, select randomly
          chosen_bin = d_choices[rand() % 2];
        }
      } else if (is_above[0] && is_above[1]) {
        // Both are above the median
        chosen_bin = d_choices[rand() % 2];
      }
    }
    bins[chosen_bin]++;
  }
}


void allocate_three_choice() {
  for (int i = 0; i < n; i++) {
    int chosen_bin = 0;
    if (b != 0 && i % b == 0) {
      memcpy(bins, last_batch_bins, MAX_BINS);
    }
    int d_choices[3] = {rand() % m, rand() % m, rand() % m};
    if (k == 0) {
      chosen_bin = find_min_bin(d_choices, 3);
    } else if (k == 1) {
      int is_above[3];
      for (int l = 0; l < 3; l++) {
        is_above[l] = query_is_above_median(d_choices[l]);
      }

      if (is_above[0] && !is_above[1] && !is_above[2]) {
        chosen_bin = d_choices[0];
      } else if (!is_above[0] && is_above[1] && !is_above[2]) {
        chosen_bin = d_choices[1];
      } else if (!is_above[0] && !is_above[1] && is_above[2]) {
        chosen_bin = d_choices[2];
      } else {
        if (k > 1) {
          for (int l = 0; l < 3; l++) {
            is_above[l] = query_is_in_top_25_percent(d_choices[l]);
          }

          if (is_above[0] && !is_above[1] && !is_above[2]) {
            chosen_bin = d_choices[0];
          } else if (!is_above[0] && is_above[1] && !is_above[2]) {
            chosen_bin = d_choices[1];
          } else if (!is_above[0] && !is_above[1] && is_above[2]) {
            chosen_bin = d_choices[2];
          } else if (!is_above[0] && !is_above[1] && !is_above[2]) {
            chosen_bin = d_choices[rand() % 3];
          } else if (is_above[0] && is_above[1] && !is_above[2]) {
            chosen_bin = d_choices[rand() % 2];  // Picks between indices 0 and 1
          } else if (!is_above[0] && is_above[1] && is_above[2]) {
            chosen_bin = d_choices[(rand() % 2) + 1];  // Picks between indices 1 and 2
          } else if (is_above[0] && !is_above[1] && is_above[2]) {
            chosen_bin = d_choices[(rand() % 2 == 0) ? 0 : 2];  // Picks between indices 0 and 2
          } else if (is_above[0] && is_above[1] && is_above[2]) {
            chosen_bin = d_choices[rand() % 3];
          }

        } else {
          if (!is_above[0] && !is_above[1] && !is_above[2]) {
            chosen_bin = d_choices[rand() % 3];
          } else if (is_above[0] && is_above[1] && !is_above[2]) {
            chosen_bin = d_choices[rand() % 2];  // Picks between indices 0 and 1
          } else if (!is_above[0] && is_above[1] && is_above[2]) {
            chosen_bin = d_choices[(rand() % 2) + 1];  // Picks between indices 1 and 2
          } else if (is_above[0] && !is_above[1] && is_above[2]) {
            chosen_bin = d_choices[(rand() % 2 == 0) ? 0 : 2];  // Picks between indices 0 and 2
          } else if (is_above[0] && is_above[1] && is_above[2]) {
            chosen_bin = d_choices[rand() % 3];
          }
        }
      }
    }
    bins[chosen_bin]++;
  }
}

void allocate_four_choice() {
  for (int i = 0; i < n; i++) {
    if (b != 0 && i % b == 0) {
      memcpy(bins, last_batch_bins, MAX_BINS);
    }
    int d_choices[4] = {rand() % m, rand() % m, rand() % m, rand() % m};
    int chosen_bin = find_min_bin(d_choices, 4);
    bins[chosen_bin]++;
  }
}

void allocate_ten_choice() {
  for (int i = 0; i < n; i++) {
    if (b != 0 && i % b == 0) {
      memcpy(bins, last_batch_bins, MAX_BINS);
    }
    int d_choices[10] = {rand() % m, rand() % m, rand() % m, rand() % m,
                         rand() % m, rand() % m, rand() % m, rand() % m,
                         rand() % m, rand() % m};
    int chosen_bin = find_min_bin(d_choices, 10);
    bins[chosen_bin]++;
  }
}

void allocate_one_plus_beta_choice() {
  for (int i = 0; i < n; i++) {
    if (b != 0 && i % b == 0) {
      memcpy(bins, last_batch_bins, MAX_BINS);
    }
    if ((double)rand() / RAND_MAX < beta) {
      int chosen_bin = rand() % m;
      bins[chosen_bin]++;
    } else {
      int d_choices[2] = {rand() % m, rand() % m};
      int chosen_bin = (bins[d_choices[0]] < bins[d_choices[1]]) ? d_choices[0]
                                                                 : d_choices[1];
      bins[chosen_bin]++;
    }
  }
}

void run_allocation() {
  if (strcmp(scheme, "one-choice") == 0) {
    allocate_one_choice();
  } else if (strcmp(scheme, "two-choice") == 0) {
    allocate_two_choice();
  } else if (strcmp(scheme, "three-choice") == 0) {
    allocate_three_choice();
  } else if (strcmp(scheme, "four-choice") == 0) {
    allocate_four_choice();
  } else if (strcmp(scheme, "ten-choice") == 0) {
    allocate_ten_choice();
  } else if (strcmp(scheme, "one-plus-beta") == 0) {
    allocate_one_plus_beta_choice();
  }
}

void plot_average_gap(int num_points, double gaps[], int bins[]) {
  FILE *gnuplotPipe = popen("gnuplot -persistent", "w");
  if (gnuplotPipe) {
    fprintf(gnuplotPipe, "set title 'Gap / Number of Bins'\n");
    fprintf(gnuplotPipe, "set xlabel 'Number of Bins (m)'\n");
    fprintf(gnuplotPipe, "set ylabel ' Gap'\n");
    fprintf(gnuplotPipe, "plot '-' with linespoints title 'Gap'\n");
    for (int i = 0; i < num_points; i++) {
      fprintf(gnuplotPipe, "%d %f\n", bins[i], gaps[i]);
    }
    fprintf(gnuplotPipe, "e\n");
    fflush(gnuplotPipe);
    pclose(gnuplotPipe);
  } else {
    printf("Error: Unable to open Gnuplot.");
  }
}

int main(int argc, char *argv[]) {
  if (argc < 5 || argc > 8) {
    printf("Usage: %s <m> <n> <scheme> <d/beta> [T] [b] [k] \n", argv[0]);
    return 1;
  }

  m = atoi(argv[1]);
  n = atoi(argv[2]);
  strcpy(scheme, argv[3]);

  if (strcmp(scheme, "one-choice") == 0 || strcmp(scheme, "two-choice") == 0 ||
      strcmp(scheme, "three-choice") == 0 ||
      strcmp(scheme, "four-choice") == 0 || strcmp(scheme, "ten-choice") == 0) {
    d = atoi(argv[4]);
  } else if (strcmp(scheme, "one-plus-beta") == 0) {
    beta = atof(argv[4]);
  } else {
    printf("Unknown allocation scheme\n");
    return 1;
  }

  if (argc > 5) {
    T = atoi(argv[5]);
  }

  if (argc > 6) {
    b = atoi(argv[6]);
  }

  if (argc > 7) {
    k = atoi(argv[7]);
  }

  strcpy(scheme, argv[3]);

  srand(time(NULL));
  double avg_gap[T];
  double variance_gap;
  double gaps[MAX_BINS];
  int bins_array[MAX_BINS];
  int index = 0, t, j;
  double sum = 0;

  for (j = 1; j <= m; j += 25) {
    sum = 0;
    for (t = 0; t < T; t++) {
      reset_bins();
      run_allocation();
      int max_load = bins[0];
      int min_load = bins[0];
      for (int i = 0; i < j; i++) {
        if (bins[i] > max_load) {
          max_load = bins[i];
        }
        if (bins[i] < min_load) {
          min_load = bins[i];
        }
      }
      double gap = max_load - ((double)n / m);
      avg_gap[t] = gap;
    }

    for (t = 0; t < T; t++) {
      sum += avg_gap[t];
    }
    gaps[index] = sum / T;
    bins_array[index] = j;
    index++;
  }

  plot_average_gap(index, gaps, bins_array);

  return 0;
}
