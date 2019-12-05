#pragma once


#include <string>
#include <ostream>
#include <exception>
#include <assert.h>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <dumbnose/mmap_file.hpp>

namespace dumbnose { 
namespace btree {
namespace impl_ {


typedef int rec_count_t;
typedef mmap_file<>::node_index_t node_index_t;


template <class key_t, class record_t, rec_count_t records_per_node>
class node /*: public boost::enable_shared_from_this<node<key_t,record_t,records_per_node> >*/
{
public:

	typedef node<key_t,record_t,records_per_node> this_t;
	//typedef boost::shared_ptr<node> this_ptr;
	typedef this_t* this_ptr;
	typedef const this_ptr const_this_ptr;

private:
	// an entry in the node's array of key/record pairs it should only 
	// be allocated as part of this array, or part of a subtree (see next struct)
	//
	// @todo	may need to rethink how keys are stored to reduce the amount of copying that 
	//			can occcur
	struct entry_t
	{
		entry_t():smaller_node_(0) {}

		node_index_t smaller_node_;
		key_t key_;
		record_t record_;
	};

public:
	//static const rec_count_t records_per_node_ = (64*1024 - 4*sizeof(void*)) / (sizeof(entry_t));
	static const rec_count_t records_per_node_ = records_per_node;

	// this is the position where splits will occur
	static const rec_count_t middle_ = records_per_node_/2 + 1;

	node(mmap_file<>* file, node_index_t this_node, node_index_t parent) : 
		file_(file), this_node_(this_node), parent_node_(parent), record_count_(0) 
	{}

	void init(mmap_file<>* file, node_index_t this_node, node_index_t parent)
	{
		file_ = file;
		this_node_ = this_node;
		parent_node_ = parent;
		record_count_ = 0;
	}

	const record_t* find(const key_t& key) const
	{
		this_ptr location;
		return const_cast<node*>(this)->find_record_and_node(key,location);
	}

	void file(mmap_file<>* val) {file_ = val;}

	// from any node in the tree, find the root and return it
	this_t* find_root()
	{
		if(parent_node_==0) return this;

		return parent_node()->find_root();
	}

	// insert the key and record into btree, starting at the root.
	void insert(const key_t& key, record_t& record)
	{
		// make sure inserts happen at the root
		if(this->parent_node_!=0) {	find_root()->insert(key,record); return;}

		this_ptr location;
		if(find_record_and_node(key,location) != 0) throw std::exception("node already exists");
		assert(location!=0); if(location==0) throw std::exception("find_record_and_node returned a null location");

		// ensure there is room, if needed to make room, find the new location (starting at root)
		if(location->make_room()) {
			this->find_root()->find_record_and_node(key,location);
			assert(location!=0); if(location==0) throw std::exception("find_record_and_node returned a null location");
		}

		subtree_t subtree(file_);
		subtree.entry().key_ = key;
		subtree.entry().record_ = record;
		location->insert_into_node(subtree);
	}

	std::ostream& output(std::ostream& os, int depth)
	{
		std::string indent(depth,'\t');

		os << indent << "[NODE]" << std::endl;

		for(int i=0 ; i<record_count_ ; ++i) {
			os << indent << "Key[" << i << "]:  " << entries_[i].key_ << std::endl;
			if(entries_[i].smaller_node_!=0) convert_index_to_ptr(entries_[i].smaller_node_)->output(os,depth+1);
		}

		if(larger_node_ != 0) {
			os << indent << "[LARGER]" << std::endl;
			larger_node()->output(os,depth+1);
		}

		return os;
	}

	bool full()
	{
		return record_count_==records_per_node_;
	}

	int depth()
	{
		if(entries_[0].smaller_node_==0) return 1;

		return convert_index_to_ptr(entries_[0].smaller_node_)->depth() + 1;
	}

	void validate()
	{
		if(record_count_==0) return;

		// make sure entries are in the correct order
		for(rec_count_t i=1 ; i<record_count_ ; ++i) {
			if(!(entries_[i-1].key_<entries_[i].key_)) handle_validation_error("node violates entry ordering");
		}

		// make sure the smaller invariant holds
		for(rec_count_t i=0 ; i<record_count_ ; ++i) {
			if(entries_[i].smaller_node_!=0) convert_index_to_ptr(entries_[i].smaller_node_)->validate_less_than(entries_[i].key_);
		}

		// make sure the larger invariant holds
		for(rec_count_t i=1 ; i<record_count_ ; ++i) {
			if(entries_[i].smaller_node_!=0) convert_index_to_ptr(entries_[i].smaller_node_)->validate_greater_than(entries_[i-1].key_);
		}
		if(larger_node_!=0) larger_node()->validate_greater_than(entries_[record_count_-1].key_);

		// validate parent/child relationship
		for(rec_count_t i=0 ; i<record_count_ ; ++i) {
			if(entries_[i].smaller_node_!=0) convert_index_to_ptr(entries_[i].smaller_node_)->validate_parent_node(this_node_);
		}
		if(larger_node_!=0) this->larger_node()->validate_parent_node(this_node_);

		// recursively validate each child
		for(rec_count_t i=0 ; i<record_count_ ; ++i) {
			if(entries_[i].smaller_node_!=0) convert_index_to_ptr(entries_[i].smaller_node_)->validate();
		}
		if(larger_node_!=0) this->larger_node()->validate();
	}

private:

