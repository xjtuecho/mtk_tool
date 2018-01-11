//
// MT6589 DATA adjustment.
//    Echo <echo.xjtu@gmail.com> 2013.10.08
//

#include <stdio.h>
#include <stdint.h>
#include <conio.h>

#define MAKE_DWORD(buf,idx)		((((unsigned long)buf[idx+3])<<24)|(((unsigned long)buf[idx+2])<<16)|(((unsigned long)buf[idx+1])<<8)|(((unsigned long)buf[idx+0])&0xFF))
#define WRITE_DWORD(buf,idx,dw)	{buf[idx+3]=((dw>>24)&0xFF);buf[idx+2]=((dw>>16)&0xFF);buf[idx+1]=((dw>>8)&0xFF);buf[idx+0]=((dw>>0)&0xFF);}

void show_buf(unsigned char *buf)
{
	int i;
	printf("    ");
	for(i=0;i<16;i++)
	{
		printf("%02X ", i);
	}

	printf("\n");

	for(i=0;i<512;i++)
	{
		if(!(i%16))
		{
			printf("\n%02X  ", i>>4);
		}
		printf("%02X ", buf[i]);
	}
}

int read_mbr(const char *name, unsigned char *buf)
{
	FILE *fp=NULL;
	int ret;

	if (!(fp = fopen (name, "rb")))
	{
		printf("Error: Open read file fail\n");
		return -1;
	}

	if(512 != (ret = fread(buf, 1, 512, fp)))
	{
		printf("Error: reading file fail, ret=%d\n",  ret);
		return -1;
	}

	if(fp)
	{
		fclose(fp);
	}

	return 0;
}

int write_mbr(const char *name, unsigned char *buf)
{
	FILE *fp=NULL;
	int ret;

	if (!(fp = fopen (name, "wb")))
	{
		printf("Error: Open write file fail\n");
		return -1;
	}

	if(512 != (ret = fwrite(buf, 1, 512, fp)))
	{
		printf("Error: reading file fail, ret=%d\n",  ret);
		return -1;
	}

	if(fp)
	{
		fclose(fp);
	}

	return 0;
}

#define USRDATA 0x34f80000

int main()
{


	uint32_t A,B,C,D,E, a,b,c,d,e, S = 0;
	uint64_t addr;

	static unsigned char ebr1_buf[512];
	static unsigned char ebr2_buf[512];

	unsigned char ebr1_name[64];
	unsigned char ebr2_name[64];

	if(read_mbr("EBR1", ebr1_buf))
	{
		return -1;
	}
	if(read_mbr("EBR2", ebr2_buf))
	{
		return -1;
	}

	printf("MTK EBR1 and EBR2 adjustment tool. V1.0\n"
			"     by Echo <echo.xjtu@gmail.com>\n\n", B>>(20-9));

#if 0
	printf("this is ebr1...\n");
	show_buf(ebr1_buf);
	printf("\nebr1 done...\n\n");
	printf("this is ebr2...\n");
	show_buf(ebr2_buf);
	printf("\nebr2 done...\n\n");
#endif

	A = MAKE_DWORD(ebr1_buf, 0x1E6);
	B = MAKE_DWORD(ebr1_buf, 0x1EA);
	C = MAKE_DWORD(ebr1_buf, 0x1F6);
	D = MAKE_DWORD(ebr2_buf, 0x1C6);
	E = MAKE_DWORD(ebr2_buf, 0x1CA);

	printf(	"A = 0x1E6@EBR1 = 0x%08X\n"
			"B = 0x1EA@EBR1 = 0x%08X\n"
			"C = 0x1F6@EBR1 = 0x%08X\n"
			"D = 0x1C6@EBR2 = 0x%08X\n"
			"E = 0x1CA@EBR2 = 0x%08X\n",
			A, B, C, D, E);

	//printf("D = (A+B)-C = 0x%08X\n", (A+B)-C);
	//printf("E = 0xFFFFFFFF-(A+B) = 0x%08X\n", 0xFFFFFFFF-(A+B));
	//printf("A+B = 0x%08X\n", A+B);
	printf("The data of your phone is %dMB\n", B>>(20-9));
	printf("The USRDATA address is 0x%08X\n", USRDATA);
	addr = (uint64_t)USRDATA + ((uint64_t)B<<9);
	printf("The FAT address is 0x%08X", addr>>32);
	printf("%08X\n", addr&0xFFFFFFFF);

	// New data size;
	printf("\n\nplease input new data size (MB):");
	scanf("%d", &S);
	printf("%d\n", S);

	a = A;				// data start sector address
	b = S<<(20-9);			// new data sector size
	c = C;
	d = (a + b) - c;
	e = 0xFFFFFFFF - (a + b);

	printf(	"A = 0x1E6@EBR1 = 0x%08X\n"
			"B = 0x1EA@EBR1 = 0x%08X\n"
			"C = 0x1F6@EBR1 = 0x%08X\n"
			"D = 0x1C6@EBR2 = 0x%08X\n"
			"E = 0x1CA@EBR2 = 0x%08X\n",
			a, b, c, d, e);

	snprintf(ebr1_name, 60, "EBR1_%dMB", S);
	snprintf(ebr2_name, 60, "EBR2_%dMB", S);

	WRITE_DWORD(ebr1_buf, 0x1E6, a);
	WRITE_DWORD(ebr1_buf, 0x1EA, b);
	WRITE_DWORD(ebr1_buf, 0x1F6, c);
	WRITE_DWORD(ebr2_buf, 0x1C6, d);
	WRITE_DWORD(ebr2_buf, 0x1CA, e);

	printf("\n\nwriting %s and %s...\n", ebr1_name, ebr2_name);
	write_mbr(ebr1_name, ebr1_buf);
	write_mbr(ebr2_name, ebr2_buf);

	// verify new data size
	printf("\nreading %s and %s...\n", ebr1_name, ebr2_name);
	if(read_mbr(ebr1_name, ebr1_buf))
	{
		return -1;
	}
	if(read_mbr(ebr2_name, ebr2_buf))
	{
		return -1;
	}
	A = MAKE_DWORD(ebr1_buf, 0x1E6);
	B = MAKE_DWORD(ebr1_buf, 0x1EA);
	C = MAKE_DWORD(ebr1_buf, 0x1F6);
	D = MAKE_DWORD(ebr2_buf, 0x1C6);
	E = MAKE_DWORD(ebr2_buf, 0x1CA);

	printf(	"A = 0x1E6@EBR1 = 0x%08X\n"
			"B = 0x1EA@EBR1 = 0x%08X\n"
			"C = 0x1F6@EBR1 = 0x%08X\n"
			"D = 0x1C6@EBR2 = 0x%08X\n"
			"E = 0x1CA@EBR2 = 0x%08X\n",
			A, B, C, D, E);
	//printf("D = (A+B)-C = 0x%08X\n", (A+B)-C);
	//printf("E = 0xFFFFFFFF-(A+B) = 0x%08X\n", 0xFFFFFFFF-(A+B));
	//printf("A+B = 0x%08X\n", A+B);
	printf("The data of your phone is %dMB\n", B>>(20-9));
	printf("The USRDATA address is 0x%08X\n", USRDATA);
	addr = (uint64_t)USRDATA + ((uint64_t)B<<9);
	printf("The FAT address is 0x%08X", addr>>32);
	printf("%08X\n", addr&0xFFFFFFFF);

	getch();

	return 0;
}

