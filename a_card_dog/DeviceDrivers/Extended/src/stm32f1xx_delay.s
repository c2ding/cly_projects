		AREA DELAY, CODE, READONLY
		
delay_us	PROC
			EXPORT	delay_us

			LSR r1,r1,#2 ;逻辑右移(除4) 1个周期
			MUL r0,r0,r1 ;乘 1个周期
			SUB r0,r0,#3 ;减 1个周期
			NOP
			NOP
loop		SUBS r0,r0,#1 ;1时钟周期
			BNE loop ;条件跳转 每次4个时钟周期
			NOP
			BX lr ;3个时钟周期
			ENDP

delay_ms	PROC
			EXPORT	delay_ms
			
			PUSH.W {r2} ;2个周期
			LSR r1,r1,#2
			;MOVS r2,#0
loop1		MOV r2,#1000
			MUL r2,r2,r1 ;乘
			SUB r2,r2,#2 ;减
			NOP
			NOP
			NOP
loop2		SUBS r2,r2,#1
			BNE loop2
			SUBS r0,r0,#1 ;1时钟周期
			BNE loop1 ;条件跳转 每次4个时钟周期
			POP {r2} ;2时钟周期
			BX lr ;3个时钟周期
			ENDP

			END
