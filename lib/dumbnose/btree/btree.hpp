#pragma once

//#include <dumbnose/mmap_file.hpp>
#include <ostream>
#include <boost/shared_ptr.hpp>
#include "impl_/node.hpp"
#include <boost/mpl/identity.hpp>
#include <boost/mpl/comparison.hpp>



namespace dumbnose { 
namespace btree {


using namespace boost::mpl;

// performs a binary search at compile time to find the largest number of records that fit
template<class key_t, class record_t, int max_size, int records_per_node=max_size/2, int step=max_size/2, bool found=false>
struct node_max_sizer
{
	typedef impl_::node<key_t,record_t,records_per_node> this_t;

	// cut the step size in half each time (can't be smaller than 1)
	static const int step_ = if_c<step<=1, int_<1>, int_<step/2> >::type::value;

	// calc the size of this and the next block
	static const int size_ = sizeof(this_t);
	static const int next_size_ = sizeof(impl_::node<key_t,record_t,records_per_node+1>);

	// if we are at the largest size that fits, we have found what we're looking for
	static const bool found_ = (size_<=max_size) && (next_size_>max_size);

	// 
	typedef typename eval_if_c<found_, 
				this_t, 
				eval_if_c<(size_<max_size), 
					typename identity<node_max_sizer<key_t,record_t,max_size,records_per_node+step_, step_, found_> >::type ,
					typename identity<node_max_sizer<key_t,record_t,max_size,records_per_node-step_, step_, found_> >::type 
					>
				> type;

	static const int records_per_node_ = type::records_per_node_;
};


template<class key_t, class record_t>
class btree
{
public:

	static const int records_per_node_ = node_max_sizer<key_t,record_t,mmap_file<>::block_size_>::records_per_node_;

	typedef impl_::node<key_t,record_t,records_per_node_> node_t;
	typedef typename node_t::this_ptr node_ptr;
	//typedef boost::shared_ptr<node_t> node_ptr;
	//typedef mmap_file<>::ptr_t node_ptr;

	btree() : file_(L"b_tree.bt") {
		if(file_.block_count() > 1) {
			root_ = reinterpret_cast<node_ptr>(file_.get_reference(1).get());
			root_->file(&file_);

			root_ = root_->find_root();
		} else {
			mmap_file<>::node_index_t index;  mmap_file<>::ptr_t ptr;
			boost::tuples::tie(index,ptr) = file_.allocate_block();

			root_ = node_ptr(new (ptr.get()) node_t(&file_,index,0));
		}
	}

	~btree() {}

	const record_t* find(const key_t& key) const
	{
		return root_->find(key);
	}

	void insert(const key_t& key, record_t& record)
	{
		root_->insert(key,record);
		root_ = root_->find_root()/*->shared_from_this()*/;
	}

	std::ostream& operator<<(std::ostream& os) const
	{
		return root_->output(os,0);
	}

	int depth()
	{
		return root_->depth();
	}

	// make sure all keys obey the ordering of the tree
	void validate()
	{
		root_->validate();
	}

private:
	mmap_file<> file_;
	node_ptr root_;

};


}} // namespace dumbnose { namespace btree {


template <class key_t, class record_t>
std::ostream& operator<<(std::ostream& os, const dumbnose::btree::btree<key_t,record_t>& tree)
{
	return tree << os;
}

