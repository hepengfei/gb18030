#ifndef _GB_HPP_
#define _GB_HPP_

// 
// ����GBϵ�б����ַ���һЩ���ߺ���
// 
// ������Ҫ֧��GB18030��GBK����GB2312��֧������
// 

//
// GB18030�ı������
// ��Ϊ���ֽڡ�˫�ֽڡ����ֽ�����
// ���ֽڣ�[0x00,0x7F]
// ˫�ֽڣ�[0x81,0xFE] [0x40,0xFE] !0x7F
// ���ֽڣ�[0x81,0xFE] [0x30,0x39] [0x81,0xFE] [0x30,0x39]
// ����˫�ֽ��У��ڶ����ֽڷ�Χ�в�����0x7F
//
// GBK�ı������ͬ�����ĵ��ֽں�˫�ֽڡ�
// 

// 
// ����һ�麯���ж�ָ���ֽ��Ƿ�����ض��������
// 

// �Ƿ����GB����ĵ��ֽڹ���
static inline
bool isGBByte1 (unsigned char byte) {
	return byte <= 0x7F;
}

// �Ƿ����GB�����˫�ֽڹ���
static inline
bool isGBByte2 (unsigned char byte1,
		unsigned char byte2) {
	return  (byte1 >= 0x81) && (byte1 <= 0xFE) &&
		(byte2 >= 0x40) && (byte2 <= 0xFE) && (byte2 != 0x7F);
}

// �Ƿ����GB��������ֽڹ���
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
// ����һ�麯���ж��ض�һ���ֽ��Ƿ�Ϸ���GB�������� 
//

// �ж�һ���ֽ��Ƿ�Ϸ���GB����
static inline
bool isGBCode1 (unsigned char byte) {
	return isGBByte1 (byte);
}

// �ж������ֽ��Ƿ�Ϸ���GB���룬����Ϊ�������ֽڻ�һ��˫�ֽ�
static inline
bool isGBCode2 (unsigned char byte1,
		unsigned char byte2) {
	if (isGBCode1 (byte1))
		return isGBCode1 (byte2);
	else
		return isGBByte2 (byte1, byte2);
}

// �ж��ĸ��ֽ��Ƿ�Ϸ���GB���룬�������ĸ����ֽڻ�����˫�ֽڻ�һ������
// �ڣ�ʵ�ַ�ʽΪ�ж��Ƿ�Ϊ�������ֽ�GB���л�һ�����ֽ�GB����
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
// �ж�һ���ֽڴ��Ƿ�Ϊ�Ϸ���GB���룬��NULLָ�����ַ���""�����棻�ַ�
// ����������'\0'��β�ġ�
//
// ʵ�ַ�ʽΪ�������ֽڲ����Ϲ��򣬾��ж������ֽڣ��ٲ����Ͼ��ж��ĸ���
// �ڣ����������ϻ��ֽڲ��㣬�򷵻ؼ١�
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
// ȡһ��GB������ֽڴ����Ӵ�
unsigned int gbSubString (char *dest, const char* src, unsigned int start, unsigned int end) {
	return end - start;
}
*/
// ����ָ���ֽڳ����ڣ��ж��ٸ��ֽ��ǺϷ�GB������ֽڴ�
// ע�ⵥλ�����ֽ�
static
unsigned int gbCountByte (const char* str, unsigned int len) {
	unsigned int count = 0;
	while (str && *str && len) {
		if (isGBByte1 (str[0])) {	// ����lenһ��>=1����Ϊѭ������Ϊlen > 0
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
// ��֤һ���ֽڴ��Ƿ�Ϊ�Ϸ���GB2312���У����пɰ���ASCII�롣
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

