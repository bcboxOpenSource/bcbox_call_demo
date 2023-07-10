#ifndef __RSA_H__
#define __RSA_H__
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Computes a^b mod c，计算a^b mod c
 */
int modpow(long long a, long long b, int c) ;
/**
 * Find a random exponent x between 3 and n - 1 such that gcd(x, phi) = 1,在3和n-1之间找到随机指数x，使得gcd(x,phi)=1
 * this distribution is similarly nowhere near uniform,这种分布同样不接近制服
 */
int randExponent(int phi, int n);

/**
 * Find a random (probable) prime between 3 and n - 1在3和（n-1）之间找一个随机素数, this distribution is，
 * nowhere near uniform, see prime gaps
 */
int randPrime(int n) ;
/**
 * Encode the message of given length, using the public key (exponent, modulus)
 * The resulting array will be of size len/bytes, each index being the encryption
 * of "bytes" consecutive characters, given by m = (m1 + m2*128 + m3*128^2 + ..),
 * encoded = m^exponent mod modulus
 * 使用公钥（指数、模数)对给定长度的消息进行编码）
 得到的数组将是大小为len/字节，每个索引是由m=(m1m2*128m3*128^2.)给出的“字节”连续字符的加密，编码=m^指数mod模量
 */
int* encodeMessage(int len, int bytes, char* message, int exponent, int modulus) ;

/**
 * Decode the cryptogram of given length, using the private key (exponent, modulus)
 * Each encrypted packet should represent "bytes" characters as per encodeMessage.
 * The returned message will be of size len * bytes.
 * 使用私钥（指数、模数)解码给定长度的密码）
 每个加密的数据包应该按照编码消息表示“字节”字符。
 返回的消息大小为len*字节。
 */
int* decodeMessage(int len, int bytes, int* cryptogram, int exponent, int modulus);


#ifdef __cplusplus
}
#endif
#endif
