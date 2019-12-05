#ifndef __LOCK_HPP
#define __LOCK_HPP


namespace dumbnose
{


class lock_base{};

template<typename lock_t>
class lock_holder : public lock_base
{
public:
	lock_holder(const lock_t& lock) : lock_(lock) {
		lock_.acquire();
	}

	lock_holder(const lock_t& lock, unsigned int wait_time) : lock_(lock) {
		lock_.acquire(wait_time);
	}

	~lock_holder(){lock_.release();}
private:
	const lock_t& lock_;
};

template<typename lock_t>
lock_holder<lock_t>
hold_lock(lock_t& lock)
{
	return lock_holder<lock_t>(lock);
}

typedef const lock_base& lock_type;

#define HOLD_LOCK(lock_var) const dumbnose::lock_base& lock##__LINE__ = dumbnose::hold_lock(lock_var);


} // namespace dumbnose


#endif // #ifndef __LOCK_HPP

