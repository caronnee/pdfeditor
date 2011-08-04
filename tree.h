/** \file Contains declaration of item that should be used in tree-based models. They are bookmark handlings and analyze igem handling */
#ifndef __TREE__
#define __TREE__

#include <QString>
#include <QChar>
#include "typedefs.h"

//class pre strom rozhodujuci o tom, ci je dane slovo v PDF
//toto by mohlo byt virtual
////TODO nejaka globalna funkcia typu preprocessing, ci to ma byt velke, male alebo jake, aby to vyhovovalo
////TODO zmenit na KMP alebo iny chytry algoritmus
class Accept
{
protected:
	QChar _ch;
	Accept * _next;
	Accept * _prev;
public:
	QChar getChar()const;
	Accept(QChar ch, Accept * prev);
	virtual Accept * changeAccepted(QChar input);
	virtual Accept* accept(QChar ch);
	void setNext ( Accept * acc);
	Accept * next()const ;
	void setPrev ( Accept * acc);
	Accept * prev()const;
	bool isEnd()const;
	bool isBegin()const;
	virtual ~Accept() ;
};

class AcceptSet : public Accept
{
	std::vector< Accept *> acc;
public:
	AcceptSet(QString s, Accept * p) ;
	virtual Accept* accept(QChar ch);
	virtual ~AcceptSet();
};
//class AcceptSpace : public Accept
//{
//	bool _accepted;
//public:
//	AcceptSpace(QChar ot, Accept * prev);
//	virtual Accept * accept(QChar c) ;
//};
// {0-19}
class AcceptRange : public Accept
{
	int _beg, _end, _iter;
public:
	AcceptRange(QChar ot, int beg, int end, Accept * prev);
	virtual Accept * changeAccepted(QChar input);
	virtual Accept * accept(QChar c);
	virtual ~AcceptRange();
};
class Tree
{
	Accept * _actual;
	Accept * _root;
	QString _search;
	size_t _position;

public:
	enum Special
	{
		QUESTION= '?',
		STAR = '*',
		SLASH = '\\',
		LBRACKET = '[',
		RBRACKET =']',
		DOT = '.'
	};
	int _begin;
	int _end; 
	int _tokens;

public:
	enum TreeTokens
	{
		Next, 
		Found
	};
	void Clear();
	Tree();
	QString Tree::revertPattern(QString s);

	~Tree();
	bool setPattern(QString pattern);

private:
	bool _regexp;
	bool _caseSensitive;
	bool _concateHyphen;
	bool _forward;
	void setAccept(QString pattern, int & i);
public:
	void setText(QString text);
	TreeTokens search();
	void setFlags( int flags ) ;
	bool validateSearch( QString srch ) ;
	QString revertNormal( QString text ) ;
	QString getNextSpecialToken( QString s, int& i ) ;
};
#endif  // __TREE__
