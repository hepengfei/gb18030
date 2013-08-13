#ifndef _GB_HPP_
#define _GB_HPP_

// 
// 关于GB系列编码字符的一些工具函数
// 
// 现在主要支持GB18030和GBK，对GB2312的支持有限
// 

//
// GB18030的编码规则：
// 分为单字节、双字节、四字节三类
// 单字节：[0x00,0x7F]
// 双字节：[0x81,0xFE] [0x40,0xFE] !0x7F
// 四字节：[0x81,0xFE] [0x30,0x39] [0x81,0xFE] [0x30,0x39]
// 其中双字节中，第二个字节范围中不包含0x7F
//
// GBK的编码规则同上述的单字节和双字节。
// 

// 
// 下面一组函数判断指定字节是否符合特定编码规则
// 

// 是否符合GB编码的单字节规则
static inline
bool isGBByte1 (unsigned char byte) {
	return byte <= 0x7F;
}

// 是否符合GB编码的双字节规则
static inline
bool isGBByte2 (unsigned char byte1,
		unsigned char byte2) {
	return  (byte1 >= 0x81) && (byte1 <= 0xFE) &&
		(byte2 >= 0x40) && (byte2 <= 0xFE) && (byte2 != 0x7F);
}

// 是否符合GB编码的四字节规则
static inline
bool isGBByte4 (unsigned char byte1,
		unsigned char byte2,
		unsigned char byte3,
		unsigned char byte4) {
	return  (byte1 >= 0x81) && (byte1 <= 0xFE) &&
		(byte2 >= 0x30) && (byte2 <= 0x39) &&
		(byte3 >= 0x81) && (byte3 <= 0xFE) &&
		(byte4 >= 0x30) && (byte4 <= 0x39);
}

//
// 下面一组函数判断特定一组字节是否合法的GB编码序列 
//

// 判断一个字节是否合法的GB编码
static inline
bool isGBCode1 (unsigned char byte) {
	return isGBByte1 (byte);
}

// 判断两个字节是否合法的GB编码，可能为两个单字节或一个双字节
static inline
bool isGBCode2 (unsigned char byte1,
		unsigned char byte2) {
	if (isGBCode1 (byte1))
		return isGBCode1 (byte2);
	else
		return isGBByte2 (byte1, byte2);
}

// 判断四个字节是否合法的GB编码，可能是四个单字节或两个双字节或一个四字
// 节；实现方式为判断是否为两个二字节GB序列或一个四字节GB序列
static inline
bool isGBCode4 (unsigned char byte1,
		unsigned char byte2,
		unsigned char byte3,
		unsigned char byte4) {
	if (isGBCode2 (byte1, byte2))
		return isGBCode2 (byte3, byte4);
	else
		return isGBByte4 (byte1, byte2, byte3, byte4);
}

//
// 判断一个字节串是否为合法的GB编码，对NULL指针或空字符串""返回真；字符
// 串必须是以'\0'结尾的。
//
// 实现方式为，单个字节不符合规则，就判断两个字节，再不符合就判断四个字
// 节，若都不符合或字节不足，则返回假。
// 
static
bool isGBString (const char *str) {
	while (str && *str)
		if (isGBByte1 (str[0]))
			str += 1;
		else if (str[1])
			if (isGBByte2 (str[0], str[1]))
				str += 2;
			else if (str[2] && str[3])
				if (isGBByte4 (str[0], str[1], str[2], str[3]))
					str += 4;
				else
					return false;
			else
				return false;
		else
			return false;
	return true;
}
/*
// 取一个GB编码的字节串的子串
unsigned int gbSubString (char *dest, const char* src, unsigned int start, unsigned int end) {
	return end - start;
}
*/
// 计算指定字节长度内，有多少个字节是合法GB编码的字节串
// 注意单位都是字节
static
unsigned int gbCountByte (const char* str, unsigned int len) {
	unsigned int count = 0;
	while (str && *str && len) {
		if (isGBByte1 (str[0])) {	// 这里len一定>=1，因为循环条件为len > 0
			str += 1;
			count += 1;
			len -= 1;
		} else if (len >= 2 && isGBByte2 (str[0], str[1])) {
			str += 2;
			count += 2;
			len -= 2;
		} else if (len >= 4 && isGBByte4 (str[0], str[1], str[2], str[3])) {
			str += 4;
			count += 4;
			len -= 4;
		} else {
			break;
		}
	}

	return count;
}

#if _TEST_

#include <iostream>
#include <string>
namespace test {
	
	void isGBString () {
		char str[2049]={0};
		while (!std::cin.eof()) {
			std::cin.getline (str,2048);
			std::cout << ::isGBString (str) << std::endl;
		}
	}

	void gbCountByte () {
		char str[2049] = {0};
		int len = 53;
		while (!std::cin.eof()) {
			std::cin.getline (str,2048);
			std::cout << ::gbCountByte (str, len) << std::endl;
		}
	}
}

#endif	// _TEST_

/*
// 验证一个字节串是否为合法的GB2312序列，其中可包含ASCII码。
static
bool isValidGB2312 (const char *buffer) {
	const char *p = buffer;

	while (p && *p) {
		if (*p & 0x80)
			if (isValidGB2312 (*p, *(p+1)))
				p += 2;
			else
				return false;
		else
			p += 1;
	}

	return true;
}
*/

#endif	// _GB_HPP_

