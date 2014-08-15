
#include "stdafx.h"

#include "Similarity.h"
#include <fstream>
#include <iostream>
//#include <iostream.h>
#include <vector>
#include <stack>




Similarity::Similarity()
{
	_RelationalSymbol = L"~^#%$*+&@?!";
	
	 _alpha = 1.6;     //计算基本义原相似度时的参数 
	 _beta1 = 0.5;     //4种描述式相似度的权值 
	 _beta2 = 0.2;
	 _beta3 = 0.17;
	 _beta4 = 0.13;
	
	 _delta = 0.2;
	 _gama = 0.2;
	
	 _Default_Distance = 20;
	_SemantemeFileName = ".\\HowNet\\WHOLE.DAT";
	_ConceptFileName = ".\\HowNet\\glossary.dat";
	_ConceptIndexFileName = ".\\HowNet\\glossary.idx";
	
	initialize();
	int len =   mapConceptIndex.size();
	std::cout << "mapConceptIndex len =" << len << std::endl;
	std::wstring word = L"T恤";
	std::map<std::wstring,std::string>::iterator it = mapConceptIndex.find(word);
	if (it != mapConceptIndex.end())
	{
		CHAR szTemp[512];
	/*	memset(szTemp,0,sizeof(szTemp));
		WideCharToMultiByte(CP_ACP, 0, it->second.Word.c_str(), -1, szTemp, sizeof(szTemp), NULL, NULL);
		std::cout <<szTemp <<std::endl;
		memset(szTemp,0,sizeof(szTemp));
		WideCharToMultiByte(CP_ACP, 0, it->second.Type.c_str(), -1, szTemp, sizeof(szTemp), NULL, NULL);
		std::cout <<szTemp <<std::endl;
		memset(szTemp,0,sizeof(szTemp));
		WideCharToMultiByte(CP_ACP, 0, it->second.Concepts.c_str(), -1, szTemp, sizeof(szTemp), NULL, NULL);
		std::cout <<szTemp <<std::endl;*/
	}
	len =  mapSemantems.size();
	std::cout << "mapSemantems len =" << len <<std::endl;
	len = mapIndexSemantmes.size();
	std::cout << "mapIndexSemantmes len =" << len <<std::endl;
}


Similarity::~Similarity()
{
}

bool Similarity::initialize()
{
	bool bRet(false);
	bRet = ConstructSemantems();
    if (!bRet) {
		return bRet; 
	}

    bRet = ConstructConcept();
    if (!bRet)
	{
		return bRet; 
	}
	

	return bRet;
}


/// 加载HowNet概念库索引
bool Similarity::ConstructConcept()
{
	bool bRet(true);
	std::fstream  in;
	std::string strLine;
	in.open(_ConceptFileName.c_str(),std::ios::binary|std::ios::in);
	char buffer[1024],col1[512],col2[512],col3[512];
	memset(buffer,0,sizeof(buffer));
	memset(col1,0,sizeof(col1));
	memset(col2,0,sizeof(col2));
	memset(col3,0,sizeof(col3));
	while(in.getline(buffer,sizeof(buffer)))
	{
	//	std::cout << buffer << std::endl;
		strLine = buffer;
		splitLine(buffer,col1,col2,col3);

		std::wstring  wLine(L""),wKey(L"");
		ChangeToW(wLine,buffer);
		ChangeToW(wKey,col1);
		std::map<std::wstring,std::string>::iterator it;
		it = mapConceptIndex.find(wKey);
		if (it == mapConceptIndex.end())
			mapConceptIndex.insert(std::pair<std::wstring,std::string>(wKey,strLine));
		else
		{
			it->second += ";";
			it->second += strLine;
		}

		
	/*	Glossary t_glossary;
		ChangeToW(t_glossary.Word,col1);
		ChangeToW(t_glossary.Type,col2);
		ChangeToW(t_glossary.Concepts,col3);
		std::map<std::wstring,Glossary>::iterator it;
		it = mapConceptIndex.find(t_glossary.Word);
		if (it == mapConceptIndex.end())
			mapConceptIndex.insert(std::pair<std::wstring,Glossary>(t_glossary.Word,t_glossary));*/
		

	}
	return bRet;
}

 /// 加载HowNet义原树
