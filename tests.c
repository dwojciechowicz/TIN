//tests.c
//Autorzy: Magdalena Zych
//	   Krzysztof Gorlach
//Data: 24.05.2020

#include "tests.h"

// funkcje testowe
void test_check_arg()
{
	char* tab1[] = {"control_mod.c", "para", "1", "1", "10"};
	char* tab2[] = {"control_mod.c", "para"};
	char* tab3[] = {"control_mod.c", "para", "a", "1", "10"};
	char* tab4[] = {"control_mod.c", "para", "1", "a", "10"};
	char* tab5[] = {"control_mod.c", "para", "1", "1", "a"};
	char* tab6[] = {"control_mod.c", "para", "3", "1", "10"};
	char* tab7[] = {"control_mod.c", "para", "1", "70", "10"};
	char* tab8[] = {"control_mod.c", "para", "1", "1", "150"};
	CU_ASSERT_TRUE(check_arguments(5, tab1));
	CU_ASSERT_FALSE(check_arguments(5, tab2));
	CU_ASSERT_FALSE(check_arguments(5, tab3));
	CU_ASSERT_FALSE(check_arguments(5, tab4));
	CU_ASSERT_FALSE(check_arguments(5, tab5));
	CU_ASSERT_FALSE(check_arguments(5, tab6));
	CU_ASSERT_FALSE(check_arguments(5, tab7));
	CU_ASSERT_FALSE(check_arguments(5, tab8));
}

void test_identify_action()
{
	int action_type = -1;
	char* tab1[] = {"control_mod.c", "stop"};
	identify_action(2, tab1, &action_type);
	CU_ASSERT_EQUAL(action_type, 0);

	action_type = -1;
	char* tab2[] = {"control_mod.c", "diag"};
	identify_action(2, tab2, &action_type);
	CU_ASSERT_EQUAL(action_type, 1);

	action_type = -1;
	char* tab3[] = {"control_mod.c", "para", "1", "1", "10"};
	identify_action(2, tab3, &action_type);
	CU_ASSERT_EQUAL(action_type, 2);

	action_type = -1;
	char* tab4[] = {"control_mod.c", "blad"};
	identify_action(2, tab4, &action_type);
	CU_ASSERT_NOT_EQUAL(action_type, 0);
	CU_ASSERT_NOT_EQUAL(action_type, 1);
	CU_ASSERT_NOT_EQUAL(action_type, 2);
}

void test_load_params()
{
	char str[15] = "";
	char* tab1[] = {"control_mod.c", "para", "1", "1", "10"};
	load_params(str, tab1);
	CU_ASSERT_STRING_EQUAL(str, "para 1 1 10");
}

void test_check_param_com()
{
	CU_ASSERT_TRUE(check_param_communication("para 1 1 10"));
	CU_ASSERT_FALSE(check_param_communication("para1 1 10"));
	CU_ASSERT_FALSE(check_param_communication("para"));
	CU_ASSERT_FALSE(check_param_communication("abcpara"));
}

void test_get_server_parameters()
{
	char server_ip[20];
	int server_port;
	CU_ASSERT_TRUE(get_server_parameters(server_ip, &server_port, 1));
	CU_ASSERT_FALSE(get_server_parameters(server_ip, &server_port, 4));
	CU_ASSERT_FALSE(get_server_parameters(server_ip, &server_port, -8));
}

void testuj()
{
	 // inicjalizacja rejestru testow
	CU_initialize_registry();
	
	 // dodanie grupy testow
	CU_pSuite pSuite = CU_add_suite("MySuite", NULL, NULL);
	
	// dodanie testow do grupy
	CU_add_test(pSuite, "test_check_arg", test_check_arg);
	CU_add_test(pSuite, "test_identify_action", test_identify_action);
	CU_add_test(pSuite, "test_load_params", test_load_params);
	CU_add_test(pSuite, "test_check_param_com", test_check_param_com);
	CU_add_test(pSuite, "test_get_server_parameters", test_get_server_parameters);
	
	// uruchomienie wszystkich testow
	CU_basic_run_tests();
	
	// czyszczenie rejestru
	CU_cleanup_registry();	
}
