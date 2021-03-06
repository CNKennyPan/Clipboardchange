#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<windows.h>
using namespace std;


//获取剪切板内容
PCHAR getcontent() 
{
	HGLOBAL hGlobal;
	PCHAR pText;
	PCHAR pGlobal;

	OpenClipboard(GetDesktopWindow()); // Open the clipboard

	hGlobal = GetClipboardData(CF_TEXT);
	if (hGlobal == FALSE) // is equal "NULL" condition
	{
		CloseClipboard();
		return 0;
	}
	else
	{
		// Allocation memory 
		pText = new char[GlobalSize(hGlobal) + 1];

		// lock and get the clipboard text address.
		pGlobal = (char*)GlobalLock(hGlobal);
		strcpy(pText, pGlobal); // copy string

		GlobalUnlock(hGlobal); // Unlock
		CloseClipboard(); // Close the clipboard

		//cout << "现在剪切板的内容是： " << pText << endl;
		return pText;
	}
}


//写入剪切板
void writeclipboard(char *result,PCHAR content)
{
	if (OpenClipboard(GetDesktopWindow()))//打开剪贴板  
	{
		HANDLE hClip;
		char* pBuf;
		EmptyClipboard();//清空剪贴板  

						 //写入数据  
		hClip = GlobalAlloc(GMEM_MOVEABLE, strlen(result) + 1);
		pBuf = (char*)GlobalLock(hClip);
		strcpy(pBuf, result);
		GlobalUnlock(hClip);//解锁  
		if (SetClipboardData(CF_TEXT, hClip) == NULL)
		{
			cout << "-》剪切板更新失败！" << "错误原因：" << GetLastError() << endl;
		}
		else
		{
			cout << endl << "====================" << endl << "-》剪切板更新成功！" << endl << "-》原内容：" << content << "-》现内容:" << result << endl << "====================" << endl;
		}
		//关闭剪贴板  
		CloseClipboard();
	}
}

//获取替换目标和内容
void getaim(char *a, char *b)
{
	cout << "-》请输入您需要替换的目标： ";
	cin >> a;
	cout << "-》请输入您需要替换的结果： ";
	cin >> b;
	//cout << a << endl << b << endl;

}



//查找要替换目标在内容中的位置
void getaimplace(char *aim,PCHAR content,int place[])
{

	bool exist = false;
	place[0] = -1;
	place[3] = 0;
	//int number1 = strlen(aim);
	//int number2 = strlen(content);
	//判断目标是否比内容长
	if (strlen(content) >= strlen(aim))
	{
		for (unsigned int i = 0; i<strlen(content);i++)
		{
			for (unsigned int k = 0; k < strlen(aim);k++)
			{
				char temp = content[k+i];
				if (content[k + i] == aim[k])
				{
					place[3] = place[3] + 1;
					place[0] = k + i;
				}
			}
			if (place[3] == strlen(aim)) 
			{
				exist = true;
				break;
			}
			else 
			{
				place[3] = 0;
			}
		}
		if (exist)
		{
			place[1] = place[0] - strlen(aim) +1;
			place[2] = place[0] + 1;
			//cout << "需要替换的目标在内容中的位置止起点为： " << place[1] << "-" << place[2] << endl;

		}
		else {
			cout << endl << "====================" << endl << "-》找不到目标！ " << endl;
		}
	}
	else 
	{
		cout << endl << "====================" << endl << "-》目标过长！ " << endl;
	}
}




//替换内容
void replace(int place[], PCHAR content, char *body)
{
	
	//cout << place[0] << place[1] << place[2] << place[3] << body << endl; 
	if (place[0] != -1) {

		int n;
		n = place[1];
		PCHAR resulthead;
		PCHAR resultfoot;
		PCHAR result;


		resulthead = new char[place[1] + 1];
		resultfoot = new char[strlen(content) - place[2] + 1];
		result = new char[(strlen(resulthead) + strlen(body) + strlen(resultfoot)) + 1];
		

		for (int i = 0; i < place[1]; i++)
		{
			resulthead[i] = content[i];
		}
		resulthead[place[1]] = '\0';
		int a = 0;
		for (unsigned int i = place[2]; i < strlen(content); i++)
		{
			resultfoot[a] = content[i];
			a = a + 1;
		}
		a = 0;
		resultfoot[strlen(content) - place[2]] = '\0';
		for (unsigned int i = 0; i < (strlen(resulthead)); i++)
		{
			result[i] = resulthead[i];
		}
		for (unsigned int i = (strlen(resulthead)); i < (strlen(resulthead) + strlen(body)); i++)
		{
			result[i] = body[a];
			a = a + 1;
		}
		a = 0;
		for (unsigned int i = (strlen(resulthead) + strlen(body)); i < (strlen(resulthead) + strlen(body) + strlen(resultfoot)); i++)
		{
			result[i] = resultfoot[a];
			a = a + 1;
		}
		a = 0;
		result[(strlen(resulthead) + strlen(body) + strlen(resultfoot))] = '\0';

		//cout << "即将设置剪切板的内容为： " << result << endl;
		writeclipboard(result, content);

	}
	else {
		cout <<  "-》无需要替换内容！ " << endl << "====================" << endl;

	}

}



//检测内容是否相同
BOOL testcontent(PCHAR a, PCHAR b)
{
	BOOL same = true;
	for (unsigned int i = 0; i < strlen(a); i++)
	{
		if (a[i] != b[i]) 
		{
			same = false;
		}
	}
	
	return same;
}

//心跳提示
void waiting(int &a)
{
	if (a < 10)
	{
		if (a == 0)
		{
			cout << "请复制需要替换的内容";
			a++;
		}
		cout << "·";
		a++;
	}
	else
	{
		cout << "·" << endl;
		a = 0;

	}
}



//循环监听
void listenclipboardchange()
{
	
	char a[10];
	char b[10];
	getaim(a, b);
	//char result[40];
	int place[4];
	int wait=0;

	PCHAR resorce = getcontent();
	
	if (resorce) {
		getaimplace(a, resorce,place);
		replace(place, resorce, b);
	}
	else
	{
		cout << "-》剪切板没有内容" << endl;
	}
	Sleep(1000);
	
	PCHAR newresorce;
	do 
	{
		newresorce = getcontent();
		if (newresorce)
		{
			if (strlen(newresorce) != strlen(resorce))
			{
				getaimplace(a, newresorce, place);
				replace(place, newresorce, b);
				resorce = getcontent();
				Sleep(1000);
				wait = 0;
			}
			else 
			{
				if (!testcontent(newresorce, resorce)) 
				{
					getaimplace(a, newresorce, place);
					replace(place, newresorce, b);
					resorce = getcontent();
					Sleep(1000);
					wait = 0;
				}
				else 
				{
					waiting(wait);
					Sleep(1000);
					
				}
			}			
		}
		else
		{
			cout << "-》剪切板没有内容" << endl;
			Sleep(1000);
			wait = 0;
		}
		
		
	} while (true);


}




int main()
{

	listenclipboardchange();
	/*PCHAR resorce = getcontent();
	if (resorce) {
		char a[10];
		char b[10];
		char result[40];
		int place[4];
		getaim(a, b);
		getaimplace(a, resorce, place);
		replace(place, resorce, b, result);
	}
	else 
	{
		cout << "剪切板没有内容" << endl;
	}*/
	
	cin.ignore();
	return 0;
}