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
	QChar getChar()const { return _ch; }
	Accept(QChar ch, Accept * prev) : _ch(ch), _next(NULL), _prev(prev) { }

	virtual Accept * changeAccepted(QChar input)
	{
		return _prev;
	}
	virtual Accept* accept(QChar ch)
	{
		if (ch == _ch)
			return _next;
		return _prev;
	}
	void setNext ( Accept * acc) { _next = acc; }
	Accept * next()const { return _next;}
	void setPrev ( Accept * acc) { _prev = acc; }
	Accept * prev()const { return _prev;}
	bool isEnd()const { return _next == NULL; }
	bool isBegin()const { return _prev == this; } //resp root, TODO vlastna podclass?Treba?
	virtual ~Accept() {}
};

class AcceptSet : public Accept
{
	std::vector< Accept *> acc;
public:
	AcceptSet(QString s, Accept * p) :Accept(' ', p)
	{
		for (size_t i = 0; i < s.length(); i++)
		{
			if (s[i] == '\\')
				continue;
			acc.push_back(new Accept(s[i], this));
		}
		_prev = p;
	}
	virtual Accept* accept(QChar ch)
	{
		for(size_t i =0; i < acc.size(); i++)
		{
			if(acc[i]->accept(ch)!=this)
				return next();
		}
		return prev();
	}
	virtual ~AcceptSet() {}
};
class AcceptSpace : public Accept
{
	bool _accepted;
public:
	AcceptSpace(QChar ot, Accept * prev) : Accept(ot, prev), _accepted(false)	{	}
	virtual Accept * accept(QChar c) 
	{
		if ( c == ' ') //ak je to whitespace, TODO
		{
			_accepted = true;
			if ( _next == NULL )
				return NULL;
			return this;
		}
		bool pom = _accepted;
		_accepted = false; //do povodneho stavu
		if (pom)
			return _next->accept(c);
		return _prev;
	}
};
// {0-19}
class AcceptRange : public Accept
{
	int _beg, _end, _iter;
public:
	AcceptRange(QChar ot, int beg, int end, Accept * prev) : Accept(ot, prev)
	{
		_beg = beg; _end = end; _iter = 0;
	}
	virtual Accept * changeAccepted(QChar input)
	{
//TODO later
		return NULL;
	}
	virtual Accept * accept(QChar c)  //zatial iba jedno pismeno
	{
		if (c != _ch)
		{
			int i = _iter;
			_iter = 0;
			if ( i < _beg) //nedociahli sme na spodnu hranicu
				return _prev->changeAccepted(c);
			return next()->accept(c);
		}
		_iter ++;
		if (_iter < _end) 
			return this;
		_iter = 0;
		return _prev;
	}
	virtual ~AcceptRange() {}
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
	int _end; //poradie v tokenu
	int _tokens;

public:
	enum TreeTokens
	{
		Next, //ma dalsi token
		Found //je v koncovom stave
	};
	void Clear()
	{
		while(_root!=NULL)
		{
			Accept * t = _root;
			_root = t->next();
			delete t; //TODO check
		}
	}
	Tree() : _regexp(false),_position (-1),_root(NULL),_caseSensitive(false),_concateHyphen(false),
		_actual (NULL),_begin (0),_end (0),_tokens(-1)
	{}

	QString Tree::revertPattern(QString s)
	{
		if (!_regexp)
			return revertNormal(s); //v tomto okamihu je string validny
		int i =0;
		QString res;
		while (i < s.size())
		{ //vsetky rozoznavane znaky-> .?*[]
			QString token = getNextSpecialToken(s,i);
			res.push_front(token);
		}
		return res;
	}

