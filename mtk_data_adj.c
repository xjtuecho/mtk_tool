//
// MT6589 DATA
//
#include <stdio.h>

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
}


int main()
{
	unsigned long A,B,C,D,E;
	static unsigned char ebr1_buf[512];
	static unsigned char ebr2_buf[512];

	read_mbr("EBR1", ebr1_buf);
	read_mbr("EBR2", ebr2_buf);

#if 1
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

	printf("D = (A+B)-C = 0x%08X\n", (A+B)-C);
	printf("E = 0xFFFFFFFF-(A+B) = 0x%08X\n", 0xFFFFFFFF-(A+B));
	printf("A+B = 0x%08X\n", A+B);
	printf("The data of your phone is %dMB\n", B>>(20-9));


	//WRITE_DWORD(ebr1_buf, 4, 0xAABBCCDD);
	//write_mbr("EBR1_new", ebr1_buf);


	return 0;
}

