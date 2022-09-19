

/* Specify the arguments */
struct input{
	int a;
	int b;
	string exec<>;
};


program PART_B_PROG{
	version PART_B_VERS{
		/* Takes a numbers structure and gives the integer result. */
		string blackbox(input)=1;
	}=1;
}=0x12345678;