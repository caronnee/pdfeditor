#ifndef __TREE__
#define __TREE__

class Accept
{
protected:
	char _ch;
	Accept * _next;
	Accept * _prev;
public:
	Accept(char a, Accept * prev) _ch(ch), _next(NULL), _prev(prev) { }

	virtual Accept * accept(char ch)
	{
		if (ch == _ch)
			return _next;
		return _prev;
	}
	void setNext ( Accept * acc) { _next = acc; }
	Accept * next()const { return _next;}
} ;
class AcceptSet : public Accept
{
	std::vector<Accept *> acc;
public:
	AcceptSet(std::string s, Accept * p) 
	{
		for (int i = 0; i < s.length(); i++)
			acc.push_back(new Accept(s[i], this));
		_prev = p;
	}
	virtual Accept * accept(char ch)
	{
		for(int i =0; i < acc.size(); i++)
		 {
			 if(acc[i]->accept(ch))
				 return _next;
				 //TODO
		 }
	}
};
#endif  // __TREE__
