static short quant_equ[113] = {
 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 8,
 9, 9, 10, 10, 11, 11, 12, 12, 13, 13,
 14, 14, 15, 15, 16, 16, 17, 17, 18, 18,
 19, 19, 20, 20, 21, 21, 22, 22, 23, 23,
 24, 24, 25, 25, 26, 26, 27, 27, 28, 28,
 29, 29, 30, 30, 31, 31, 32, 32, 33, 33,
 34, 34, 35, 35, 36, 36, 36, 36, 36, 36,
 36, 36, 37, 37, 37, 37, 37, 37, 37, 37,
 38, 38, 38, 38, 38, 38, 38, 38, 39, 39,
 39, 39, 39, 39, 39, 39, 40, 40, 40, 40,
 40, 40, 40, 40, 41, 41, 41, 41, 41, 41,
 41, 41
};

static short quant_table_1_to_2[4096];
static short quant_table_1_to_3[4096];
static short quant_table_1_to_4[4096];
static short quant_table_1_to_5[4096];
static short quant_table_1_to_6[4096];
static short quant_table_1_to_7[4096];
static short quant_table_1_to_8[4096];
static short quant_table_1_to_10[4096];
static short quant_table_1_to_12[4096];
static short quant_table_1_to_14[4096];
static short quant_table_1_to_16[4096];
static short quant_table_1_to_18[4096];
static short quant_table_1_to_20[4096];
static short quant_table_1_to_22[4096];
static short quant_table_1_to_24[4096];
static short quant_table_1_to_26[4096];
static short quant_table_1_to_28[4096];
static short quant_table_1_to_30[4096];
static short quant_table_1_to_32[4096];
static short quant_table_1_to_34[4096];
static short quant_table_1_to_36[4096];
static short quant_table_1_to_38[4096];
static short quant_table_1_to_40[4096];
static short quant_table_1_to_42[4096];
static short quant_table_1_to_44[4096];
static short quant_table_1_to_46[4096];
static short quant_table_1_to_48[4096];
static short quant_table_1_to_50[4096];
static short quant_table_1_to_52[4096];
static short quant_table_1_to_54[4096];
static short quant_table_1_to_56[4096];
static short quant_table_1_to_58[4096];
static short quant_table_1_to_60[4096];
static short quant_table_1_to_62[4096];
static short quant_table_1_to_64[4096];
static short quant_table_1_to_72[4096];
static short quant_table_1_to_80[4096];
static short quant_table_1_to_88[4096];
static short quant_table_1_to_96[4096];
static short quant_table_1_to_104[4096];
static short quant_table_1_to_112[4096];
static short quant_table_2_to_3[4096];
static short quant_table_2_to_4[4096];
static short quant_table_2_to_5[4096];
static short quant_table_2_to_6[4096];
static short quant_table_2_to_7[4096];
static short quant_table_2_to_8[4096];
static short quant_table_2_to_10[4096];
static short quant_table_2_to_12[4096];
static short quant_table_2_to_14[4096];
static short quant_table_2_to_16[4096];
static short quant_table_2_to_18[4096];
static short quant_table_2_to_20[4096];
static short quant_table_2_to_22[4096];
static short quant_table_2_to_24[4096];
static short quant_table_2_to_26[4096];
static short quant_table_2_to_28[4096];
static short quant_table_2_to_30[4096];
static short quant_table_2_to_32[4096];
static short quant_table_2_to_34[4096];
static short quant_table_2_to_36[4096];
static short quant_table_2_to_38[4096];
static short quant_table_2_to_40[4096];
static short quant_table_2_to_42[4096];
static short quant_table_2_to_44[4096];
static short quant_table_2_to_46[4096];
static short quant_table_2_to_48[4096];
static short quant_table_2_to_50[4096];
static short quant_table_2_to_52[4096];
static short quant_table_2_to_54[4096];
static short quant_table_2_to_56[4096];
static short quant_table_2_to_58[4096];
static short quant_table_2_to_60[4096];
static short quant_table_2_to_62[4096];
static short quant_table_2_to_64[4096];
static short quant_table_2_to_72[4096];
static short quant_table_2_to_80[4096];
static short quant_table_2_to_88[4096];
static short quant_table_2_to_96[4096];
static short quant_table_2_to_104[4096];
static short quant_table_2_to_112[4096];
static short quant_table_3_to_4[4096];
static short quant_table_3_to_5[4096];
static short quant_table_3_to_6[4096];
static short quant_table_3_to_7[4096];
static short quant_table_3_to_8[4096];
static short quant_table_3_to_10[4096];
static short quant_table_3_to_12[4096];
static short quant_table_3_to_14[4096];
static short quant_table_3_to_16[4096];
static short quant_table_3_to_18[4096];
static short quant_table_3_to_20[4096];
static short quant_table_3_to_22[4096];
static short quant_table_3_to_24[4096];
static short quant_table_3_to_26[4096];
static short quant_table_3_to_28[4096];
static short quant_table_3_to_30[4096];
static short quant_table_3_to_32[4096];
static short quant_table_3_to_34[4096];
static short quant_table_3_to_36[4096];
static short quant_table_3_to_38[4096];
static short quant_table_3_to_40[4096];
static short quant_table_3_to_42[4096];
static short quant_table_3_to_44[4096];
static short quant_table_3_to_46[4096];
static short quant_table_3_to_48[4096];
static short quant_table_3_to_50[4096];
static short quant_table_3_to_52[4096];
static short quant_table_3_to_54[4096];
static short quant_table_3_to_56[4096];
static short quant_table_3_to_58[4096];
static short quant_table_3_to_60[4096];
static short quant_table_3_to_62[4096];
static short quant_table_3_to_64[4096];
static short quant_table_3_to_72[4096];
static short quant_table_3_to_80[4096];
static short quant_table_3_to_88[4096];
static short quant_table_3_to_96[4096];
static short quant_table_3_to_104[4096];
static short quant_table_3_to_112[4096];
static short quant_table_4_to_5[4096];
static short quant_table_4_to_6[4096];
static short quant_table_4_to_7[4096];
static short quant_table_4_to_8[4096];
static short quant_table_4_to_10[4096];
static short quant_table_4_to_12[4096];
static short quant_table_4_to_14[4096];
static short quant_table_4_to_16[4096];
static short quant_table_4_to_18[4096];
static short quant_table_4_to_20[4096];
static short quant_table_4_to_22[4096];
static short quant_table_4_to_24[4096];
static short quant_table_4_to_26[4096];
static short quant_table_4_to_28[4096];
static short quant_table_4_to_30[4096];
static short quant_table_4_to_32[4096];
static short quant_table_4_to_34[4096];
static short quant_table_4_to_36[4096];
static short quant_table_4_to_38[4096];
static short quant_table_4_to_40[4096];
static short quant_table_4_to_42[4096];
static short quant_table_4_to_44[4096];
static short quant_table_4_to_46[4096];
static short quant_table_4_to_48[4096];
static short quant_table_4_to_50[4096];
static short quant_table_4_to_52[4096];
static short quant_table_4_to_54[4096];
static short quant_table_4_to_56[4096];
static short quant_table_4_to_58[4096];
static short quant_table_4_to_60[4096];
static short quant_table_4_to_62[4096];
static short quant_table_4_to_64[4096];
static short quant_table_4_to_72[4096];
static short quant_table_4_to_80[4096];
static short quant_table_4_to_88[4096];
static short quant_table_4_to_96[4096];
static short quant_table_4_to_104[4096];
static short quant_table_4_to_112[4096];
static short quant_table_5_to_6[4096];
static short quant_table_5_to_7[4096];
static short quant_table_5_to_8[4096];
static short quant_table_5_to_10[4096];
static short quant_table_5_to_12[4096];
static short quant_table_5_to_14[4096];
static short quant_table_5_to_16[4096];
static short quant_table_5_to_18[4096];
static short quant_table_5_to_20[4096];
static short quant_table_5_to_22[4096];
static short quant_table_5_to_24[4096];
static short quant_table_5_to_26[4096];
static short quant_table_5_to_28[4096];
static short quant_table_5_to_30[4096];
static short quant_table_5_to_32[4096];
static short quant_table_5_to_34[4096];
static short quant_table_5_to_36[4096];
static short quant_table_5_to_38[4096];
static short quant_table_5_to_40[4096];
static short quant_table_5_to_42[4096];
static short quant_table_5_to_44[4096];
static short quant_table_5_to_46[4096];
static short quant_table_5_to_48[4096];
static short quant_table_5_to_50[4096];
static short quant_table_5_to_52[4096];
static short quant_table_5_to_54[4096];
static short quant_table_5_to_56[4096];
static short quant_table_5_to_58[4096];
static short quant_table_5_to_60[4096];
static short quant_table_5_to_62[4096];
static short quant_table_5_to_64[4096];
static short quant_table_5_to_72[4096];
static short quant_table_5_to_80[4096];
static short quant_table_5_to_88[4096];
static short quant_table_5_to_96[4096];
static short quant_table_5_to_104[4096];
static short quant_table_5_to_112[4096];
static short quant_table_6_to_7[4096];
static short quant_table_6_to_8[4096];
static short quant_table_6_to_10[4096];
static short quant_table_6_to_12[4096];
static short quant_table_6_to_14[4096];
static short quant_table_6_to_16[4096];
static short quant_table_6_to_18[4096];
static short quant_table_6_to_20[4096];
static short quant_table_6_to_22[4096];
static short quant_table_6_to_24[4096];
static short quant_table_6_to_26[4096];
static short quant_table_6_to_28[4096];
static short quant_table_6_to_30[4096];
static short quant_table_6_to_32[4096];
static short quant_table_6_to_34[4096];
static short quant_table_6_to_36[4096];
static short quant_table_6_to_38[4096];
static short quant_table_6_to_40[4096];
static short quant_table_6_to_42[4096];
static short quant_table_6_to_44[4096];
static short quant_table_6_to_46[4096];
static short quant_table_6_to_48[4096];
static short quant_table_6_to_50[4096];
static short quant_table_6_to_52[4096];
static short quant_table_6_to_54[4096];
static short quant_table_6_to_56[4096];
static short quant_table_6_to_58[4096];
static short quant_table_6_to_60[4096];
static short quant_table_6_to_62[4096];
static short quant_table_6_to_64[4096];
static short quant_table_6_to_72[4096];
static short quant_table_6_to_80[4096];
static short quant_table_6_to_88[4096];
static short quant_table_6_to_96[4096];
static short quant_table_6_to_104[4096];
static short quant_table_6_to_112[4096];
static short quant_table_7_to_8[4096];
static short quant_table_7_to_10[4096];
static short quant_table_7_to_12[4096];
static short quant_table_7_to_14[4096];
static short quant_table_7_to_16[4096];
static short quant_table_7_to_18[4096];
static short quant_table_7_to_20[4096];
static short quant_table_7_to_22[4096];
static short quant_table_7_to_24[4096];
static short quant_table_7_to_26[4096];
static short quant_table_7_to_28[4096];
static short quant_table_7_to_30[4096];
static short quant_table_7_to_32[4096];
static short quant_table_7_to_34[4096];
static short quant_table_7_to_36[4096];
static short quant_table_7_to_38[4096];
static short quant_table_7_to_40[4096];
static short quant_table_7_to_42[4096];
static short quant_table_7_to_44[4096];
static short quant_table_7_to_46[4096];
static short quant_table_7_to_48[4096];
static short quant_table_7_to_50[4096];
static short quant_table_7_to_52[4096];
static short quant_table_7_to_54[4096];
static short quant_table_7_to_56[4096];
static short quant_table_7_to_58[4096];
static short quant_table_7_to_60[4096];
static short quant_table_7_to_62[4096];
static short quant_table_7_to_64[4096];
static short quant_table_7_to_72[4096];
static short quant_table_7_to_80[4096];
static short quant_table_7_to_88[4096];
static short quant_table_7_to_96[4096];
static short quant_table_7_to_104[4096];
static short quant_table_7_to_112[4096];
static short quant_table_8_to_10[4096];
static short quant_table_8_to_12[4096];
static short quant_table_8_to_14[4096];
static short quant_table_8_to_16[4096];
static short quant_table_8_to_18[4096];
static short quant_table_8_to_20[4096];
static short quant_table_8_to_22[4096];
static short quant_table_8_to_24[4096];
static short quant_table_8_to_26[4096];
static short quant_table_8_to_28[4096];
static short quant_table_8_to_30[4096];
static short quant_table_8_to_32[4096];
static short quant_table_8_to_34[4096];
static short quant_table_8_to_36[4096];
static short quant_table_8_to_38[4096];
static short quant_table_8_to_40[4096];
static short quant_table_8_to_42[4096];
static short quant_table_8_to_44[4096];
static short quant_table_8_to_46[4096];
static short quant_table_8_to_48[4096];
static short quant_table_8_to_50[4096];
static short quant_table_8_to_52[4096];
static short quant_table_8_to_54[4096];
static short quant_table_8_to_56[4096];
static short quant_table_8_to_58[4096];
static short quant_table_8_to_60[4096];
static short quant_table_8_to_62[4096];
static short quant_table_8_to_64[4096];
static short quant_table_8_to_72[4096];
static short quant_table_8_to_80[4096];
static short quant_table_8_to_88[4096];
static short quant_table_8_to_96[4096];
static short quant_table_8_to_104[4096];
static short quant_table_8_to_112[4096];
static short quant_table_10_to_12[4096];
static short quant_table_10_to_14[4096];
static short quant_table_10_to_16[4096];
static short quant_table_10_to_18[4096];
static short quant_table_10_to_20[4096];
static short quant_table_10_to_22[4096];
static short quant_table_10_to_24[4096];
static short quant_table_10_to_26[4096];
static short quant_table_10_to_28[4096];
static short quant_table_10_to_30[4096];
static short quant_table_10_to_32[4096];
static short quant_table_10_to_34[4096];
static short quant_table_10_to_36[4096];
static short quant_table_10_to_38[4096];
static short quant_table_10_to_40[4096];
static short quant_table_10_to_42[4096];
static short quant_table_10_to_44[4096];
static short quant_table_10_to_46[4096];
static short quant_table_10_to_48[4096];
static short quant_table_10_to_50[4096];
static short quant_table_10_to_52[4096];
static short quant_table_10_to_54[4096];
static short quant_table_10_to_56[4096];
static short quant_table_10_to_58[4096];
static short quant_table_10_to_60[4096];
static short quant_table_10_to_62[4096];
static short quant_table_10_to_64[4096];
static short quant_table_10_to_72[4096];
static short quant_table_10_to_80[4096];
static short quant_table_10_to_88[4096];
static short quant_table_10_to_96[4096];
static short quant_table_10_to_104[4096];
static short quant_table_10_to_112[4096];
static short quant_table_12_to_14[4096];
static short quant_table_12_to_16[4096];
static short quant_table_12_to_18[4096];
static short quant_table_12_to_20[4096];
static short quant_table_12_to_22[4096];
static short quant_table_12_to_24[4096];
static short quant_table_12_to_26[4096];
static short quant_table_12_to_28[4096];
static short quant_table_12_to_30[4096];
static short quant_table_12_to_32[4096];
static short quant_table_12_to_34[4096];
static short quant_table_12_to_36[4096];
static short quant_table_12_to_38[4096];
static short quant_table_12_to_40[4096];
static short quant_table_12_to_42[4096];
static short quant_table_12_to_44[4096];
static short quant_table_12_to_46[4096];
static short quant_table_12_to_48[4096];
static short quant_table_12_to_50[4096];
static short quant_table_12_to_52[4096];
static short quant_table_12_to_54[4096];
static short quant_table_12_to_56[4096];
static short quant_table_12_to_58[4096];
static short quant_table_12_to_60[4096];
static short quant_table_12_to_62[4096];
static short quant_table_12_to_64[4096];
static short quant_table_12_to_72[4096];
static short quant_table_12_to_80[4096];
static short quant_table_12_to_88[4096];
static short quant_table_12_to_96[4096];
static short quant_table_12_to_104[4096];
static short quant_table_12_to_112[4096];
static short quant_table_14_to_16[4096];
static short quant_table_14_to_18[4096];
static short quant_table_14_to_20[4096];
static short quant_table_14_to_22[4096];
static short quant_table_14_to_24[4096];
static short quant_table_14_to_26[4096];
static short quant_table_14_to_28[4096];
static short quant_table_14_to_30[4096];
static short quant_table_14_to_32[4096];
static short quant_table_14_to_34[4096];
static short quant_table_14_to_36[4096];
static short quant_table_14_to_38[4096];
static short quant_table_14_to_40[4096];
static short quant_table_14_to_42[4096];
static short quant_table_14_to_44[4096];
static short quant_table_14_to_46[4096];
static short quant_table_14_to_48[4096];
static short quant_table_14_to_50[4096];
static short quant_table_14_to_52[4096];
static short quant_table_14_to_54[4096];
static short quant_table_14_to_56[4096];
static short quant_table_14_to_58[4096];
static short quant_table_14_to_60[4096];
static short quant_table_14_to_62[4096];
static short quant_table_14_to_64[4096];
static short quant_table_14_to_72[4096];
static short quant_table_14_to_80[4096];
static short quant_table_14_to_88[4096];
static short quant_table_14_to_96[4096];
static short quant_table_14_to_104[4096];
static short quant_table_14_to_112[4096];
static short quant_table_16_to_18[4096];
static short quant_table_16_to_20[4096];
static short quant_table_16_to_22[4096];
static short quant_table_16_to_24[4096];
static short quant_table_16_to_26[4096];
static short quant_table_16_to_28[4096];
static short quant_table_16_to_30[4096];
static short quant_table_16_to_32[4096];
static short quant_table_16_to_34[4096];
static short quant_table_16_to_36[4096];
static short quant_table_16_to_38[4096];
static short quant_table_16_to_40[4096];
static short quant_table_16_to_42[4096];
static short quant_table_16_to_44[4096];
static short quant_table_16_to_46[4096];
static short quant_table_16_to_48[4096];
static short quant_table_16_to_50[4096];
static short quant_table_16_to_52[4096];
static short quant_table_16_to_54[4096];
static short quant_table_16_to_56[4096];
static short quant_table_16_to_58[4096];
static short quant_table_16_to_60[4096];
static short quant_table_16_to_62[4096];
static short quant_table_16_to_64[4096];
static short quant_table_16_to_72[4096];
static short quant_table_16_to_80[4096];
static short quant_table_16_to_88[4096];
static short quant_table_16_to_96[4096];
static short quant_table_16_to_104[4096];
static short quant_table_16_to_112[4096];
static short quant_table_18_to_20[4096];
static short quant_table_18_to_22[4096];
static short quant_table_18_to_24[4096];
static short quant_table_18_to_26[4096];
static short quant_table_18_to_28[4096];
static short quant_table_18_to_30[4096];
static short quant_table_18_to_32[4096];
static short quant_table_18_to_34[4096];
static short quant_table_18_to_36[4096];
static short quant_table_18_to_38[4096];
static short quant_table_18_to_40[4096];
static short quant_table_18_to_42[4096];
static short quant_table_18_to_44[4096];
static short quant_table_18_to_46[4096];
static short quant_table_18_to_48[4096];
static short quant_table_18_to_50[4096];
static short quant_table_18_to_52[4096];
static short quant_table_18_to_54[4096];
static short quant_table_18_to_56[4096];
static short quant_table_18_to_58[4096];
static short quant_table_18_to_60[4096];
static short quant_table_18_to_62[4096];
static short quant_table_18_to_64[4096];
static short quant_table_18_to_72[4096];
static short quant_table_18_to_80[4096];
static short quant_table_18_to_88[4096];
static short quant_table_18_to_96[4096];
static short quant_table_18_to_104[4096];
static short quant_table_18_to_112[4096];
static short quant_table_20_to_22[4096];
static short quant_table_20_to_24[4096];
static short quant_table_20_to_26[4096];
static short quant_table_20_to_28[4096];
static short quant_table_20_to_30[4096];
static short quant_table_20_to_32[4096];
static short quant_table_20_to_34[4096];
static short quant_table_20_to_36[4096];
static short quant_table_20_to_38[4096];
static short quant_table_20_to_40[4096];
static short quant_table_20_to_42[4096];
static short quant_table_20_to_44[4096];
static short quant_table_20_to_46[4096];
static short quant_table_20_to_48[4096];
static short quant_table_20_to_50[4096];
static short quant_table_20_to_52[4096];
static short quant_table_20_to_54[4096];
static short quant_table_20_to_56[4096];
static short quant_table_20_to_58[4096];
static short quant_table_20_to_60[4096];
static short quant_table_20_to_62[4096];
static short quant_table_20_to_64[4096];
static short quant_table_20_to_72[4096];
static short quant_table_20_to_80[4096];
static short quant_table_20_to_88[4096];
static short quant_table_20_to_96[4096];
static short quant_table_20_to_104[4096];
static short quant_table_20_to_112[4096];
static short quant_table_22_to_24[4096];
static short quant_table_22_to_26[4096];
static short quant_table_22_to_28[4096];
static short quant_table_22_to_30[4096];
static short quant_table_22_to_32[4096];
static short quant_table_22_to_34[4096];
static short quant_table_22_to_36[4096];
static short quant_table_22_to_38[4096];
static short quant_table_22_to_40[4096];
static short quant_table_22_to_42[4096];
static short quant_table_22_to_44[4096];
static short quant_table_22_to_46[4096];
static short quant_table_22_to_48[4096];
static short quant_table_22_to_50[4096];
static short quant_table_22_to_52[4096];
static short quant_table_22_to_54[4096];
static short quant_table_22_to_56[4096];
static short quant_table_22_to_58[4096];
static short quant_table_22_to_60[4096];
static short quant_table_22_to_62[4096];
static short quant_table_22_to_64[4096];
static short quant_table_22_to_72[4096];
static short quant_table_22_to_80[4096];
static short quant_table_22_to_88[4096];
static short quant_table_22_to_96[4096];
static short quant_table_22_to_104[4096];
static short quant_table_22_to_112[4096];
static short quant_table_24_to_26[4096];
static short quant_table_24_to_28[4096];
static short quant_table_24_to_30[4096];
static short quant_table_24_to_32[4096];
static short quant_table_24_to_34[4096];
static short quant_table_24_to_36[4096];
static short quant_table_24_to_38[4096];
static short quant_table_24_to_40[4096];
static short quant_table_24_to_42[4096];
static short quant_table_24_to_44[4096];
static short quant_table_24_to_46[4096];
static short quant_table_24_to_48[4096];
static short quant_table_24_to_50[4096];
static short quant_table_24_to_52[4096];
static short quant_table_24_to_54[4096];
static short quant_table_24_to_56[4096];
static short quant_table_24_to_58[4096];
static short quant_table_24_to_60[4096];
static short quant_table_24_to_62[4096];
static short quant_table_24_to_64[4096];
static short quant_table_24_to_72[4096];
static short quant_table_24_to_80[4096];
static short quant_table_24_to_88[4096];
static short quant_table_24_to_96[4096];
static short quant_table_24_to_104[4096];
static short quant_table_24_to_112[4096];
static short quant_table_26_to_28[4096];
static short quant_table_26_to_30[4096];
static short quant_table_26_to_32[4096];
static short quant_table_26_to_34[4096];
static short quant_table_26_to_36[4096];
static short quant_table_26_to_38[4096];
static short quant_table_26_to_40[4096];
static short quant_table_26_to_42[4096];
static short quant_table_26_to_44[4096];
static short quant_table_26_to_46[4096];
static short quant_table_26_to_48[4096];
static short quant_table_26_to_50[4096];
static short quant_table_26_to_52[4096];
static short quant_table_26_to_54[4096];
static short quant_table_26_to_56[4096];
static short quant_table_26_to_58[4096];
static short quant_table_26_to_60[4096];
static short quant_table_26_to_62[4096];
static short quant_table_26_to_64[4096];
static short quant_table_26_to_72[4096];
static short quant_table_26_to_80[4096];
static short quant_table_26_to_88[4096];
static short quant_table_26_to_96[4096];
static short quant_table_26_to_104[4096];
static short quant_table_26_to_112[4096];
static short quant_table_28_to_30[4096];
static short quant_table_28_to_32[4096];
static short quant_table_28_to_34[4096];
static short quant_table_28_to_36[4096];
static short quant_table_28_to_38[4096];
static short quant_table_28_to_40[4096];
static short quant_table_28_to_42[4096];
static short quant_table_28_to_44[4096];
static short quant_table_28_to_46[4096];
static short quant_table_28_to_48[4096];
static short quant_table_28_to_50[4096];
static short quant_table_28_to_52[4096];
static short quant_table_28_to_54[4096];
static short quant_table_28_to_56[4096];
static short quant_table_28_to_58[4096];
static short quant_table_28_to_60[4096];
static short quant_table_28_to_62[4096];
static short quant_table_28_to_64[4096];
static short quant_table_28_to_72[4096];
static short quant_table_28_to_80[4096];
static short quant_table_28_to_88[4096];
static short quant_table_28_to_96[4096];
static short quant_table_28_to_104[4096];
static short quant_table_28_to_112[4096];
static short quant_table_30_to_32[4096];
static short quant_table_30_to_34[4096];
static short quant_table_30_to_36[4096];
static short quant_table_30_to_38[4096];
static short quant_table_30_to_40[4096];
static short quant_table_30_to_42[4096];
static short quant_table_30_to_44[4096];
static short quant_table_30_to_46[4096];
static short quant_table_30_to_48[4096];
static short quant_table_30_to_50[4096];
static short quant_table_30_to_52[4096];
static short quant_table_30_to_54[4096];
static short quant_table_30_to_56[4096];
static short quant_table_30_to_58[4096];
static short quant_table_30_to_60[4096];
static short quant_table_30_to_62[4096];
static short quant_table_30_to_64[4096];
static short quant_table_30_to_72[4096];
static short quant_table_30_to_80[4096];
static short quant_table_30_to_88[4096];
static short quant_table_30_to_96[4096];
static short quant_table_30_to_104[4096];
static short quant_table_30_to_112[4096];
static short quant_table_32_to_34[4096];
static short quant_table_32_to_36[4096];
static short quant_table_32_to_38[4096];
static short quant_table_32_to_40[4096];
static short quant_table_32_to_42[4096];
static short quant_table_32_to_44[4096];
static short quant_table_32_to_46[4096];
static short quant_table_32_to_48[4096];
static short quant_table_32_to_50[4096];
static short quant_table_32_to_52[4096];
static short quant_table_32_to_54[4096];
static short quant_table_32_to_56[4096];
static short quant_table_32_to_58[4096];
static short quant_table_32_to_60[4096];
static short quant_table_32_to_62[4096];
static short quant_table_32_to_64[4096];
static short quant_table_32_to_72[4096];
static short quant_table_32_to_80[4096];
static short quant_table_32_to_88[4096];
static short quant_table_32_to_96[4096];
static short quant_table_32_to_104[4096];
static short quant_table_32_to_112[4096];
static short quant_table_34_to_36[4096];
static short quant_table_34_to_38[4096];
static short quant_table_34_to_40[4096];
static short quant_table_34_to_42[4096];
static short quant_table_34_to_44[4096];
static short quant_table_34_to_46[4096];
static short quant_table_34_to_48[4096];
static short quant_table_34_to_50[4096];
static short quant_table_34_to_52[4096];
static short quant_table_34_to_54[4096];
static short quant_table_34_to_56[4096];
static short quant_table_34_to_58[4096];
static short quant_table_34_to_60[4096];
static short quant_table_34_to_62[4096];
static short quant_table_34_to_64[4096];
static short quant_table_34_to_72[4096];
static short quant_table_34_to_80[4096];
static short quant_table_34_to_88[4096];
static short quant_table_34_to_96[4096];
static short quant_table_34_to_104[4096];
static short quant_table_34_to_112[4096];
static short quant_table_36_to_38[4096];
static short quant_table_36_to_40[4096];
static short quant_table_36_to_42[4096];
static short quant_table_36_to_44[4096];
static short quant_table_36_to_46[4096];
static short quant_table_36_to_48[4096];
static short quant_table_36_to_50[4096];
static short quant_table_36_to_52[4096];
static short quant_table_36_to_54[4096];
static short quant_table_36_to_56[4096];
static short quant_table_36_to_58[4096];
static short quant_table_36_to_60[4096];
static short quant_table_36_to_62[4096];
static short quant_table_36_to_64[4096];
static short quant_table_36_to_72[4096];
static short quant_table_36_to_80[4096];
static short quant_table_36_to_88[4096];
static short quant_table_36_to_96[4096];
static short quant_table_36_to_104[4096];
static short quant_table_36_to_112[4096];
static short quant_table_38_to_40[4096];
static short quant_table_38_to_42[4096];
static short quant_table_38_to_44[4096];
static short quant_table_38_to_46[4096];
static short quant_table_38_to_48[4096];
static short quant_table_38_to_50[4096];
static short quant_table_38_to_52[4096];
static short quant_table_38_to_54[4096];
static short quant_table_38_to_56[4096];
static short quant_table_38_to_58[4096];
static short quant_table_38_to_60[4096];
static short quant_table_38_to_62[4096];
static short quant_table_38_to_64[4096];
static short quant_table_38_to_72[4096];
static short quant_table_38_to_80[4096];
static short quant_table_38_to_88[4096];
static short quant_table_38_to_96[4096];
static short quant_table_38_to_104[4096];
static short quant_table_38_to_112[4096];
static short quant_table_40_to_42[4096];
static short quant_table_40_to_44[4096];
static short quant_table_40_to_46[4096];
static short quant_table_40_to_48[4096];
static short quant_table_40_to_50[4096];
static short quant_table_40_to_52[4096];
static short quant_table_40_to_54[4096];
static short quant_table_40_to_56[4096];
static short quant_table_40_to_58[4096];
static short quant_table_40_to_60[4096];
static short quant_table_40_to_62[4096];
static short quant_table_40_to_64[4096];
static short quant_table_40_to_72[4096];
static short quant_table_40_to_80[4096];
static short quant_table_40_to_88[4096];
static short quant_table_40_to_96[4096];
static short quant_table_40_to_104[4096];
static short quant_table_40_to_112[4096];
static short quant_table_42_to_44[4096];
static short quant_table_42_to_46[4096];
static short quant_table_42_to_48[4096];
static short quant_table_42_to_50[4096];
static short quant_table_42_to_52[4096];
static short quant_table_42_to_54[4096];
static short quant_table_42_to_56[4096];
static short quant_table_42_to_58[4096];
static short quant_table_42_to_60[4096];
static short quant_table_42_to_62[4096];
static short quant_table_42_to_64[4096];
static short quant_table_42_to_72[4096];
static short quant_table_42_to_80[4096];
static short quant_table_42_to_88[4096];
static short quant_table_42_to_96[4096];
static short quant_table_42_to_104[4096];
static short quant_table_42_to_112[4096];
static short quant_table_44_to_46[4096];
static short quant_table_44_to_48[4096];
static short quant_table_44_to_50[4096];
static short quant_table_44_to_52[4096];
static short quant_table_44_to_54[4096];
static short quant_table_44_to_56[4096];
static short quant_table_44_to_58[4096];
static short quant_table_44_to_60[4096];
static short quant_table_44_to_62[4096];
static short quant_table_44_to_64[4096];
static short quant_table_44_to_72[4096];
static short quant_table_44_to_80[4096];
static short quant_table_44_to_88[4096];
static short quant_table_44_to_96[4096];
static short quant_table_44_to_104[4096];
static short quant_table_44_to_112[4096];
static short quant_table_46_to_48[4096];
static short quant_table_46_to_50[4096];
static short quant_table_46_to_52[4096];
static short quant_table_46_to_54[4096];
static short quant_table_46_to_56[4096];
static short quant_table_46_to_58[4096];
static short quant_table_46_to_60[4096];
static short quant_table_46_to_62[4096];
static short quant_table_46_to_64[4096];
static short quant_table_46_to_72[4096];
static short quant_table_46_to_80[4096];
static short quant_table_46_to_88[4096];
static short quant_table_46_to_96[4096];
static short quant_table_46_to_104[4096];
static short quant_table_46_to_112[4096];
static short quant_table_48_to_50[4096];
static short quant_table_48_to_52[4096];
static short quant_table_48_to_54[4096];
static short quant_table_48_to_56[4096];
static short quant_table_48_to_58[4096];
static short quant_table_48_to_60[4096];
static short quant_table_48_to_62[4096];
static short quant_table_48_to_64[4096];
static short quant_table_48_to_72[4096];
static short quant_table_48_to_80[4096];
static short quant_table_48_to_88[4096];
static short quant_table_48_to_96[4096];
static short quant_table_48_to_104[4096];
static short quant_table_48_to_112[4096];
static short quant_table_50_to_52[4096];
static short quant_table_50_to_54[4096];
static short quant_table_50_to_56[4096];
static short quant_table_50_to_58[4096];
static short quant_table_50_to_60[4096];
static short quant_table_50_to_62[4096];
static short quant_table_50_to_64[4096];
static short quant_table_50_to_72[4096];
static short quant_table_50_to_80[4096];
static short quant_table_50_to_88[4096];
static short quant_table_50_to_96[4096];
static short quant_table_50_to_104[4096];
static short quant_table_50_to_112[4096];
static short quant_table_52_to_54[4096];
static short quant_table_52_to_56[4096];
static short quant_table_52_to_58[4096];
static short quant_table_52_to_60[4096];
static short quant_table_52_to_62[4096];
static short quant_table_52_to_64[4096];
static short quant_table_52_to_72[4096];
static short quant_table_52_to_80[4096];
static short quant_table_52_to_88[4096];
static short quant_table_52_to_96[4096];
static short quant_table_52_to_104[4096];
static short quant_table_52_to_112[4096];
static short quant_table_54_to_56[4096];
static short quant_table_54_to_58[4096];
static short quant_table_54_to_60[4096];
static short quant_table_54_to_62[4096];
static short quant_table_54_to_64[4096];
static short quant_table_54_to_72[4096];
static short quant_table_54_to_80[4096];
static short quant_table_54_to_88[4096];
static short quant_table_54_to_96[4096];
static short quant_table_54_to_104[4096];
static short quant_table_54_to_112[4096];
static short quant_table_56_to_58[4096];
static short quant_table_56_to_60[4096];
static short quant_table_56_to_62[4096];
static short quant_table_56_to_64[4096];
static short quant_table_56_to_72[4096];
static short quant_table_56_to_80[4096];
static short quant_table_56_to_88[4096];
static short quant_table_56_to_96[4096];
static short quant_table_56_to_104[4096];
static short quant_table_56_to_112[4096];
static short quant_table_58_to_60[4096];
static short quant_table_58_to_62[4096];
static short quant_table_58_to_64[4096];
static short quant_table_58_to_72[4096];
static short quant_table_58_to_80[4096];
static short quant_table_58_to_88[4096];
static short quant_table_58_to_96[4096];
static short quant_table_58_to_104[4096];
static short quant_table_58_to_112[4096];
static short quant_table_60_to_62[4096];
static short quant_table_60_to_64[4096];
static short quant_table_60_to_72[4096];
static short quant_table_60_to_80[4096];
static short quant_table_60_to_88[4096];
static short quant_table_60_to_96[4096];
static short quant_table_60_to_104[4096];
static short quant_table_60_to_112[4096];
static short quant_table_62_to_64[4096];
static short quant_table_62_to_72[4096];
static short quant_table_62_to_80[4096];
static short quant_table_62_to_88[4096];
static short quant_table_62_to_96[4096];
static short quant_table_62_to_104[4096];
static short quant_table_62_to_112[4096];
static short quant_table_64_to_72[4096];
static short quant_table_64_to_80[4096];
static short quant_table_64_to_88[4096];
static short quant_table_64_to_96[4096];
static short quant_table_64_to_104[4096];
static short quant_table_64_to_112[4096];
static short quant_table_72_to_80[4096];
static short quant_table_72_to_88[4096];
static short quant_table_72_to_96[4096];
static short quant_table_72_to_104[4096];
static short quant_table_72_to_112[4096];
static short quant_table_80_to_88[4096];
static short quant_table_80_to_96[4096];
static short quant_table_80_to_104[4096];
static short quant_table_80_to_112[4096];
static short quant_table_88_to_96[4096];
static short quant_table_88_to_104[4096];
static short quant_table_88_to_112[4096];
static short quant_table_96_to_104[4096];
static short quant_table_96_to_112[4096];
static short quant_table_104_to_112[4096];
static short *quant_tables[42][42] = {
{ 0, &quant_table_1_to_2[2048], &quant_table_1_to_3[2048], &quant_table_1_to_4[2048], &quant_table_1_to_5[2048],
 &quant_table_1_to_6[2048], &quant_table_1_to_7[2048], &quant_table_1_to_8[2048], &quant_table_1_to_10[2048],
 &quant_table_1_to_12[2048], &quant_table_1_to_14[2048], &quant_table_1_to_16[2048], &quant_table_1_to_18[2048],
 &quant_table_1_to_20[2048], &quant_table_1_to_22[2048], &quant_table_1_to_24[2048], &quant_table_1_to_26[2048],
 &quant_table_1_to_28[2048], &quant_table_1_to_30[2048], &quant_table_1_to_32[2048], &quant_table_1_to_34[2048],
 &quant_table_1_to_36[2048], &quant_table_1_to_38[2048], &quant_table_1_to_40[2048], &quant_table_1_to_42[2048],
 &quant_table_1_to_44[2048], &quant_table_1_to_46[2048], &quant_table_1_to_48[2048], &quant_table_1_to_50[2048],
 &quant_table_1_to_52[2048], &quant_table_1_to_54[2048], &quant_table_1_to_56[2048], &quant_table_1_to_58[2048],
 &quant_table_1_to_60[2048], &quant_table_1_to_62[2048], &quant_table_1_to_64[2048], &quant_table_1_to_72[2048],
 &quant_table_1_to_80[2048], &quant_table_1_to_88[2048], &quant_table_1_to_96[2048], &quant_table_1_to_104[2048],
 &quant_table_1_to_112[2048]},
{ 0,0, &quant_table_2_to_3[2048], &quant_table_2_to_4[2048], &quant_table_2_to_5[2048],
 &quant_table_2_to_6[2048], &quant_table_2_to_7[2048], &quant_table_2_to_8[2048], &quant_table_2_to_10[2048],
 &quant_table_2_to_12[2048], &quant_table_2_to_14[2048], &quant_table_2_to_16[2048], &quant_table_2_to_18[2048],
 &quant_table_2_to_20[2048], &quant_table_2_to_22[2048], &quant_table_2_to_24[2048], &quant_table_2_to_26[2048],
 &quant_table_2_to_28[2048], &quant_table_2_to_30[2048], &quant_table_2_to_32[2048], &quant_table_2_to_34[2048],
 &quant_table_2_to_36[2048], &quant_table_2_to_38[2048], &quant_table_2_to_40[2048], &quant_table_2_to_42[2048],
 &quant_table_2_to_44[2048], &quant_table_2_to_46[2048], &quant_table_2_to_48[2048], &quant_table_2_to_50[2048],
 &quant_table_2_to_52[2048], &quant_table_2_to_54[2048], &quant_table_2_to_56[2048], &quant_table_2_to_58[2048],
 &quant_table_2_to_60[2048], &quant_table_2_to_62[2048], &quant_table_2_to_64[2048], &quant_table_2_to_72[2048],
 &quant_table_2_to_80[2048], &quant_table_2_to_88[2048], &quant_table_2_to_96[2048], &quant_table_2_to_104[2048],
 &quant_table_2_to_112[2048]},
{ 0,0,0, &quant_table_3_to_4[2048], &quant_table_3_to_5[2048],
 &quant_table_3_to_6[2048], &quant_table_3_to_7[2048], &quant_table_3_to_8[2048], &quant_table_3_to_10[2048],
 &quant_table_3_to_12[2048], &quant_table_3_to_14[2048], &quant_table_3_to_16[2048], &quant_table_3_to_18[2048],
 &quant_table_3_to_20[2048], &quant_table_3_to_22[2048], &quant_table_3_to_24[2048], &quant_table_3_to_26[2048],
 &quant_table_3_to_28[2048], &quant_table_3_to_30[2048], &quant_table_3_to_32[2048], &quant_table_3_to_34[2048],
 &quant_table_3_to_36[2048], &quant_table_3_to_38[2048], &quant_table_3_to_40[2048], &quant_table_3_to_42[2048],
 &quant_table_3_to_44[2048], &quant_table_3_to_46[2048], &quant_table_3_to_48[2048], &quant_table_3_to_50[2048],
 &quant_table_3_to_52[2048], &quant_table_3_to_54[2048], &quant_table_3_to_56[2048], &quant_table_3_to_58[2048],
 &quant_table_3_to_60[2048], &quant_table_3_to_62[2048], &quant_table_3_to_64[2048], &quant_table_3_to_72[2048],
 &quant_table_3_to_80[2048], &quant_table_3_to_88[2048], &quant_table_3_to_96[2048], &quant_table_3_to_104[2048],
 &quant_table_3_to_112[2048]},
{ 0,0,0,0, &quant_table_4_to_5[2048],
 &quant_table_4_to_6[2048], &quant_table_4_to_7[2048], &quant_table_4_to_8[2048], &quant_table_4_to_10[2048],
 &quant_table_4_to_12[2048], &quant_table_4_to_14[2048], &quant_table_4_to_16[2048], &quant_table_4_to_18[2048],
 &quant_table_4_to_20[2048], &quant_table_4_to_22[2048], &quant_table_4_to_24[2048], &quant_table_4_to_26[2048],
 &quant_table_4_to_28[2048], &quant_table_4_to_30[2048], &quant_table_4_to_32[2048], &quant_table_4_to_34[2048],
 &quant_table_4_to_36[2048], &quant_table_4_to_38[2048], &quant_table_4_to_40[2048], &quant_table_4_to_42[2048],
 &quant_table_4_to_44[2048], &quant_table_4_to_46[2048], &quant_table_4_to_48[2048], &quant_table_4_to_50[2048],
 &quant_table_4_to_52[2048], &quant_table_4_to_54[2048], &quant_table_4_to_56[2048], &quant_table_4_to_58[2048],
 &quant_table_4_to_60[2048], &quant_table_4_to_62[2048], &quant_table_4_to_64[2048], &quant_table_4_to_72[2048],
 &quant_table_4_to_80[2048], &quant_table_4_to_88[2048], &quant_table_4_to_96[2048], &quant_table_4_to_104[2048],
 &quant_table_4_to_112[2048]},
{ 0,0,0,0,0, &quant_table_5_to_6[2048], &quant_table_5_to_7[2048], &quant_table_5_to_8[2048], &quant_table_5_to_10[2048],
 &quant_table_5_to_12[2048], &quant_table_5_to_14[2048], &quant_table_5_to_16[2048], &quant_table_5_to_18[2048],
 &quant_table_5_to_20[2048], &quant_table_5_to_22[2048], &quant_table_5_to_24[2048], &quant_table_5_to_26[2048],
 &quant_table_5_to_28[2048], &quant_table_5_to_30[2048], &quant_table_5_to_32[2048], &quant_table_5_to_34[2048],
 &quant_table_5_to_36[2048], &quant_table_5_to_38[2048], &quant_table_5_to_40[2048], &quant_table_5_to_42[2048],
 &quant_table_5_to_44[2048], &quant_table_5_to_46[2048], &quant_table_5_to_48[2048], &quant_table_5_to_50[2048],
 &quant_table_5_to_52[2048], &quant_table_5_to_54[2048], &quant_table_5_to_56[2048], &quant_table_5_to_58[2048],
 &quant_table_5_to_60[2048], &quant_table_5_to_62[2048], &quant_table_5_to_64[2048], &quant_table_5_to_72[2048],
 &quant_table_5_to_80[2048], &quant_table_5_to_88[2048], &quant_table_5_to_96[2048], &quant_table_5_to_104[2048],
 &quant_table_5_to_112[2048]},
{ 0,0,0,0,0,0, &quant_table_6_to_7[2048], &quant_table_6_to_8[2048], &quant_table_6_to_10[2048],
 &quant_table_6_to_12[2048], &quant_table_6_to_14[2048], &quant_table_6_to_16[2048], &quant_table_6_to_18[2048],
 &quant_table_6_to_20[2048], &quant_table_6_to_22[2048], &quant_table_6_to_24[2048], &quant_table_6_to_26[2048],
 &quant_table_6_to_28[2048], &quant_table_6_to_30[2048], &quant_table_6_to_32[2048], &quant_table_6_to_34[2048],
 &quant_table_6_to_36[2048], &quant_table_6_to_38[2048], &quant_table_6_to_40[2048], &quant_table_6_to_42[2048],
 &quant_table_6_to_44[2048], &quant_table_6_to_46[2048], &quant_table_6_to_48[2048], &quant_table_6_to_50[2048],
 &quant_table_6_to_52[2048], &quant_table_6_to_54[2048], &quant_table_6_to_56[2048], &quant_table_6_to_58[2048],
 &quant_table_6_to_60[2048], &quant_table_6_to_62[2048], &quant_table_6_to_64[2048], &quant_table_6_to_72[2048],
 &quant_table_6_to_80[2048], &quant_table_6_to_88[2048], &quant_table_6_to_96[2048], &quant_table_6_to_104[2048],
 &quant_table_6_to_112[2048]},
{ 0,0,0,0,0,0,0, &quant_table_7_to_8[2048], &quant_table_7_to_10[2048],
 &quant_table_7_to_12[2048], &quant_table_7_to_14[2048], &quant_table_7_to_16[2048], &quant_table_7_to_18[2048],
 &quant_table_7_to_20[2048], &quant_table_7_to_22[2048], &quant_table_7_to_24[2048], &quant_table_7_to_26[2048],
 &quant_table_7_to_28[2048], &quant_table_7_to_30[2048], &quant_table_7_to_32[2048], &quant_table_7_to_34[2048],
 &quant_table_7_to_36[2048], &quant_table_7_to_38[2048], &quant_table_7_to_40[2048], &quant_table_7_to_42[2048],
 &quant_table_7_to_44[2048], &quant_table_7_to_46[2048], &quant_table_7_to_48[2048], &quant_table_7_to_50[2048],
 &quant_table_7_to_52[2048], &quant_table_7_to_54[2048], &quant_table_7_to_56[2048], &quant_table_7_to_58[2048],
 &quant_table_7_to_60[2048], &quant_table_7_to_62[2048], &quant_table_7_to_64[2048], &quant_table_7_to_72[2048],
 &quant_table_7_to_80[2048], &quant_table_7_to_88[2048], &quant_table_7_to_96[2048], &quant_table_7_to_104[2048],
 &quant_table_7_to_112[2048]},
{ 0,0,0,0,0,0,0,0, &quant_table_8_to_10[2048],
 &quant_table_8_to_12[2048], &quant_table_8_to_14[2048], &quant_table_8_to_16[2048], &quant_table_8_to_18[2048],
 &quant_table_8_to_20[2048], &quant_table_8_to_22[2048], &quant_table_8_to_24[2048], &quant_table_8_to_26[2048],
 &quant_table_8_to_28[2048], &quant_table_8_to_30[2048], &quant_table_8_to_32[2048], &quant_table_8_to_34[2048],
 &quant_table_8_to_36[2048], &quant_table_8_to_38[2048], &quant_table_8_to_40[2048], &quant_table_8_to_42[2048],
 &quant_table_8_to_44[2048], &quant_table_8_to_46[2048], &quant_table_8_to_48[2048], &quant_table_8_to_50[2048],
 &quant_table_8_to_52[2048], &quant_table_8_to_54[2048], &quant_table_8_to_56[2048], &quant_table_8_to_58[2048],
 &quant_table_8_to_60[2048], &quant_table_8_to_62[2048], &quant_table_8_to_64[2048], &quant_table_8_to_72[2048],
 &quant_table_8_to_80[2048], &quant_table_8_to_88[2048], &quant_table_8_to_96[2048], &quant_table_8_to_104[2048],
 &quant_table_8_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0, &quant_table_10_to_12[2048], &quant_table_10_to_14[2048], &quant_table_10_to_16[2048], &quant_table_10_to_18[2048],
 &quant_table_10_to_20[2048], &quant_table_10_to_22[2048], &quant_table_10_to_24[2048], &quant_table_10_to_26[2048],
 &quant_table_10_to_28[2048], &quant_table_10_to_30[2048], &quant_table_10_to_32[2048], &quant_table_10_to_34[2048],
 &quant_table_10_to_36[2048], &quant_table_10_to_38[2048], &quant_table_10_to_40[2048], &quant_table_10_to_42[2048],
 &quant_table_10_to_44[2048], &quant_table_10_to_46[2048], &quant_table_10_to_48[2048], &quant_table_10_to_50[2048],
 &quant_table_10_to_52[2048], &quant_table_10_to_54[2048], &quant_table_10_to_56[2048], &quant_table_10_to_58[2048],
 &quant_table_10_to_60[2048], &quant_table_10_to_62[2048], &quant_table_10_to_64[2048], &quant_table_10_to_72[2048],
 &quant_table_10_to_80[2048], &quant_table_10_to_88[2048], &quant_table_10_to_96[2048], &quant_table_10_to_104[2048],
 &quant_table_10_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0, &quant_table_12_to_14[2048], &quant_table_12_to_16[2048], &quant_table_12_to_18[2048],
 &quant_table_12_to_20[2048], &quant_table_12_to_22[2048], &quant_table_12_to_24[2048], &quant_table_12_to_26[2048],
 &quant_table_12_to_28[2048], &quant_table_12_to_30[2048], &quant_table_12_to_32[2048], &quant_table_12_to_34[2048],
 &quant_table_12_to_36[2048], &quant_table_12_to_38[2048], &quant_table_12_to_40[2048], &quant_table_12_to_42[2048],
 &quant_table_12_to_44[2048], &quant_table_12_to_46[2048], &quant_table_12_to_48[2048], &quant_table_12_to_50[2048],
 &quant_table_12_to_52[2048], &quant_table_12_to_54[2048], &quant_table_12_to_56[2048], &quant_table_12_to_58[2048],
 &quant_table_12_to_60[2048], &quant_table_12_to_62[2048], &quant_table_12_to_64[2048], &quant_table_12_to_72[2048],
 &quant_table_12_to_80[2048], &quant_table_12_to_88[2048], &quant_table_12_to_96[2048], &quant_table_12_to_104[2048],
 &quant_table_12_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0, &quant_table_14_to_16[2048], &quant_table_14_to_18[2048],
 &quant_table_14_to_20[2048], &quant_table_14_to_22[2048], &quant_table_14_to_24[2048], &quant_table_14_to_26[2048],
 &quant_table_14_to_28[2048], &quant_table_14_to_30[2048], &quant_table_14_to_32[2048], &quant_table_14_to_34[2048],
 &quant_table_14_to_36[2048], &quant_table_14_to_38[2048], &quant_table_14_to_40[2048], &quant_table_14_to_42[2048],
 &quant_table_14_to_44[2048], &quant_table_14_to_46[2048], &quant_table_14_to_48[2048], &quant_table_14_to_50[2048],
 &quant_table_14_to_52[2048], &quant_table_14_to_54[2048], &quant_table_14_to_56[2048], &quant_table_14_to_58[2048],
 &quant_table_14_to_60[2048], &quant_table_14_to_62[2048], &quant_table_14_to_64[2048], &quant_table_14_to_72[2048],
 &quant_table_14_to_80[2048], &quant_table_14_to_88[2048], &quant_table_14_to_96[2048], &quant_table_14_to_104[2048],
 &quant_table_14_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_16_to_18[2048],
 &quant_table_16_to_20[2048], &quant_table_16_to_22[2048], &quant_table_16_to_24[2048], &quant_table_16_to_26[2048],
 &quant_table_16_to_28[2048], &quant_table_16_to_30[2048], &quant_table_16_to_32[2048], &quant_table_16_to_34[2048],
 &quant_table_16_to_36[2048], &quant_table_16_to_38[2048], &quant_table_16_to_40[2048], &quant_table_16_to_42[2048],
 &quant_table_16_to_44[2048], &quant_table_16_to_46[2048], &quant_table_16_to_48[2048], &quant_table_16_to_50[2048],
 &quant_table_16_to_52[2048], &quant_table_16_to_54[2048], &quant_table_16_to_56[2048], &quant_table_16_to_58[2048],
 &quant_table_16_to_60[2048], &quant_table_16_to_62[2048], &quant_table_16_to_64[2048], &quant_table_16_to_72[2048],
 &quant_table_16_to_80[2048], &quant_table_16_to_88[2048], &quant_table_16_to_96[2048], &quant_table_16_to_104[2048],
 &quant_table_16_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_18_to_20[2048], &quant_table_18_to_22[2048], &quant_table_18_to_24[2048], &quant_table_18_to_26[2048],
 &quant_table_18_to_28[2048], &quant_table_18_to_30[2048], &quant_table_18_to_32[2048], &quant_table_18_to_34[2048],
 &quant_table_18_to_36[2048], &quant_table_18_to_38[2048], &quant_table_18_to_40[2048], &quant_table_18_to_42[2048],
 &quant_table_18_to_44[2048], &quant_table_18_to_46[2048], &quant_table_18_to_48[2048], &quant_table_18_to_50[2048],
 &quant_table_18_to_52[2048], &quant_table_18_to_54[2048], &quant_table_18_to_56[2048], &quant_table_18_to_58[2048],
 &quant_table_18_to_60[2048], &quant_table_18_to_62[2048], &quant_table_18_to_64[2048], &quant_table_18_to_72[2048],
 &quant_table_18_to_80[2048], &quant_table_18_to_88[2048], &quant_table_18_to_96[2048], &quant_table_18_to_104[2048],
 &quant_table_18_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_20_to_22[2048], &quant_table_20_to_24[2048], &quant_table_20_to_26[2048],
 &quant_table_20_to_28[2048], &quant_table_20_to_30[2048], &quant_table_20_to_32[2048], &quant_table_20_to_34[2048],
 &quant_table_20_to_36[2048], &quant_table_20_to_38[2048], &quant_table_20_to_40[2048], &quant_table_20_to_42[2048],
 &quant_table_20_to_44[2048], &quant_table_20_to_46[2048], &quant_table_20_to_48[2048], &quant_table_20_to_50[2048],
 &quant_table_20_to_52[2048], &quant_table_20_to_54[2048], &quant_table_20_to_56[2048], &quant_table_20_to_58[2048],
 &quant_table_20_to_60[2048], &quant_table_20_to_62[2048], &quant_table_20_to_64[2048], &quant_table_20_to_72[2048],
 &quant_table_20_to_80[2048], &quant_table_20_to_88[2048], &quant_table_20_to_96[2048], &quant_table_20_to_104[2048],
 &quant_table_20_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_22_to_24[2048], &quant_table_22_to_26[2048],
 &quant_table_22_to_28[2048], &quant_table_22_to_30[2048], &quant_table_22_to_32[2048], &quant_table_22_to_34[2048],
 &quant_table_22_to_36[2048], &quant_table_22_to_38[2048], &quant_table_22_to_40[2048], &quant_table_22_to_42[2048],
 &quant_table_22_to_44[2048], &quant_table_22_to_46[2048], &quant_table_22_to_48[2048], &quant_table_22_to_50[2048],
 &quant_table_22_to_52[2048], &quant_table_22_to_54[2048], &quant_table_22_to_56[2048], &quant_table_22_to_58[2048],
 &quant_table_22_to_60[2048], &quant_table_22_to_62[2048], &quant_table_22_to_64[2048], &quant_table_22_to_72[2048],
 &quant_table_22_to_80[2048], &quant_table_22_to_88[2048], &quant_table_22_to_96[2048], &quant_table_22_to_104[2048],
 &quant_table_22_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_24_to_26[2048],
 &quant_table_24_to_28[2048], &quant_table_24_to_30[2048], &quant_table_24_to_32[2048], &quant_table_24_to_34[2048],
 &quant_table_24_to_36[2048], &quant_table_24_to_38[2048], &quant_table_24_to_40[2048], &quant_table_24_to_42[2048],
 &quant_table_24_to_44[2048], &quant_table_24_to_46[2048], &quant_table_24_to_48[2048], &quant_table_24_to_50[2048],
 &quant_table_24_to_52[2048], &quant_table_24_to_54[2048], &quant_table_24_to_56[2048], &quant_table_24_to_58[2048],
 &quant_table_24_to_60[2048], &quant_table_24_to_62[2048], &quant_table_24_to_64[2048], &quant_table_24_to_72[2048],
 &quant_table_24_to_80[2048], &quant_table_24_to_88[2048], &quant_table_24_to_96[2048], &quant_table_24_to_104[2048],
 &quant_table_24_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_26_to_28[2048], &quant_table_26_to_30[2048], &quant_table_26_to_32[2048], &quant_table_26_to_34[2048],
 &quant_table_26_to_36[2048], &quant_table_26_to_38[2048], &quant_table_26_to_40[2048], &quant_table_26_to_42[2048],
 &quant_table_26_to_44[2048], &quant_table_26_to_46[2048], &quant_table_26_to_48[2048], &quant_table_26_to_50[2048],
 &quant_table_26_to_52[2048], &quant_table_26_to_54[2048], &quant_table_26_to_56[2048], &quant_table_26_to_58[2048],
 &quant_table_26_to_60[2048], &quant_table_26_to_62[2048], &quant_table_26_to_64[2048], &quant_table_26_to_72[2048],
 &quant_table_26_to_80[2048], &quant_table_26_to_88[2048], &quant_table_26_to_96[2048], &quant_table_26_to_104[2048],
 &quant_table_26_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_28_to_30[2048], &quant_table_28_to_32[2048], &quant_table_28_to_34[2048],
 &quant_table_28_to_36[2048], &quant_table_28_to_38[2048], &quant_table_28_to_40[2048], &quant_table_28_to_42[2048],
 &quant_table_28_to_44[2048], &quant_table_28_to_46[2048], &quant_table_28_to_48[2048], &quant_table_28_to_50[2048],
 &quant_table_28_to_52[2048], &quant_table_28_to_54[2048], &quant_table_28_to_56[2048], &quant_table_28_to_58[2048],
 &quant_table_28_to_60[2048], &quant_table_28_to_62[2048], &quant_table_28_to_64[2048], &quant_table_28_to_72[2048],
 &quant_table_28_to_80[2048], &quant_table_28_to_88[2048], &quant_table_28_to_96[2048], &quant_table_28_to_104[2048],
 &quant_table_28_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_30_to_32[2048], &quant_table_30_to_34[2048],
 &quant_table_30_to_36[2048], &quant_table_30_to_38[2048], &quant_table_30_to_40[2048], &quant_table_30_to_42[2048],
 &quant_table_30_to_44[2048], &quant_table_30_to_46[2048], &quant_table_30_to_48[2048], &quant_table_30_to_50[2048],
 &quant_table_30_to_52[2048], &quant_table_30_to_54[2048], &quant_table_30_to_56[2048], &quant_table_30_to_58[2048],
 &quant_table_30_to_60[2048], &quant_table_30_to_62[2048], &quant_table_30_to_64[2048], &quant_table_30_to_72[2048],
 &quant_table_30_to_80[2048], &quant_table_30_to_88[2048], &quant_table_30_to_96[2048], &quant_table_30_to_104[2048],
 &quant_table_30_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_32_to_34[2048],
 &quant_table_32_to_36[2048], &quant_table_32_to_38[2048], &quant_table_32_to_40[2048], &quant_table_32_to_42[2048],
 &quant_table_32_to_44[2048], &quant_table_32_to_46[2048], &quant_table_32_to_48[2048], &quant_table_32_to_50[2048],
 &quant_table_32_to_52[2048], &quant_table_32_to_54[2048], &quant_table_32_to_56[2048], &quant_table_32_to_58[2048],
 &quant_table_32_to_60[2048], &quant_table_32_to_62[2048], &quant_table_32_to_64[2048], &quant_table_32_to_72[2048],
 &quant_table_32_to_80[2048], &quant_table_32_to_88[2048], &quant_table_32_to_96[2048], &quant_table_32_to_104[2048],
 &quant_table_32_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_34_to_36[2048], &quant_table_34_to_38[2048], &quant_table_34_to_40[2048], &quant_table_34_to_42[2048],
 &quant_table_34_to_44[2048], &quant_table_34_to_46[2048], &quant_table_34_to_48[2048], &quant_table_34_to_50[2048],
 &quant_table_34_to_52[2048], &quant_table_34_to_54[2048], &quant_table_34_to_56[2048], &quant_table_34_to_58[2048],
 &quant_table_34_to_60[2048], &quant_table_34_to_62[2048], &quant_table_34_to_64[2048], &quant_table_34_to_72[2048],
 &quant_table_34_to_80[2048], &quant_table_34_to_88[2048], &quant_table_34_to_96[2048], &quant_table_34_to_104[2048],
 &quant_table_34_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_36_to_38[2048], &quant_table_36_to_40[2048], &quant_table_36_to_42[2048],
 &quant_table_36_to_44[2048], &quant_table_36_to_46[2048], &quant_table_36_to_48[2048], &quant_table_36_to_50[2048],
 &quant_table_36_to_52[2048], &quant_table_36_to_54[2048], &quant_table_36_to_56[2048], &quant_table_36_to_58[2048],
 &quant_table_36_to_60[2048], &quant_table_36_to_62[2048], &quant_table_36_to_64[2048], &quant_table_36_to_72[2048],
 &quant_table_36_to_80[2048], &quant_table_36_to_88[2048], &quant_table_36_to_96[2048], &quant_table_36_to_104[2048],
 &quant_table_36_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_38_to_40[2048], &quant_table_38_to_42[2048],
 &quant_table_38_to_44[2048], &quant_table_38_to_46[2048], &quant_table_38_to_48[2048], &quant_table_38_to_50[2048],
 &quant_table_38_to_52[2048], &quant_table_38_to_54[2048], &quant_table_38_to_56[2048], &quant_table_38_to_58[2048],
 &quant_table_38_to_60[2048], &quant_table_38_to_62[2048], &quant_table_38_to_64[2048], &quant_table_38_to_72[2048],
 &quant_table_38_to_80[2048], &quant_table_38_to_88[2048], &quant_table_38_to_96[2048], &quant_table_38_to_104[2048],
 &quant_table_38_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_40_to_42[2048],
 &quant_table_40_to_44[2048], &quant_table_40_to_46[2048], &quant_table_40_to_48[2048], &quant_table_40_to_50[2048],
 &quant_table_40_to_52[2048], &quant_table_40_to_54[2048], &quant_table_40_to_56[2048], &quant_table_40_to_58[2048],
 &quant_table_40_to_60[2048], &quant_table_40_to_62[2048], &quant_table_40_to_64[2048], &quant_table_40_to_72[2048],
 &quant_table_40_to_80[2048], &quant_table_40_to_88[2048], &quant_table_40_to_96[2048], &quant_table_40_to_104[2048],
 &quant_table_40_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_42_to_44[2048], &quant_table_42_to_46[2048], &quant_table_42_to_48[2048], &quant_table_42_to_50[2048],
 &quant_table_42_to_52[2048], &quant_table_42_to_54[2048], &quant_table_42_to_56[2048], &quant_table_42_to_58[2048],
 &quant_table_42_to_60[2048], &quant_table_42_to_62[2048], &quant_table_42_to_64[2048], &quant_table_42_to_72[2048],
 &quant_table_42_to_80[2048], &quant_table_42_to_88[2048], &quant_table_42_to_96[2048], &quant_table_42_to_104[2048],
 &quant_table_42_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_44_to_46[2048], &quant_table_44_to_48[2048], &quant_table_44_to_50[2048],
 &quant_table_44_to_52[2048], &quant_table_44_to_54[2048], &quant_table_44_to_56[2048], &quant_table_44_to_58[2048],
 &quant_table_44_to_60[2048], &quant_table_44_to_62[2048], &quant_table_44_to_64[2048], &quant_table_44_to_72[2048],
 &quant_table_44_to_80[2048], &quant_table_44_to_88[2048], &quant_table_44_to_96[2048], &quant_table_44_to_104[2048],
 &quant_table_44_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_46_to_48[2048], &quant_table_46_to_50[2048],
 &quant_table_46_to_52[2048], &quant_table_46_to_54[2048], &quant_table_46_to_56[2048], &quant_table_46_to_58[2048],
 &quant_table_46_to_60[2048], &quant_table_46_to_62[2048], &quant_table_46_to_64[2048], &quant_table_46_to_72[2048],
 &quant_table_46_to_80[2048], &quant_table_46_to_88[2048], &quant_table_46_to_96[2048], &quant_table_46_to_104[2048],
 &quant_table_46_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_48_to_50[2048],
 &quant_table_48_to_52[2048], &quant_table_48_to_54[2048], &quant_table_48_to_56[2048], &quant_table_48_to_58[2048],
 &quant_table_48_to_60[2048], &quant_table_48_to_62[2048], &quant_table_48_to_64[2048], &quant_table_48_to_72[2048],
 &quant_table_48_to_80[2048], &quant_table_48_to_88[2048], &quant_table_48_to_96[2048], &quant_table_48_to_104[2048],
 &quant_table_48_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_50_to_52[2048], &quant_table_50_to_54[2048], &quant_table_50_to_56[2048], &quant_table_50_to_58[2048],
 &quant_table_50_to_60[2048], &quant_table_50_to_62[2048], &quant_table_50_to_64[2048], &quant_table_50_to_72[2048],
 &quant_table_50_to_80[2048], &quant_table_50_to_88[2048], &quant_table_50_to_96[2048], &quant_table_50_to_104[2048],
 &quant_table_50_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_52_to_54[2048], &quant_table_52_to_56[2048], &quant_table_52_to_58[2048],
 &quant_table_52_to_60[2048], &quant_table_52_to_62[2048], &quant_table_52_to_64[2048], &quant_table_52_to_72[2048],
 &quant_table_52_to_80[2048], &quant_table_52_to_88[2048], &quant_table_52_to_96[2048], &quant_table_52_to_104[2048],
 &quant_table_52_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_54_to_56[2048], &quant_table_54_to_58[2048],
 &quant_table_54_to_60[2048], &quant_table_54_to_62[2048], &quant_table_54_to_64[2048], &quant_table_54_to_72[2048],
 &quant_table_54_to_80[2048], &quant_table_54_to_88[2048], &quant_table_54_to_96[2048], &quant_table_54_to_104[2048],
 &quant_table_54_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_56_to_58[2048],
 &quant_table_56_to_60[2048], &quant_table_56_to_62[2048], &quant_table_56_to_64[2048], &quant_table_56_to_72[2048],
 &quant_table_56_to_80[2048], &quant_table_56_to_88[2048], &quant_table_56_to_96[2048], &quant_table_56_to_104[2048],
 &quant_table_56_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_58_to_60[2048], &quant_table_58_to_62[2048], &quant_table_58_to_64[2048], &quant_table_58_to_72[2048],
 &quant_table_58_to_80[2048], &quant_table_58_to_88[2048], &quant_table_58_to_96[2048], &quant_table_58_to_104[2048],
 &quant_table_58_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_60_to_62[2048], &quant_table_60_to_64[2048], &quant_table_60_to_72[2048],
 &quant_table_60_to_80[2048], &quant_table_60_to_88[2048], &quant_table_60_to_96[2048], &quant_table_60_to_104[2048],
 &quant_table_60_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_62_to_64[2048], &quant_table_62_to_72[2048],
 &quant_table_62_to_80[2048], &quant_table_62_to_88[2048], &quant_table_62_to_96[2048], &quant_table_62_to_104[2048],
 &quant_table_62_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_64_to_72[2048],
 &quant_table_64_to_80[2048], &quant_table_64_to_88[2048], &quant_table_64_to_96[2048], &quant_table_64_to_104[2048],
 &quant_table_64_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_72_to_80[2048], &quant_table_72_to_88[2048], &quant_table_72_to_96[2048], &quant_table_72_to_104[2048],
 &quant_table_72_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_80_to_88[2048], &quant_table_80_to_96[2048], &quant_table_80_to_104[2048],
 &quant_table_80_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_88_to_96[2048], &quant_table_88_to_104[2048],
 &quant_table_88_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_96_to_104[2048],
 &quant_table_96_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, &quant_table_104_to_112[2048]},
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