bool Similarity::ConstructSemantems()
{
	bool bRet(true);
	std::fstream  in;
	std::string strLine;
	in.open(_SemantemeFileName.c_str(),std::ios::binary|std::ios::in);
	char buffer[1024],col1[512],col2[512],col3[512];
	memset(buffer,0,sizeof(buffer));
	memset(col1,0,sizeof(col1));
	memset(col2,0,sizeof(col2));
	memset(col3,0,sizeof(col3));
	while(in.getline(buffer,sizeof(buffer)))
	{
		strLine = buffer;
	//	std::cout << buffer << std::endl;
		splitLine(buffer,col1,col2,col3);
		
		UINT id,pid;
		std::wstring  sema;
		Semanteme  t_Sem;
		id = atoi(col1);
		pid = atoi(col3);
		t_Sem.nID = id;
		t_Sem.nParentID = pid;
		ChangeToW(t_Sem.wstrSema,col2);
		std::map<std::wstring,Semanteme>::iterator it;
		it = mapSemantems.find(t_Sem.wstrSema);
		if (it == mapSemantems.end())
			mapSemantems.insert(std::pair<std::wstring,Semanteme>(t_Sem.wstrSema,t_Sem));
		else
		{
			it->second = t_Sem;
//			std::cout << "mapSemantems is exist: " << strLine << std::endl;
		}
		std::map<UINT,Semanteme>::iterator it1;
		it1 = mapIndexSemantmes.find(id);
		if (it1 == mapIndexSemantmes.end())
			mapIndexSemantmes.insert(std::pair<UINT,Semanteme>(id,t_Sem));
	

	}

	return bRet;
}


void Similarity::splitLine(char *buffer,char *Col1, char *Col2, char *Col3)
{
	if (buffer)
	{
		char seps[]   = " \t\r\n";
		char *token;
		token = strtok( buffer, seps );
		int nIndex = 0;
		while( token != NULL )
		{
			/* While there are tokens in "string" */
		//	printf( "%s\n", token );
			if (nIndex == 0)
				strcpy(Col1,token);
			else if (nIndex == 1)
				strcpy(Col2,token);
			else
				strcpy(Col3,token);
			/* Get next token: */
			token = strtok( NULL, seps );
			++nIndex;
		}



	}
}



void Similarity::ChangeToW(std::wstring &t_wstr, char *buffer)
{
	WCHAR  *pwBuffer = new WCHAR[strlen(buffer)+1];
	memset(pwBuffer,0,sizeof(WCHAR)*(strlen(buffer)+1));
	MultiByteToWideChar(CP_ACP, 0, buffer,strlen(buffer), pwBuffer, strlen(buffer));
	t_wstr = pwBuffer;

	delete []pwBuffer;
	pwBuffer = NULL;
}

