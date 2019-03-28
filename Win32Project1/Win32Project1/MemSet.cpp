inline void Mem_Set_QUAD(void *dest, unsigned int data, unsigned int count)
{
	// 函数填充32位对齐内存
	//count为QUAD的数目
	_asm {
		move edi dest;
		move ecx count;
		move eax data;
		rep stosd;
	}
}

inline void Mem_Set_WORD(void *dest, unsigned int data, unsigned int count)
{
	// 函数填充16位对齐内存
	//count为WORD的数目
	_asm {
		move edi dest;
		move ecx count;
		move eax data;
		rep stosw;
	}
}