	~Tree() { Clear(); }
	bool setPattern(QString pattern)
	{
		if (!_forward)
			pattern = revertPattern(pattern);
		Clear();
		if (!validateSearch(pattern))
			return false;
		assert(!pattern.isEmpty()); //TODO validate
		pattern = pattern.trimmed();
		if (!_caseSensitive)
			pattern = pattern.toLower();
		Accept * prev = NULL;
		int i = 0;
		setAccept(pattern,i);
		_actual->setPrev(_actual); //back to root
		_root = _actual;
		prev = _root;
		while (i < pattern.length())
		{	
			if (pattern[i] == QChar(' '))
			{
				i++;
				continue;
			};
			setAccept(pattern,i);
			prev->setNext(_actual);
			prev=_actual;
		}
		_actual = _root;
		return true;
	} //krajsie by to bolo asi odzadu ale co uz

private:
	bool _regexp;
	bool _caseSensitive;
	bool _concateHyphen;
	bool _forward;
	void setAccept(QString pattern, int & i)
	{
		if (!_regexp)
		{
			_actual = new Accept(pattern[i],_root);
			i++;
			return;
		}
		if (pattern[i] == QChar('*'))
		{
			_actual = new AcceptRange(pattern[i],0,~0,_root);
			i+=2; //zobrali sme aj dalsie				
		}
		else if (pattern[i] == QChar('\\'))
		{
			_actual = new Accept(pattern[i+1],_root);
			i +=2;
		}
		else if (pattern[i] == QChar('['))
		{
			int j = i;
			while ((j = pattern.indexOf("]", j)) != -1) {
				if (pattern[j-1] != '\\')
					break;
				++j;
			}
			QString res = pattern.mid(i+1,j-i-1);
			_actual = new AcceptSet(res,_root);
			i = j+1;
		}
		else if (pattern[i+1] == QChar('?'))
		{
			_actual = new AcceptRange(pattern[i],0,1,_root);
			i += 2;
		}
		//else if (pattern[i] == QChar(' '))
		//{
		//	if (_actual && _actual->getChar()!= ' ')
		//	{
		//		_actual = new AcceptSpace(pattern[i],_root);
		//	}
		//	i++; //spracovane
		//}
		else
		{ 
			_actual = new Accept(pattern[i],_root);
			i++;
		}
	}
public:
	void setText(QString text)
	{
		_tokens++;
		if (!_caseSensitive)
			text = text.toLower();
		if (!_forward)
			text = revertNormal(text);
		_search = text;
	}
	//ak je tj operator s whitespacom ->"test    test", "test \t test"
	//search - ..test -> .test OK
	//search - xx..test -> xx.text -> xx.(.)x

	TreeTokens search()
	{
		_position++; //pre pokracujuce veci
		//stejne toho nedostane vela a bude to brat po tokenoch
		for (; _position< _search.length();  _position++)
		{
			if (_concateHyphen && _search[_position]==QChar('-'))
				continue; //automaticky accept
			if (_actual->isBegin())
			{
				_tokens = 0;
				_begin = _position;
			}
			_actual = _actual->accept(_search[_position]);
			if (_actual==NULL) //posledne
			{
				_actual = _root;
				_end = _position;
				return Tree::Found;//kolkate pismeno to bolo. Operator budeme vediet z toho, co tam vrazame
			}
		}
		_position = -1;
		return Tree::Next;
	}

	void setFlags( int flags ) 
	{
		_caseSensitive = flags & SearchCaseSensitive;
		_concateHyphen = flags & SearchConcate;
		_regexp = flags & SearchRegexp;
		_forward = flags & SearchForward;
	}

	bool validateSearch( QString srch ) 
	{
		if (!_regexp)
			return true;
		//pravidla pre regularny vyraz
		bool previousWasSpecial = true;
		for ( int i =0; i< srch.size(); i++)
		{

		}
		return true;
	}

	QString revertNormal( QString text ) 
	{
		QString res;
		for ( int i =0; i < text.size(); i++)
			res.push_front(text[i]);
		return res;
	}

	QString getNextSpecialToken( QString s, int& i ) 
	{
		switch (s[i].unicode())
		{
		case QUESTION:
		case STAR:
		case SLASH:
			{
				QString ret;
				ret = QString(s[i]) + s[i+1];
				i+=2;
				return ret;
			}
		case LBRACKET:
			{
				int index = s.indexOf(']',i);
				QString ret = s.mid(i,index - i+1); //vratane zavoriek
				i = index+1;
				return ret;
			}
		case RBRACKET:
			assert (false);
		default:
			i++;
			return QString(s[i-1]);
		}
	}



};
#endif  // __TREE__