double Similarity::calWordSim(std::wstring word1, std::wstring word2,std::wstring type)// 计算相似度
{
	double dbVal(0.0);
	std::string wordcon1,wordcon2;
	std::map<std::wstring,std::string>::iterator it;
	it = mapConceptIndex.find(word1);
	if (it == mapConceptIndex.end())
		return dbVal;
	wordcon1 = it->second;
	it = mapConceptIndex.find(word2);
	if (it == mapConceptIndex.end())
		return dbVal;
	wordcon2 = it->second;

	std::string::size_type  pos1(0),pos2(0);
	std::vector<Glossary>  vectCon1,vectCon2;
	std::string  temp;
	Glossary t_glossary;

	char buffer[1024],col1[512],col2[512],col3[512];
	memset(buffer,0,sizeof(buffer));
	memset(col1,0,sizeof(col1));
	memset(col2,0,sizeof(col2));
	memset(col3,0,sizeof(col3));
	if (wordcon1.find(";") == std::string::npos)
	{
		strncpy(buffer,wordcon1.c_str(),wordcon1.size());
		splitLine(buffer,col1,col2,col3);
		ChangeToW(t_glossary.Word,col1);
		ChangeToW(t_glossary.Type,col2);
		ChangeToW(t_glossary.Concepts,col3);
		vectCon1.push_back(t_glossary);
	}
	else
	{
		while((pos1=wordcon1.find(";",pos2)) != std::string::npos)
		{
			temp = wordcon1.substr(pos2,pos1-pos2);
			memset(buffer,0,sizeof(buffer));
			memset(col1,0,sizeof(col1));
			memset(col2,0,sizeof(col2));
			memset(col3,0,sizeof(col3));
			strncpy(buffer,temp.c_str(),temp.size());
			splitLine(buffer,col1,col2,col3);
			ChangeToW(t_glossary.Word,col1);
			ChangeToW(t_glossary.Type,col2);
			ChangeToW(t_glossary.Concepts,col3);
			vectCon1.push_back(t_glossary);		
			pos2 = pos1+1;
		}
		temp = wordcon1.substr(pos2);
		memset(buffer,0,sizeof(buffer));
		memset(col1,0,sizeof(col1));
		memset(col2,0,sizeof(col2));
		memset(col3,0,sizeof(col3));
		strncpy(buffer,temp.c_str(),temp.size());
		splitLine(buffer,col1,col2,col3);
		ChangeToW(t_glossary.Word,col1);
		ChangeToW(t_glossary.Type,col2);
		ChangeToW(t_glossary.Concepts,col3);
		vectCon1.push_back(t_glossary);		
	}
	pos1 = pos2 = 0;
	if (wordcon2.find(";") == std::string::npos)
	{
		strncpy(buffer,wordcon1.c_str(),wordcon1.size());
		splitLine(buffer,col1,col2,col3);
		ChangeToW(t_glossary.Word,col1);
		ChangeToW(t_glossary.Type,col2);
		ChangeToW(t_glossary.Concepts,col3);
		vectCon2.push_back(t_glossary);

	}
	else
	{
		while((pos1=wordcon2.find(";",pos2)) != std::string::npos)
		{
			temp = wordcon2.substr(pos2,pos1-pos2);
			memset(buffer,0,sizeof(buffer));
			memset(col1,0,sizeof(col1));
			memset(col2,0,sizeof(col2));
			memset(col3,0,sizeof(col3));
			strncpy(buffer,temp.c_str(),temp.size());
			splitLine(buffer,col1,col2,col3);
			ChangeToW(t_glossary.Word,col1);
			ChangeToW(t_glossary.Type,col2);
			ChangeToW(t_glossary.Concepts,col3);
			vectCon2.push_back(t_glossary);
			pos2 = pos1+1;
		}
		temp = wordcon2.substr(pos2);
		memset(buffer,0,sizeof(buffer));
		memset(col1,0,sizeof(col1));
		memset(col2,0,sizeof(col2));
		memset(col3,0,sizeof(col3));
		strncpy(buffer,temp.c_str(),temp.size());
		splitLine(buffer,col1,col2,col3);
		ChangeToW(t_glossary.Word,col1);
		ChangeToW(t_glossary.Type,col2);
		ChangeToW(t_glossary.Concepts,col3);
		vectCon2.push_back(t_glossary);

	}
	int i,j;

	for(i = 0; i<vectCon1.size(); i++)
	{
		if (type != vectCon1[i].Type)
			continue;
		for(j = 0; j<vectCon2.size(); j++)
		{
			if (vectCon1[i].Type == vectCon2[j].Type)
			{
				double t_val = calConceptSim(vectCon1[i].Concepts,vectCon2[j].Concepts);
				if (dbVal < t_val)
					dbVal = t_val;
			}
		}
	}


	
	return dbVal;
}