	this_ptr convert_index_to_ptr(node_index_t index){
		if(index==0) return 0;

		this_ptr node = reinterpret_cast<this_ptr>(file_->get_reference(index).get());
		node->file_ = file_;

		return node;
	}

	// represents a subtree in the tree
	// only temporary storage, not part of the actual tree, should be on stack
	struct subtree_t
	{
		subtree_t(mmap_file<>* file):file_(file),larger_node_(0) {}

		entry_t& entry(){return entry_;}
		void entry(entry_t& new_entry, node_index_t parent) {
			entry_ = new_entry;
			if(entry_.smaller_node_ != 0) convert_index_to_ptr(entry_.smaller_node_)->parent_node_ = parent;
		}

		this_ptr convert_index_to_ptr(node_index_t index){
			if(index==0) return 0;

			this_ptr node = reinterpret_cast<this_ptr>(file_->get_reference(index).get());
			node->file_ = file_;

			return node;
		}

		node_index_t larger_node_index(){return larger_node_;}
		this_ptr larger_node(){return convert_index_to_ptr(larger_node_);}

		void larger_node(node_index_t new_larger_node, node_index_t parent) {
			larger_node_ = new_larger_node;
			if(larger_node_ != 0) larger_node()->parent_node(parent);
		}

	private:
		mmap_file<>* file_;
		entry_t entry_;
		node_index_t larger_node_;
	};

	// inserts a record into this node
	void insert_into_node(subtree_t& subtree)
	{
		assert(!this->full()); if(this->full()) throw std::exception("Attempting to insert a node into a full node");

		// if this is a result of splitting a node, fix up the original node pointer so that it now points to the new split 
		if(subtree.larger_node()!=0) {
			assert(subtree.entry().smaller_node_!=0); // if one is supplied, both must be

			node_index_t& node_ref = find_node_ptr(subtree.entry().smaller_node_);
			node_ref = subtree.larger_node_index();
		}

		rec_count_t position = find_position(subtree.entry().key_);
		free_up(position);

		entries_[position] = subtree.entry();
		if(entries_[position].smaller_node_ != 0) convert_index_to_ptr(entries_[position].smaller_node_)->parent_node(this_node_);

		record_count_++;
	}

	node_index_t& find_node_ptr(node_index_t node_ref)
	{
		if(larger_node_==node_ref) return larger_node_;

		for(rec_count_t i=0 ; i<record_count_ ; ++i) {
			if(entries_[i].smaller_node_==node_ref) return entries_[i].smaller_node_;
		}

		assert(!"Did not find the node ptr"); throw std::exception("Did not find the node ptr");
	}

	void insert_on_end_unsafe(entry_t& entry)
	{
		assert(record_count_<=records_per_node_); if(record_count_>records_per_node_) throw std::exception("insert_on_end_unsafe: inserted entry past end");

		entries_[record_count_] = entry;
		if(entries_[record_count_].smaller_node_ != 0) convert_index_to_ptr(entries_[record_count_].smaller_node_)->parent_node(this_node_);
		++record_count_;
	}

	bool make_room()
	{
		if(!this->full()) return false;

		if(parent_node_!=0) {
			parent_node()->make_room();
		}

		split();
#ifdef _DEBUG
		this->find_root()->validate();
#endif

		return true;
	}

	//
	// split this node so an insert can happen at this position
	//
	// returns - the parent node (possibly created as part of this split)
	//
	this_ptr split()
	{
		if(parent_node_==0) return split_root(); // if we're at the root, split it

		subtree_t subtree(file_);
		split_node(subtree);

        parent_node()->insert_into_node(subtree);

		return parent_node();
	}

	// returns the new root
	this_ptr split_root()
	{
		subtree_t subtree(file_);
		split_node(subtree);

		node_index_t index;  mmap_file<>::ptr_t ptr;
		boost::tuples::tie(index,ptr) = file_->allocate_block();

        this_ptr parent(new (ptr.get()) this_t(file_,index,0)); // create the new parent

		parent->larger_node(subtree.larger_node_index());
		parent->insert_on_end_unsafe(subtree.entry());

		// fix up parent nodes
		this->parent_node(index);

		return parent;
	}

	// split the node in half
	void split_node(subtree_t& subtree)
	{
		node_index_t index;  mmap_file<>::ptr_t ptr;
		boost::tuples::tie(index,ptr) = file_->allocate_block();

		this_ptr new_node = this_ptr(new (ptr.get()) this_t(file_,index,parent_node_));

		subtree.larger_node(new_node->this_node_,parent_node_);

		for(rec_count_t i=middle_+1 ; i<record_count_ ; ++i) {
			new_node->insert_on_end_unsafe(entries_[i]);
		}

		// fix up larger_node_'s
		new_node->larger_node(this->larger_node_);
		// @todo:  remove this if statement once smaller_nodes become indexes instead of pointers
		if(entries_[middle_].smaller_node_!=0) {
			this->larger_node(entries_[middle_].smaller_node_);
		} else {
			this->larger_node(0);
		}

		// move middle node to parent
		subtree.entry() = entries_[middle_];
		subtree.entry().smaller_node_ = this->this_node_/*->shared_from_this()*/;

		record_count_ = middle_;
	}

