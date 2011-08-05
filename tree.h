/** \file Contains declaration of item that should be used in tree-based models. They are bookmark handlings and analyze igem handling */
#ifndef __TREE__
#define __TREE__

#include <QString>
#include <QChar>
#include "typedefs.h"

/** \brief base class for accpeting char */
class Accept
{
protected:
	QChar _ch;
	Accept * _next;
	Accept * _prev;
public:
	QChar getChar()const;
	/** \brief constructor */
	Accept(QChar ch, Accept * prev);
	/** this was not impleneted */
	virtual Accept * changeAccepted(QChar input);
	/** \return the next class that could handle the chhxt input. If it return NULL, this state is end state */
	virtual Accept* accept(QChar ch);
	/** sets the next state */
	void setNext ( Accept * acc);
	/** get the next state */
	Accept * next()const ;
	/** sets where to return if the search fails */
	void setPrev ( Accept * acc);
	Accept * prev()const;
	/** check if this is end state (null is the next */
	bool isEnd()const;
	/** check if this is begin state (previous is null */
	bool isBegin()const;
	/** destructor */
	virtual ~Accept() ;
};
/** class for accepting sets */
/** this class will create linear list of char that is couls accetp. If it accepts any of it, it accepts */
class AcceptSet : public Accept
{
	std::vector< Accept *> acc;
public:
	/** constructor */
	AcceptSet(QString s, Accept * p) ;
	/** reimplemented method */
	virtual Accept* accept(QChar ch);
	/** destructor **/
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
/** this class is not used */
class AcceptRange : public Accept
{
	int _beg, _end, _iter;
public:
	AcceptRange(QChar ot, int beg, int end, Accept * prev);
	virtual Accept * changeAccepted(QChar input);
	virtual Accept * accept(QChar c);
	virtual ~AcceptRange();
};
/** main class responsible for searching */
class Tree
{
	Accept * _actual;
	Accept * _root;
	QString _search;
	size_t _position;

public:
	/** special chars that regexp accepts */
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
	/** clears all states that was set */
	void Clear();
	/** constructor */
	Tree();

	/** reverts pattern according to the rules */
	QString revertPattern(QString s);

	/** destructor */
	~Tree();

	/** sets the state according to the flags */
	bool setPattern(QString pattern);

private:
	bool _regexp;
	bool _caseSensitive;
	bool _concateHyphen;
	bool _forward;
	/** creates i-th accepting state */
	void setAccept(QString pattern, int & i);
public:
	/** sets token */
	void setText(QString text);
	/** perform search */
	TreeTokens search();
	/** set flags according which it should be searched */
	void setFlags( int flags ) ;
	/** not implemented yet. this should validate the pattern if there was regexpt */
	bool validateSearch( QString srch ) ;
	/** sets the string backwars because of backward  searching */
	QString revertNormal( QString text ) ;
	/** to revert correctly, special tokens must be taken care of */
	QString getNextSpecialToken( QString s, int& i ) ;
};
#endif  // __TREE__