double Similarity::calConceptSim(std::wstring concept1, std::wstring concept2)//计算概念相似度
{
	double sim(0.0);
	if (concept1.at(0) == L'{')//概念1是虚词
	{
		if (concept2.at(0) != L'{')////概念2是实词 
		{
			return sim;
		}
		else //概念2是虚词 
		{
			////去掉"{" and "}" 
			std::wstring sem1 = concept1.substr(1, concept1.size() - 2);
			std::wstring sem2 = concept2.substr(1, concept2.size() - 2);
			
			int pos1 = sem1.find(L"=");
			int pos2 = sem2.find(L"=");
			if ((pos1 == std::string::npos) ^ (pos2 == std::string::npos))//一个句法义原，一个是关系义原
			{
				return 0;
			}
			else if (pos1 == std::string::npos && pos2 == std::string::npos)//都是句法义原
			{
				sim =  calSimBase(sem1, sem2);
			}
			else//都是关系义原 
			{
				sim = calSimReal(sem1, sem2);
			}
		}
	}
	else 
	{
		if (concept2.at(0) == L'{') //概念2是虚词 
		{
			return sim;
		}
		else//概念2是实词 
		{
			//分别计算4种描述式的相似度 
			double sim1 = 0.0;
			double sim2 = 0.0;
			double sim3 = 0.0;
			double sim4 = 0.0;

			std::string::size_type  pos1(0),pos2(0);
			std::vector<std::wstring>  vectCon1,vectCon2;
			std::wstring  temp;
			if (concept1.find(L",") == std::string::npos)
			{
				vectCon1.push_back(concept1);
			}
			else
			{
				while((pos1=concept1.find(L",",pos2)) != std::string::npos)
				{
					temp = concept1.substr(pos2,pos1-pos2);
					vectCon1.push_back(temp);
					pos2 = pos1+1;
				}
				temp = concept1.substr(pos2);
				vectCon1.push_back(temp);
			}
			pos1 = pos2 = 0;
			if (concept2.find(L",") == std::string::npos)
			{
				vectCon2.push_back(concept2);
			}
			else
			{
				while((pos1=concept2.find(L",",pos2)) != std::string::npos)
				{
					temp = concept2.substr(pos2,pos1-pos2);
					vectCon2.push_back(temp);
					pos2 = pos1+1;
				}
				temp = concept2.substr(pos2);
				vectCon2.push_back(temp);
				
			}

			std::wstring BaseSem1 = L"";
			std::wstring BaseOtherSem1 = L"";
			std::wstring RelSem1 = L"";
			std::wstring SymbolSem1 = L"";
			
			std::wstring BaseSem2 = L"";
			std::wstring BaseOtherSem2 = L"";
			std::wstring RelSem2 = L"";
			std::wstring SymbolSem2 = L"";
			
			bool isfirst = true;
			int i;
			for (i = 0; i < vectCon1.size(); i++)
			{
				std::wstring s = vectCon1[i];
				if (s.find(L"=") != std::wstring::npos)
				{
					if (RelSem1 == L"")
					{
						RelSem1 += s;
					}
					else
					{
						RelSem1 += L"," + s;
					}
				}
				else if (_RelationalSymbol.find(s[0]) != std::wstring::npos)
				{
					if (SymbolSem1 == L"")
					{
						SymbolSem1 += s;
					}
					else
					{
						SymbolSem1 += L"," + s;
					}
					
				}
				else if (isfirst)
				{
					BaseSem1 += s;
					isfirst = false;
				}
				else
				{
					if (BaseOtherSem1 == L"")
					{
						BaseOtherSem1 += s;
					}
					else
					{
						BaseOtherSem1 += L"," + s;
					}
				}
			}

			isfirst = true;
			for (i = 0; i < vectCon2.size(); i++)
			{
				std::wstring s = vectCon2[i];
				if (s.find(L"=") != std::wstring::npos)
				{
					if (RelSem2 == L"")
					{
						RelSem2 += s;
					}
					else
					{
						RelSem2 += L"," + s;
					}
				}
				else if (_RelationalSymbol.find(s[0]) != std::wstring::npos)
				{
					if (SymbolSem2 == L"")
					{
						SymbolSem2 += s;
					}
					else
					{
						SymbolSem2 += L"," + s;
					}
				}
				else if (isfirst)
				{
					BaseSem2 += s;
					isfirst = false;
				}
				else
				{
					if (BaseOtherSem2 == L"")
					{
						BaseOtherSem2 += s;
					}
					else
					{
						BaseOtherSem2 += L"," + s;
					}
				}
			}

			if (BaseSem1.length() && BaseSem2.length())
			{
				sim1 = calSim1(BaseSem1, BaseSem2);
			}
			else
			{
				sim1 = 0.01;
			}

			sim2 = calSim2(BaseOtherSem1, BaseOtherSem2);
			sim3 = calSim3(RelSem1, RelSem2);
			sim4 = calSim4(SymbolSem1, SymbolSem2);			
			sim =  _beta1 * sim1 +
				_beta2 * sim1 * sim2 +
                _beta3 * sim1 * sim2 * sim3 +
                _beta4 * sim1 * sim2 * sim4 * sim4;
		}

	}

	return sim;
}




