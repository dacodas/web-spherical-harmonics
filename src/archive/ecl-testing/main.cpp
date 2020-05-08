#include <string>
#include <string_view>
#include <iostream>

#include <ecl/ecl.h>

int main(int argc, char* argv[])
{
	cl_object result, form;
	cl_type type;

	char* arguments[] = {"ecl"};
	cl_boot(1, arguments);

	const char* program {
R"foo((progn 
	(maxima::batch "/mnt/gentoo-home/dacoda/projects/web-spherical-harmonics/src/maxima/rotation.maxima")
	(maxima::displa maxima::|$k|)
	(format t "~A~%" maxima::|$k|))
)foo"
	};
	cl_object error_symbol = ecl_make_symbol("ERROR", "CL");

	form = ecl_read_from_cstring("(require 'maxima)");
	result = si_safe_eval(2, form, ECL_NIL);

	form = ecl_read_from_cstring(program);
	cl_env_ptr env = ecl_process_env();
	ECL_HANDLER_CASE_BEGIN(env, ecl_list1(error_symbol)) {
		result = si_safe_eval(2, form, ECL_NIL);
	}
	ECL_HANDLER_CASE(1, condition) {
		result = condition; 
	} ECL_HANDLER_CASE_END;

	form = ecl_read_from_cstring("t");
	result = si_safe_eval(2, form, ECL_NIL);
	type = ecl_t_of(result);

	return 1;
}
