
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <math.h>
#include "../Includes/light_task.h"


void test_all_reg_rd_wr(void** state)
{
	int fd = light_init();
	int status;
	assert_int_not_equal(fd, -1);
	status = all_reg_rd_wr(fd);
        assert_int_equal(status, 0);

}


void test_control_reg_int(void **state)
{

	int fd = light_init();
	int status;
	assert_int_not_equal(fd, -1);
	status = control_reg_int_wr(fd, 0x10);
	assert_int_not_equal(status, -1);
	status = control_reg_int_rd(fd);
	assert_int_not_equal(status, -1);
}



void test_get_lux(void **state){
	int fd = light_init();
	int status;
	assert_int_not_equal(fd, -1);
	status = get_lux();
	assert_int_not_equal(status, -1);
}


int main(void)
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_all_reg_rd_wr),
        cmocka_unit_test(test_control_reg_int),
        cmocka_unit_test(test_get_lux)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