int Similarity::Distance(std::wstring s1, std::wstring s2)
{
	int dist(_Default_Distance);
	Semanteme sem1,sem2;
	if (!GetSemantems(s1,sem1))
		return _Default_Distance;
	if (!GetSemantems(s2,sem2))
		return _Default_Distance;
	
	std::stack<Semanteme> stk1,stk2;
	while(sem1.nID != sem1.nParentID)
	{
		stk1.push(sem1);
		if (!GetParentSemantem(sem1))
			break;
		
	}
	stk1.push(sem1);

	while(sem2.nID != sem2.nParentID)
	{
		stk2.push(sem2);
		if (!GetParentSemantem(sem2))
			break;
		
	}
	stk2.push(sem2);

	if (sem1.nID != sem2.nID)
	{
		return _Default_Distance;
	}

	while (stk1.size() > 0 && stk2.size() > 0)
	{
		if (stk1.top().nID == stk2.top().nID)
		{
			stk1.pop();
			stk2.pop();
		}
		else
		{
			break;
		}
	}

	dist = stk1.size() + stk2.size();
	return dist;
}

bool Similarity::GetSemantems(std::wstring sema,Semanteme &sem)
{
	std::map<std::wstring,Semanteme>::iterator it =  mapSemantems.find(sema);
	if (it == mapSemantems.end())
		return false;
	sem = it->second;
	return true;
}

bool Similarity::GetParentSemantem(Semanteme &sem)
{
	std::map<UINT,Semanteme>::iterator it = mapIndexSemantmes.find(sem.nParentID);
	if (it == mapIndexSemantmes.end())
		return false;
	sem = it->second;
	return true;
}

double Similarity::calSimBase(std::wstring sem1, std::wstring sem2)
{
	double sim(0.0);
	//有一个是具体词，而另一个不是
	if (sem1[0] == L'(' ^ sem2[0] == L'(')
	{
		return sim;
	}
	
	if (sem1[0] == L'(' && sem2[0] == L'(')
	{
		if (sem1 != sem2)
		{
			return sim;
		}
	}
	
	if (sem1 == sem2)
	{
		sim = 1.0;
		return sim;
	}

	double dist = Distance(sem1, sem2);
	sim = _alpha / (_alpha + dist);

	return sim;
}

	
double Similarity::calSimReal(std::wstring sem1, std::wstring sem2)
{
	double sim(0.0);
	if (sem1[0] == L'(')
	{
		sem1 = sem1.substr(1, sem1.size() - 2);
	}
	
	if (sem2[0] == L'(')
	{
		sem2 = sem2.substr(1, sem2.size() - 2);
	}

	int pos1 = sem1.find(L"=");
	std::wstring rela1 = sem1.substr(0, pos1);
	
	int pos2 = sem2.find(L"=");
	std::wstring rela2 = sem2.substr(0, pos2);

	if (rela1 == rela2)
	{
		std::wstring base1 = sem1.substr(pos1+1);
		std::wstring base2 = sem2.substr(pos2+1);
		sim =  calSimBase(base1, base2);
	}

	return sim;

}

std::vector<std::wstring> Similarity::splitSemanetes(std::wstring line)
{

	std::vector<std::wstring>  vectSem;
	std::string::size_type  pos1(0),pos2(0);
	std::wstring  temp;
	if (line.find(L",") == std::string::npos)
	{
		vectSem.push_back(line);
	}
	else
	{
		while((pos1=line.find(L",",pos2)) != std::string::npos)
		{
			temp = line.substr(pos2,pos1-pos2);
			vectSem.push_back(temp);
			pos2 = pos1+1;
		}
		temp = line.substr(pos2);
		vectSem.push_back(temp);
		
	}

	return vectSem;

}

double Similarity::calSim1(std::wstring line1, std::wstring line2)
{
	double sim(0.0);
	if (line1.length() == 0 && line2.length() == 0)
	{
		sim = 1.0;
		return sim;
	}
	
	if (line1.length() == 0 || line2.length() == 0)
	{
		sim  = 0.0;
		return sim;
	}
	
	std::vector<std::wstring> sems1 = splitSemanetes(line1);
	std::vector<std::wstring> sems2 = splitSemanetes(line2);
	
	if (sems1.size() >= 1 && sems2.size() >= 1)
	{
		sim = calSimBase(sems1[0], sems2[0]);
		return sim;
	}
	else
	{
		
		return sim;
	}
}

