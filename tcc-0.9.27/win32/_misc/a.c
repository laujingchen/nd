#include <_c.h>
#include <_c2.h>
#include <stdio.h>

#def TEST "hello"
#if !defd TEST
	#err TEST is undefined
#endif

priv Void print(Void)
{
	type ro SChar String^;
	ro String s = "world";
	//s = "test";
	ro UA align = alignof(long long);
	printf(TEST " %s %zu\n", s, align);
}

priv Void cpMem(Void dest^, ro Void src^, UA nCopy)^
{
	UB dest2^ = dest;
	ro UB src2^ = src;
	for UA i = 0; i < nCopy; i++ {
		dest2^ = src2^;
		dest2++;
		src2++;
	}
	ret dest2;
}

priv ro B32 viewVec3(B32 x, B32 y, B32 z)^[3]
{
	as(x, Void);
	as(y, Void);
	as(z, Void);
	priv ro B32 vec3[3] = { 3.12, 1.24, 9.91 };
	return vec3@;
}

ro SW grodata = 30;

SW main(SW argc, SChar argv^^)
{
	puts("No more parentheses in statement headers");
	switch 1 {
	case 0:
		1 + 1;
		SW i;
	case 1:
		;
	}
	if (1 && 1) == 1 {
		printf("%d\n", 1);
	}
	while 1 {
		printf("hi\n");
		break;
	}
	for SW i = 0; i < 5; i++ {
		printf(" %d", i);
	}
	putchar('\n');
	SW i = 1;
	do {
		printf(" %d", i);
		i++;
	} while i <= 2;
	putchar('\n');
	putchar('\n');

	puts("ro and ++");
	i = 1;
	printf("%d\n", i++);
	printf("%d\n", i);
	ro B64 dub = 70.70;
	printf("%lf\n", dub);
	putchar('\n');

	puts("Array size must be a constant");
	i = 5;
	SW arr[5];
	printf("%zu\n", sizeof(arr));
	putchar('\n');

	puts("#prag");
	typedef struct {
		SChar c;
		SW i;
	} NotPacked;
#prag pack(1)
	typedef struct {
		SChar c;
		SW i;
	} Packed;
	printf("%zu %zu\n", sizeof(NotPacked), sizeof(Packed));
	putchar('\n');

	puts("Operator precedence");
	printf("%d\n", 1 << 5 * 3);
	printf("%d\n", (1 << 5) * 3);
	printf("%d\n", 1 << (5 * 3));
	UW a = 0x10;
	UW mask = 0x100;
	UW b = 0x10 | 0x100;
	printf("a | mask == b%8d\n", a | mask == b);
	printf("(a | mask) == b%8d\n", (a | mask) == b);
	printf("a | (mask == b)%8d\n", a | (mask == b));

	puts("XOR assignment does nothing");
	UW xor1 = 0x1;
	UW xor2 = 0x1;
	printf("%u\n", xor1 ` xor2);
	printf("%u\n", xor1);
	xor1 `= xor2;
	printf("%u\n", xor1);
	puts("AND assignment");
	UW and1 = 0x1;
	UW and2 = 0x0;
	printf("%u\n", and1 & and2);
	printf("%u\n", and1);
	and1 &= and2;
	printf("%u\n", and1);
	putchar('\n');

	puts("Postfix address-of");
	SW integer = 9575;
	SW ptr^ = integer@;
	typedef struct { SW i, j; } St;
	St st = { 45, 53 };
	St pst^ = st@;
	printf("%d %p %p\n", pst^.i, pst^.j@, pst^.j@);
	Void func^(Void) = print;
	func = print@;
	printf("%p %p %p\n", func, func^, print);
	func();
	putchar('\n');

	puts("Test address arithmetic");
	SW array[3] = { 1, 2, 3 };
	SW pa^ = array;
	printf("%p %d\n", pa, pa^);
	pa++;
	printf("%p %d\n", pa, pa^);
	pa++;
	printf("%p %d\n", pa, pa^);
	putchar('\n');

	puts("Compatiblity with standard header files");
	assert(1);
	typedef struct {
		SW i;
		SChar c;
	} OffsetTest;
	// `offsetof` no longer works, because certain operators have been changed
//#define offsetof(T, field) ((size_t)((T *)0)^.field@)
	//printf("%zu\n", offsetof(OffsetTest, c));
	// `alloca` no longer works, probably because we got rid of VLAs
	SW stackArr^ = alloca(5);
	for SW i = 0; i < 5; i++ {
		stackArr[0] = i;
	}
	for SW i = 0; i < 5; i++ {
		printf(" %d", stackArr[i]);
	}
	putchar('\n');
	for SW i = 0; i < 5; i++ {
		printf(" %d", i);
	}
	putchar('\n');
	putchar('\n');

	puts("The return of ?:");
#define STATICASRT(expr, msg) typedef SW _StaticAsrt_##msg[(expr) ? 1 : -1]
	STATICASRT(sizeof(SW) == 4,);
	ro B32 pi = 3.14;
	ro SW tern = pi == 3.14 ? 1
		: pi == 3.142 ? 2
		: pi == 3E0 ? 3
		: 0;
	printf("%f %d\n", pi, tern);
	putchar('\n');

	puts("Numerical literals");
	printf("bin: %d\n", 0b1001);
	printf("oct: %d\n", 0o11);
	printf("hex: %d\n", 0xF);
	printf("flt: %f\n", 1.5f);
	printf("exp: %f\n", 3E0);
	printf("llu: %llu\n", 0xDEADBEEF12345678);
	// OK: old octal prefix still works in string escape sequences
	puts("\02");
	putchar('\n');

	puts("cpMem test");
	typeof("Before copying") dest = "Before copying";
	puts(dest);
	SChar src[] = "After copying";
	puts(src);
	UA nCopied = cpMem(dest, src, _CNT(src)) - dest;
	puts(dest);
	printf("%zu\n", nCopied);
	putchar('\n');

	puts("_c2.h test");
	printf("%zu\n", _CNT(dest));
#def GETOFF(Struct, member) as(as(0, Struct ^)^.member@, UA)
	printf("%zu %zu\n", _GETOFF(OffsetTest, c), GETOFF(OffsetTest, c));
	putchar('\n');
	_ASRT(!NIL);
	//SW oldFunc()[];
	//Void staticArrParam(SW i[restrict 1]);
	//SW *__restrict__ irest;

	puts("New decls");
	SW newarr[3] = { 1, 2, 3 },
		newptr^ = newarr + 2,
		newptrptr^^ = newptr@,
		newptrarr^[3] = newarr@;
	printf("%d\n", newptr^);
	newptrptr^ = newarr + 0;
	printf("%d %d\n", newptr^, newptrptr^^);
	printf("%p %d %d %d\n", newptrarr, newptrarr^[0], newptrarr^[1], newptrarr^[2]);
	SChar hello[] = "Hello";
	// -1 to exclude NUL
	cpMem(dest, hello, _CNT(hello) - 1);
	puts(dest);
	typeof(viewVec3(0, 0, 0)) vec3View = viewVec3(0, 0, 0);
	printf("%f %f %f\n", vec3View^[0], vec3View^[1], vec3View^[2]);
	putchar('\n');
// test:
// 	jump test;
	ret 0;
}