	// find a key in the tree and the leaf node where it belongs
	// returns - the record, if it exists, 0 otherwise
	// location - the node where it exists, or the leaf node where it would be in if it existed
	record_t* find_record_and_node(const key_t& key, this_ptr& location)
	{
		location = this/*->shared_from_this()*/;

		//// linear search implementation
		//for(rec_count_t i=0 ; i<record_count_ ; ++i) {
		//	if(key<entries_[i].key_) {
		//		return entries_[i].smaller_node_ ? entries_[i].smaller_node_->find_record_and_node(key, location) : 0;
		//	} else if(key==entries_[i].key_) {
		//		return &entries_[i].record_;
		//	}
		//}
		//
		//return larger_node_ ? larger_node_->find_record_and_node(key, location) : 0;

		//if(key>entries_[record_count_-1].key_) return larger_node_ ? larger_node_->find_record_and_node(key, location) : 0;

		rec_count_t current = record_count_/2;
		rec_count_t step = current;

		while(current>=0 && current<record_count_) {
			if(key==entries_[current].key_) return &entries_[current].record_; // if found, we're done

			step /= 2; if(step==0) step = 1;

			if(key<entries_[current].key_) {
				if (current==0 || key>entries_[current-1].key_) {
					return entries_[current].smaller_node_!=0 ? convert_index_to_ptr(entries_[current].smaller_node_)->find_record_and_node(key, location) : 0;
				}

				current -= step;
			} else {
				current += step;
			}
		}

		if(current>=record_count_) return larger_node_ ? larger_node()->find_record_and_node(key, location) : 0;

		return 0;
	}

	// finds the position where the key is or would be in this node
	// does not recurse
	rec_count_t find_position(const key_t& key) const
	{
		for(rec_count_t i=0 ; i<record_count_ ; ++i) {
			if(key < entries_[i].key_) return i;

			assert(key != entries_[i].key_);
		}

		return record_count_;
	}

	// shifts all nodes to the right so that the position specified is free
	void free_up(rec_count_t position)
	{
		for(rec_count_t i=record_count_ ; i>position ; --i) {
			entries_[i] = entries_[i-1];
		}
	}

	void validate_less_than(const key_t& key)
	{
		for(rec_count_t i=0 ; i<record_count_ ; ++i) {
			if(!(entries_[i].key_<key)) {
				std::cout << entries_[i].key_ << " is not less than " << key << std::endl;
				handle_validation_error("less than invariant violated");
			}
		}
	}

	void validate_greater_than(const key_t& key)
	{
		for(rec_count_t i=0 ; i<record_count_ ; ++i) {
			if(!(key<entries_[i].key_)) {
				std::cerr << "Key: " << key << " is not greater than: " << entries_[i].key_ << std::endl;
				handle_validation_error("greater than invariant violated");
			}
		}
	}

	void validate_parent_node(node_index_t parent)
	{
		if(parent_node_!=parent) {
			std::cout << "---------------------------------------------------------------------------\n";
			std::cout << "NODE:\n";
			this->output(std::cout,0);
			std::cout << std::endl;
			std::cout << "---------------------------------------------------------------------------\n";
			std::cout << "CURRENT PARENT:\n";
			//parent_node_->output(std::cout,0);
			std::cout << std::endl;
			std::cout << "---------------------------------------------------------------------------\n";
			std::cout << "EXPECTED PARENT:\n";
			//parent->output(std::cout, 0);
			std::cout << std::endl;
			throw std::exception("parent/child invariant violated");
			//handle_validation_error("parent/child invariant violated");
		}
	}

	void handle_validation_error(const char* error)
	{
		std::cerr << "---------------------------------------------------------------------------\n";
		this->output(std::cerr,0);
		std::cerr << "---------------------------------------------------------------------------\n\n\n";
		throw std::exception(error);
	}

	this_ptr larger_node(){return convert_index_to_ptr(larger_node_);}
	void larger_node(node_index_t new_larger_node)
	{
		larger_node_ = new_larger_node;
		if(larger_node_ != 0) larger_node()->parent_node(this_node_);
	}

	this_ptr smaller_node(entry_t& entry){return convert_index_to_ptr(smaller_node_);}
	void smaller_node(node_index_t new_smaller_node)
	{
		smaller_node_ = new_smaller_node;
		if(smaller_node_ != 0) smaller_node()->parent_node(this_node_);
	}

	this_ptr parent_node() {return convert_index_to_ptr(parent_node_);}
	void parent_node(node_index_t parent) {parent_node_ = parent;}

	//
	// members
	//
	mmap_file<>* file_;
	node_index_t this_node_;
	node_index_t parent_node_;
	node_index_t larger_node_;
	rec_count_t record_count_;

	entry_t entries_[records_per_node_];
};


}}} // namespace dumbnose { namespace btree { namespace impl_ {