double Similarity::calSim2(std::wstring line1, std::wstring line2)
{
	if (line1.length() == 0 && line2.length() == 0)
	{
		return 1.0;
	}
	
	if (line1.length() == 0 || line2.length() == 0)
	{
		return 0.0;
	}

	std::vector<std::wstring> array_sems1 = splitSemanetes(line1);
	std::vector<std::wstring> array_sems2 = splitSemanetes(line2);
	std::stack<double> stk_max_sim;
	int len1 = array_sems1.size();
    int len2 = array_sems2.size();

	while (len1 != 0 && len2 != 0)
	{
		int m = -1, n = -1;
		double max_sim = 0;
		for (int i = 0; i < len1; i++)
		{
			for (int j = 0; j < len2; j++)
			{
				double simil = calSimBase(array_sems1[i], array_sems2[j]);
				if (simil > max_sim)
				{
					m = i;
					n = j;
					max_sim = simil;
				}
			}
		}
		
		if (max_sim == 0.0)
		{
			break;
		}
		
		stk_max_sim.push(max_sim);
		
		if (m != -1)
		{
			array_sems1.erase(array_sems1.begin()+m);
		}
		
		if (n != -1)
		{
			array_sems2.erase(array_sems2.begin()+n);
		}
		
		len1 = array_sems1.size();
		len2 = array_sems2.size();
	}
	//把整体相似度还原为部分相似度的加权平均,这里权值取一样，即计算算术平均 
	if (stk_max_sim.size() == 0)
	{
		return 0.0;
	}
	
	double sum = 0.0;
	int count = stk_max_sim.size();
	while (stk_max_sim.size() > 0)
	{
		double temp = stk_max_sim.top();
		sum += temp;
		stk_max_sim.pop();
	}
	
	return sum / count;
}

double Similarity::calSim3(std::wstring  line1, std::wstring  line2)
{
	if (line1.length() == 0 && line2.length() == 0)
	{
		return 1.0;
	}

	if (line1.length() == 0 || line2.length() == 0)
	{
		return 0.0;
	}
	std::vector<std::wstring> array_sems1 = splitSemanetes(line1);
	std::vector<std::wstring> array_sems2 = splitSemanetes(line2);


	std::stack<double> stk_sim ;

	int len1 = array_sems1.size();
	int len2 = array_sems2.size();

	while (len1 != 0 && len2 != 0)
	{
		for (int j = 0; j < len2; j++)
		{
			double ss = calSimReal(array_sems1[len1 - 1], array_sems2[j]);
			if (ss != 0)
			{
				stk_sim.push(ss);
				array_sems2.erase(array_sems2.begin()+j);
				break;
			}
		}

		array_sems1.erase(array_sems1.begin() + len1 - 1);

		len1 = array_sems1.size();
		len2 = array_sems2.size();
	}

	if (stk_sim.size() == 0)
	{
		return 0;
	}

	double sum = 0;

	int count = stk_sim.size();
	while (stk_sim.size() > 0)
	{
		sum += stk_sim.top();
		stk_sim.pop();
	}
	return sum / count;
}


double Similarity::calSim4(std::wstring line1, std::wstring line2)
{
	if (line1.length() == 0 && line2.length() == 0)
	{
		return 1;
	}

	if (line1.length() == 0 || line2.length() == 0)
	{
		return 0;
	}
	std::vector<std::wstring> array_sems1 = splitSemanetes(line1);
	std::vector<std::wstring> array_sems2 = splitSemanetes(line2);


	std::stack<double> stk_sim;

	int len1 = array_sems1.size();
	int len2 = array_sems2.size();

	while (len1 != 0 && len2 != 0)
	{
		wchar_t sym1 = (array_sems1[len1 - 1])[0];
		for (int j = 0; j < len2; j++)
		{
			wchar_t sym2 = (array_sems2[j])[0];
			if (sym1 == sym2)
			{
				std::wstring base1 = (array_sems1[len1 - 1]).substr(1);
				std::wstring base2 = (array_sems2[j]).substr(1);
				double ss = calSimBase(base1, base2);
				stk_sim.push(ss);
				array_sems2.erase(array_sems2.begin() + j);
				break;
			}
		}

		array_sems1.erase(array_sems1.begin() + len1 - 1);

		len1 = array_sems1.size();
		len2 = array_sems2.size();
	}

	if (stk_sim.size() == 0)
	{
		return 0;
	}

	double sum = 0;

	int count = stk_sim.size();
	while (stk_sim.size() > 0)
	{
		sum += stk_sim.top();
		stk_sim.pop();
	}
	return sum / count;
}