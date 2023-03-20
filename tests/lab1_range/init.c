void range_1_check(void) {
	printk("%s%R%s", "This is a testcase: ", 2023, 2023, "\n");
	printk("the range is %R, size = %d\n", 1, 9, 9 - 1);
	printk("Test minus: (-2.-3) = %R\n",-2,-3);
	printk("Test winth: ( 2, 3) = %2R\n",2,3);
	printk("Test flag 0: (02,04) = %02R\n",2,4);
	printk("Test flag -: (2 ,5 ) = %-2R\n",2,5);
}

void mips_init() {
	range_1_check();
	halt();
}
