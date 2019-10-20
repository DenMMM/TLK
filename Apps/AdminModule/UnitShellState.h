//---------------------------------------------------------------------------
#ifndef UnitShellStateH
#define UnitShellStateH
//---------------------------------------------------------------------------
// Main Shell State
#define mssConfig       1       // ������� ���������
#define mssErrorConfig  2       // ������ �������� ��������
#define mssErrorState   4       // ������ ������ � ������ ���������
#define mssErrorLog     8       // ������ ������ � ������ ����
//---------------------------------------------------------------------------
class MShellState
{
public:
    unsigned State;
    unsigned User;

	MShellState():
		State(0),
		User(0)
	{
	}
};
//---------------------------------------------------------------------------
#endif

