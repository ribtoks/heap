
/*
	�������� �������, ������� ���� ������ ��������� 
	��������� � ������ ������� ����-��������.

	������� ���������:
	source - �����, � ������� ���� ��������� ���������
	substring - ���������, ��������� ������� ����
	sourceSize - ������ ������, � ������� ����
	length - ������ ���������, ������� ����

	���������� ������� �������� ���������, ��� -1
	 � ������ ���������� �� � ������
*/
int Moore_Horspool(const char* source, const char* substring, int sourceSize, int length);


/*
	�������� �������, ������� ���� ������ ��������� 
	��������� � ������ ������� ����� �����-����.

	������� ���������:
	source - �����, � ������� ���� ��������� ���������
	substring - ���������, ��������� ������� ����
	sourceSize - ������ ������, � ������� ����
	length - ������ ���������, ������� ����

	���������� ������� �������� ���������, ��� -1
	 � ������ ���������� �� � ������
*/
int TurboBoyerMoore(const char* source, const char* substring, int sourceSize, int length);