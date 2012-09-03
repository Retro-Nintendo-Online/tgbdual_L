/******************************************************************
**                                                               **
**                                                               **
**              Z80-custom (nintendo game boy)                   **
**                      disassembler                             **
**                                                               **
**                                                               **
**     LAST UP DATE:  2006/09/06                                 **
**                    2006/05/15                                 **
**                                                               **
******************************************************************/

/* �g�p��͕K��BOOL�l��TRUE���Ԃ��Ă��邱�Ƃ��m�F���邱�ƁB	*/


/* static unsigned char status__________
** ���݂̏�Ԃ�\���X�e�[�^�X�t���O
** 8bit����bit�ɂ��ꂼ�������^���Ă���
**
** 1bit:������Ԃ�\���t���O 
**     opcode����͂�disasm����
** 2bit:2byte opcode��̓t���O
** 3bit:op1��̓t���O
** 4bit:op1,2byte��̓t���O
** 5bit:op2��̓t���O
** 6bit:op2,2byte��̓t���O
** 7bit:NOT
** 8bit:�����t���O_____________________*/


#define Z80CDISASM

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "z80cdisasm.h"

/*______________________________________________________**
** z80cdisasm: "z80 custom"dis asemmbler				**
** ����:DASM_STRSET *s: ��͌��ʂ̊i�[��			    **
**     :unsigned char opcode: �I�y�R�[�h�i�o�C�i���l�j  **
**     :bool reset: reset�t���O�B�K�v�ɉ�����true��n�� **
** �߂�l:false : opcode�̒ǉ��ǂݍ��݂��K�v            **
**       :true  : ��͊���                              **
**__________________________________________________________________________________**
** �ÓI�ϐ��̂��ߏ������������K�v�����A�����瑤�ł̒����͍s��Ȃ��B					**
**__________________________________________________________________________________*/
bool z80cdisasm(DASM_STRSET *s, unsigned char opcode, bool reset)
{
	static unsigned char status = STARTDISASM;
	static DASM_STRSET strset;

	if (reset)
	{
		ResetDisAsm(&strset, &status);
		return true;
	}
	StatusCheckAndGo(&strset, &status, opcode);

	if (status & ENDDISASM)
	{
		EndProcessing(s, &strset, &status);
		return true;
	}
	return false;
}

void StrSetToStr(char *string, const DASM_STRSET *strset)
{
	sprintf(string, "$%s\t%s\t%s\t%s", strset->UsedOpCode,
		strset->nemonic, strset->op1, strset->op2);
}

static void ResetDisAsm(DASM_STRSET *strset, unsigned char *status)
{
		*status = STARTDISASM;
		InitializeStrSet(strset);
}

static void StatusCheckAndGo(DASM_STRSET *strset, unsigned char *status, unsigned char opcode)
{
	if (*status & STARTDISASM)
		FirstProcessing(strset, status, opcode);
	else if (*status & TWOBYTEOPCODE)
		TwoByteOpCodeAnalysis(strset, status, opcode);
	else if (*status & FIRSTOP)
		FirstOperandAnalysis(strset, status, opcode);
	else if (*status & TWOBYTEFOP)
		TwoByteFOpAnalysis(strset, status, opcode);
	else if (*status & SECONDOP)
		SecondOperandAnalys(strset, status, opcode);
	else if (*status & TWOBYTESOP)
		TwoByteSOpAnalysis(strset, status, opcode);
	else
		exit(-1);

	PushOpCode(strset, opcode);
}

static void FirstProcessing(DASM_STRSET *strset, unsigned char *status, unsigned char opcode)
{
	InitializeStrSet(strset);
	*status = OpCodeToNemonic(strset, opcode);
}

static void TwoByteOpCodeAnalysis(DASM_STRSET *strset, unsigned char *status, unsigned char opcode)
{
	*status = TwoByteOpCoToN(strset, opcode);
}

static void FirstOperandAnalysis(DASM_STRSET *strset, unsigned char *status, unsigned char opcode)
{
	SetOperand(strset->op1, opcode);
	EndCheck(status, FIRSTOP);
}

static void TwoByteFOpAnalysis(DASM_STRSET *strset, unsigned char *status, unsigned char opcode)
{
	SetTwoByteOperand(strset->op1, opcode);
	EndCheck(status, TWOBYTEFOP);
}

static void SecondOperandAnalys(DASM_STRSET *strset, unsigned char *status, unsigned char opcode)
{
	SetOperand(strset->op2, opcode);
	EndCheck(status, SECONDOP);
}

static void TwoByteSOpAnalysis(DASM_STRSET *strset, unsigned char *status, unsigned char opcode)
{
	SetTwoByteOperand(strset->op2, opcode);
	EndCheck(status, TWOBYTESOP);
}

static void EndProcessing(DASM_STRSET *s, DASM_STRSET *strset, unsigned char *status)
{
	memcpy(s, strset, sizeof(DASM_STRSET));
	ResetDisAsm(strset, status);
}

static void InitializeStrSet(DASM_STRSET *strset)
{
	int i;
	for (i = 0; i < STRSIZE; i++)
	{
		strset->nemonic[i] = '\0';
		strset->UsedOpCode[i] = '\0';
		strset->op1[i] = '\0';
		strset->op2[i] = '\0';
	}
}

static void PushOpCode(DASM_STRSET *strset, unsigned char opcode)
{
	char buf[STRSIZE];

	sprintf(buf, "%02X", opcode);
	strcat(strset->UsedOpCode, buf);
}

static void SetOperand(char *operand, unsigned char opcode)
{
	if (0 == strcmp(operand, "SP+signed"))
		sprintf(operand, "SP%+02d", ((signed char)opcode));
	else if (0 == strcmp(operand, "signed"))
		sprintf(operand, "$%+02d", ((signed char)opcode));
	else if (0 == strcmp(operand, "()"))
		sprintf(operand, "(%02X)", opcode);
	else
		sprintf(operand, "$%02X", opcode);
}

static void SetTwoByteOperand(char *operand, unsigned char opcode)
{
	char buf[STRSIZE];

	if (operand[0] == '(')
		sprintf(buf, "(%02X%s", opcode, operand+1); // (���΂�����operand+1�ƂȂ�B)�͂��̂܂ܗ��p
	else
		sprintf(buf, "$%02X%s", opcode, operand+1); // $�}�[�N���΂�����operand+1�ƂȂ�B
	strcpy(operand, buf);
}

static void EndCheck(unsigned char *status, unsigned char checkstatus)
{
	*status -= checkstatus;
	if (!(*status))
		*status = ENDDISASM;
}

