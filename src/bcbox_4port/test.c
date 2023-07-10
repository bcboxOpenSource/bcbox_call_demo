#include <stdio.h>
#include "rsa.h"
#include <string.h>
#include <stdlib.h>

#if 0  //先不加密
int main(void) {
    int len;
	int pk_e,pk_n,sk_d,sk_n,pk_byte;
    int *encoded, *decoded;
    char buffer[64];
	//MakeKey(&pk_e,&pk_n,&pk_byte,&sk_d,&sk_n);
	memset(buffer,0xaa,64);
    sprintf(buffer,"%s","123456781234567812345678");
    len=strlen(buffer);
    printf("加密得密文为：\r\n");
    encoded = encodeMessage(len, pk_byte, buffer, pk_e, pk_n);
    printf("\r\n编码成功完成 ... \r\n");

    printf("正在解码编码的信息 ... \r\n");
    printf("解码得明文为：\r\n");
    decoded = decodeMessage(len/pk_byte, pk_byte, encoded, sk_d, sk_n);
    free(encoded);
    free(decoded);
    return 0;
}
#endif 