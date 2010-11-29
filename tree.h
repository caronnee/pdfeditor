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
	bool isBegin()const { return _prev == NULL; }
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

// {0-19}
class AcceptRange : public Accept
{
	int _beg, _end, _iter;
public:
	AcceptRange(char ot, int beg, int end, Accept * prev) : Accept(ot, prev)
	{
		_ch = ot; _beg = beg; _end = end; _iter = 0;
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
	Accept * actual;
	Accept * root;
	int size;
public:
	int getSize()const
	{
		return size;
	}
	Tree(std::string pattern)
	{
		size = 0;
		if (pattern.empty()||(pattern[0] == '\\' && pattern.length() ==1))
			throw "Wrong pattern input";
		Accept * prev = NULL;
		size_t i = 0;
		root = actual;
		prev = root;
		while (i < pattern.length())
		{	
			setAccept(pattern,i);
			prev->setNext(actual);
			prev=actual;
		}
		actual = root;
	} //krajsie by to bolo asi odzadu ale co uz


	bool checkPattern() { return true; } //TODO dorobit na checkovanie, ci je to v spravnom tvare, tabulka pre preddefinovanie, kam sa ma skocit

private:
	void setAccept(std::string pattern, size_t & i)
	{
		switch (pattern[i])
			{
				case '*':
					{
						actual = new AcceptRange(pattern[0],0,~0,root);
						i+=2; //zobrali sme aj dalsie
						break;
					}
				case '\\':
				{
					root = new Accept(pattern[1],root);
					i +=2;
					break;
				}
				case '?':
				{
					root = new AcceptRange(pattern[0],0,1,root);
					i += 2;
					break;
				}
				default:
				{
					actual = new Accept(pattern[0],root);
					i++;
				}

			}
	}
public:
	int fill(std::string valueToFill)
	{
		//stejne toho nedostane vela
		for ( size_t i = 0; i< valueToFill.length(); i++)
		{
			if(actual == root)
				size = -1;
			size++;
			actual = actual->accept(valueToFill[i]);
			if (actual->isEnd())
			{
				actual = root;
				return i;//kolkate pismeno to bolo. Operator budeme vediet z toho, co tam vrazame
			}
		}
		return -1;
	}
};
#endif  // __TREE__
