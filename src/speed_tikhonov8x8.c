#include "exact.h"
#include "filters.h"
#include "grfilter.h"
#include "matrices.h"

#define LEN 64

int main(int argc, char *argv[]) {

  int n_inputs;
  int firdeg = 10, msdeg = 3, msm = 8;
  double buffer_in[BATCH_SIZE][LEN];
  double buffer_out_exact[BATCH_SIZE][LEN];
  double buffer_out_mat[BATCH_SIZE][LEN];
  double buffer_out_fir[firdeg][BATCH_SIZE][LEN];
  double buffer_out_ms[msdeg * msm][BATCH_SIZE][LEN];

  double diff;
  double acc_error_mat = 0, acc_error_fir[10] = {0}, acc_error_ms[3 * 8] = {0};

  // read inputs
  FILE *fp_in = fopen(argv[1], "r");
  FILE *fp_out = fopen(argv[2], "w+");
  if (fp_in != NULL)
    fscanf(fp_in, "%d", &n_inputs);

  int n_batches = ceil((double)n_inputs / (double)BATCH_SIZE);
  int cur_batch_size = 0;
  clock_t t_temp = 0, t_exact = 0, t_mat = 0, t_fir[10] = {0},
          t_ms[3 * 8] = {0};

  for (int b = 0; b < n_batches; b++) {
    cur_batch_size = b < n_batches - 1 ? BATCH_SIZE : n_inputs - b * BATCH_SIZE;
    for (int i = 0; i < cur_batch_size; i++) {
      for (int j = 0; j < LEN; j++)
        fscanf(fp_in, "%lf", &buffer_in[i][j]);
    }

    // Exact filter
    t_temp = clock();
    for (int i = 0; i < cur_batch_size; i++)
      exact_filter_8x8(buffer_in[i], buffer_out_exact[i], h8x8_tik);
    t_exact += clock() - t_temp;

    // matrix multiplication
    t_temp = clock();
    for (int i = 0; i < cur_batch_size; i++)
      mat_times_vec(buffer_in[i], buffer_out_mat[i], tik8x8_t16, LEN);
    t_mat += clock() - t_temp;
    for (int i = 0; i < cur_batch_size; i++) {
      for (int j = 0; j < LEN; j++) {
        diff = buffer_out_exact[i][j] - buffer_out_mat[i][j];
        acc_error_mat += diff * diff;
      }
    }

    // FIR graph filter
    const double *fir_coeffs_ptr[10] = {
        tik8x8_fir1_coeffs, tik8x8_fir2_coeffs, tik8x8_fir3_coeffs,
        tik8x8_fir4_coeffs, tik8x8_fir5_coeffs, tik8x8_fir6_coeffs,
        tik8x8_fir7_coeffs, tik8x8_fir8_coeffs, tik8x8_fir9_coeffs,
        tik8x8_fir10_coeffs};
    for (int ord = 1; ord <= firdeg; ord++) {
      t_temp = clock();
      for (int i = 0; i < cur_batch_size; i++)
        fir_graph_filter(buffer_in[i], buffer_out_fir[ord - 1][i], LEN, ord,
                         fir_coeffs_ptr[ord - 1], NE_LDD8X8, MEV_LDD8X8,
                         Ldd8x8_a, Ldd8x8_w);
      t_fir[ord - 1] += clock() - t_temp;
      for (int i = 0; i < cur_batch_size; i++) {
        for (int j = 0; j < LEN; j++) {
          diff = buffer_out_exact[i][j] - buffer_out_fir[ord - 1][i][j];
          acc_error_fir[ord - 1] += diff * diff;
        }
      }
    }

    // multishift graph filter
    const double *ms_coeffs_ptr[3 * 8] = {
        tik8x8_msl1m1_coeffs, tik8x8_msl1m2_coeffs, tik8x8_msl1m3_coeffs,
        tik8x8_msl1m4_coeffs, tik8x8_msl1m5_coeffs, tik8x8_msl1m6_coeffs,
        tik8x8_msl1m7_coeffs, tik8x8_msl1m8_coeffs, tik8x8_msl2m1_coeffs,
        tik8x8_msl2m2_coeffs, tik8x8_msl2m3_coeffs, tik8x8_msl2m4_coeffs,
        tik8x8_msl2m5_coeffs, tik8x8_msl2m6_coeffs, tik8x8_msl2m7_coeffs,
        tik8x8_msl2m8_coeffs, tik8x8_msl3m1_coeffs, tik8x8_msl3m2_coeffs,
        tik8x8_msl3m3_coeffs, tik8x8_msl3m4_coeffs, tik8x8_msl3m5_coeffs,
        tik8x8_msl3m6_coeffs, tik8x8_msl3m7_coeffs, tik8x8_msl3m8_coeffs};
    const int *ms_powers_ptr[3 * 8] = {
        tik8x8_msl1m1_powers, tik8x8_msl1m2_powers, tik8x8_msl1m3_powers,
        tik8x8_msl1m4_powers, tik8x8_msl1m5_powers, tik8x8_msl1m6_powers,
        tik8x8_msl1m7_powers, tik8x8_msl1m8_powers, tik8x8_msl2m1_powers,
        tik8x8_msl2m2_powers, tik8x8_msl2m3_powers, tik8x8_msl2m4_powers,
        tik8x8_msl2m5_powers, tik8x8_msl2m6_powers, tik8x8_msl2m7_powers,
        tik8x8_msl2m8_powers, tik8x8_msl3m1_powers, tik8x8_msl3m2_powers,
        tik8x8_msl3m3_powers, tik8x8_msl3m4_powers, tik8x8_msl3m5_powers,
        tik8x8_msl3m6_powers, tik8x8_msl3m7_powers, tik8x8_msl3m8_powers};
    for (int l = 1; l <= msdeg; l++) {
      for (int m = 1; m <= msm; m++) {
        // parse the power list
        int idx_list[3 * 8] = {0}, pow_list[3 * 8] = {0};
        get_multishift_terms(ms_powers_ptr[(l - 1) * msm + m - 1], l, m,
                             NOPS_LDD8X8, idx_list, pow_list);
        t_temp = clock();
        for (int i = 0; i < cur_batch_size; i++)
          multishift_graph_filter(
              buffer_in[i], buffer_out_ms[(l - 1) * msm + m - 1][i], LEN, l, m,
              ms_coeffs_ptr[(l - 1) * msm + m - 1], idx_list, pow_list,
              nes_bdd8x8, alists_bdd8x8, wlists_bdd8x8);
        t_ms[(l - 1) * msm + m - 1] += clock() - t_temp;
        for (int i = 0; i < cur_batch_size; i++) {
          for (int j = 0; j < LEN; j++) {
            diff = buffer_out_exact[i][j] -
                   buffer_out_ms[(l - 1) * msm + m - 1][i][j];
            acc_error_ms[(l - 1) * msm + m - 1] += diff * diff;
          }
        }
      }
    }

#if CONFIG_DEBUG
    // write output GFT coefficients
    for (int i = 0; i < cur_batch_size; i++) {
      fprintf(fp_out, "Input #%d: \n", b * BATCH_SIZE + i);
      for (int j = 0; j < LEN; j++)
        fprintf(fp_out, "%.8lf ", buffer_in[i][j]);
      fprintf(fp_out, "\n");
      fprintf(fp_out, "Exact filter: ");
      for (int j = 0; j < LEN; j++)
        fprintf(fp_out, "%.8lf ", buffer_out_exact[i][j]);
      fprintf(fp_out, "\n");
      fprintf(fp_out, "FIR order 1: ");
      for (int j = 0; j < LEN; j++)
        fprintf(fp_out, "%.8lf ", buffer_out_fir[0][i][j]);
      fprintf(fp_out, "\n");
      fprintf(fp_out, "FIR order 2: ");
      for (int j = 0; j < LEN; j++)
        fprintf(fp_out, "%.8lf ", buffer_out_fir[1][i][j]);
      fprintf(fp_out, "\n");
      fprintf(fp_out, "FIR order 3: ");
      for (int j = 0; j < LEN; j++)
        fprintf(fp_out, "%.8lf ", buffer_out_fir[2][i][j]);
      fprintf(fp_out, "\n");
      fprintf(fp_out, "Multishifts order 3 nterms 8: ");
      for (int j = 0; j < LEN; j++)
        fprintf(fp_out, "%.8lf ", buffer_out_msm3[0][i][j]);
      fprintf(fp_out, "\n");
    }
#endif
  }

  // write runtime
  double time_exact = ((double)t_exact) / CLOCKS_PER_SEC;
  double time_mat = ((double)t_mat) / CLOCKS_PER_SEC;
  fprintf(fp_out, "#input = %d\n", n_inputs);
  fprintf(fp_out, "Exact filter:    %.8lf\n", time_exact);
  fprintf(fp_out, "Matrix filter:    %.8lf", time_mat);
  fprintf(fp_out, " (error = %.8lf)\n", acc_error_mat / ((double)n_inputs));
  for (int ord = 1; ord <= firdeg; ord++) {
    double time_fir = ((double)t_fir[ord - 1]) / CLOCKS_PER_SEC;
    fprintf(fp_out, "FIR filter (order = %d):    %.8lf", ord, time_fir);
    fprintf(fp_out, " (error = %.8lf)\n",
            acc_error_fir[ord - 1] / ((double)n_inputs));
  }
  for (int l = 1; l <= msdeg; l++) {
    for (int m = 1; m <= msm; m++) {
      double time_ms = ((double)t_ms[(l - 1) * msm + m - 1]) / CLOCKS_PER_SEC;
      fprintf(fp_out, "Multishift filter (order = %d, m = %d):    %.8lf", l, m,
              time_ms);
      fprintf(fp_out, " (error = %.8lf)\n",
              acc_error_ms[(l - 1) * msm + m - 1] / ((double)n_inputs));
    }
  }

  fclose(fp_in);
  fclose(fp_out);
  return 0;
}
