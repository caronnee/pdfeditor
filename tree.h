#ifndef __TREE__
#define __TREE__

//class pre strom rozhodujuci o tom, ci je dane slovo v PDF
//toto by mohlo byt virtual
////TODO nejaka globalna funkcia typu preprocessing, ci to ma byt velke, male alebo jake, aby to vyhovovalo
////TODO zmenit na KMP alebo iny chytry algoritmus
class Accept
{
protected:
	char _ch;
	Accept * _next;
	Accept * _prev;
public:
	char getChar()const { return _ch; }
	Accept(char ch, Accept * prev) : _ch(ch), _next(NULL), _prev(prev) { }

	virtual Accept* accept(char ch)
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
	AcceptSet(std::string s, Accept * p) :Accept(' ', p)
	{
		for (size_t i = 0; i < s.length(); i++)
			acc.push_back(new Accept(s[i], this));
		_prev = p;
	}
	virtual Accept* accept(char ch)
	{
		for(size_t i =0; i < acc.size(); i++)
		{
			if(acc[i]->accept(ch))
				return acc[i]->next();
		}
		return prev();
	}
	virtual ~AcceptSet() {}
};
class AcceptSpace : public Accept
{
	bool _accepted;
public:
	AcceptSpace(char ot, Accept * prev) : Accept(ot, prev), _accepted(false)	{	}
	virtual Accept * accept(char c) 
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
	AcceptRange(char ot, int beg, int end, Accept * prev) : Accept(ot, prev)
	{
		_beg = beg; _end = end; _iter = 0;
	}
	virtual Accept * accept(char c) 
	{
		if (c != _ch)
		{
			int i = _iter;
			_iter = 0;
			if ( i < _beg) //nedociahli sme na spodnu hranicu
				return _prev;
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
	std::string _search;
	size_t _position;
public:
	int _begin;
	int _end; //poradie v tokenu, pretoze sa mi to naramne hodi:)
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
	Tree() 
	{ 
		_position = -1;
		_root= NULL;
		_actual = NULL;
		_begin = 0;
		_end = 0;
		_tokens = -1;
	}
	~Tree() { Clear(); }
	void setPattern(std::string pattern)
	{
		Clear(); //TODO checkovat, ci tam ten pattern uz nahodou nie je, takze netreba clear
		if (pattern.empty()||(pattern[0] == '\\' && pattern.length() ==1))
			throw "Wrong pattern input";
		Accept * prev = NULL;
		size_t i = 0;
		setAccept(pattern,i);
		_actual->setPrev(_actual); //back to root
		_root = _actual;
		prev = _root;
		while (i < pattern.length())
		{	
			setAccept(pattern,i);
			prev->setNext(_actual);
			prev=_actual;
		}
		_actual = _root;
	} //krajsie by to bolo asi odzadu ale co uz

	bool checkPattern() { return true; } //TODO dorobit na checkovanie, ci je to v spravnom tvare, tabulka pre preddefinovanie, kam sa ma skocit

private:
	void setAccept(std::string pattern, size_t & i)
	{
		switch (pattern[i])
		{
			case '*':
				{
					_actual = new AcceptRange(pattern[0],0,~0,_root);
					i+=2; //zobrali sme aj dalsie
					break;
				}
			case '\\':
				{
					_actual = new Accept(pattern[1],_root);
					i +=2;
					break;
				}
			case '?':
				{
					_actual = new AcceptRange(pattern[i+1],0,1,_root);
					i += 2;
					break;
				}
			case ' ':
				{
					if (_actual && _actual->getChar()!= ' ')
					{
						_actual = new AcceptSpace(pattern[i],_root);
					}
					i++; //spracovane
				}
			default:
				{ 
					_actual = new Accept(pattern[i],_root);
					i++;
				}

		}
	}
public:
	void setText(std::string text)
	{
		_tokens++;
		_search = text;
	}
	//ak je tj operator s whitespacom ->"test    test", "test \t test"
	//search - ..test -> .test OK
	//search - xx..test -> xx.text -> xx.(.)x

	void acceptSpace()
	{
		_actual = _actual->accept(' ');
		_position =-1;
	}
	TreeTokens search()
	{
		_position++; //pre polracujuce veci
		//stejne toho nedostane vela a bude to brat po tokenoch
		for (; _position< _search.length();  _position++)
		{
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
};
#endif  // __TREE__
