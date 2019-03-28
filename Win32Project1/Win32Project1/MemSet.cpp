inline void Mem_Set_QUAD(void *dest, unsigned int data, unsigned int count)
{
	// �������32λ�����ڴ�
	//countΪQUAD����Ŀ
	_asm {
		move edi dest;
		move ecx count;
		move eax data;
		rep stosd;
	}
}

inline void Mem_Set_WORD(void *dest, unsigned int data, unsigned int count)
{
	// �������16λ�����ڴ�
	//countΪWORD����Ŀ
	_asm {
		move edi dest;
		move ecx count;
		move eax data;
		rep stosw;
	}
}
