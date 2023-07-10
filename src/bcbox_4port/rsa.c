#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "rsa.h"


/**
 * Computes a^b mod c，计算a^b mod c
 */
int modpow(long long a, long long b, int c) {
    int res = 1;
    while(b > 0) {
        /* Need long multiplication else this will overflow...
         必须使用长乘法，否则这将溢出*/
        if(b & 1) {
            res = (res * a) % c;
        }
        b = b >> 1;
        a = (a * a) % c; /* Same deal here */
    }
    return res;
}

/**
 * Encode the message m using public exponent and modulus, c = m^e mod n使用公共指数和模量对消息m进行编码，c = m^e Mod n
 */
int encode(int m, int e, int n) {
    return modpow(m, e, n);
}

/**
 * Decode cryptogram c using private exponent and public modulus, m = c^d mod n，用私有指数和公共模量解码密码c，m = c^d Mod n
 */
int decode(int c, int d, int n) {
    return modpow(c, d, n);
}

/**
 * Encode the message of given length, using the public key (exponent, modulus)
 * The resulting array will be of size len/bytes, each index being the encryption
 * of "bytes" consecutive characters, given by m = (m1 + m2*128 + m3*128^2 + ..),
 * encoded = m^exponent mod modulus
 * 使用公钥（指数、模数)对给定长度的消息进行编码）
 得到的数组将是大小为len/字节，每个索引是由m=(m1m2*128m3*128^2.)给出的“字节”连续字符的加密，编码=m^指数mod模量
 */
int* encodeMessage(int len, int bytes, char* message, int exponent, int modulus) {
    int *encoded = (int *)malloc((len/bytes) * sizeof(int));
    int x, i, j;
    for(i = 0; i < len; i += bytes) {
        x = 0;
        for(j = 0; j < bytes; j++) x += message[i + j] * (1 << (7 * j));
        encoded[i/bytes] = encode(x, exponent, modulus);
#ifndef MEASURE
        printf("%d ", encoded[i/bytes]);
#endif
    }
    return encoded;
}

/**
 * Decode the cryptogram of given length, using the private key (exponent, modulus)
 * Each encrypted packet should represent "bytes" characters as per encodeMessage.
 * The returned message will be of size len * bytes.
 * 使用私钥（指数、模数)解码给定长度的密码）
 每个加密的数据包应该按照编码消息表示“字节”字符。
 返回的消息大小为len*字节。
 */
int* decodeMessage(int len, int bytes, int* cryptogram, int exponent, int modulus) {
    int *decoded = (int *)malloc(len * bytes * sizeof(int));
    int x, i, j;
    for(i = 0; i < len; i++) {
        x = decode(cryptogram[i], exponent, modulus);
        for(j = 0; j < bytes; j++) {
            decoded[i*bytes + j] = (x >> (7 * j)) % 128;
#ifndef MEASURE
            if(decoded[i*bytes + j] != '\0') printf("%c", decoded[i*bytes + j]);
#endif
        }
    }
    return decoded;
}


