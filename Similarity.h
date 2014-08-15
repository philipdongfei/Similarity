
#include "stdafx.h"

#include <string>
#include <map>
#include<vector>

typedef enum
{
	ADJ,
        ADV,
        CLAS,
        CONJ,
        ECHO,
        EXPR,
        N,
        NUM,
        PREP,
        PRON,
        STRU,
        UNSET,
        V,                 
        W     
}WORDTYPE;

class Semanteme
{
public:
	Semanteme()
	{
		nID = 0;
		wstrSema = L"";
		nParentID = 0;
	};

/*	Semanteme(UINT id, std::wstring sema, UINT pid)
	{
		nID = id;
		wstrSema = sema;
		nParentID = pid;
	};*/



	UINT    nID;
	std::wstring   wstrSema;
	UINT    nParentID;
};

class Glossary
{
public:
	Glossary()
	{
		Word = L"";
		Type = L"";
		Concepts = L"";

	}
	std::wstring  Word;
	std::wstring  Type;
	std::wstring  Concepts;
};


class Similarity
{
public:
	Similarity();	
	~Similarity();


	bool initialize();
	bool ConstructConcept();
	bool ConstructSemantems();
	void splitLine(char *buffer,char *Col1, char *Col2, char *Col3);
	void ChangeToW(std::wstring &t_wstr, char *buffer);

	double calWordSim(std::wstring word1, std::wstring word2,std::wstring type);// 计算相似度
	double calConceptSim(std::wstring concept1, std::wstring concept2);//计算概念相似度
	int Distance(std::wstring s1, std::wstring s2);
	bool GetParentSemantem(Semanteme &sem);
	bool GetSemantems(std::wstring sema,Semanteme &sem);
	double calSimBase(std::wstring sem1, std::wstring sem2);
	double calSimReal(std::wstring sem1, std::wstring sem2);
	double calSim1(std::wstring line1, std::wstring line2);///// 计算第一独立义原描述式的相似度
	double calSim2(std::wstring line1, std::wstring line2); /// 计算其他独立义原描述式的相似度 
	double calSim3(std::wstring  line1, std::wstring  line2);//计算关系义原描述式的相似度
	 double calSim4(std::wstring line1, std::wstring line2);  /// 计算符号义原描述式的相似度 
	std::vector<std::wstring> splitSemanetes(std::wstring line);
	
private:
		std::wstring _RelationalSymbol;

        double _alpha;     //计算基本义原相似度时的参数 
        double _beta1;     //4种描述式相似度的权值 
        double _beta2 ;
        double _beta3;
        double _beta4;

        double _delta;
        double _gama;

        int _Default_Distance;
		std::string _SemantemeFileName;
		std::string _ConceptFileName;
		std::string _ConceptIndexFileName;

	//	std::map<std::wstring,Glossary>  mapConceptIndex;
		std::map<std::wstring,std::string>  mapConceptIndex;
        std::map<std::wstring,Semanteme> mapSemantems;
        std::map<UINT,Semanteme> mapIndexSemantmes;


       
};


