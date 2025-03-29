#ifndef RTT_TEST_HH
#define RTT_TEST_HH

void TestRTTConfig();
void TestRTTOutput();

bool check_file_content(const char* filename, int _col, int _lin, double _sum = 0, int _idx_col = -1);

#endif