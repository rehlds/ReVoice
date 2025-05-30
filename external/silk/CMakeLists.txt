#-------------------------------------------------------------------------------
# Project Definition
#-------------------------------------------------------------------------------

project(        "Silk"
  VERSION       "1.0.9"
  DESCRIPTION   "Silk voice codec"
  HOMEPAGE_URL  "http://developer.skype.com/silk"
  LANGUAGES     "C"
)

#-------------------------------------------------------------------------------
# Target Definition
#-------------------------------------------------------------------------------

set(TARGET_NAME "silk")
set(TARGET_ALIAS "Silk::${TARGET_NAME}")

add_library("${TARGET_NAME}")
add_library("${TARGET_ALIAS}" ALIAS "${TARGET_NAME}")

#-------------------------------------------------------------------------------
# Source Files
#-------------------------------------------------------------------------------

target_sources("${TARGET_NAME}"
  PUBLIC
    "include/silk/SKP_Silk_control.h"
    "include/silk/SKP_Silk_errors.h"
    "include/silk/SKP_Silk_SDK_API.h"
    "include/silk/SKP_Silk_typedef.h"

  PRIVATE
    "src/SKP_Silk_A2NLSF.c"
    "src/SKP_Silk_allpass_int_FLP.c"
    "src/SKP_Silk_allpass_int.c"
    "src/SKP_Silk_ana_filt_bank_1.c"
    "src/SKP_Silk_apply_sine_window_FLP.c"
    "src/SKP_Silk_autocorrelation_FLP.c"
    "src/SKP_Silk_biquad_alt.c"
    "src/SKP_Silk_biquad.c"
    "src/SKP_Silk_burg_modified_FLP.c"
    "src/SKP_Silk_bwexpander_32.c"
    "src/SKP_Silk_bwexpander_FLP.c"
    "src/SKP_Silk_bwexpander.c"
    "src/SKP_Silk_CNG.c"
    "src/SKP_Silk_code_signs.c"
    "src/SKP_Silk_common_pitch_est_defines.h"
    "src/SKP_Silk_control_audio_bandwidth.c"
    "src/SKP_Silk_control_codec_FLP.c"
    "src/SKP_Silk_corrMatrix_FLP.c"
    "src/SKP_Silk_create_init_destroy.c"
    "src/SKP_Silk_dec_API.c"
    "src/SKP_Silk_decimate2_coarse_FLP.c"
    "src/SKP_Silk_decimate2_coarsest_FLP.c"
    "src/SKP_Silk_decode_core.c"
    "src/SKP_Silk_decode_frame.c"
    "src/SKP_Silk_decode_parameters.c"
    "src/SKP_Silk_decode_pitch.c"
    "src/SKP_Silk_decode_pulses.c"
    "src/SKP_Silk_decoder_set_fs.c"
    "src/SKP_Silk_define.h"
    "src/SKP_Silk_detect_SWB_input.c"
    "src/SKP_Silk_enc_API.c"
    "src/SKP_Silk_encode_frame_FLP.c"
    "src/SKP_Silk_encode_parameters.c"
    "src/SKP_Silk_encode_pulses.c"
    "src/SKP_Silk_energy_FLP.c"
    "src/SKP_Silk_find_LPC_FLP.c"
    "src/SKP_Silk_find_LTP_FLP.c"
    "src/SKP_Silk_find_pitch_lags_FLP.c"
    "src/SKP_Silk_find_pred_coefs_FLP.c"
    "src/SKP_Silk_gain_quant.c"
    "src/SKP_Silk_HP_variable_cutoff_FLP.c"
    "src/SKP_Silk_init_encoder_FLP.c"
    "src/SKP_Silk_Inlines.h"
    "src/SKP_Silk_inner_product_FLP.c"
    "src/SKP_Silk_interpolate.c"
    "src/SKP_Silk_k2a_FLP.c"
    "src/SKP_Silk_LBRR_reset.c"
    "src/SKP_Silk_levinsondurbin_FLP.c"
    "src/SKP_Silk_lin2log.c"
    "src/SKP_Silk_log2lin.c"
    "src/SKP_Silk_lowpass_int.c"
    "src/SKP_Silk_lowpass_short.c"
    "src/SKP_Silk_LP_variable_cutoff.c"
    "src/SKP_Silk_LPC_analysis_filter_FLP.c"
    "src/SKP_Silk_LPC_inv_pred_gain_FLP.c"
    "src/SKP_Silk_LPC_inv_pred_gain.c"
    "src/SKP_Silk_LPC_synthesis_filter.c"
    "src/SKP_Silk_LPC_synthesis_order16.c"
    "src/SKP_Silk_LSF_cos_table.c"
    "src/SKP_Silk_LTP_analysis_filter_FLP.c"
    "src/SKP_Silk_LTP_scale_ctrl_FLP.c"
    "src/SKP_Silk_MA.c"
    "src/SKP_Silk_macros.h"
    "src/SKP_Silk_main_FLP.h"
    "src/SKP_Silk_main.h"
    "src/SKP_Silk_NLSF_MSVQ_decode_FLP.c"
    "src/SKP_Silk_NLSF_MSVQ_decode.c"
    "src/SKP_Silk_NLSF_MSVQ_encode_FLP.c"
    "src/SKP_Silk_NLSF_stabilize.c"
    "src/SKP_Silk_NLSF_VQ_rate_distortion_FLP.c"
    "src/SKP_Silk_NLSF_VQ_sum_error_FLP.c"
    "src/SKP_Silk_NLSF_VQ_weights_laroia_FLP.c"
    "src/SKP_Silk_NLSF2A_stable.c"
    "src/SKP_Silk_NLSF2A.c"
    "src/SKP_Silk_noise_shape_analysis_FLP.c"
    "src/SKP_Silk_NSQ_del_dec.c"
    "src/SKP_Silk_NSQ.c"
    "src/SKP_Silk_pitch_analysis_core_FLP.c"
    "src/SKP_Silk_pitch_est_defines_FLP.h"
    "src/SKP_Silk_pitch_est_defines.h"
    "src/SKP_Silk_pitch_est_tables.c"
    "src/SKP_Silk_PLC.c"
    "src/SKP_Silk_PLC.h"
    "src/SKP_Silk_prefilter_FLP.c"
    "src/SKP_Silk_process_gains_FLP.c"
    "src/SKP_Silk_process_NLSFs_FLP.c"
    "src/SKP_Silk_quant_LTP_gains_FLP.c"
    "src/SKP_Silk_range_coder.c"
    "src/SKP_Silk_regularize_correlations_FLP.c"
    "src/SKP_Silk_resampler_down2_3.c"
    "src/SKP_Silk_resampler_down2.c"
    "src/SKP_Silk_resampler_down3.c"
    "src/SKP_Silk_resampler_private_AR2.c"
    "src/SKP_Silk_resampler_private_ARMA4.c"
    "src/SKP_Silk_resampler_private_copy.c"
    "src/SKP_Silk_resampler_private_down_FIR.c"
    "src/SKP_Silk_resampler_private_down4.c"
    "src/SKP_Silk_resampler_private_IIR_FIR.c"
    "src/SKP_Silk_resampler_private_up2_HQ.c"
    "src/SKP_Silk_resampler_private_up4.c"
    "src/SKP_Silk_resampler_private.h"
    "src/SKP_Silk_resampler_rom.c"
    "src/SKP_Silk_resampler_rom.h"
    "src/SKP_Silk_resampler_structs.h"
    "src/SKP_Silk_resampler_up2.c"
    "src/SKP_Silk_resampler.c"
    "src/SKP_Silk_residual_energy_FLP.c"
    "src/SKP_Silk_scale_copy_vector_FLP.c"
    "src/SKP_Silk_scale_vector_FLP.c"
    "src/SKP_Silk_schur_FLP.c"
    "src/SKP_Silk_setup_complexity.h"
    "src/SKP_Silk_shell_coder.c"
    "src/SKP_Silk_sigm_Q15.c"
    "src/SKP_Silk_SigProc_FIX.h"
    "src/SKP_Silk_SigProc_FLP.h"
    "src/SKP_Silk_solve_LS_FLP.c"
    "src/SKP_Silk_sort_FLP.c"
    "src/SKP_Silk_sort.c"
    "src/SKP_Silk_structs_FLP.h"
    "src/SKP_Silk_structs.h"
    "src/SKP_Silk_sum_sqr_shift.c"
    "src/SKP_Silk_tables_FLP.h"
    "src/SKP_Silk_tables_gain.c"
    "src/SKP_Silk_tables_LTP.c"
    "src/SKP_Silk_tables_NLSF_CB0_10_FLP.c"
    "src/SKP_Silk_tables_NLSF_CB0_10.c"
    "src/SKP_Silk_tables_NLSF_CB0_10.h"
    "src/SKP_Silk_tables_NLSF_CB0_16_FLP.c"
    "src/SKP_Silk_tables_NLSF_CB0_16.c"
    "src/SKP_Silk_tables_NLSF_CB0_16.h"
    "src/SKP_Silk_tables_NLSF_CB1_10_FLP.c"
    "src/SKP_Silk_tables_NLSF_CB1_10.c"
    "src/SKP_Silk_tables_NLSF_CB1_10.h"
    "src/SKP_Silk_tables_NLSF_CB1_16_FLP.c"
    "src/SKP_Silk_tables_NLSF_CB1_16.c"
    "src/SKP_Silk_tables_NLSF_CB1_16.h"
    "src/SKP_Silk_tables_other_FLP.c"
    "src/SKP_Silk_tables_other.c"
    "src/SKP_Silk_tables_pitch_lag.c"
    "src/SKP_Silk_tables_pulses_per_block.c"
    "src/SKP_Silk_tables_sign.c"
    "src/SKP_Silk_tables_type_offset.c"
    "src/SKP_Silk_tables.h"
    "src/SKP_Silk_tuning_parameters.h"
    "src/SKP_Silk_VAD.c"
    "src/SKP_Silk_VQ_nearest_neighbor_FLP.c"
    "src/SKP_Silk_warped_autocorrelation_FLP.c"
    "src/SKP_Silk_wrappers_FLP.c"
)

#-------------------------------------------------------------------------------
# Include Directories
#-------------------------------------------------------------------------------

target_include_directories("${TARGET_NAME}"
  PUBLIC
    "${PROJECT_SOURCE_DIR}/include"

  PRIVATE
    "${PROJECT_SOURCE_DIR}/include/silk"
)

#-------------------------------------------------------------------------------
# Compile Options
#-------------------------------------------------------------------------------

target_compile_options("${TARGET_NAME}"
  PRIVATE
    $<IF:$<C_COMPILER_ID:MSVC>,/W0,-w>
    $<$<AND:$<C_COMPILER_ID:MSVC>,$<NOT:$<CONFIG:Debug>>>:/fp:fast>
    $<$<AND:$<NOT:$<C_COMPILER_ID:MSVC>>,$<NOT:$<CONFIG:Debug>>>:-ffast-math>
)
