		AREA DELAY, CODE, READONLY
		
delay_us	PROC
			EXPORT	delay_us

			LSR r1,r1,#2 ;�߼�����(��4) 1������
			MUL r0,r0,r1 ;�� 1������
			SUB r0,r0,#3 ;�� 1������
			NOP
			NOP
loop		SUBS r0,r0,#1 ;1ʱ������
			BNE loop ;������ת ÿ��4��ʱ������
			NOP
			BX lr ;3��ʱ������
			ENDP

delay_ms	PROC
			EXPORT	delay_ms
			
			PUSH.W {r2} ;2������
			LSR r1,r1,#2
			;MOVS r2,#0
loop1		MOV r2,#1000
			MUL r2,r2,r1 ;��
			SUB r2,r2,#2 ;��
			NOP
			NOP
			NOP
loop2		SUBS r2,r2,#1
			BNE loop2
			SUBS r0,r0,#1 ;1ʱ������
			BNE loop1 ;������ת ÿ��4��ʱ������
			POP {r2} ;2ʱ������
			BX lr ;3��ʱ������
			ENDP

			END
