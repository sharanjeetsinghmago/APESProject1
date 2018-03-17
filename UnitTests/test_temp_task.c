
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <math.h>
#include "../Includes/temp_task.h"



void test_all_temprg_rd_wr(void** state)
{
	int fd = temp_init();
	int status;
	assert_int_not_equal(fd, -1);
	status = all_temprg_rd_wr();
        assert_int_equal(status, 0);

}


void test_tlowcheck(void **state)
{

	int fd = temp_init();
	int status;
	assert_int_not_equal(fd, -1);
	status = write_tlow_reg(0x02, 45);
	assert_int_not_equal(status, -1);
	status = read_tlow_reg(0x02);
	assert_int_not_equal(status, -1);
}



void test_read_temp_data_reg(void **state)
{
	int fd = temp_init();
	int status;
	assert_int_not_equal(fd, -1);
	status = read_temp_data_reg(0);
	assert_int_not_equal(status, -300);
}


int main(void)
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_all_temprg_rd_wr),
        cmocka_unit_test(test_tlowcheck),
        cmocka_unit_test(test_read_temp_data_reg)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
