template<typename W, typename S, typename E> 
class Node {
private:
	std::list< Link<W,S,E> > inputs; //FIELD
	std::vector< Link<W,S,E>* > outputs; //FIELD
	Index<W,S,E>* index; //FIELD
	
	void update_output(const Link<W,S,E>* oldLink, const Link<W,S,E>* newLink);
	void remove_output(const Link<W,S,E>* pLink);
	void add_output(const Link<W,S,E>* pLink);
	
public:
	Node();
	Node(const Index<W,S,E>* pIndex);
	~Node(); 
	
	void add_input(	const unsigned int nOrigin,
			const W& wWeight);
	void remove_input(const unsigned int nOrigin);
	void clear();
	bool contains_input(const unsigned int nOrigin) const;
	bool contains_output(const unsigned int nTarget) const;
	
	class input_port {	
		private:
			typename std::list< Link<W,S,E> >::iterator current;

			friend class Node;
			input_port(const typename std::list< Link<W,S,E> >::iterator iLink);
	
	}; //class input_port
	
	class output_port {
		private:
			typename std::vector< Link<W,S,E>* >::iterator current;
			
			friend class Node;
			output_port(const typename std::vector< Link<W,S,E>* >::iterator iLink);
		public:
			
	}; //class output_port
		
	input_port  input_begin()  { return input_port( inputs.begin() ); }
	input_port  input_end()    { return input_port( inputs.end() ); }
	output_port output_begin() { return output_port( outputs.begin() ); }
	output_port output_end()   { return output_port( outputs.end() ); }
		
}; //class Node
